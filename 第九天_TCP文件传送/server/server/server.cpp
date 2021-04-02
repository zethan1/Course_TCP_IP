//文件传输需要：文件名、文件扩展名、文件长度
//文件打不开可以用二进制的格式来打开编辑

#include "stdio.h"
#include "winsock2.h"
#pragma comment(lib,"ws2_32.lib")
#define SERVERIP "127.0.0.1"
#define SERVERPORT 3333
#define SIZE 2//4096	//方便调试的时候看字节，刚好两位
#define BUFSIZE 4096

int RecvFile(SOCKET sock);
int main(void)
{
	int ret,len;
	WSADATA data;
	SOCKET sockListen,sockTx;
	struct sockaddr_in addrServer;

	ret=WSAStartup(MAKEWORD(2,2),&data);
	if (SOCKET_ERROR==ret)
	{
		return -1;
	}

	sockListen=socket(AF_INET,SOCK_STREAM,0);
	if (INVALID_SOCKET==sockListen)
	{
		WSACleanup();
		return -2;
	}

	memset(&addrServer,0,sizeof(struct sockaddr_in));
	addrServer.sin_family=AF_INET;
	addrServer.sin_addr.S_un.S_addr=inet_addr(SERVERIP);
	addrServer.sin_port=htons(SERVERPORT);

	len=sizeof(struct sockaddr);
	ret=bind(sockListen,(struct sockaddr*)&addrServer,len);
	if (SOCKET_ERROR==ret)
	{
		closesocket(sockListen);
		WSACleanup();
		return -3;
	}

	ret=listen(sockListen,1);
	if (SOCKET_ERROR==ret)
	{
		closesocket(sockListen);
		WSACleanup();
		return -4;
	}

	len=sizeof(struct sockaddr);
	sockTx=accept(sockListen,NULL,&len);
	if (INVALID_SOCKET==sockTx)
	{
		closesocket(sockListen);
		WSACleanup();
		return -5;
	}

	printf("接收到客户端的连接\n");

	RecvFile(sockTx);

	closesocket(sockListen);
	closesocket(sockTx);
	WSACleanup();
	system("pause");
	return 0;
}

int RecvFile(SOCKET sock)
{
	int ret;
	FILE *fp;
	char sendBuf[BUFSIZE],recvBuf[BUFSIZE],filename[512];

	
	memset(recvBuf,0,sizeof(recvBuf));
	ret=recv(sock,recvBuf,sizeof(recvBuf),0);
	if (SOCKET_ERROR==ret)
	{
		return -1;
	}

	printf("成功接收客户端发来的文件名: %s\n",recvBuf);

	memset(sendBuf,0,sizeof(sendBuf));
	strcpy(sendBuf,recvBuf);
	ret=send(sock,sendBuf,strlen(sendBuf),0);
	if (SOCKET_ERROR==ret)
	{
		return -2;
	}

	printf("请输入你希望保存的文件名（带扩展名）\n");
	scanf("%s",filename);
    fp=fopen(filename,"wb");
	if (NULL==fp)
	{
		return -3;
	}

	memset(recvBuf,0,sizeof(recvBuf));
	ret=recv(sock,recvBuf,sizeof(recvBuf),0);
	if (SOCKET_ERROR==ret)
	{
		return -4;
	}

	printf("成功接收客户端发来的文件长度: %s\n",recvBuf);
	
	//int len;
	//len = atoi(recvBuf);	//得到接收长度
	//补充完整方法：
	//1、每次接收字节数累计，如果等于len，退出接收
	//2、
	
	memset(sendBuf,0,sizeof(sendBuf));
	strcpy(sendBuf,recvBuf);
	ret=send(sock,sendBuf,strlen(sendBuf),0);	//不是要显示2聊天信息，这里可以不+1
	if (SOCKET_ERROR==ret)
	{
		return -5;
	}

	//为了测试,这里只接收前SIZE字节
	ret=recv(sock,recvBuf,SIZE,0);
	if (SOCKET_ERROR==ret)
	{
		return -6;
	} 

	fwrite(recvBuf,ret,1,fp);
	fclose(fp);
	return 0;	
}
