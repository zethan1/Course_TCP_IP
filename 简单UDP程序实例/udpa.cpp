#include "stdio.h"
#include "winsock2.h"
#pragma comment(lib,"ws2_32.lib")
#define IPME "127.0.0.1"
#define PORTME 5678
#define IPTO "127.0.0.1"
#define PORTTO 8765

void main(void)
{
	int ret,len;
	WSADATA data;
	SOCKET sockMe;
	SOCKADDR_IN addrMe,addrTo,addr;
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
	addrMe.sin_addr.S_un.S_addr=inet_addr(IPME);
	addrMe.sin_port=htons(PORTME);

	len=sizeof(sockaddr);
	ret=bind(sockMe,(sockaddr*)&addrMe,len);
	if (sockMe==SOCKET_ERROR)
	{
		printf("bind error,errcode=%d\n",WSAGetLastError());
		closesocket(sockMe);
		WSACleanup();
		return;
	}

	printf("\n本地聊天程序启动成功，绑定到 %s:%d\n",inet_ntoa(addrMe.sin_addr),ntohs(addrMe.sin_port));

	addrTo.sin_family=AF_INET;
	addrTo.sin_addr.S_un.S_addr=inet_addr(IPTO);
	addrTo.sin_port=htons(PORTTO);
	printf("\n准备和 %s:%d 聊天\n",inet_ntoa(addrTo.sin_addr),ntohs(addrTo.sin_port));

	printf("\n请输入聊天信息:\n");
	memset(sendBuf,0,sizeof(sendBuf));
	scanf("%s",sendBuf);
	len=sizeof(sockaddr);


	ret=sendto(sockMe,sendBuf,strlen(sendBuf)+1,0,(sockaddr*)&addrTo,len);
	if (sockMe==SOCKET_ERROR)
	{
		printf("sendto error,errcode=%d\n",WSAGetLastError());
		closesocket(sockMe);
		WSACleanup();
		return;
	}

	printf("\n向对方发送以下信息成功:\n%s\n",sendBuf);
	printf("\n准备接收信息(不一定是对方发来的！)\n");

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
	closesocket(sockMe);  
	WSACleanup();
	system("pause");
}