#include "ShareMemory.h"

ShareMemory::ShareMemory(int key)
{
	m_shmID = getShmID(key, 0, 0);
}
ShareMemory::ShareMemory(int key, int size)
{
	m_shmID = getShmID(key, size, O_CREAT | O_EXCL);
}
ShareMemory::ShareMemory(const char* name)
{
	key_t key = ftok(name, RandX);
	m_shmID = getShmID(key, 0, 0);
}
ShareMemory::ShareMemory(const char* name, int size)
{
	key_t key = ftok(name, RandX);
	m_shmID = getShmID(key, size, O_CREAT | O_EXCL);
}
ShareMemory:: ~ShareMemory()
{
	
}
void* ShareMemory::mapShm()
{
	m_shmAddr = = shmat(m_shmID, NULL, 0);
	if (m_shmAddr == (void*)-1)
	{
		//��ӡ��־
		return NULL;
	}
	return m_shmAddr;
}
int unmapShm()
{
	int ret = shmdt(m_shmAddr);
	return ret;
}
int ShareMemory::delShm()
{
	int ret = shmctl(m_shmID, IPC_RMID, NULL);
	return ret;
}
int ShareMemory::getShmID(key_t key, int shmSize, int flag);
{
	int ret = shmget(key, shmsize, flag);
	if (ret < 0)
	{
		//��ӡ��־
		return -1��
	}
	return ret;
}