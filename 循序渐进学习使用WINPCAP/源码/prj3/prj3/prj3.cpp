#include "pcap.h" 
#pragma comment(lib,"wpcap.lib")
/* prototype of the packet handler */ 
void packet_handler(u_char *param, const struct 
					pcap_pkthdr *header, const u_char *pkt_data); 
int main() 
{ 
	pcap_if_t *alldevs,*d; 
	int inum,i=0; 
	pcap_t *adhandle; 
	char errbuf[PCAP_ERRBUF_SIZE]; 
	
	/* ����������б� */ 
	if (pcap_findalldevs(&alldevs, errbuf) == -1) 
	{ 	fprintf(stderr,"Error in pcap_findalldevs: %s\n", errbuf); 
		exit(1); 
	} 
	
	/* ��ӡ������Ϣ */ 
	for(d=alldevs; d; d=d->next) 
	{ 	printf("%d. %s", ++i, d->name); 
		if (d->description) printf(" (%s)\n", d->description); 
		else printf(" (No description available)\n"); 
	} 
	
	if(i==0) 
	{   printf("\nNo interfaces found! Make sure WinPcap is installed.\n"); 
		return -1; 
	} 
	
	printf("Enter the interface number (1-%d):",i); 
	scanf("%d", &inum);//����Ҫѡ��򿪵������� 
	
	if(inum < 1 || inum > i) //�жϺŵĺϷ��� 
	{  printf("\nInterface number out of range.\n"); 
		/* Free the device list */ 
		pcap_freealldevs(alldevs); 
		return -1; 
	} 
	
	/* �ҵ�Ҫѡ��������ṹ */ 
	for(d=alldevs, i=0; i< inum-1 ;d=d->next, i++); 
	
	/* ��ѡ������� */ 
	if ( (adhandle= pcap_open_live(d->name, // �豸���� 
		65536,   // portion of the packet to capture.  
		// 65536 grants that the whole packet will be captured on all the MACs. 
		1,       // ����ģʽ 
		1000,     // ����ʱΪ1�� 
		errbuf   // error buffer 
		) ) == NULL) 
	{  fprintf(stderr,"\nUnable to open the adapter. %s is not supported by WinPcap\n"); 
		/* Free the device list */ 
		pcap_freealldevs(alldevs); 
		return -1; 
	} 
	
	printf("\nlistening on %s...\n", d->description); 
	
	/* At this point, we don't need any more the device list. Free it */ 
	pcap_freealldevs(alldevs); 
	
	/* ��ʼ����� */ 
	pcap_loop(adhandle, 0, packet_handler, NULL); 
	return 0; 
} 
/* ��ÿһ�����������ݰ����øú��� */ 
void packet_handler(u_char *param, const struct 
					pcap_pkthdr *header, const u_char *pkt_data) 
{ 
	struct tm *ltime; 
	char timestr[16]; 
	
	/* ��ʱ���ת��Ϊ�׶��ı�׼��ʽ*/ 
	ltime=localtime(&header->ts.tv_sec); 
	strftime( timestr, sizeof timestr, "%H:%M:%S", ltime); 
	printf("%s,%.6d len:%d\n", timestr, header->ts.tv_usec, header->len); 
}
