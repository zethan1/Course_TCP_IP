#include "stdio.h"
#include "winsock2.h"
#pragma comment(lib,"ws2_32.lib")
#define SERVERIP    "127.0.0.1"
#define SERVERPORT  6666
int main(void)
{
	int ret;
	char sendBuf[512],recvBuf[512];
	WSADATA data;
	SOCKET sockClient;

	ret=WSAStartup(MAKEWORD(2,2),&data);
	if (SOCKET_ERROR==ret)
	{
		printf("WSAStartup ����");
		return -1;
	}

	sockClient=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (INVALID_SOCKET==sockClient)
	{
		printf("socket ����");
		WSACleanup();
		return -2;
	}

	struct sockaddr_in addrServer;
	int len;
	addrServer.sin_addr.S_un.S_addr=inet_addr(SERVERIP);
	addrServer.sin_family=AF_INET;
	addrServer.sin_port=htons(SERVERPORT);
	len=sizeof(sockaddr);

	printf("�ͻ��˳�ʼ����ɣ��������ӷ�����\n\n");
	ret=connect(sockClient,(sockaddr*)&addrServer,len);
	if (SOCKET_ERROR==ret)
	{
		printf("connect ����");
		closesocket(sockClient);
		WSACleanup();
		return -3;
	}

	printf("���ӷ������ɹ������濪ʼͨ��...\n\n");

	memset(recvBuf,0,sizeof(recvBuf));
	ret=recv(sockClient,recvBuf,sizeof(recvBuf),0);
	if (SOCKET_ERROR==ret)
	{
		printf("recv ����");
		closesocket(sockClient);
		WSACleanup();
		return -4;
	}

	printf("�ӷ������յ�������Ϣ:\n%s\n\n",recvBuf);

	memset(sendBuf,0,sizeof(sendBuf));
	strcpy(sendBuf,"I am �ͻ���");
	ret=send(sockClient,sendBuf,strlen(sendBuf)+1,0);
	if (SOCKET_ERROR==ret)
	{
		printf("send ����");
		closesocket(sockClient);
		WSACleanup();
		return -5;
	}

	printf("�ɹ������������������Ϣ:\n%s\n\n",sendBuf);

	closesocket(sockClient);
	WSACleanup();
	system("pause");
	return 0;
}