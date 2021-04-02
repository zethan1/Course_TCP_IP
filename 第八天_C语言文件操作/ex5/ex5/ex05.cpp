//文件的大小至少是4kb，为了对齐字节。
#include "stdio.h"
struct student_type
{
	char name[10];
	int num;
	int age;
	char addr[15];
}stud;

int main(void)
{
	FILE *fp;
	fp=fopen("stu_list","rb");
	if (NULL==fp)
	{
		printf("can not open fiel\n");
		return -1;
	}

	fseek(fp,sizeof(struct student_type),SEEK_SET);
	fread(&stud,sizeof(struct student_type),1,fp);
	printf("%s %d %d %s\n",stud.name,stud.num,stud.age,stud.addr);

/*	int len;
	fseek(fp,0,SEEK_END);
	len=ftell(fp);
	printf("ÎÄ¼þ´óÐ¡Îª %d ×Ö½Ú\n",len);*/

	fclose(fp);
}
