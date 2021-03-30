#include "stdio.h"
#include "conio.h"
#include "winsock2.h"
#pragma comment(lib,"ws2_32.lib")
#define MAX_SIZE 4096

SOCKET SocketControl;
SOCKET SocketData;
int RespondCode;
char Respond[MAX_SIZE];
char Command[MAX_SIZE];
int  connect(char *srvAddr);
bool RecvRespond();
bool SendCommand();
bool DataConnect(char *srvAddr);
int list(char *srvAddr);
void quit();
void main(int argc,char* argv[])
{
	int ret;
	if (argc!=2)
	{
		printf("usage:\n %s servername\n",argv[0]);
		return ;
	}

	ret=connect(argv[1]);
	if (ret<0)
	{
		printf("�޷����ӵ�������\n");
		return;
	}
	else printf("���ӷ������ɹ�\n");

	list(argv[1]);
	quit();
}

int  connect(char *srvAddr)
{
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2,2),&WSAData)!=0)
	{
		printf("��ʼ��Winsockʧ�ܣ�\n");
		return -1;
	}

	SocketControl=socket(AF_INET,SOCK_STREAM,0);
	if (SocketControl==INVALID_SOCKET)
	{
		printf("��������Socketʧ�ܣ�\n");
		WSACleanup();
		return -2;
	}


	u_long IpAddress; 
	IpAddress=inet_addr(srvAddr);//�ж�������IP��ַ
	if (IpAddress==INADDR_NONE)
	{
		hostent* pHostent=gethostbyname(srvAddr);
		if (pHostent)
			IpAddress=(*(in_addr*)pHostent->h_addr_list[0]).S_un.S_addr;

	}

	sockaddr_in serveraddr;  
	memset(&serveraddr,0,sizeof(serveraddr));
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_port=htons(21);
	serveraddr.sin_addr.s_addr=IpAddress;

	int nConnect;
	nConnect=connect(SocketControl,(sockaddr*)&serveraddr,sizeof(serveraddr));//����FTP��������
	if (nConnect==SOCKET_ERROR)
	{
		printf("�������ӽ���ʧ�ܣ�\n");
		closesocket(SocketControl);
		WSACleanup();
		return -3;
	}

	printf("���ӵ�%s\n",srvAddr);

	if (!RecvRespond())
		return -4;
	else
	{//�ж�����Ӧ����
		if (RespondCode==220)
		{
			printf("Server:   %s\n",Respond);
		}
		else
		{
			printf("����������Ӧ����\n");
			closesocket(SocketControl);
			WSACleanup();
			return -5;
		}

	}

	printf("�������û���:");
	char username[30];
	memset(username,0,sizeof(username));
	scanf("%s",username);

	memset(Command,0,MAX_SIZE);
	//�ϳ�USER����
	memcpy(Command,"USER ",strlen("USER "));
	memcpy(Command+strlen("USER "),username,strlen(username));
	memcpy(Command+strlen("USER ")+strlen(username),"\r\n",2);
	if (!SendCommand())
		return -6;

	printf("Client:   %s",Command);

	if (!RecvRespond())
		return -7;
	else
	{//�ж�USERӦ����
		if (RespondCode==220 || RespondCode==331)
		{
			printf("Server:  %s\n",Respond);
		}
		else
		{
			printf("USER��Ӧ����\n");
			closesocket(SocketControl);
			WSACleanup();
			return -8;
		}
	}

	if (RespondCode==331)//User name okay,need password
	{ 
		printf("����������:");
		char password[30];
		memset(password,0,sizeof(password));

		char ch;
		ch=getchar();//���������û���ʱ��\n
		while (true)
		{
			ch=getch();
			//putchar(ch);
			if (ch=='\r') break;
			//if (ch=='\n') break;

			password[strlen(password)]=ch;
		}

		//scanf("%s",password);


		//�ϳ�PASS����
		memset(Command,0,MAX_SIZE);
		memcpy(Command,"PASS ",strlen("PASS "));
		memcpy(Command+strlen("PASS "),password,strlen(password));
		memcpy(Command+strlen("PASS ")+strlen(password),"\r\n",2);
		if (!SendCommand())
			return -9;

		printf("Client: PASS ******\n");

		if (!RecvRespond())
			return -10;
		else
		{//�ж�PASS��Ӧ��
			if (RespondCode==230)
			{
				printf("Server:  %s\n",Respond);
			}
			else
			{
				printf("PASS��Ӧ����\n");
				closesocket(SocketControl);
				WSACleanup();
				return -11;
			}
		}

	}

	return 0;

}


bool RecvRespond()
{
	int nRecv;
	memset(Respond,0,MAX_SIZE);
	nRecv=recv(SocketControl,Respond,MAX_SIZE,0);//ͨ�����ӽ�����Ӧ
	if (nRecv==SOCKET_ERROR)
	{
		printf("Socket����ʧ�ܣ�\n");
		closesocket(SocketControl);
		WSACleanup();
		return false;
	}

	//����Ӧ�н�����Ӧ��
	char* ReplyCodes=new char[3];
	memset(ReplyCodes,0,3);
	memcpy(ReplyCodes,Respond,3);
	RespondCode=atoi(ReplyCodes);
	return true;
}

bool SendCommand()
{
	int nSend;
	nSend=send(SocketControl,Command,strlen(Command),0);
	if (nSend==SOCKET_ERROR)
	{
		printf("Socket����ʧ�ܣ�\n");
		closesocket(SocketControl);
		WSACleanup();
		return false;
	}

	return true;
}

bool DataConnect(char *srvAddr)
{
	memset(Command,0,MAX_SIZE);
	//�ϳ�PASV����
	memcpy(Command,"PASV",strlen("PASV"));
	memcpy(Command+strlen("PASV"),"\r\n",2);

	if (!SendCommand())
		return false;

	if (!RecvRespond())
		return false;
	else
	{
		//�ж�PASV��Ӧ��
		if (RespondCode!=227)
		{
			printf("PASV��Ӧ����\n");
			closesocket(SocketControl);
			WSACleanup();
			return false;
		}

	}


	char* part[6];
	//����PASVӦ����Ϣ
	if (strtok(Respond,"("))
	{
		for (int i=0;i<5;i++)
		{
			part[i]=strtok(NULL,",");
			if (!part[i])
				return false;
		}
		part[5]=strtok(NULL,")");

		if (!part[5])
			return false;

	}
	else
		return false;

	unsigned short ServerPort;   //��ȡ���������ݶ˿�
	ServerPort=unsigned short((atoi(part[4])<<8)+atoi(part[5]));
	//ServerPort=unsigned short((atoi(part[4])<<8)| atoi(part[5]));
	SocketData=socket(AF_INET,SOCK_STREAM,0);
	if (SocketData==INVALID_SOCKET)
	{
		printf("��������Socketʧ�ܣ�\n");
		closesocket(SocketControl);
		WSACleanup();
		return false;
	}

	u_long IpAddress;
	IpAddress=inet_addr(srvAddr);
	if (IpAddress==INADDR_NONE)
	{
		hostent* pHostent=gethostbyname(srvAddr);
		if (pHostent)
			IpAddress=(*(in_addr*)pHostent->h_addr_list[0]).S_un.S_addr;	  
	}

	sockaddr_in serveraddr2;
	memset(&serveraddr2,0,sizeof(serveraddr2));
	serveraddr2.sin_family=AF_INET;
	serveraddr2.sin_port=htons(ServerPort);
	serveraddr2.sin_addr.s_addr=IpAddress;

	int nConnect=connect(SocketData,(sockaddr*)&serveraddr2,sizeof(serveraddr2));
	if (nConnect==SOCKET_ERROR)
	{
		printf("��������ʧ�ܣ�\n");
		closesocket(SocketControl);
		closesocket(SocketData);
		WSACleanup();
		return false;
	}

	return true;
}

int list(char *srvAddr)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!DataConnect(srvAddr))
		return -1;

	memset(Command,0,MAX_SIZE);
	//�ϳ�LIST����
	memcpy(Command,"LIST",strlen("LIST"));
	memcpy(Command+strlen("LIST"),"\r\n",2);
	if (!SendCommand())
		return -2;

	printf("Client:  %s\n",Command);
	if (!RecvRespond())
		return -3;
	else
	{
		if (RespondCode==125 || RespondCode==150 || RespondCode==226)
		{
			printf("Server:  %s\n",Respond);
		}
		else
		{
			printf("LIST��Ӧ���� \n");
			closesocket(SocketControl);
			WSACleanup();
			return -4;
		}
	}

	printf("Client: ......\n");
	char  dirinfo[MAX_SIZE];
	memset(dirinfo,0,sizeof(dirinfo));

	while (true)
	{
		char ListBuf[MAX_SIZE];
		memset(ListBuf,0,MAX_SIZE);
		int nRecv;
		nRecv=recv(SocketData,ListBuf,MAX_SIZE,0);//��ȡĿ¼��Ϣ
		if (nRecv==SOCKET_ERROR)
		{
			printf("LIST���մ��� \n");
			closesocket(SocketData);
			WSACleanup();
			return -5;
		}
		if (nRecv<=0) break;

		strcat(dirinfo,ListBuf);
	} 

	closesocket(SocketData);
	if (!RecvRespond())
		return -6;
	else
	{
		if (RespondCode==226)
		{
			printf("Server:  %s\n",Respond);
		}
		else
		{
			printf("LIST��Ӧ���� \n");
			closesocket(SocketControl);
			WSACleanup();
			return -6;
		}
	}

	printf("\n%s\n",dirinfo);
	return 0;
}

void quit()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	memset(Command,0,MAX_SIZE);
	//�ϳ�QUIT����
	memcpy(Command,"QUIT",strlen("QUIT"));
	memcpy(Command+strlen("QUIT"),"\r\n",2);
	if (!SendCommand())
		return;

	printf("Client:  %s\n",Command);
	if (!RecvRespond())
		return;
	else
	{
		if (RespondCode==221)
		{
			printf("Server:  %s\n",Respond);
		}
		else
		{
			printf("QUIT��Ӧ���� \n");
			closesocket(SocketControl);
			WSACleanup();
			return;
		}
	}

	closesocket(SocketControl);
	WSACleanup();
}