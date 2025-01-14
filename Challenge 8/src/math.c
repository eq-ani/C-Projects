#include "math.h"


// goal: read equations from a file and write them in a solved state to another
// file
//   NOTE: format details specified in instructions
// param qfile: file with math questions to solve
// param afile: file to write equations with answers in
// example:
//   qfile at start of function:
//     12 + 13
//     24 / 5
//     8 * 234
//     65 - 78
//     239 % 13
//   afile after function:
//      12 +  13 = 25
//      24 /   5 = 4
//       8 * 234 = 1872
//      65 -  78 = -13
//     239 %  13 = 5
//
// TODO: Complete the function
void solve(const char *qfile, const char *afile)
{
  FILE *rfile = fopen(qfile, "r");
  FILE *wfile = fopen(afile, "w");
  if (wfile == NULL && rfile == NULL)
  {
    return;
  }
  else if (wfile == NULL)
  {
    fclose(rfile);
    return;
  }
  else if (rfile == NULL)
  {
    fclose(wfile);
    return; 
  }
  else
  {
    int num1;
    int num2;
    int eq;
    char operand;
    while(fscanf(rfile, "%d %c %d", &num1, &operand, &num2) > 0)
    {
        switch(operand)
        {
          case '+':
            eq = num1 + num2;
            break; 
          case '-':
            eq = num1 - num2;
            break;
          case '*':
            eq = num1 * num2;
            break;
          case '/':
            eq = num1 / num2;
            break;
          case '%':
            eq = num1 % num2;
            break;
          default:
            continue;
        }
      fprintf(wfile,"%3d %c %3d = %d\n", num1,operand,num2,eq); 
      }
    fclose(rfile);
    fclose(wfile);
  }
}
