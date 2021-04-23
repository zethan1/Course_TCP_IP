#include "prohead.h"
#include "stdio.h"
#include "winsock2.h"
#pragma comment(lib,"ws2_32.lib")

void main(int argc,char* argv[])
{
	if (argc!=2)
	{
		printf("usage:\n %s countNum\n",argv[0]);
		return ;
	}

	WSADATA WSAData;  
	if (WSAStartup(MAKEWORD(2,2),&WSAData)!=0)
	{
      printf("WSAStartup��ʼ��ʧ�ܣ�\n");
	  return;
	}

	SOCKET Socket;
	Socket=socket(AF_INET,SOCK_RAW,IPPROTO_IP);//����ԭʼ�׽���
	if (Socket==INVALID_SOCKET)
	{
		printf("����Socketʧ�ܣ�\n");
		WSACleanup();
		return;
	}

	int RecvTime=5000;//����Socket����ѡ��
	if (setsockopt(Socket,SOL_SOCKET,SO_RCVTIMEO,(char *)&RecvTime,
		sizeof(RecvTime))==SOCKET_ERROR)
	{
      printf("����Socketѡ��ʧ�ܣ�\n");
	  closesocket(Socket);
	  WSACleanup();
	  return;
	}

	char HostName[128];  
	if (gethostname(HostName,128)==SOCKET_ERROR)//��ñ�����������
	{
      printf("���������ʧ�ܣ�\n");
	  closesocket(Socket);
	  WSACleanup();
	  return;
	}

	hostent* pHostent;
	pHostent=gethostbyname(HostName);//��ñ�������IP��ַ
	if (pHostent==NULL)
	{
      printf("�������IP��ַʧ�ܣ�\n");
	  closesocket(Socket);
	  WSACleanup();
	  return;
	}

	sockaddr_in HostAddr; //����Socket��ַ�ṹ
	memset(&HostAddr,0,sizeof(HostAddr));
	HostAddr.sin_family=AF_INET;
	HostAddr.sin_addr.s_addr=(*(in_addr*)pHostent->h_addr).S_un.S_addr;

//	HostAddr.sin_addr.s_addr=inet_addr("27.45.2.83");//����ʹ��

    int nBind; 
	nBind=bind(Socket,(struct sockaddr*)&HostAddr,sizeof(HostAddr));//��Socket������
	if (nBind==SOCKET_ERROR)
	{
      printf("��Socketʧ�ܣ�\n");
	  closesocket(Socket);
	  WSACleanup();
	  return;
	}

	DWORD dwValue=1; //����Socket����ģʽ
	if (ioctlsocket(Socket,SIO_RCVALL,&dwValue)==SOCKET_ERROR)
	{
      printf("����Socket����ʧ�ܣ�\n");
	  closesocket(Socket);
	  WSACleanup();
	  return;
	}

	printf("�汾   �ܳ���   ��־λ   Ƭƫ��   Э��   Դ��ַ           Ŀ�ĵ�ַ\n");
	int nCount=atoi(argv[1]);
	for (int i=0;i<nCount;i++)
	{
		char RecvBuf[65535];
		memset(RecvBuf,0,sizeof(RecvBuf));
		int nRecv;  
		nRecv=recv(Socket,RecvBuf,sizeof(RecvBuf),0);//���ν���IP���ݰ�
        if (nRecv==SOCKET_ERROR)
		{
          printf("����IP���ݰ�ʧ��!\n");
		  closesocket(Socket);
		  WSACleanup();
		  return;
		}
		else
		{
			ip_head IpHead=*(ip_head*)RecvBuf;
			if ((IpHead.Version>>4)==IPV4_VERSION)
				printf("IPV4 ");
			if ((IpHead.Version>>4)==IPV6_VERSION)
				printf("IPV6 ");
			
			printf("  %-4u ",ntohs(IpHead.TotalLen));
			printf("    D=%u M=%u",((ntohs(IpHead.Flags))>>14)&0x01,((ntohs(IpHead.Flags))>>13)&0x01);
			

			printf("  %u        ",ntohs(IpHead.FragOffset)&0x1fff);
			
			switch (IpHead.Protocol)
			{ 
			case ICMP_PACKET:
				printf("ICMP");
				break;
			case IGMP_PACKET:
				printf("ICMP");
				break;
			case TCP_PACKET:
				printf("TCP ");
				break;
			case EGP_PACKET:
				printf("EGP ");
				break;
			case UDP_PACKET:
				printf("UDP ");
				break;
			case OSPF_PACKET:
				printf("OSPF");
				break;
			default:
				printf("Unkn");	
				break;
			}

			printf("   %-15s",inet_ntoa(*(in_addr*)&IpHead.SourceAddr));
			printf("  %s",inet_ntoa(*(in_addr*)&IpHead.DestinAddr));
			printf("\n");

		}        
	}

   closesocket(Socket);
   WSACleanup(); //�����Socket���
   system("pause");
}