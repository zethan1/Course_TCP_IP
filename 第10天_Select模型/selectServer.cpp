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
	fd_set fdsock;  //���������׽��ֵļ���
	fd_set fdread;  //selectҪ���Ŀɶ��׽��ּ���
	struct sockaddr_in addrServer,addrClient;
	char recvBuf[BUFSIZE];//�������ڽ��տͻ��˷�����Ϣ�Ļ���
	char sendBuf[] ="Connect succeed. Please send a message to me.\n"; //�����ͻ��˵���Ϣ

	WSADATA wsaData;
	ret=WSAStartup(MAKEWORD(2,2),&wsaData);
	if(SOCKET_ERROR==ret)
	{
		printf("����winsock.dllʧ�ܣ�\n");
		return -1;
	}

	sockListen = socket(AF_INET,SOCK_STREAM,0);
	if (INVALID_SOCKET==sockListen) 
	{
		printf("�����׽���ʧ�ܣ�\n");
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
		printf("��ַ��ʧ�ܣ�\n");
		closesocket(sockListen);
		WSACleanup();
		return -3;
	}

	ret=listen(sockListen,3);
	if(SOCKET_ERROR==ret)      
	{
		printf("listen��������ʧ�ܣ�\n");
		closesocket(sockListen);
		WSACleanup();
		return -4;
	}
	else
		printf("�����������ɹ�,���ڼ���......\n"); 

	FD_ZERO(&fdsock);//��ʼ�� fdsock Ϊ�ռ���
	FD_SET(sockListen, &fdsock);//�� sockListen ���뵽�׽��ּ��� fdsock

	/***ѭ�����������������շ�����***/
	u_int i,count,msgCount=0;
	while(true)
	{
		if (msgCount>=20) break;//Ϊ�˲��Է���,������20����Ϣ
		FD_ZERO(&fdread);//��ʼ�� fdread Ϊ�ռ���
		fdread=fdsock;//�� fdsock �е������׽�����ӵ� fdread ��
		count=fdsock.fd_count;
		
		if(select(0, &fdread, NULL, NULL, NULL)==SOCKET_ERROR)
		{
			printf("Select����ʧ�ܣ�\n");
			break;//��ֹѭ���˳�����
		}

		for(i=0;i<count;i++)
		{
			if (FD_ISSET(fdsock.fd_array[i], &fdread)) 
			{
				if(fdsock.fd_array[i]==sockListen)
				{	//�пͻ��������󵽴�, ������������
					len=sizeof(struct sockaddr);
					sockTX=accept (sockListen, (struct sockaddr *) &addrClient, &len);
					if(sockTX==INVALID_SOCKET) 
					{  
						printf("accept��������ʧ�ܣ�\n");
						break;
					}
					else
					{
						printf("%s:%d���ӳɹ���\n",inet_ntoa(addrClient.sin_addr),
							ntohs(addrClient.sin_port));
						send(sockTX,sendBuf,strlen(sendBuf)+1,0) ;//����һ����Ϣ
						FD_SET(sockTX, &fdsock);//�����׽��ּ���fdsock
					}
				}
				else
				{	//�пͻ��������ݣ���������
					memset( recvBuf,0, sizeof(recvBuf));//����������
					ret=recv(fdsock.fd_array[i],recvBuf,sizeof(recvBuf),0);
					if(SOCKET_ERROR==ret) 
						printf("������Ϣʧ��!\n");
					else
					{  //��ʾ�յ�����Ϣ
						len=sizeof(struct sockaddr);
						getpeername(fdsock.fd_array[i], 
							(struct sockaddr *)&addrClient, &len); //��ȡ�Է�IP��ַ
						printf("%s:%d˵: %s\n", inet_ntoa(addrClient.sin_addr),
							ntohs(addrClient.sin_port),recvBuf);
						msgCount++;
					}
				}
			}
		}
	}

	/***��������***/
	for(i=0;i<fdsock.fd_count; i++)
		closesocket (fdsock.fd_array[i]);//�ر������׽���
	WSACleanup();//ע��WinSock��̬���ӿ�
	system("pause");
	return 0;
}
