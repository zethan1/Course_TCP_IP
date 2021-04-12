#include "stdio.h"
#include "conio.h"
#include "winsock2.h"
#pragma comment(lib,"ws2_32.lib")
#define MAX_SIZE 1024

SOCKET Socket;
char Command[MAX_SIZE];
char Respond[MAX_SIZE];
bool RespondStatus;

bool SendCommand();
bool RecvRespond();
int connect(char *srvName,char *account,char *password);
int receive();
int quit();

int main(int argc,char* argv[])
{
	char srvName[256],account[30],password[30];
	
	memset(srvName,0,sizeof(srvName));
	printf("�������ʼ�������������IP(��pop.126.com): ");
	scanf("%s",srvName);

	memset(account,0,sizeof(account));
	printf("�������˻���(@֮ǰ�Ĳ���): ");
	scanf("%s",account);

	memset(password,0,sizeof(password));
	printf("�������������Ȩ��: ");
	char ch;
	ch=getchar();//���������û���ʱ��\n
	while (true)
	{
		ch=getch();
		if (ch=='\r') break;
		password[strlen(password)]=ch;
		printf("*");
	}
	
	printf("\n");
	if ( connect(srvName,account,password)<0) 
			return -1;
	
	printf("\n�����ʼ��������ɹ�,���濪ʼ��ȡ�ʼ�\n");
	printf("Ϊ�˼�,������ȡ���µ�һ���ʼ�\n\n");
		
	if (receive()<0) 
	{
		printf("�����ʼ�ʧ��\n");
		closesocket(Socket);
		WSACleanup();
		system("pause");
		return -2;
	}

	if (quit()<0)
	{
		closesocket(Socket);
		WSACleanup();
		system("pause");
		return -3;
	}
	return 0;
}


int connect(char *srvName,char *account,char *password)
{
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2,2),&WSAData)!=0)
	{
       printf("��ʼ��Winsockʧ�ܣ�\n");
	   return -1;
	}

	Socket=socket(AF_INET,SOCK_STREAM,0);
	if (Socket==INVALID_SOCKET)
	{
      printf("����Socketʧ�ܣ�\n");
	  WSACleanup();
	  return -2;
	}

	int IpAddress;
	IpAddress=inet_addr(srvName);
	if (IpAddress==INADDR_NONE)
	{
        hostent* pHostent=gethostbyname(srvName);
	if (pHostent)
		IpAddress=(*(in_addr*)pHostent->h_addr).s_addr;
	}

	struct sockaddr_in serveraddr;
	memset(&serveraddr,0,sizeof(serveraddr));
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_port=htons(110);
    serveraddr.sin_addr.s_addr=IpAddress;

	int nConnect;
	nConnect=connect(Socket, (sockaddr*)&serveraddr,sizeof(sockaddr));
	if (nConnect==SOCKET_ERROR)
	{
       printf("���ӽ���ʧ�ܣ�\n");
	   closesocket(Socket);
	   WSACleanup();
	   return -3;
	}

	printf("\nClient: Connect to %s...\n",srvName);
	if (!RecvRespond())
		return -4;
	else
	{
       if (RespondStatus==true)
	   {
          printf("Server:  %s\n",Respond);
		}
	   else
	   {
         printf("������Ӧ����\n");
		 closesocket(Socket);
		 WSACleanup();
		 return -5;
	   }
	}

	memset(Command,0,MAX_SIZE);//�ϳ�USER����
	memcpy(Command,"USER ",strlen("USER "));
	memcpy(Command+strlen("USER "),account,strlen(account));
	memcpy(Command+strlen("USER ")+strlen(account),"\r\n",2);
	if (!SendCommand())//����USER����
		return -6;
	printf("Client:  %s\n",Command);
	if (!RecvRespond())
		return -7;
	else
	{
		if (RespondStatus==true)//�ж�USER��Ӧ״̬
        {
           printf("Server:  %s\n",Respond);
		 }
		else
		{
           printf("USER��Ӧ����\n");
		   closesocket(Socket);
		   WSACleanup();
		   return -8;
		}
	}

	memset(Command,0,MAX_SIZE);//�ϳ�PASS����
	memcpy(Command,"PASS ",strlen("PASS "));
	memcpy(Command+strlen("PASS "),password,strlen(password));
	memcpy(Command+strlen("PASS ")+strlen(password),"\r\n",2);
	if (!SendCommand())
	  return -9;
	printf("Client: PASS ******\n");
	if (!RecvRespond())
		return -10;
	else
	{
       if (RespondStatus==true)//�ж�PASS��Ӧ״̬
	   {
          printf("Server:  %s\n",Respond);
		}
	   else
	   {
		   printf("PASS��Ӧ����\n");
		   closesocket(Socket);
		   WSACleanup();
		   return -11;
	   }
	}

	return 0;
}

int receive()
{
	memset(Command,0,MAX_SIZE);//�ϳ�STAT����
	memcpy(Command,"STAT",strlen("STAT"));
    memcpy(Command+strlen("STAT"),"\r\n",2);
	if (!SendCommand())
		return -1;
	printf("Client:  %s\n",Command);
	
	if (!RecvRespond())
		return -2;
	else
	{
       if (RespondStatus==true)//�ж�STAT��Ӧ״̬
	   {
           printf("Server:  %s\n",Respond);
		}
	   else
	   {
         printf("STAT��Ӧ����\n");
		 closesocket(Socket);
		 WSACleanup();
		 return -3;
	   }
	}

	char* pLast=strtok(Respond," ");
	pLast=strtok(NULL," ");
	int len=strlen(pLast);

	memset(Command,0,MAX_SIZE);//�ϳ�RETR����,�������µ��ʼ�
	memcpy(Command,"RETR ",strlen("RETR "));
	memcpy(Command+strlen("RETR "),pLast,len);
	memcpy(Command+strlen("RETR ")+len,"\r\n",2);
	if (!SendCommand())
		return -4;
	printf("Client:  %s\n",Command);
	if (!RecvRespond())
		return -5;
	else
	{
       if (RespondStatus==true)//�ж�RETR��Ӧ״̬
	   {
          printf("Server:  %s\n",Respond);
		}
	   else
	   {
         printf("RETR��Ӧ���� \n");
		 closesocket(Socket);
		 WSACleanup();
		 return -6;
	   }  
	}

	char MailBuf[MAX_SIZE];
	char *pmail;
	pmail=new char[MAX_SIZE*MAX_SIZE*50];
	char mailend[5];
	int lenMail=0;

	while (true)
	{
		memset(MailBuf,0,MAX_SIZE);
		int nRecv;
		nRecv=recv(Socket,MailBuf,MAX_SIZE,0);
		if (nRecv==SOCKET_ERROR)//��ȡ�ʼ���Ϣ
		{
			printf("RETR���մ��� \n");
			closesocket(Socket);
			WSACleanup();
			return -7;
		}

		memcpy(pmail+lenMail,MailBuf,nRecv);//��ʾ�ʼ���Ϣ
		lenMail+=nRecv;
		memcpy(mailend,pmail+lenMail-5,5);

		if ((mailend[0]==13)&&(mailend[1]==10)&&(mailend[2]=='.')\
			&&(mailend[3]==13)&&(mailend[4]==10))
			break;  
	}

	printf("�ʼ���Ϣ����:\n");
	for (int i=0;i<lenMail;i++) printf("%c",pmail[i]);
	
	delete [] pmail;
	pmail = NULL;

	return 0;
}

int  quit()
{
	memset(Command,0,MAX_SIZE);
	memcpy(Command,"QUIT",strlen("QUIT"));
	memcpy(Command+strlen("QUIT"),"\r\n",2);
	if (!SendCommand())
		return -1;
	printf("Client:  %s\n",Command);
	if (!RecvRespond())
		return -2;
	else
	{
      if (RespondStatus==true)
	  {
        printf("Server:  %s\n",Respond);
	  }
	  else
	  {
        printf("QUIT��Ӧ����\n");
		closesocket(Socket);
		WSACleanup();
		return -3;       
	  }
	}

	closesocket(Socket);
	WSACleanup();
	return 0;
}

bool SendCommand()
{
  int nSend;
  nSend=send(Socket,Command,strlen(Command),0);
  if (nSend==SOCKET_ERROR)
  {
     printf("Socket����ʧ�ܣ�\n");
	 closesocket(Socket);
	 WSACleanup();
	 return false;
  }

  return true;
}

bool RecvRespond()
{
	int nRecv;
	memset(Respond,0,MAX_SIZE);
	nRecv=recv(Socket,Respond,MAX_SIZE,0);
	if (nRecv==SOCKET_ERROR)
	{
       printf("Socket����ʧ�ܣ�\n");
	   closesocket(Socket);
	   WSACleanup();
	   return false;
	}

	if (memcmp(Respond,"+OK",3)==0)
		RespondStatus=true;
	else
		RespondStatus=false;

	return true;	
}
