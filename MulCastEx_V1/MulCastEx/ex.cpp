//在应用层

#include "winsock2.h"
#include "stdio.h"
#define  BUFSIZE  1024
#define LOOPCOUNT  100
#define MYADDR    "192.168.10.102"//本机IP，根据具体情况修改
#define MYPORT      1234
#define MULTIADDR  "224.1.1.1"
#pragma comment(lib,"ws2_32.lib")

/*检查系统中是否安装了合适版本的Winsock DLL.*/
int CheckWinsockVersion(void)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	//异步I/O和多址广播只有在Winsock2.0以上版本才支持
	wVersionRequested=MAKEWORD(2,2);
	err=WSAStartup(wVersionRequested,&wsaData);
	if (err==0)
	{
		if ((LOBYTE(wsaData.wVersion)==2) && (HIBYTE(wsaData.wVersion)==2))
			return 0; /*Winsock 版本可接受，成功返回*/
		err= WSAVERNOTSUPPORTED; /* 不支持，失败返回 */
	}
	
	printf("Winsock DLL does not support requested API version.\n");
	WSACleanup();
	return err;
}

int main()
{
	int nRet, i;
	int nIP_TTL = 1;	// 在本子网中传播
	int iLen;
	SOCKADDR_IN stSrcAddr, stMultiAddr;
	SOCKET hSock, hNewSock;
	char sendBuf[]="Message number:             ";
	char recvBuf[BUFSIZE];
	
	nRet = CheckWinsockVersion();  // 检查Winsock版本号
	if (nRet) 
	{
		printf("WSAStartup failed: %d\r\n",nRet);
		return -1;
	}

	// 创建一个多播套接字
	hSock = WSASocket(AF_INET, SOCK_DGRAM,  IPPROTO_UDP, 
		(LPWSAPROTOCOL_INFO)NULL,0, WSA_FLAG_OVERLAPPED|WSA_FLAG_MULTIPOINT_C_LEAF| WSA_FLAG_MULTIPOINT_D_LEAF);
	if (hSock == INVALID_SOCKET)
	{
		printf("Line %s,WSASocket() failed,Err:%d\n",__LINE__,WSAGetLastError());
		exit(1);
	} 
	
	stSrcAddr.sin_family = AF_INET;
	stSrcAddr.sin_port = htons(MYPORT);
	stSrcAddr.sin_addr.s_addr = inet_addr(MYADDR);

	// 将套接口绑定到用户指定地址
	nRet = bind(hSock, (struct sockaddr*)&stSrcAddr, sizeof(struct sockaddr));
	if (nRet == SOCKET_ERROR) 
		 printf("Line %s,bind failed, Err: %d\n",__LINE__,WSAGetLastError());
			
	stMultiAddr.sin_addr.S_un.S_addr=inet_addr(MULTIADDR);
	stMultiAddr.sin_family = AF_INET;
	stMultiAddr.sin_port=htons(MYPORT);

			 
	 // 加入到指定多址广播组，指定为既作发送者又作接收者
	 // 在IP Multicast中，返回的套接字描述符和输入的套接字描述符相同
	hNewSock = WSAJoinLeaf (hSock, /* socket */
		(struct sockaddr*)&stMultiAddr,	 /* multicast address */
		sizeof(sockaddr),    		/* length of addr struct */	
		NULL,                   /* caller data buffer */
		NULL,                   /* callee data buffer */
		NULL,                   /* socket QOS setting */
		NULL,                   /* socket group QOS */
		JL_BOTH);               /* do both: send *and* receive */
	if (hNewSock == INVALID_SOCKET) 
		printf("Line %s,WSAJoinLeaf() failed, Err: %d\n",__LINE__,WSAGetLastError());
			 
	SOCKADDR_IN stSendAddr;

	// 循环发送/接收数据
	for (i=0;i<LOOPCOUNT; i++) 
	{
		static int iCounter = 1;
		itoa(iCounter++, &sendBuf[16], 10);
		
		nRet=sendto(hSock,sendBuf,strlen(sendBuf)+1,0,
			(struct sockaddr*)&stMultiAddr,sizeof(struct sockaddr));
		if (nRet == SOCKET_ERROR)
			printf("Line %s,sendto() failed,Err:%d\n",__LINE__,WSAGetLastError());

		iLen = sizeof(struct sockaddr);
		memset(recvBuf,0,sizeof(recvBuf));
		nRet=recvfrom(hSock,recvBuf,sizeof(recvBuf),0,
			(struct sockaddr*)&stSendAddr,&iLen);
		if (nRet == SOCKET_ERROR) 
			printf("recvfrom() failed, Err:%d\n",WSAGetLastError());
		else 
		{
			printf("recvfrom() received %d bytes from %s, port %d :%s\r\n",
				nRet, inet_ntoa(stSendAddr.sin_addr), ntohs(stSendAddr.sin_port), recvBuf);
		}
	} 
   
	closesocket(hNewSock);
	closesocket(hSock);
	WSACleanup();
	return 0;
}
