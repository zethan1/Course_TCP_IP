#include "stdio.h"
#define SIZE 4
struct student_type
{
	char name[10];
	int num;
	int age;
	char addr[15];
}stud[SIZE];

int save(void);

void main(void)
{
	int i;
	/*int len=sizeof(struct student_type);
	printf("len=%d\n",len);
	return;
	*/

	for (i=0;i<SIZE;i++)
		scanf("%s%d%d%s",stud[i].name,&stud[i].num,&stud[i].age,stud[i].addr);
	save();
}

int save(void)
{
	FILE *fp;
	int i;
	size_t ret;
	
	fp=fopen("stu_list","wb");
	if (NULL==fp)
	{
		printf("cannot open file\n");
		return -1;
	}

	
	for (i=0;i<SIZE;i++)
	{
		ret=fwrite(&stud[i],sizeof(struct student_type),1,fp);
		if (ret!=1) 
		{
			printf("file write error\n");
		    return -2;
		}
	}

	fclose(fp);
	return 0;
}