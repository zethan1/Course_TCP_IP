#include "stdio.h"
#include "winsock2.h"
#pragma comment(lib, "ws2_32.lib")
#define SERVERIP "127.0.0.1"
#define SERVERPORT 6666         

int main(void)
{
	WSADATA data;
	SOCKET sockClient;  
	struct sockaddr_in addrServer,addrClient; 
	int ret,len; 
	char sendBuf[512],recvBuf[512]; 

	ret=WSAStartup(MAKEWORD(2,2),&data);
	if(SOCKET_ERROR==ret)
	{
		printf("加载winsock.dll失败！\n");
		return -1;
	}

	sockClient= socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (INVALID_SOCKET==sockClient) 
	{
		printf("创建套接字失败！\n");
		WSACleanup();
		return -2;
	}


	memset(&addrServer,0,sizeof(addrServer));
	addrServer.sin_addr.S_un.S_addr=inet_addr(SERVERIP);
	addrServer.sin_family=AF_INET;
	addrServer.sin_port=htons(SERVERPORT);

	len=sizeof(struct sockaddr);
	ret=connect(sockClient,(struct sockaddr *)&addrServer,len);

	if(SOCKET_ERROR==ret)    
	{
		printf("连接失败！错误代码:%d\n",WSAGetLastError());
		closesocket(sockClient);
		WSACleanup();
		return -3;
	}

	len=sizeof(struct sockaddr);
	ret=getsockname(sockClient,(struct sockaddr*)&addrClient,&len);
	if(SOCKET_ERROR!=ret)
	{
		printf("连接服务器成功,本地地址为 %s:%d\n",
			inet_ntoa(addrClient.sin_addr),ntohs(addrClient.sin_port));
	}

	memset(recvBuf,0,sizeof(recvBuf));
	ret=recv(sockClient,recvBuf,sizeof(recvBuf),0);
	if(SOCKET_ERROR==ret)
	{
		printf("接收信息失败！错误代码: %d\n",WSAGetLastError());
		closesocket(sockClient);
		WSACleanup(); 
		return -4;
	}
	else
		printf("收到服务器发来的信息: %s\n",recvBuf); 

	while (1)
	{
		printf("请输入发给服务器的信息:\n");
		memset(sendBuf,0,sizeof(sendBuf));
		gets(sendBuf);//scanf("%s",sendBuf);
		if ( strcmp(sendBuf,"q")==0) break;

		ret=send(sockClient,sendBuf,strlen(sendBuf)+1,0);
		if(SOCKET_ERROR==ret)
		{
			printf("发送信息失败！错误代码: %d\n",WSAGetLastError());
			break;
		}
		else
			printf("信息发送成功！\n");

	}

	closesocket(sockClient); 
	WSACleanup(); 
	system("pause");
	return 0;
}

