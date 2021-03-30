#include "stdio.h"
#define SIZE 4
struct student_type
{
	char name[10];
	int num;
	int age;
	char addr[15];
}stud[SIZE];

void main(void)
{
	int i;
	FILE *fp;
	fp=fopen("stu_list","rb");
//	fread(stud,sizeof(struct student_type),SIZE,fp);
	
	for (i=0;i<SIZE;i++)
	{
		fread(&stud[i],sizeof(struct student_type),1,fp);
		printf("%-10s %4d %4d %-15s\n",
			stud[i].name,stud[i].num,stud[i].age,stud[i].addr);
	}

	fclose(fp);	
}

