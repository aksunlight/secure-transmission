#include"SecKeyShm.h"

/*
class NodeSHMInfo
{
public:
	int status;
	int seckeyID;
	char clientID[12];
	char serverID[12];
	char seckey[128];
};
*/

SecKeyShm::SecKeyShm(int key):
	ShareMemory(key)
{
	
}
SecKeyShm::SecKeyShm(int key, int maxNode):
	ShareMemory(key, sizeof(int) + maxNode * sizeof(NodeSHMInfo)), m_maxNode(maxNode)
{
	void* shmhead = mapShm();

	memset(shmhead, 0x00, sizeof(int) + maxNode * sizeof(NodeSHMInfo));

	memcpy(shmhead, &m_maxNode, sizeof(int));

	unmapShm();
}
SecKeyShm::SecKeyShm(const char* pathName):
	ShareMemory(pathName)
{
	
}
SecKeyShm::SecKeyShm(const char* pathName, int maxNode):
	ShareMemory(pathName, sizeof(int) + maxNode * sizeof(NodeSHMInfo)), m_maxNode(maxNode)
{
	void* shmhead = mapShm();

	memset(shmhead, 0x00, sizeof(int) + maxNode * sizeof(NodeSHMInfo));

	memcpy(shmhead, &m_maxNode, sizeof(int));

	unmapShm();
}
SecKeyShm::~SecKeyShm()
{

}

int SecKeyShm::shmWrite(NodeSHMInfo* pNodeInfo)
{
	void* addr = mapShm();

	memcpy(&m_maxNode, addr, sizeof(int));

	NodeSHMInfo* phead = (NodeSHMInfo*)((char*)addr + sizeof(int));

	int i = 0;
	//如果密钥信息存在
	for (i = 0; i < m_maxNode; ++i)
	{
		if (strcmp(pNodeInfo->clientID, phead[i].clientID) == 0
			&& strcmp(pNodeInfo->serverID, phead[i].serverID) == 0)
		{
			memcpy(&phead[i], pNodeInfo, sizeof(NodeSHMInfo));
			phead[i].status = 0;  //密钥已更新，未使用
			break;
		}
	}

	//如果密钥信息不存在
	if (i == m_maxNode)
	{
		NodeSHMInfo temp;
		memset(&temp, 0x00, sizeof(NodeSHMInfo));
		for (i = 0; i < m_maxNode; ++i)
		{
			if (memcpy(&temp, &phead[i], sizeof(NodeSHMInfo)) == 0)
			{
				memcpy(&phead[i], pNodeInfo, sizeof(NodeSHMInfo));
				phead[i].status = 0;  //密钥已写入，未使用
				break;
			}
		}
	}

	//如果共享内存空间不够
	unmapShm();
	if (i == m_maxNode)
	{
		//打印日志
		std::cout << "no space to use!" << std::endl;
		return -1;
	}

	return 0;
}
int SecKeyShm::shmRead(const char* clientID, const char* serverID, NodeSHMInfo* pNodeInfo)
{
	void* addr = mapShm();

	memcpy(&m_maxNode, addr, sizeof(int));

	NodeSHMInfo* phead = (NodeSHMInfo*)((char*)addr + sizeof(int));

	int i = 0;
	//如果密钥信息存在
	for (i = 0; i < m_maxNode; ++i)
	{
		if (strcmp(pNodeInfo->clientID, phead[i].clientID) == 0
			&& strcmp(pNodeInfo->serverID, phead[i].serverID) == 0)
		{
			memcpy(pNodeInfo, &phead[i], sizeof(NodeSHMInfo));
			pNodeInfo->status = 1;  //密钥被使用
			break;
		}
	}

	//如果密钥信息不存在
	unmapShm();
	if (i == m_maxNode)
	{
		std::cout << "no find!" << std::endl;
		return -1;
	}
	
	return 0;
}
void SecKeyShm::printShm()
{
	void* addr = mapShm();

	memcpy(&m_maxNode, addr, sizeof(int));

	NodeSHMInfo* phead = (NodeSHMInfo*)((char*)addr + sizeof(int));

	//打印密钥信息
	for (int i = 0; i < m_maxNode; ++i)
	{
		std::cout << "No." << i << " key information" << std::endl;
		std::cout << phead[i].status << std::endl;
		std::cout << phead[i].seckeyID << std::endl;
		std::cout << phead[i].seckey << std::endl;
		std::cout << phead[i].clientID << std::endl;
		std::cout << phead[i].serverID << std::endl;
	}

	return;
}