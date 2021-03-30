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
		printf("无法连接到服务器\n");
		return;
	}
	else printf("连接服务器成功\n");

	list(argv[1]);
	quit();
}

int  connect(char *srvAddr)
{
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2,2),&WSAData)!=0)
	{
		printf("初始化Winsock失败！\n");
		return -1;
	}

	SocketControl=socket(AF_INET,SOCK_STREAM,0);
	if (SocketControl==INVALID_SOCKET)
	{
		printf("创建控制Socket失败！\n");
		WSACleanup();
		return -2;
	}


	u_long IpAddress; 
	IpAddress=inet_addr(srvAddr);//判断域名或IP地址
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
	nConnect=connect(SocketControl,(sockaddr*)&serveraddr,sizeof(serveraddr));//建立FTP控制连接
	if (nConnect==SOCKET_ERROR)
	{
		printf("控制连接建立失败！\n");
		closesocket(SocketControl);
		WSACleanup();
		return -3;
	}

	printf("连接到%s\n",srvAddr);

	if (!RecvRespond())
		return -4;
	else
	{//判断连接应答码
		if (RespondCode==220)
		{
			printf("Server:   %s\n",Respond);
		}
		else
		{
			printf("控制连接响应错误！\n");
			closesocket(SocketControl);
			WSACleanup();
			return -5;
		}

	}

	printf("请输入用户名:");
	char username[30];
	memset(username,0,sizeof(username));
	scanf("%s",username);

	memset(Command,0,MAX_SIZE);
	//合成USER命令
	memcpy(Command,"USER ",strlen("USER "));
	memcpy(Command+strlen("USER "),username,strlen(username));
	memcpy(Command+strlen("USER ")+strlen(username),"\r\n",2);
	if (!SendCommand())
		return -6;

	printf("Client:   %s",Command);

	if (!RecvRespond())
		return -7;
	else
	{//判断USER应答码
		if (RespondCode==220 || RespondCode==331)
		{
			printf("Server:  %s\n",Respond);
		}
		else
		{
			printf("USER响应错误！\n");
			closesocket(SocketControl);
			WSACleanup();
			return -8;
		}
	}

	if (RespondCode==331)//User name okay,need password
	{ 
		printf("请输入密码:");
		char password[30];
		memset(password,0,sizeof(password));

		char ch;
		ch=getchar();//接收输入用户名时的\n
		while (true)
		{
			ch=getch();
			//putchar(ch);
			if (ch=='\r') break;
			//if (ch=='\n') break;

			password[strlen(password)]=ch;
		}

		//scanf("%s",password);


		//合成PASS命令
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
		{//判断PASS响应码
			if (RespondCode==230)
			{
				printf("Server:  %s\n",Respond);
			}
			else
			{
				printf("PASS响应错误！\n");
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
	nRecv=recv(SocketControl,Respond,MAX_SIZE,0);//通过连接接收响应
	if (nRecv==SOCKET_ERROR)
	{
		printf("Socket接收失败！\n");
		closesocket(SocketControl);
		WSACleanup();
		return false;
	}

	//从响应中解析响应码
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
		printf("Socket发送失败！\n");
		closesocket(SocketControl);
		WSACleanup();
		return false;
	}

	return true;
}

bool DataConnect(char *srvAddr)
{
	memset(Command,0,MAX_SIZE);
	//合成PASV命令
	memcpy(Command,"PASV",strlen("PASV"));
	memcpy(Command+strlen("PASV"),"\r\n",2);

	if (!SendCommand())
		return false;

	if (!RecvRespond())
		return false;
	else
	{
		//判断PASV响应码
		if (RespondCode!=227)
		{
			printf("PASV响应错误！\n");
			closesocket(SocketControl);
			WSACleanup();
			return false;
		}

	}


	char* part[6];
	//分离PASV应答信息
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

	unsigned short ServerPort;   //获取服务器数据端口
	ServerPort=unsigned short((atoi(part[4])<<8)+atoi(part[5]));
	//ServerPort=unsigned short((atoi(part[4])<<8)| atoi(part[5]));
	SocketData=socket(AF_INET,SOCK_STREAM,0);
	if (SocketData==INVALID_SOCKET)
	{
		printf("创建数据Socket失败！\n");
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
		printf("数据连接失败！\n");
		closesocket(SocketControl);
		closesocket(SocketData);
		WSACleanup();
		return false;
	}

	return true;
}

int list(char *srvAddr)
{
	// TODO: 在此添加控件通知处理程序代码
	if (!DataConnect(srvAddr))
		return -1;

	memset(Command,0,MAX_SIZE);
	//合成LIST命令
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
			printf("LIST响应错误！ \n");
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
		nRecv=recv(SocketData,ListBuf,MAX_SIZE,0);//读取目录信息
		if (nRecv==SOCKET_ERROR)
		{
			printf("LIST接收错误！ \n");
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
			printf("LIST响应错误！ \n");
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
	// TODO: 在此添加控件通知处理程序代码
	memset(Command,0,MAX_SIZE);
	//合成QUIT命令
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
			printf("QUIT响应错误！ \n");
			closesocket(SocketControl);
			WSACleanup();
			return;
		}
	}

	closesocket(SocketControl);
	WSACleanup();
}