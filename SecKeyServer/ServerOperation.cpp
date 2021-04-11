#include "ServerOperation.h"
#include <iostream>
#include <pthread.h>
#include <string.h>
#include "RequestFactory.h"
#include "RespondFactory.h"
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <signal.h>
using namespace std;

bool ServerOperation::m_stop = false;	// 静态变量初始化

ServerOperation::ServerOperation(ServerInfo * info)
{
	memcpy(&m_info, info, sizeof(ServerInfo));
	m_shm = new SecKeyShm(m_info.shmkey, m_info.maxnode);
}

ServerOperation::~ServerOperation()
{

}

void ServerOperation::startWork()
{
	//socket-setsockopt-bind-listen
	m_server.setListen(m_info.sPort);
	pthread_t threadID;
	while (1)
	{
		//accept新的客户端连接
		m_client = m_server.acceptConn();

		//创建一个子线程
		pthread_create(&threadID, NULL, working, this);
		//设置子线程为分离属性
		pthread_detach(threadID);
		m_listSocket.insert(make_pair(threadID, m_client));
	}
}

int ServerOperation::secKeyAgree(RequestMsg * reqMsg, char ** outData, int & outLen)
{
	//验证消息认证码
	char key[64];
	unsigned int len;
	unsigned char md[SHA256_DIGEST_LENGTH];
	char authCode[SHA256_DIGEST_LENGTH * 2 + 1] = {0};

	memset(key, 0x00, sizeof(key));
	sprintf(key, "@%s+%s@", reqMsg->serverId, reqMsg->clientId);
	HMAC(EVP_sha256(), key, strlen(key), (unsigned char *)reqMsg->r1, strlen(reqMsg->r1), md, &len);
	for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
		sprintf(&authCode[2 * i], "%02x", md[i]);
	}
	//将生成的消息认证码和客户端的r1的消息认证做比对
	if (strcmp(authCode, reqMsg->authCode) != 0)
	{
		cout << "消息认证码错误" << endl;
		return -1;
	}

	//生成随机字符串r2
	RespondMsg rspMsg;
	memset(&rspMsg, 0x00, sizeof(rspMsg));
	getRandString(sizeof(rspMsg.r2), rspMsg.r2);

	//将随机字符串r2和r1进行拼接, 然后生成秘钥
	char buf[64];
	char md[SHA_DIGEST_LENGTH];
	char seckey[SHA_DIGEST_LENGTH * 2 + 1];
	memset(buf, 0x00, sizeof(buf));
	memset(seckey, 0x00, sizeof(seckey));
	sprintf(buf, "%s%s", reqMsg->r1, rspMsg.r2);
	SHA1((unsigned char *)buf, strlen((char *)buf), md);
	for (int i = 0; i<SHA_DIGEST_LENGTH; i++)
	{
		sprintf(&seckey[i * 2], "%02x", md[i]);
	}

	//给应答结构体赋值
	rspMsg.seckeyid = 1;  //获取秘钥ID
	rspMsg.rv = 0;
	strcpy(rspMsg.serverId, m_info.serverID);
	strcpy(rspMsg.clientId, reqMsg->clientId);

	//将要发送给客户端的应答结构体进行编码
	int dataLen;
	char *sendData = NULL;
	CodecFactory *factory = new RespondFactory(&rspMsg);
	Codec *pCodec = factory->createCodec();
	pCodec->msgEncode(&sendData, dataLen);
	delete factory;
	delete pCodec;

	//发送数据给客户端
	pthread_t thread = pthread_self();
	TcpSocket* socket = m_listSocket[thread];
	socket->sendMsg(sendData, dataLen);
	free(sendData);

	//写秘钥信息到共享内存
	NodeSHMInfo node;
	memset(&node, 0x00, sizeof(NodeSHMInfo));
	node.status = 0;
	strcpy(node.seckey, seckey);
	strcpy(node.clientID, rspMsg.clientId);
	strcpy(node.serverID, m_info.serverID);
	node.seckeyID = rspMsg.seckeyid;

	//将秘钥信息写入共享内存
	m_shm->shmWrite(&node);

	//关闭连接
	socket->disConnect();

	return 0;
}



void ServerOperation::getRandString(int len, char * randBuf)
{
	int flag = -1;
	// 设置随机种子
	srand(time(NULL));
	// 随机字符串: A-Z, a-z, 0-9, 特殊字符(!@#$%^&*()_+=)
	char chars[] = "!@#$%^&*()_+=";
	for (int i = 0; i < len - 1; ++i)
	{
		flag = rand() % 4;
		switch (flag)
		{
		case 0:
			randBuf[i] = 'Z' - rand() % 26;
			break;
		case 1:
			randBuf[i] = 'z' - rand() % 26;
			break;
		case 3:
			randBuf[i] = rand() % 10 + '0';
			break;
		case 2:
			randBuf[i] = chars[rand() % strlen(chars)];
			break;
		default:
			break;
		}
	}
	randBuf[len - 1] = '\0';
}

// 友元函数, 可以在该友元函数中通过对应的类对象调用期私有成员函数或者私有变量
// 子线程 - 进行业务流程处理
void * working(void * arg)
{
	//接收数据
	pthread_t thread = pthread_self();
	ServerOperation *op = (ServerOperation *)arg;
	TcpSocket* socket = op->m_listSocket[thread];

	char *inData;
	int dataLen;
	socket->recvMsg(&inData, dataLen);

	//解码
	CodecFactory *factory = new RequestFactory();
	Codec *pCodec = factory->createCodec();
	RequestMsg *pMsg = (RequestMsg *)pCodec->msgDecode(inData, dataLen);
	delete factory;
	//delete pCodec;

	//判断clientID是否合法

	//判断客户端要请求什么服务
	char *outData;

	switch(pMsg->cmdType)
	{
	case RequestCodec::NewOrUpdate:
		op->secKeyAgree(pMsg, &outData, dataLen);
		break;

	case RequestCodec::Check:
		op->secKeyCheck();
		break;
	case RequestCodec::Revoke:
		op->secKeyRevoke();
		break;
	case RequestCodec::View:
		op->secKeyView();
		break;

	default:
		break;
	}
}
