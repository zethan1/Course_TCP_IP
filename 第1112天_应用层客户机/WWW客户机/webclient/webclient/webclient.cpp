#include "stdio.h"
#include "conio.h"
#include "string.h"
#include "winsock2.h"
#pragma comment(lib,"ws2_32.lib")
#define MAX_SIZE 1024

char Command[MAX_SIZE];
char Respond[MAX_SIZE];
bool RespondStatus;
SOCKET Socket;
int browse(char *url);

int main()
{
	char url[256];
	memset(url,0,sizeof(url));
	printf("������Ҫ�������ַ: ");
	scanf("%s",url);
	
	//strcpy(url,"http://news.ifeng.com/c/7sJZuEaH4d6");
	//strcpy(url,"news.ifeng.com/c/7sJZuEaH4d6");
	//strcpy(url,"www.wyu.edu.cn");
	//strcpy(url,"http://www.wyu.edu.cn");
	browse(url);



	
	system("pause");
	return 0;
}

int browse(char *url)
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

	char TempUrl[256];//��URLɾ��Э��
    memset(TempUrl,0,sizeof(TempUrl));
	if (strstr(url,"http://"))
		strcpy(TempUrl,url+7);
	else
		strcpy(TempUrl,url);

	int UrlPos;//��URL��ȡ������
	char HostName[256];
	char FilePath[256];
	UrlPos=strchr(TempUrl,'/')-TempUrl;
	
	memset(HostName,0,sizeof(HostName));
	memset(FilePath,0,sizeof(FilePath));
	if (UrlPos<0)//Ĭ����ҳ
	{ 
		strcpy(HostName,TempUrl);
		strcpy(FilePath,"/");
	}
	else
	{
		strncpy(HostName,TempUrl,UrlPos);
		strcpy(FilePath,TempUrl+UrlPos);
	}
	
	int IpAddress;//�ж�������IP��ַ
	IpAddress=inet_addr(HostName);
	if (IpAddress==INADDR_NONE)
	{
      hostent* pHostent=gethostbyname(HostName);
	  if (pHostent)
		  IpAddress=(*(in_addr*)pHostent->h_addr).s_addr;
  	}

	struct sockaddr_in serveraddr;
	memset(&serveraddr,0,sizeof(sockaddr_in));
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_port=htons(80);
	serveraddr.sin_addr.s_addr=IpAddress;

	int nConnect;
	nConnect=connect(Socket,(sockaddr*)&serveraddr,sizeof(sockaddr));
	if (nConnect==SOCKET_ERROR)
	{
      printf("���ӽ���ʧ�ܣ�\n");
	  closesocket(Socket);
	  WSACleanup();
	  return -3;
	}

	memset(Command,0,MAX_SIZE);//����GET����
	memcpy(Command,"GET ",4);
	memcpy(Command+4,FilePath,strlen(FilePath));
	memcpy(Command+4+strlen(FilePath)," HTTP/1.1\r\nHost: ",17);
	memcpy(Command+21+strlen(FilePath),HostName,strlen(HostName));
	memcpy(Command+21+strlen(FilePath)+strlen(HostName),"\r\n\r\n",4);

	
	int nSend;//����GET����
	nSend=send(Socket,Command,strlen(Command),0);
	if (nSend==SOCKET_ERROR)
	{
       printf("Socket����ʧ��!\n");
	   closesocket(Socket);
	   WSACleanup();
	   return -4;
	}

	
	printf("%s",Command);

	char *RecvBuf;
	RecvBuf=new char[MAX_SIZE*MAX_SIZE];
	memset(RecvBuf,0,sizeof(RecvBuf));

	int totlen=0;
	while (true)
	{
       memset(Respond,0,MAX_SIZE);
	   int nRecv;//����GET��Ӧ
	   nRecv=recv(Socket,Respond,MAX_SIZE,0);
       if (nRecv==SOCKET_ERROR)
	   {
         printf("Socket����ʧ�ܣ�\n");
		 closesocket(Socket);
		 WSACleanup();
		 return -5;
	   }

	   strcat(RecvBuf,Respond);
	   totlen+=nRecv;

	  if (strstr(Respond,"</html>"))
		   break;
	}

	char RespCode[4];//����Ӧ�н�����Ӧ��
	memset(RespCode,0,sizeof(RespCode));
	strncpy(RespCode,RecvBuf+9,3);
	
	/*int HtmlPos;//����Ӧ����ȡHTML
	char HeadText[1024];
	memset(HeadText,0,sizeof(HeadText));
	char *HtmlText=0;
	if (atoi(RespCode)==200)
	{
		HtmlText=strstr(RecvBuf,"<html");
		int len2=strlen(HtmlText);
		if (len2>0)		HtmlPos=totlen-len2;
		else 
		{
			HtmlText=strstr(RecvBuf,"<!DOCTYPE html");
			len2=strlen(HtmlText);
			HtmlPos=totlen-len2;
		}
	
	  strncpy(HeadText,RecvBuf,HtmlPos);
	}
	else
	{
      printf("GET��Ӧ����\n");
	  closesocket(Socket);
	  WSACleanup();
	  return -6;
	}
	
	printf("%s",HeadText);
	printf("%s",HtmlText);
	*/

	if (atoi(RespCode)==200)
	{
		printf("%s",RecvBuf);
	}
	else
	{
       printf("GET��Ӧ����\n");
	  closesocket(Socket);
	  WSACleanup();
	  return -6;
	}

	delete [] RecvBuf;
	RecvBuf = NULL;
	closesocket(Socket);
	WSACleanup();
	return 0;
}