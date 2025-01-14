#include <data.h>
#include <stdio.h>
#include <stdlib.h>

int convertCharToNumber(char ch) {
  if (ch >= '0' && ch <= '9') {
    return ch - '0';
  } else if (ch >= 'A' && ch <= 'F') {
    return ch - 'A' + 10;
  } else {
    return -1;
  }
}

char convertNumberToChar(int n) {
  if (n >= 0 && n <= 9) {
    return n + '0';
  } else if (n >= 10 && n <= 15) {
    return n - 10 + 'A';
  } else {
    return 0;
  }
}

Data convert_to_base_n(Data src, unsigned char n) 
{
  DigitalNumber *dat = src.data;
  Data new_data = {n, src.sign, src.number_bits, 0, NULL};
  int times = 1;
  int num = 0;
  while(dat != NULL)
  {
    num = num + convertCharToNumber(dat->number) * times;
    dat = dat->next;
    times = times * src.base;
  }
  if(num == 0)
  {
    DigitalNumber *zerocase =(DigitalNumber *)malloc(sizeof(DigitalNumber));
    zerocase->next = NULL;
    zerocase->number = convertNumberToChar(0);
    Data ret = {n, src.sign, src.number_bits, 1, zerocase};
    return ret;
  }
  dat = NULL;
  int size = 0;
  while(num > 0)
  {
    DigitalNumber *replace = (DigitalNumber *)malloc(sizeof(DigitalNumber));
    replace->next = NULL;
    replace->number = convertNumberToChar(num % n);
    if(dat == NULL)
    {
      new_data.data = replace;
    }
    else 
    {
      dat->next = replace;
    }
    num = num / n;
    dat = replace;
    size = size + 1;
  }
  new_data.len = size;
  return new_data;
}

int convert_to_int(Data src) 
{
  int num = 0;
  int times = 1;
  DigitalNumber *dat = src.data;
  for (int i = 0; i < src.len; i++)
  {
    num = num + convertCharToNumber(dat->number) * times;
    dat = dat->next;
    times = times * src.base;
  }
  if (src.sign == 1 && (num & (1 << (src.number_bits - 1))))
  {
    num = num - (1 << src.number_bits);
  }
  return num;
}

Data left_shift(Data src, int n) 
{
  DigitalNumber *zeroDN = (DigitalNumber*)malloc(sizeof(DigitalNumber));
  zeroDN->number = '0';
  Data zerodata = {2, src.sign, src.number_bits, 1, zeroDN};
  if (n >= src.number_bits)
  {
    return zerodata;
  }
  DigitalNumber *dat = NULL;
  Data new_data = convert_to_base_n(src, 2);
  DigitalNumber *head = new_data.data;
  for (int i = 0; i < n; i++)
  {
    DigitalNumber *n = (DigitalNumber*)malloc(sizeof(DigitalNumber));
    n->number = '0'; 
    n->next = NULL;
    if(i == 0)
    {
      new_data.data = n;
    }
    else
    {
      dat->next = n;
    }
    dat = n;
  }
  dat->next = head;
  new_data.len = new_data.len + n;
  if (new_data.len > new_data.number_bits)
  {
    dat = new_data.data;
    for (int i = 0; i < new_data.number_bits - 1; i++)
    {
      dat = dat->next;
    }
    dat->next = NULL;
    free(dat->next);
    new_data.len = new_data.number_bits;
  }
  int prev_num;
  DigitalNumber *prev = NULL;
  dat = new_data.data;
  
  while (dat != NULL)
  {
    if (dat->number != '0')
    {
      prev_num = 0;
      prev = dat;
    }
    dat = dat->next;
    prev_num = prev_num + 1;
  }
  if (prev != NULL)
  {
    new_data.len = (new_data.len - prev_num) + 1;
    prev->next = NULL;
    free(prev->next);
  }
  else
  {
    return zerodata;
  }
  return new_data;
}

Data right_shift(Data src, int n) 
{
  int count = 0;
  int num = convert_to_int(src);
  DigitalNumber *dat1;
  Data new_data = convert_to_base_n(src, 2);
  for (int i = 0; new_data.data != NULL && i < n; i++) 
  { 
    dat1 = new_data.data;
    new_data.data = dat1->next;
    count++;
  }
  new_data.len = new_data.len - count;
  if(src.sign == 1 && (num & (1 << (new_data.number_bits - 1)))) 
  {
    DigitalNumber *sigbit = new_data.data;
    while(sigbit->next != NULL)
    {
      sigbit = sigbit->next;
    }
    int count = 0;
    for(int i = 0; i < n && new_data.len < new_data.number_bits; i++)
    {
      DigitalNumber *dat2 = (DigitalNumber*)malloc(sizeof(DigitalNumber));
      dat2->number = '1';
      dat2->next = NULL;
      if (new_data.data == NULL) 
      {
        new_data.data = dat2;
        sigbit = dat2;
      }
      else 
      {
        sigbit->next = dat2;
        sigbit = dat2;
      }
      count++;
    }
    new_data.len = new_data.len + count;
  } 
  new_data = convert_to_base_n(new_data, 2);
	return new_data;
}

