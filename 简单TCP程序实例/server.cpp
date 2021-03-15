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
	SOCKET sockListen,sockTongxun;

	ret=WSAStartup(MAKEWORD(2,2),&data);
	if (SOCKET_ERROR==ret)
	{
		printf("WSAStartup ����");
		return -1;
	}

	sockListen=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (INVALID_SOCKET==sockListen)
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
	ret=bind(sockListen,(sockaddr*)&addrServer,len);
	if (SOCKET_ERROR==ret)
	{
		printf("bind ����");
		closesocket(sockListen);
		WSACleanup();
		return -3;
	}

	ret=listen(sockListen,1);
	if (SOCKET_ERROR==ret)
	{
		printf("listen ����");
		closesocket(sockListen);
		WSACleanup();
		return -4;
	}

	printf("�����������ɹ������ڼ���...\n\n");

	struct sockaddr_in addrClient;
	len=sizeof(sockaddr);
	sockTongxun=accept(sockListen,(sockaddr*)&addrClient,&len);
	if (INVALID_SOCKET==sockTongxun)
	{
		printf("accept ����");
		closesocket(sockListen);
		WSACleanup();
		return -5;
	}

	printf("����һ���ͻ������ӣ����濪ʼͨ��\n\n");

	memset(sendBuf,0,sizeof(sendBuf));

	strcpy(sendBuf,"Welcome to tcpserver");
	ret=send(sockTongxun,sendBuf,strlen(sendBuf)+1,0);
	if (SOCKET_ERROR==ret)
	{
		printf("send ����");
		closesocket(sockListen);
		closesocket(sockTongxun);
		WSACleanup();
		return -6;
	}

	printf("��ͻ��˳ɹ�����������Ϣ:\n%s\n\n",sendBuf);

	memset(recvBuf,0,sizeof(recvBuf));
	ret=recv(sockTongxun,recvBuf,sizeof(recvBuf),0);
	if (SOCKET_ERROR==ret)
	{
		printf("recv ����");
		closesocket(sockListen);
		closesocket(sockTongxun);
		WSACleanup();
		return -7;
	}

	printf("�ӿͻ����յ�������Ϣ:\n%s\n\n",recvBuf);

	closesocket(sockListen);
	closesocket(sockTongxun);
	WSACleanup();
	system("pause");
	return 0;
}