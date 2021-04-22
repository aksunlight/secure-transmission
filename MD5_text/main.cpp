#define _CRT_SECURE_NO_WARNINGS
#include <openssl/md5.h>		// md5ͷ�ļ�
#include <openssl/hmac.h>		// hmacͷ�ļ�
#include <openssl/evp.h>        //evp_md5()ͷ�ļ�
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void getMD5(const char* str, char* result)
{
	MD5_CTX ctx;
	// ��ʼ��
	MD5_Init(&ctx);
	// �������
	MD5_Update(&ctx, str, strlen(str));
	// ������
	unsigned char md[16] = { 0 };
	MD5_Final(md, &ctx);
	for (int i = 0; i < 16; ++i)
	{
		sprintf(&result[i * 2], "%02x", md[i]);
	}
}

int main01()  //md5_test
{
	char result[64] = { 0 };
	getMD5("hello, md5", result);
	printf("md5 value: %s\n", result);
	system("pause");

	return 0;
}

void getHMAC(const char* str, const char* key, char* result)
{
	HMAC_CTX* ctx = HMAC_CTX_new();
	const EVP_MD* md = EVP_md5();
	HMAC_Init_ex(ctx, key, strlen(key), md, NULL);
	HMAC_Update(ctx, (const unsigned char*)str, strlen(str));
	unsigned char md2[16] = { 0 };
	HMAC_Final(ctx, md2, NULL);
	for (int i = 0; i < 16; ++i)
	{
		sprintf(&result[i * 2], "%02x", md2[i]);
	}
}

void getHMAC_SHA256(const char* str, const char* key, char* result)
{
	const EVP_MD* md = EVP_sha256();
	unsigned char md2[32] = { 0 };  //hmac-sha256ժҪ�㷨����ժҪ����Ϊ258λ��
	HMAC(md, key, strlen(key), (unsigned char*)str, strlen(str), md2, NULL);

	for (int i = 0; i < 32; ++i)
	{
		sprintf(&result[i * 2], "%02x", md2[i]);
	}

}

int main()  //ǩ���㷨����
{
	char result[256] = { 0 };
	getHMAC_SHA1("I an plaintext", "I am key", result);
	printf("hmac value: %s\n", result);
	system("pause");

	return 0;
}