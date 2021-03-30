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
		FD_ZERO(&fdread);//��ʼ��fd_set
		FD_SET(fdServer, &fdread);//�����׽��־������Ӧ��fd_set
		tv.tv_sec = 2;//�������Ǵ�����select�ȴ�����󷵻أ����ⱻ������Ҳ�������Ϸ���
		tv.tv_usec = 0;
		
		select(0, &fdread, NULL, NULL, &tv);
		nSize = sizeof(server);
		if (FD_ISSET(fdServer, &fdread))//����׽��־������fd_set�˵���ͻ����Ѿ���connect�����󷢹����ˣ����Ͽ���accept�ɹ�
		{
			AcceptSocket = accept(fdServer,( sockaddr*) &server, &nSize);
			break;
		}
		else//��û�пͻ��˵�connect�������ǿ���ȥ������£�������û����select��ʽ�������׽��ֳ�����������������û��select,���������е�accept��ʱ��ͻ���ǡ��û��connect������ô����ͻᱻ�������������κ�����
		{
			//do something
			::MessageBox(NULL, "waiting", "recv", MB_ICONINFORMATION);//���������󣬼���ȥ����Ƿ��пͻ�����������
		}
	}
	
	char buffer[128];
	ZeroMemory(buffer, 128);
	ret = recv(AcceptSocket,buffer,128,0);//����ͬ��������select���÷�������һ��
	::MessageBox(NULL,buffer,"recv",MB_ICONINFORMATION);
	closesocket(AcceptSocket);
	WSACleanup();
	return;
}