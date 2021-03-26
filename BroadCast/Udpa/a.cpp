#include "stdio.h"
#include "winsock2.h"
#pragma comment(lib,"ws2_32.lib")
#define IP "192.168.10.107"
#define PORT 9999
#define BROADCASTIP "255.255.255.255"


void main(void)
{
	int ret,len;
	WSADATA data;
	SOCKET sockMe;
	SOCKADDR_IN addrMe,addrBroadcast,addr;
	char sendBuf[256],recvBuf[256];

	ret=WSAStartup(MAKEWORD(2,2),&data);
	if (ret==SOCKET_ERROR)
	{
		printf("wsastartup error\n");
		return;
	}

	sockMe=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if (sockMe==INVALID_SOCKET)
	{
		printf("socket error,errcode=%d\n",WSAGetLastError());
		closesocket(sockMe);
		WSACleanup();
		return;
	}

	

	addrMe.sin_family=AF_INET;
	addrMe.sin_addr.S_un.S_addr=inet_addr(IP);
	addrMe.sin_port=htons(PORT);

	len=sizeof(sockaddr);
	ret=bind(sockMe,(sockaddr*)&addrMe,len);
	if (sockMe==SOCKET_ERROR)
	{
		printf("bind error,errcode=%d\n",WSAGetLastError());
		closesocket(sockMe);
		WSACleanup();
		return;
	}


	bool optval=true;
	setsockopt(sockMe,SOL_SOCKET,SO_BROADCAST,(char*)&optval,sizeof(bool)); //书P167

	printf("\n本地聊天程序启动成功，绑定到 %s:%d\n",inet_ntoa(addrMe.sin_addr),ntohs(addrMe.sin_port));

	addrBroadcast.sin_family=AF_INET;
	addrBroadcast.sin_addr.S_un.S_addr=inet_addr(BROADCASTIP);
	addrBroadcast.sin_port=htons(PORT);
	printf("\n准备发送广播消息,输入00退出");

	while (true)
	{
	printf("\n请输入聊天信息:\n");
	memset(sendBuf,0,sizeof(sendBuf));
	scanf("%s",sendBuf);
	if (strcmp(sendBuf,"00")==0) break;
	len=sizeof(sockaddr);


	ret=sendto(sockMe,sendBuf,strlen(sendBuf)+1,0,(sockaddr*)&addrBroadcast,len);
	if (sockMe==SOCKET_ERROR)
	{
		printf("sendto error,errcode=%d\n",WSAGetLastError());
		closesocket(sockMe);
		WSACleanup();
		return;
	}

	//printf("\n向对方发送以下信息成功:\n%s\n",sendBuf);
	printf("\n准备接收信息\n");

	memset(recvBuf,0,sizeof(recvBuf));
	len=sizeof(sockaddr);

	ret=recvfrom(sockMe,recvBuf,sizeof(recvBuf),0,(sockaddr*)&addr,&len);
	if (sockMe==SOCKET_ERROR)
	{
		printf("recvfrom error,errcode=%d\n",WSAGetLastError());
		closesocket(sockMe);
		WSACleanup();
		return;
	}

	printf("\n成功收到%s:%d发送的以下信息:\n%s\n",inet_ntoa(addr.sin_addr),ntohs(addr.sin_port),recvBuf);

	printf("\n");
	}
	closesocket(sockMe);  
	WSACleanup();
	system("pause");
}
