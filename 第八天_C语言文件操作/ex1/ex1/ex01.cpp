#include "stdio.h"
int main(void)
{
	FILE *fp;
	char ch,filename[10];

	printf("请输入文件名:");
	scanf("%s",filename);
	fp=fopen(filename,"w");
	if (NULL==fp)
	{
		printf("cannot open file\n");
		return -1;
	}

	ch=getchar();//接收执行scanf语句时最后输入的回车符
	ch=getchar();//接收输入的第一个字符
	while (ch!='#')
	{
      fputc(ch,fp);
	  putchar(ch);
	  ch=getchar();
	}

	fclose(fp);
	return 0;
}
