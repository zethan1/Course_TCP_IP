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
		printf("WSAStartup 错误");
		return -1;
	}

	sockListen=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (INVALID_SOCKET==sockListen)
	{
		printf("socket 错误");
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
		printf("bind 错误");
		closesocket(sockListen);
		WSACleanup();
		return -3;
	}

	ret=listen(sockListen,1);
	if (SOCKET_ERROR==ret)
	{
		printf("listen 错误");//可以用WSAGetLastError()可以看错误码出错的原因是什么
		closesocket(sockListen);
		WSACleanup();
		return -4;
	}

	printf("服务器启动成功，正在监听...\n\n");

	struct sockaddr_in addrClient;
	len=sizeof(sockaddr);
	sockTongxun=accept(sockListen,(sockaddr*)&addrClient,&len);
	//将(sockaddr*)&addrClient改为NULL，即不用这个值；&len也可以写成0；同样可以运行
	//sockaddr*)&addrClient 是对方的地址信息
	/*
	// 可以用getpeername来获得地址
	// getpeername(sockTongxun, (sockaddr*)&addrClient, &len);
	// 获得ip地址：inet_ntoa(addrClient.sin_addr)；获得客户端端口：ntohs(addrClient.sin_port)
	*/
	
	//getsockname()获得本机的ip和端口
	
	if (INVALID_SOCKET==sockTongxun)
	{
		printf("accept 错误");
		closesocket(sockListen);
		WSACleanup();
		return -5;
	}

	printf("接收一个客户端连接，下面开始通信\n\n");

	memset(sendBuf,0,sizeof(sendBuf));  //好习惯，初始化数据清零，不然可能出现乱码

	strcpy(sendBuf,"Welcome to tcpserver");
	ret=send(sockTongxun,sendBuf,strlen(sendBuf)+1,0);
	if (SOCKET_ERROR==ret)
	{
		printf("send 错误");
		closesocket(sockListen);
		closesocket(sockTongxun);
		WSACleanup();
		return -6;
	}

	printf("向客户端成功发送以下信息:\n%s\n\n",sendBuf);

	memset(recvBuf,0,sizeof(recvBuf));
	ret=recv(sockTongxun,recvBuf,sizeof(recvBuf),0);
	if (SOCKET_ERROR==ret)
	{
		printf("recv 错误");
		closesocket(sockListen);
		closesocket(sockTongxun);
		WSACleanup();
		return -7;
	}

	printf("从客户端收到以下信息:\n%s\n\n",recvBuf);

	closesocket(sockListen);
	closesocket(sockTongxun);
	WSACleanup();
	system("pause");
	return 0;
}
