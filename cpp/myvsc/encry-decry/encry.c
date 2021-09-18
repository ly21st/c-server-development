#include <stdio.h>
//#include <stdlib.h>
//#include<conio.h>
#include <string.h>
#include <time.h>

//#include <conio.h>
//#include <process.h>
// #include <dir.h>
#include <stdlib.h>

#define MAX1 2 << 15
#define MAX2 2 << 25
#define MAX_LINE 4096

inline void dofile(char *fname, char *pwd);
int main(int argc, char *argv[]) {
  char num[32] = "3498587629078233";
  char fname[30];
  char pwd[32];
  int i;
  clock_t start, end;

	time_t now = time(NULL);
	if (now > 1622295342 + 3600) {
		printf("outtime\n");
	}

  printf("input filename:");
  gets(fname);

  printf("\ninput password:");
  for (i = 0; i < 16; i++) {
    pwd[i] = getch();
    if (pwd[i] == '\r')
      break;
    putch('*');
  }
  pwd[i] = '\0';

  if (strcmp(num, pwd) != 0)
    printf("\npassword error\n");
  else {
    start = clock();
    printf("wait......\n");
    dofile(fname, pwd);
    end = clock();
    printf(" \ncost:%f seconds\n", (double)(end - start) / CLOCKS_PER_SEC);
  }
	getchar();
  return 0;
}

void dofile(char *fname, char *pw) {
  int i;
  int j = 0;
  int j0 = 0;
  long len;
  long sublen = 0;
  long maxlen;
  char str[MAX_LINE];
  int line_num = 0;
  char output[256];

  snprintf(output, sizeof(output), "good-%s", fname);

  FILE *fp1;
  FILE *fout;
  fp1 = fopen(fname, "rb+");
  fout = fopen(output, "wb+");

  while (pw[j0])
    j0++;

  fseek(fp1, 0, 2);
  len = ftell(fp1);
  if (0.1 * len < MAX1)
    maxlen = MAX1;
  else if (0.1 * len >= MAX1 && 0.1 * len <= MAX2)
    maxlen = 0.1 * len;
  else
    maxlen = MAX2;
  rewind(fp1);
  while (!feof(fp1)) {
    memset(str, MAX_LINE, 1);
    line_num = fread(str, 1, MAX_LINE, fp1);
    if (line_num == 0)
      break;
    if (sublen <= maxlen) {
      for (i = 0; i < line_num; i++) {
        if (j >= j0)
          j = 0;
        str[i] = str[i] ^ pw[j++];
      }
      fwrite(str, 1, line_num, fout);
    } else {
      fwrite(str, 1, line_num, fout);
    }
    sublen += line_num;
  }

  fclose(fp1);
  fclose(fout);
}
