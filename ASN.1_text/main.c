#include <stdio.h>
#include "teacher.h"
#include <string.h>
#include <stdlib.h>

int main()
{
	Teacher tea;
	memset(&tea, 0x00, sizeof(Teacher));
	strcpy(tea.name, "����");
	tea.age = 20;
	tea.p = (char*)malloc(100);
	strcpy(tea.p, "���ǳ�һ��2�������ѧ��ʦ");
	tea.plen = strlen(tea.p);

	// ����
	char* outData;
	int outlen;
	encodeTeacher(&tea, &outData, &outlen);

	// ����
	Teacher* pt;
	decodeTeacher(outData, outlen, &pt);
	printf("name:	%s\n", pt->name);
	printf("age:	%d\n", pt->age);
	printf("p:	%s\n", pt->p);
	printf("plen:	%d\n", pt->plen);

	freeTeacher(&pt);

	system("pause");

	return 0;
}