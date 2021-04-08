#ifndef _TEACHER_H
#define _TEACHER_H
typedef struct _Teacher
{
	char name[64]; 
	int age;   
	char *p;
	long plen;
}Teacher;

// 编码结构体
//p: 输入参数, 待编码的结构体
//outData: 输出参数, 保存编码之后的字符串
//outlen: 输出参数, 编码之后的字符串的长度
int encodeTeacher(Teacher* p, char** outData, int* outlen);
// 解码结构体

//inData: 输入参数
//inLen: inData的长度
//p:保存解码之后的数据
int decodeTeacher(char* inData, int inLen, Teacher**p);
// 释放内存函数

//释放内存
void freeTeacher(Teacher** p);

#endif	// _TEACHER_H
