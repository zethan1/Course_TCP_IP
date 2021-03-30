#include "stdio.h"
#include "stdlib.h"
#include "winsock2.h"
#pragma comment(lib,"ws2_32.lib")
#define SERVERIP "127.0.0.1"
#define SERVERPORT 3333
#define SIZE 2//4096
#define BUFSIZE 4096

int SendFile(SOCKET sock,char filename[512]);
int main(void)
{
	int ret,len;
	WSADATA data;
	SOCKET sockClient;
	struct sockaddr_in addrServer;
	char fname[512];

	ret=WSAStartup(MAKEWORD(2,2),&data);
	if (SOCKET_ERROR==ret)
	{
		return -1;
	}

	sockClient=socket(AF_INET,SOCK_STREAM,0);
	if (INVALID_SOCKET==sockClient)
	{
		WSACleanup();
		return -2;
	}

	memset(&addrServer,0,sizeof(struct sockaddr_in));
	addrServer.sin_family=AF_INET;
	addrServer.sin_addr.S_un.S_addr=inet_addr(SERVERIP);
	addrServer.sin_port=htons(SERVERPORT);

	len=sizeof(struct sockaddr);
	ret=connect(sockClient,(sockaddr*)&addrServer,len);
	if (SOCKET_ERROR==ret)
	{
		closesocket(sockClient);
		WSACleanup();
		return -3;
	}

	printf("连接服务器成功\n");

	printf("请输入你需要传送的文件名,带扩展名\n");
	scanf("%s",fname);
	SendFile(sockClient,fname);
	//SendFile(sockClient,"test.txt");
	
	closesocket(sockClient);
	WSACleanup();

	system("pause");
	return 0;
}

int SendFile(SOCKET sock,char filename[512])
{
	int ret,cnt,len1,lenlast;
	char sendBuf[BUFSIZE],recvBuf[BUFSIZE];
	FILE *fp;
	int totlen;
	fp=fopen(filename,"rb");
	if (NULL==fp)
	{
		printf("open file error\n");
		return -1;
	}

    fseek(fp,0,SEEK_END);
	totlen=ftell(fp);
	printf("文件%s的长度为%d字节\n",filename,totlen);

	memset(sendBuf,0,sizeof(sendBuf));
	strcpy(sendBuf,filename);

	ret=send(sock,sendBuf,strlen(sendBuf)+1,0);
	if (SOCKET_ERROR==ret)
	{
		return -2;
	}

	memset(recvBuf,0,sizeof(recvBuf));
	ret=recv(sock,recvBuf,sizeof(recvBuf),0);
	if (SOCKET_ERROR==ret|| (strcmp(sendBuf,recvBuf)!=0) )
	{
		return -3;
	} 

	printf("向服务器发送文件名成功\n");

	memset(sendBuf,0,sizeof(sendBuf));
	itoa(totlen,sendBuf,10);
	ret=send(sock,sendBuf,strlen(sendBuf)+1,0);
	if (SOCKET_ERROR==ret)
	{
		return -4;
	}

	memset(recvBuf,0,sizeof(recvBuf));
	ret=recv(sock,recvBuf,sizeof(recvBuf),0);
	if (SOCKET_ERROR==ret|| (strcmp(sendBuf,recvBuf)!=0) )
	{
		return -5;
	} 

	printf("向服务器发送文件长度成功\n");
	
	cnt=totlen/SIZE;
	len1=SIZE;
	if (totlen%SIZE)
	{
		lenlast=totlen-cnt*SIZE;
		cnt=cnt+1;
	}

	printf("\n\n文件长度为%d字节,\n每次传送%d字节,\n需要分%d次传送\n",
		totlen,SIZE,cnt);
	printf("这个程序只传送了一次\n\n");
		

	rewind(fp);
	fread(sendBuf,SIZE,1,fp);
	ret=send(sock,sendBuf,SIZE,0);
	if (SOCKET_ERROR==ret)
	{
		return -6;
	}
	
	fclose(fp);
	return 0;
}

