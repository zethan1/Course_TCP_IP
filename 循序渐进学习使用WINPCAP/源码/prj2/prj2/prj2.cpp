#include "pcap.h"
#include "Winsock2.h"
#pragma comment(lib,"wpcap.lib")
#pragma comment(lib,"Ws2_32.lib")
void ifprint(pcap_if_t *d); 
int  main() 
 {  
	pcap_if_t *alldevs, *d; 
    char errbuf[PCAP_ERRBUF_SIZE+1]; 
    if (pcap_findalldevs(&alldevs, errbuf) == -1) /* 获得网卡的列表 */
    {fprintf(stderr,"Error in pcap_findalldevs: %s\n",errbuf); 
      return -1; 
     } 
     
	/* 循环调用ifprint() 来显示pcap_if结构的信息*/ 
     for(d=alldevs;d;d=d->next)
{  ifprint(d);  }  
      return 1 ; 
} 

 /* Print all the available information on the given interface */ 
void ifprint(pcap_if_t *d) 
{  	IN_ADDR sinaddr;//IPv4地址
	pcap_addr_t *a; 
	printf("%s\n",d->name); /* Name */
	if (d->description) /* Description */
		printf("\tDescription: %s\n",d->description); 
	/* Loopback Address*/ 
	printf("\tLoopback: %s\n",(d->flags & PCAP_IF_LOOPBACK)?"yes":"no");
	/* IP addresses */ 
	for(a=d->addresses;a;a=a->next) 
	{
		printf("\tAddress Family:#%d\n",a->addr->sa_family); 
    	/*关于 sockaddr_in 结构请参考程序后注释*/ 
		switch(a->addr->sa_family) 
		{ 
		  case AF_INET: 
			printf("\tAddress Family Name: AF_INET\n");//打印网络地址类型 
			if (a->addr)//打印IP地址 
			{
             sinaddr=((struct sockaddr_in *)a->addr)->sin_addr;
			 printf("\tAddress:%d.%d.%d.%d\n",
				 sinaddr.S_un.S_un_b.s_b1,sinaddr.S_un.S_un_b.s_b2,
				 sinaddr.S_un.S_un_b.s_b3,sinaddr.S_un.S_un_b.s_b4);
           	}
			if (a->netmask)//打印掩码 
			{ sinaddr=((struct sockaddr_in *)a->netmask)->sin_addr;
			  printf("\tNetmask:%s\n",inet_ntoa(sinaddr));
			}
			if (a->broadaddr)//打印广播地址 
			{   sinaddr=((struct sockaddr_in *)a->broadaddr)->sin_addr;
			    printf("\tBroadcast Address:%s\n",inet_ntoa(sinaddr));
			}
			if (a->dstaddr)//目的地址 
			{ sinaddr=((struct sockaddr_in *)a->dstaddr)->sin_addr;
			  printf("\tDestination Address:%s\n",inet_ntoa(sinaddr));
			}
			break; 
			
		  default:printf("\tAddress Family Name:Unknown\n"); break; 
		} /*end of switch*/
	} /*end of for */
	printf("\n"); 
} /*void ifprint(pcap_if_t *d) */
