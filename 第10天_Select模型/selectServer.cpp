#include "stdio.h"
#include "winsock2.h"
#pragma comment(lib, "ws2_32.lib")
#define SERVERIP "127.0.0.1"
#define SERVERPORT 6666         
#define BUFSIZE 512   

int main(void)
{
	int ret,len;
	SOCKET sockListen,sockTX; 
	fd_set fdsock;  //保存所有套接字的集合
	fd_set fdread;  //select要检测的可读套接字集合
	struct sockaddr_in addrServer,addrClient;
	char recvBuf[BUFSIZE];//定义用于接收客户端发来信息的缓区
	char sendBuf[] ="Connect succeed. Please send a message to me.\n"; //发给客户端的信息

	WSADATA wsaData;
	ret=WSAStartup(MAKEWORD(2,2),&wsaData);
	if(SOCKET_ERROR==ret)
	{
		printf("加载winsock.dll失败！\n");
		return -1;
	}

	sockListen = socket(AF_INET,SOCK_STREAM,0);
	if (INVALID_SOCKET==sockListen) 
	{
		printf("创建套接字失败！\n");
		WSACleanup();
		return -2;
	}

	len = sizeof(struct sockaddr_in);
	memset(&addrServer,0,len);
	addrServer.sin_addr.S_un.S_addr = inet_addr(SERVERIP);
	addrServer.sin_family =AF_INET;
	addrServer.sin_port = htons(SERVERPORT);

	ret=bind(sockListen,( struct sockaddr *)&addrServer,len);
	if(SOCKET_ERROR==ret)    
	{
		printf("地址绑定失败！\n");
		closesocket(sockListen);
		WSACleanup();
		return -3;
	}

	ret=listen(sockListen,3);
	if(SOCKET_ERROR==ret)      
	{
		printf("listen函数调用失败！\n");
		closesocket(sockListen);
		WSACleanup();
		return -4;
	}
	else
		printf("服务器启动成功,正在监听......\n"); 

	FD_ZERO(&fdsock);//初始化 fdsock 为空集合
	FD_SET(sockListen, &fdsock);//将 sockListen 加入到套接字集合 fdsock

	/***循环：接收连接请求并收发数据***/
	u_int i,count,msgCount=0;
	while(true)
	{
		if (msgCount>=20) break;//为了测试方便,最多接收20条消息
		FD_ZERO(&fdread);//初始化 fdread 为空集合
		fdread=fdsock;//将 fdsock 中的所有套接字添加到 fdread 中
		count=fdsock.fd_count;
		
		if(select(0, &fdread, NULL, NULL, NULL)==SOCKET_ERROR)
		{
			printf("Select调用失败！\n");
			break;//终止循环退出程序
		}

		for(i=0;i<count;i++)
		{
			if (FD_ISSET(fdsock.fd_array[i], &fdread)) 
			{
				if(fdsock.fd_array[i]==sockListen)
				{	//有客户连接请求到达, 接收连接请求
					len=sizeof(struct sockaddr);
					sockTX=accept (sockListen, (struct sockaddr *) &addrClient, &len);
					if(sockTX==INVALID_SOCKET) 
					{  
						printf("accept函数调用失败！\n");
						break;
					}
					else
					{
						printf("%s:%d连接成功！\n",inet_ntoa(addrClient.sin_addr),
							ntohs(addrClient.sin_port));
						send(sockTX,sendBuf,strlen(sendBuf)+1,0) ;//发送一条信息
						FD_SET(sockTX, &fdsock);//将新套接字加入fdsock
					}
				}
				else
				{	//有客户发来数据，接收数据
					memset( recvBuf,0, sizeof(recvBuf));//缓冲区清零
					ret=recv(fdsock.fd_array[i],recvBuf,sizeof(recvBuf),0);
					if(SOCKET_ERROR==ret) 
						printf("接收信息失败!\n");
					else
					{  //显示收到的信息
						len=sizeof(struct sockaddr);
						getpeername(fdsock.fd_array[i], 
							(struct sockaddr *)&addrClient, &len); //获取对方IP地址
						printf("%s:%d说: %s\n", inet_ntoa(addrClient.sin_addr),
							ntohs(addrClient.sin_port),recvBuf);
						msgCount++;
					}
				}
			}
		}
	}

	/***结束处理***/
	for(i=0;i<fdsock.fd_count; i++)
		closesocket (fdsock.fd_array[i]);//关闭所有套接字
	WSACleanup();//注销WinSock动态链接库
	system("pause");
	return 0;
}
