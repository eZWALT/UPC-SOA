/*
 * libc.c 
 */

#include <libc.h>
#include <types.h>

int errno;

void itodeca(int a, char *b)
{
  int i, i1;
  char c;
  
  if (a==0) { b[0]='0'; b[1]=0; return ;}
  
  i=0;
  while (a>0)
  {
    b[i]=(a%10)+'0';
    a=a/10;
    i++;
  }
  
  for (i1=0; i1<i/2; i1++)
  {
    c=b[i1];
    b[i1]=b[i-i1-1];
    b[i-i1-1]=c;
  }
  b[i]=0;
}

void itohexa(int a, char* b)
{
  int i, r;
  char c;
  
  if (a == 0) {b[0] = '0'; b[1]=0; return ;}
  
  i = 0;
  while (a > 0)
  {
    r = a % 16;
    a = a / 16;
    
    if (r < 10) b[i] = '0' + r;
    else 		b[i] = 'A' + r - 10;
    
    ++i; 
  }
  
  int i1;
  for (i1=0; i1<i/2; i1++)
  {
    c=b[i1];
    b[i1]=b[i-i1-1];
    b[i-i1-1]=c;  
  }
  b[i] = 0;
}

/* Append SRC to the end of DEST */
void strcat(char * dest, char * src)
{
  char * destPtr = dest + strlen(dest);
  while (*destPtr++ = *src++);
}

int strlen(char *a)
{
  int i;
  
  i=0;
  
  while (a[i]!=0) i++;
  
  return i;
}
