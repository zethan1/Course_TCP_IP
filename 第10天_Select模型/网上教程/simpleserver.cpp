#include "iostream"
#include  "winsock2.h"
#include "windows.h"
using namespace std;

#define InternetAddr "127.0.0.1"
#define iPort 5055
#pragma comment(lib, "ws2_32.lib")


void main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2,2), &wsa);
	SOCKET fdServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(InternetAddr);
	server.sin_port = htons(iPort);
	int ret = bind(fdServer, (sockaddr*)&server, sizeof(server));
	ret = listen(fdServer, 4);
	
	SOCKET AcceptSocket;
	fd_set fdread;
	timeval tv;
	int nSize; 
	
	while(1)
	{
		FD_ZERO(&fdread);//初始化fd_set
		FD_SET(fdServer, &fdread);//分配套接字句柄到相应的fd_set
		tv.tv_sec = 2;//这里我们打算让select等待两秒后返回，避免被锁死，也避免马上返回
		tv.tv_usec = 0;
		
		select(0, &fdread, NULL, NULL, &tv);
		nSize = sizeof(server);
		if (FD_ISSET(fdServer, &fdread))//如果套接字句柄还在fd_set里，说明客户端已经有connect的请求发过来了，马上可以accept成功
		{
			AcceptSocket = accept(fdServer,( sockaddr*) &server, &nSize);
			break;
		}
		else//还没有客户端的connect请求，我们可以去做别的事，避免像没有用select方式的阻塞套接字程序被锁死的情况，如果没用select,当程序运行到accept的时候客户端恰好没有connect请求，那么程序就会被锁死，做不了任何事情
		{
			//do something
			::MessageBox(NULL, "waiting", "recv", MB_ICONINFORMATION);//别的事做完后，继续去检查是否有客户端连接请求
		}
	}
	
	char buffer[128];
	ZeroMemory(buffer, 128);
	ret = recv(AcceptSocket,buffer,128,0);//这里同样可以用select，用法和上面一样
	::MessageBox(NULL,buffer,"recv",MB_ICONINFORMATION);
	closesocket(AcceptSocket);
	WSACleanup();
	return;
}