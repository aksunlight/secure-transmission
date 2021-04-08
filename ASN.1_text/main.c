#include <stdio.h>
#include "teacher.h"
#include <string.h>
#include <stdlib.h>

int main()
{
	Teacher tea;
	memset(&tea, 0x00, sizeof(Teacher));
	strcpy(tea.name, "李明");
	tea.age = 20;
	tea.p = (char*)malloc(100);
	strcpy(tea.p, "我是初一（2）班的数学老师");
	tea.plen = strlen(tea.p);

	// 编码
	char* outData;
	int outlen;
	encodeTeacher(&tea, &outData, &outlen);

	// 解码
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