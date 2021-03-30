#include "stdio.h"
#include "string.h"

int main(void)
{
	char ch[4];
	short i;
	FILE *fp1,*fp2;
	fp1=fopen("file1.txt","w");
	fp2=fopen("file2.bin","wb");
	if (NULL==fp1 || NULL==fp2)
	{
		printf("can not create file\n");
		return -1;
	}

	memset(ch,0,sizeof(ch));
	strcpy(ch,"123");
	fwrite(ch,strlen(ch),1,fp1);
	
	
	i=123;
	fwrite(&i,sizeof(short),1,fp2);
	//fwrite(ch,strlen(ch),1,fp2);

	fclose(fp1);
	fclose(fp2);
	
	return 0;
}