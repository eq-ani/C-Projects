#include "interpreter.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* reset the system to a defulat status */
void initialize_system(System *sys) {
  sys->registers[EAX] = 0;
  sys->registers[EDX] = 0;
  sys->registers[ECX] = 0;
  sys->registers[ESP] = MEMORY_SIZE - 256;
  sys->registers[EBP] = MEMORY_SIZE - 256;
  sys->registers[EIP] = 0;  // Program counter

  sys->memory.num_instructions = 0;
  for (int i = 0; i < MEMORY_SIZE; i++) {
    sys->memory.instruction[i] = NULL;
    sys->memory.data[i] = 0;
  }
  sys->comparison_flag = 0;
}

/* Remove leading and extra space, and \n from the input string and return the
 * length of updated string */
int reformat(char *line) {
  int idx, size = 0, flag = 0;
  int line_size = strlen(line);
  for (idx = 0; idx < line_size && line[idx] == ' '; idx++)
    ;
  for (; idx < line_size; idx++) {
    if (line[idx] == '\n') break;
    if (line[idx] == ' ') {
      if (!flag) {
        line[size++] = line[idx];
        flag = 1;
      }
    } else {
      line[size++] = line[idx];
      flag = 0;
    }
  }

  line[size] = '\0';
  return size;
}

/* Load all the instruction from the file into the instruction segment in the
 * system */
void load_instructions_from_file(System *sys, const char *filename) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("Error opening file");
    exit(EXIT_FAILURE);
  }

  char line[256];
  int address = 0;

  while (fgets(line, sizeof(line), file) != NULL && address < MEMORY_SIZE) {
    // Remove newline character
    line[strlen(line) - 1] = '\0';
    // Save instruction to the memory
    int size = reformat(line);
    if (size == 0) continue;
    sys->memory.instruction[address] = strdup(line);
    address++;
    // Reach out the end of the instruction
    if (strcmp(line, "END") == 0) break;
  }
  sys->memory.num_instructions = address;

  fclose(file);
}

/* Return value could be the name of one of the valid registers, or NOT_REG for
 * any other input */
RegisterName get_register_by_name(const char *name) {
  if (strcmp(name, "%EAX") == 0) return EAX;
  if (strcmp(name, "%EDX") == 0) return EDX;
  if (strcmp(name, "%ECX") == 0) return ECX;
  if (strcmp(name, "%ESP") == 0) return ESP;
  if (strcmp(name, "%EBP") == 0) return EBP;
  if (strcmp(name, "%EIP") == 0) return EIP;
  return NOT_REG;  // indicate this is not a register
}

/*
This function accepts an operand that can be represented in different formats:
registers, memory, or constant values. For example, a valid operand could be
%EAX, (%EAX), -20(%EAX), or $10.

The function returns one of the following: REG for a register operand, MEM for a
memory operand, or CONST for a constant operand. If the parsing succeeds, the
function will return the appropriate type; otherwise, it will return UNKNOWN in
the event of a parsing error.

For a register operand: reg will be one of the registers, and value will be -1.
For a memory operand: reg will be the register storing the memory address, and
value will be the offset of that memory address. For a constant operand: reg
will be NOT_REG, and value will be the constant value.
*/
MemoryType get_memory_type(const char *operand) {
  MemoryType result = {UNKNOWN, NOT_REG, -1};
  result.reg = get_register_by_name(operand);
  if (result.reg != NOT_REG) {
    result.type = REG;
  } else {
    if (operand[0] == '$') {
      result.type = CONST;
      result.value = atoi(&operand[1]);
    } else if (strstr(operand, "(") && strstr(operand, ")")) {
      char str[5];
      if (operand[0] == '(') {
        sscanf(operand, "(%s)", str);
        result.value = 0;
      } else {
        sscanf(operand, "%d(%s", &result.value, str);
      }
      str[strlen(str) - 1] = '\0';
      result.reg = get_register_by_name(str);
      if (result.reg != NOT_REG) {
        result.type = MEM;
      }
    }
  }
  return result;
}

/*
This function takes a string that represnts a label in the instruction.
It returns the memory address of the next instruction
corresponding to the label in the system. It returns -1 if the label is not
found or the label does not come with . as the first character
*/
int get_addr_from_label(System *sys, const char *label) {
  if (label[0] != '.') {
    return -1;
  }
  for (int i = 0; i < sys->memory.num_instructions; i++) {
    if (strcmp(sys->memory.instruction[i], label) == 0) {
      return (i + 1) * 4;
    }
  }
  return -1;
}


/*
The execute_movl function validates and executes a movl instruction, ensuring
source and destination operands are of known and appropriate types, and then
performs the move operation if valid.

It will return Success if there is no error.
It will return INSTRUCTION_ERROR if src or dst is a undifined memory space. In
this case, the type of a MemoryType data will be UNKNOWN. It will return
INSTRUCTION_ERROR if dst is a constant value instead of a register or memory
address. It will return INSTRUCTION_ERROR if both src and dst are memory
addresses. It will return MEMORY_ERROR if there is a memory address from src or
dst that is an invalid memory address (less than 0, or greater than (MEMORY_SIZE
- 1) * 4).

If there is any error, all the system registers, memory, and
system status should remain unchanged.
Do not change EIP in this function.
HINT: you may use get_memory_type in this function.
*/
ExecResult execute_movl(System *sys, char *src, char *dst) {
  
  MemoryType memD = get_memory_type(dst);
  MemoryType memS = get_memory_type(src);

  if (memD.type == UNKNOWN || memS.type == UNKNOWN) 
  {
    return INSTRUCTION_ERROR;
  }
  if (memD.type == CONST) 
  {
    return INSTRUCTION_ERROR; 
  }
  if (memD.type == MEM && memS.type == MEM) {
    return INSTRUCTION_ERROR; 
  }
  
  int saddr;
  int daddr;
  if (memD.type == MEM) 
  {
    daddr = sys->registers[memD.reg] + memD.value;
    if (daddr < 0 || daddr > (MEMORY_SIZE - 1) * 4) 
    {
      return MEMORY_ERROR;
    }
  }
  if (memS.type == MEM) 
  {
    saddr = sys->registers[memS.reg] + memS.value;
    if (saddr < 0 || saddr > (MEMORY_SIZE - 1) * 4) 
    {
      return MEMORY_ERROR;
    }
  }

  int val = 0;  
  if (memS.type == REG) 
  {
    val = sys->registers[memS.reg]; 
  }
  if (memS.type == CONST) 
  {
    val = memS.value; 
  } 
  if (memS.type == MEM)
  {
    val = sys->memory.data[saddr / 4];
  }

  if (memD.type == REG)
  {
    sys->registers[memD.reg] = val;
  }
  if (memD.type == MEM)
  {
    sys->memory.data[daddr / 4] = val;
  }
  return SUCCESS;
}


/*
The execute_addl function validates and executes a addl instruction, ensuring
source and destination operands are of known and appropriate types, and then
performs the add operation if valid.

It will return Success if there is no error.
It will return INSTRUCTION_ERROR if src or dst is a undifined memory space. In
this case, the type of a MemoryType data will be UNKNOWN. It will return
INSTRUCTION_ERROR if dst is a constant value instead of a register or memory
address. It will return INSTRUCTION_ERROR if both src and dst are memory
addresses. It will return MEMORY_ERROR if there is a memory address from src or
dst that is an invalid memory address (less than 0, or
greater than (MEMORY_SIZE - 1) * 4).

If there is any error, all the system registers, memory, and
system status should remain unchanged.
Do not change EIP in this function.
HINT: you may use get_memory_type in this function.
*/
ExecResult execute_addl(System *sys, char *src, char *dst) 
{
  MemoryType memD = get_memory_type(dst);
  MemoryType memS = get_memory_type(src);
  
  if(memS.type == UNKNOWN || memD.type == UNKNOWN)
  {
    return INSTRUCTION_ERROR;
  }

  if (memD.type == CONST) 
  {
    return INSTRUCTION_ERROR;
  }

  if (memS.type == MEM && memD.type == MEM) 
  {
    return INSTRUCTION_ERROR;
  }

  int daddr;
  int saddr;
  if(memD.type == MEM)
  {
    daddr = sys->registers[memD.reg] + memD.value;
    if(daddr < 0 || daddr > (MEMORY_SIZE - 1) * 4)
    {
      return MEMORY_ERROR;
    }
  }
  if(memS.type == MEM)
  {
    saddr = sys->registers[memS.reg] + memS.value;
    if(saddr < 0 || saddr > (MEMORY_SIZE - 1) * 4)
    {
      return MEMORY_ERROR;
    }
  }

  int val = 0;
  if(memS.type == REG)
  {
    val = sys->registers[memS.reg];
  } 
  if (memS.type == CONST)
  {
    val = memS.value;
  } 
  if (memS.type == MEM)
  {
    val = sys->memory.data[saddr / 4];
  }

  if(memD.type == REG)
  {
    sys->registers[memD.reg] += val;
  }
  if (memD.type == MEM)
  {
    sys->memory.data[daddr / 4] += val;
  }
  return SUCCESS;
}

/*
The execute_push function validates and executes a pushl instruction, ensuring
source operands is of known and appropriate type, and then performs the push
operation if valid.

It will return Success if there is no error.
It will return INSTRUCTION_ERROR if src is a undifined memory space. In this
case, the type of a MemoryType data will be UNKNOWN. It will return MEMORY_ERROR
if the address stored in src is an invalid memory address (less than 0, or
greater than (MEMORY_SIZE - 1) * 4).
It will return MEMORY_ERROR if esp is
an invalid memory address: less than 4, greater than or equal to MEMORY_SIZE *
4).

If there is any error, all the system registers, memory, and
system status should remain unchanged.
Do not change EIP in this function.
HINT: you may use get_memory_type in this function.
*/
ExecResult execute_push(System *sys, char *src) {
  MemoryType memS = get_memory_type(src);
  if(memS.type == UNKNOWN)
  {
    return INSTRUCTION_ERROR;
  }
  int saddr;
  if(memS.type == MEM)
  {
    saddr = sys->registers[memS.reg] + memS.value;
    if(saddr < 0 || saddr > (MEMORY_SIZE - 1) * 4)
    {
      return MEMORY_ERROR;
    }
  }
  if(sys->registers[ESP] < 4 || sys->registers[ESP] >= MEMORY_SIZE * 4) 
  {
    return MEMORY_ERROR;
  }

  int val = 0;
  if(memS.type == REG)
  {
    val = sys->registers[memS.reg];
  } 
  if (memS.type == CONST)
  {
    val = memS.value;
  } 
  if (memS.type == MEM)
  {
    val = sys->memory.data[saddr / 4];
  }
  sys->registers[ESP] -= 4;
  sys->memory.data[sys->registers[ESP] / 4] = val;
  return SUCCESS;
}

/*
The execute_pop function validates and executes a popl instruction, ensuring the
destination operand is of known and appropriate type, and then performs the pop
operation if valid.

It will return SUCCESS if there is no error.
It will return INSTRUCTION_ERROR if dst is not a register or memory address.
It will return MEMORY_ERROR if dst is an invalid memory address: less than 0, or
greater than (MEMORY_SIZE - 1) * 4).
It will return MEMORY_ERROR if the
address stored in esp is an invalid memory address: less than 0, or
greater than (MEMORY_SIZE - 1) * 4).

If there is any error, all the system registers, memory, and
system status should remain unchanged.
Do not change EIP in this function.
HINT: you may use get_memory_type in this function.
*/
ExecResult execute_pop(System *sys, char *dst) 
{
  MemoryType memD = get_memory_type(dst);
  if(memD.type != MEM && memD.type != REG)
  {
    return INSTRUCTION_ERROR;
  }
  int daddr;
  if(memD.type == MEM){
    int daddr = sys->registers[memD.reg] + memD.value;
    if(daddr < 0 || daddr > (MEMORY_SIZE - 1) * 4)
    {
      return MEMORY_ERROR;
    }
  }
  if(sys->registers[ESP] < 4 || sys->registers[ESP] >= MEMORY_SIZE * 4) 
  {
    return MEMORY_ERROR;
  }
  
  int val = sys->memory.data[sys->registers[ESP] / 4];
  if(memD.type == REG)
  {
    sys->registers[memD.reg] = val;
  } 
  else if (memD.type == MEM)
  {
    daddr = sys->registers[memD.reg] + memD.value;
    sys->memory.data[daddr / 4] = val;
  }
  sys->registers[ESP] += 4;
  return SUCCESS;
}

/*
The execute_cmpl function validates and executes a cmpl instruction, ensuring
the source and destination operands are of known and appropriate types, and then
performs the compare operation and update comparison_flag in the system if
valid.

It will return SUCCESS if there is no error.
It will return INSTRUCTION_ERROR if src or dst is an undefined memory space.
It will return INSTRUCTION_ERROR if both src and dst are memory addresses.
It will return MEMORY_ERROR if there is a memory address from src or dst that is
an invalid memory address (less than 0, or
greater than (MEMORY_SIZE - 1) * 4).

If there is any error, all the system registers, memory, and
system status should remain unchanged. You can decide the value of
comparison_flag for each comparison result.
Do not change EIP in this function.
HINT: you may use get_memory_type in this function.
*/
ExecResult execute_cmpl(System *sys, char *src, char *dst) 
{
  MemoryType memS = get_memory_type(src);
  MemoryType memD = get_memory_type(dst);

  if(memS.type == UNKNOWN || memD.type == UNKNOWN)
  {
    return INSTRUCTION_ERROR;
  }

  if (memS.type == MEM && memD.type == MEM) 
  {
    return INSTRUCTION_ERROR;
  }

  int daddr;
  int saddr;
  if(memD.type == MEM)
  {
    daddr = sys->registers[memD.reg] + memD.value;
    if(daddr < 0 || daddr > (MEMORY_SIZE - 1) * 4)
    {
      return MEMORY_ERROR;
    }
  }
  if(memS.type == MEM)
  {
    saddr = sys->registers[memS.reg] + memS.value;
    if(saddr < 0 || saddr > (MEMORY_SIZE - 1) * 4)
    {
      return MEMORY_ERROR;
    }
  }

  int valS = 0;
  int valD = 0;
  if(memD.type == REG)
  {
    valD = sys->registers[memD.reg];
  } 
  if (memD.type == CONST)
  {
    valD = memD.value;
  } 
  if (memD.type == MEM)
  {
    valD = sys->memory.data[daddr / 4];
  }

  if(memS.type == REG)
  {
    valS = sys->registers[memS.reg];
  } 
  if (memS.type == CONST)
  {
    valS = memS.value;
  } 
  if (memS.type == MEM)
  {
    valS = sys->memory.data[saddr / 4];
  }

  int val = 0;
  if(valS < valD)
  {
    val = -1;
  } 
  else if(valD == valS)
  {
    val = 0;
  } 
  else 
  {
    val = 1;
  }

  sys->comparison_flag = val;

  return SUCCESS;
}

/*
The execute_jmp function validates and executes a condition or direct jump
instruction, ensuring the destination operands is of known label, and then
performs the direct jump operation, or condition jump if condition is met.
A valid condition argument should be one of the following strings: "JE", "JNE",
"JL", "JG", or "JMP"

It will return SUCCESS if the jump is executed successfully no matter whether
condition is met. It will return PC_ERROR if the destination label cannot be
found in the instruction segment in the system.

If there is any error, all the system registers (except for EIP), memory, and
system status should remain unchanged.
Please update program counter (EIP) in this function.
HINT: you may use get_addr_from_label in this function.
*/
ExecResult execute_jmp(System *sys, char *condition, char *dst) {
  int addr = get_addr_from_label(sys, dst);
  if(addr == -1)
  {
    return PC_ERROR;
  }

  int val = 0;
  if(strcmp(condition, "JMP") == 0)
  {
    sys->registers[EIP] = addr;
    val = 1;
  }
  if(strcmp(condition, "JE") == 0)
  {
    if(sys->comparison_flag == 0)
    {
      sys->registers[EIP] = addr;
      val = 1;
    }
  }
  if(strcmp(condition, "JNE") == 0)
  {
    if(sys->comparison_flag != 0)
    {
      sys->registers[EIP] = addr;
      val = 1;
    }
  }
  if(strcmp(condition, "JL") == 0)
  {
    if(sys->comparison_flag == 1)
    {
      sys->registers[EIP] = addr;
      val = 1;
    }
  }
  if(strcmp(condition, "JG") == 0)
  {
    if(sys->comparison_flag == -1)
    {
      sys->registers[EIP] = addr;
      val = 1;
    }
  }
  
  if(val == 0)
  {
    sys->registers[EIP] += 4;
  }
  return SUCCESS;
}

/*
The execute_call function validates and executes a call instruction, ensuring
the destination operand is a known label, and then performs the call operation.

It will return SUCCESS if the call is executed successfully.
It will return PC_ERROR if the destination label cannot be found in the
instruction segment in the system.

If there is any error, all the system registers (except for EIP), memory, and
system status should remain unchanged.
Please update program counter (EIP) in this function.
HINT: you may use get_addr_from_label in this function.
*/
ExecResult execute_call(System *sys, char *dst) 
{
  int addr = get_addr_from_label(sys, dst);
  sys->registers[EIP] += 4;
  if (addr == -1)
  {
    return PC_ERROR;
  }
  else
  {
    execute_push(sys, "%EIP");
    sys->registers[EIP] = addr;
  }
  return SUCCESS; 
}

/*
The execute_ret function validates and executes a return instruction, which pops
the return address from the stack and update EIP (program counter).

It will return SUCCESS if the return is executed successfully.
It will return PC_ERROR if the return address is invalid (less than 0 or greater
than or equal to the number of instructions).

If there is any error, all the system registers (except for EIP and ESP),
memory, and system status should remain unchanged.
Please update program counter (EIP) in this function.
*/
ExecResult execute_ret(System *sys) 
{
  int esp = sys->registers[ESP];
  int addr = sys->memory.data[esp / 4];
  if(addr < 0 || addr / 4 >= sys->memory.num_instructions)
  {
    sys->registers[EIP] += 4;
    return PC_ERROR;
  }
  execute_pop(sys, "%EIP");
  sys->registers[EIP] = addr;
  return SUCCESS;
}

/*
Utilizing the EIP register's value (also known as the program counter), the
function fetches instructions from the instruction segment in system memory. It
then executes each instruction, which can be one of MOVL, ADDL PUSHL, POPL,
CMPL, CALL, RET, JMP, JNE, JE, JL, or JG, by employing the corresponding execute
functions. This process continues until the program encounters any Error status
or the END instruction. During the execution, it will ignore all the
instructions that are not listed above and continue to the next one.
Please update program counter (EIP) for MOVL, ADDL, PUSHL, POPL, and CMPL in
this function.
*/
void execute_instructions(System *sys) 
{
  char inst[256];
  int end = 0;
  
  while(end == 0)
  {
    strcpy(inst, sys->memory.instruction[sys->registers[EIP] / 4]);
    char* exec;
    char* src;
    char* dst;
    char* temp = &inst[0];
    char* tok = strtok(temp, " ");
    int num = 1; 
    while (tok != NULL)
    {
      switch(num)
      {
        case 1:
          exec = tok;
          break;
        case 2:
          src = tok;
          break;
        case 3:
          dst = tok;
      }
      tok = strtok(NULL, " ");
      num++;
    }
    //int val = sscanf(inst, "%s %s %s", exec, src, dst);
    ExecResult outcome;
    
    if (strcmp(exec, "MOVL") == 0) 
    {
      outcome = execute_movl(sys, src, dst);
      sys->registers[EIP] += 4;
    } 
    else if (strcmp(exec, "ADDL") == 0) 
    {
      outcome = execute_addl(sys, src, dst);
      sys->registers[EIP] += 4; 
    } 
    else if (strcmp(exec, "PUSHL") == 0) 
    {
      outcome = execute_push(sys, src);
      sys->registers[EIP] += 4;
    } 
    else if (strcmp(exec, "POPL") == 0) 
    {
      outcome = execute_pop(sys, src);
      sys->registers[EIP] += 4;
    } 
    else if (strcmp(exec, "CMPL") == 0) 
    {
      outcome = execute_cmpl(sys, src, dst);
      sys->registers[EIP] += 4;
    } 
    else if (strcmp(exec, "JMP") == 0) 
    {
      outcome = execute_jmp(sys, "JMP", src);
    } 
    else if (strcmp(exec, "JE") == 0) 
    {
      outcome = execute_jmp(sys, "JE", src);
    } 
    else if (strcmp(exec, "JNE") == 0) 
    {
      outcome = execute_jmp(sys, "JNE", src);
    } 
    else if (strcmp(exec, "JL") == 0) 
    {
      outcome = execute_jmp(sys, "JL", src);
    } 
    else if (strcmp(exec, "JG") == 0) 
    {
      outcome = execute_jmp(sys, "JG", src);
    } 
    else if (strcmp(exec, "CALL") == 0) 
    {
      outcome = execute_call(sys, src);
    } 
    else if (strcmp(exec, "RET") == 0) 
    {
      outcome = execute_ret(sys);
    }
    else if (strcmp(exec, "END") == 0)
    {
      end = -1;
    } 
    else 
    {
      outcome = SUCCESS;
      sys->registers[EIP] += 4;
    }
    if (outcome != SUCCESS)
    {
      end = -1;
    }
  }
}
