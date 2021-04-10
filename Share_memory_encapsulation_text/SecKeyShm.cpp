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
	//�����Կ��Ϣ����
	for (i = 0; i < m_maxNode; ++i)
	{
		if (strcmp(pNodeInfo->clientID, phead[i].clientID) == 0
			&& strcmp(pNodeInfo->serverID, phead[i].serverID) == 0)
		{
			memcpy(&phead[i], pNodeInfo, sizeof(NodeSHMInfo));
			phead[i].status = 0;  //��Կ�Ѹ��£�δʹ��
			break;
		}
	}

	//�����Կ��Ϣ������
	if (i == m_maxNode)
	{
		NodeSHMInfo temp;
		memset(&temp, 0x00, sizeof(NodeSHMInfo));
		for (i = 0; i < m_maxNode; ++i)
		{
			if (memcpy(&temp, &phead[i], sizeof(NodeSHMInfo)) == 0)
			{
				memcpy(&phead[i], pNodeInfo, sizeof(NodeSHMInfo));
				phead[i].status = 0;  //��Կ��д�룬δʹ��
				break;
			}
		}
	}

	//��������ڴ�ռ䲻��
	unmapShm();
	if (i == m_maxNode)
	{
		//��ӡ��־
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
	//�����Կ��Ϣ����
	for (i = 0; i < m_maxNode; ++i)
	{
		if (strcmp(pNodeInfo->clientID, phead[i].clientID) == 0
			&& strcmp(pNodeInfo->serverID, phead[i].serverID) == 0)
		{
			memcpy(pNodeInfo, &phead[i], sizeof(NodeSHMInfo));
			pNodeInfo->status = 1;  //��Կ��ʹ��
			break;
		}
	}

	//�����Կ��Ϣ������
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

	//��ӡ��Կ��Ϣ
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