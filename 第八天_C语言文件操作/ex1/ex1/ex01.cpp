#include "stdio.h"
int main(void)
{
	FILE *fp;
	char ch,filename[10];

	printf("�������ļ���:");
	scanf("%s",filename);
	fp=fopen(filename,"w");
	if (NULL==fp)
	{
		printf("cannot open file\n");
		return -1;
	}

	ch=getchar();//����ִ��scanf���ʱ�������Ļس���
	ch=getchar();//��������ĵ�һ���ַ�
	while (ch!='#')
	{
      fputc(ch,fp);
	  putchar(ch);
	  ch=getchar();
	}

	fclose(fp);
	return 0;
}
