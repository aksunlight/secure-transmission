#include <cstdio>
#include "ServerOperation.h"
#include <sys/ipc.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <iostream>
using namespace std;
int main()
{
	ServerInfo info;
	strcpy(info.serverID, "0001");
	info.maxnode = 10;
	info.shmkey = 0x12340000;
	strcpy(info.dbUse, "");
	strcpy(info.dbPasswd, "");
	strcpy(info.dbSID, "");
	info.sPort = 9898;

	ServerOperation server(&info);
	server.startWork();

    return 0;
}