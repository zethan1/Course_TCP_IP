#include "pcap.h" 
#pragma comment(lib,"wpcap")
int main() 
{ 
	pcap_if_t *alldevs; 
	pcap_if_t *d; 
	int inum; 
	int i=0; 
	pcap_t *adhandle; 
	int res; 
	char errbuf[PCAP_ERRBUF_SIZE]; 
	struct tm *ltime; 
	char timestr[16]; 
	struct pcap_pkthdr *header; 
	const unsigned char *pkt_data; 
	
	
	/* Retrieve the device list */ 
	if (pcap_findalldevs(&alldevs, errbuf) == -1) 
	{ 	fprintf(stderr,"Error in pcap_findalldevs: %s\n", errbuf); 
		exit(1); 
	} 
	
	/* Print the list */ 
	for(d=alldevs; d; d=d->next) 
	{ 	printf("%d. %s", ++i, d->name); 
		if (d->description) printf(" (%s)\n", d->description); 
		else printf(" (No description available)\n"); 
	} 
	
	if(i==0) 
	{ 	printf("\nNo interfaces found! Make sure WinPcap is installed.\n"); 
		return -1; 
	} 
	
	printf("Enter the interface number (1-%d):",i); 
	scanf("%d", &inum); 
	
	if(inum < 1 || inum > i) 
	{ 	printf("\nInterface number out of range.\n"); 
		pcap_freealldevs(alldevs); 	/* Free the device list */ 
		return -1; 
	} 
	
	/* Jump to the selected adapter */ 
	for(d=alldevs, i=0; i< inum-1 ;d=d->next, i++); 
	
	/* Open the adapter */ 
	if ( (adhandle= pcap_open_live(d->name, // name of the device 
		65536,   // portion of the packet to capture.  
		// 65536 grants that the whole packet will be captured on all the MACs. 
		1,       // promiscuous mode 
		1000,     // read timeout 
		errbuf   // error buffer 
		) ) == NULL) 
	{ 	fprintf(stderr,"\nUnable to open the adapter. %s is not supported by WinPcap\n"); 
		pcap_freealldevs(alldevs); 	/* Free the device list */ 
		return -1; 
	} 
	
	printf("\nlistening on %s...\n", d->description); 
	
	/* At this point, we don't need any more the device list. Free it */ 
	pcap_freealldevs(alldevs); 
	
	/* 此处循环调用 pcap_next_ex来接受数据包*/ 
	while((res = pcap_next_ex( adhandle, &header,&pkt_data)) >= 0)
	{ 	if(res == 0) continue;    /* Timeout elapsed */ 
	
	    /* convert the timestamp to readable format */ 
		ltime=localtime(&header->ts.tv_sec); 
		strftime( timestr, sizeof timestr,"%H:%M:%S", ltime); 
		printf("%s,%.6d len:%d\n", timestr, header->ts.tv_usec, header->len); 
	} 
	
	if(res == -1)
	{ 	printf("Error reading the packets: %s\n",pcap_geterr(adhandle)); 
		return -1; 
	} 
	
	return 0; 
 }
