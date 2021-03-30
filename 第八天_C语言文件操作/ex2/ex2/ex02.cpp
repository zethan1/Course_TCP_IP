#include "stdio.h"
int main(void)
{
	FILE *in,*out;
	char ch,infile[50],outfile[50];
	
	printf("Enter the input file name:\n");
	scanf("%s",infile);

	printf("Enter the output file name:\n");
	scanf("%s",outfile);

	in=fopen(infile,"r");
	if (NULL==in)
	{
		printf("cannot open input file\n");
		return -1;
	}

	out=fopen(outfile,"w");
	if (NULL==out)
	{
		printf("cannot open output file\n");
		return -2;
	}

	while (!feof(in)) 
		{
			ch=fgetc(in);
			fputc(ch,out);
		
		//ch=getc(in);
		//putc(ch,out);
		
	    }

	fclose(in);
	fclose(out);
	return 0;
}