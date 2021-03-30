#include "stdio.h"
void main(void)
{
	FILE *fp1,*fp2;
	fp1=fopen("file1.txt","r");
	fp2=fopen("file2.txt","w");
	while (!feof(fp1)) putchar(getc(fp1));

	rewind(fp1);
	
	while(!feof(fp1)) putc(getc(fp1),fp2);
	
	fclose(fp1);
	fclose(fp2);
}