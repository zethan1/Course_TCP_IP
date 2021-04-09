/* 
* Copyright (c) 1999 - 2002 
* Politecnico di Torino. All rights reserved. 
* 
* Redistribution and use in source and binary 
forms, with or without 
* modification, are permitted provided that: (1) 
source code distributions 
* retain the above copyright notice and this 
paragraph in its entirety, (2) 
* distributions including binary code include 
the above copyright notice and 
* this paragraph in its entirety in the 
documentation or other materials 
* provided with the distribution, and (3) all 
advertising materials mentioning 
* features or use of this software display the 
following acknowledgement: 
* ``This product includes software developed by 
the Politecnico 
* di Torino, and its contributors.'' Neither the 
name of 
* the University nor the names of its 
contributors may be used to endorse 
* or promote products derived from this software 
without specific prior 
* written permission. 
* THIS SOFTWARE IS PROVIDED ``AS IS'' AND 
WITHOUT ANY EXPRESS OR IMPLIED 
* WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE 
IMPLIED WARRANTIES OF 
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
PURPOSE. 
*/ 
#include "pcap.h" 
#pragma comment(lib,"wpcap")
#pragma comment(lib,"ws2_32")
/* 4 BIT��IP��ַ���� */
typedef struct ip_address 
{ 	u_char byte1; 
	u_char byte2; 
	u_char byte3; 
	u_char byte4; 
}ip_address; 

/* IPv4 ͷ�Ķ��� */ 
typedef struct ip_header
{ 	u_char ver_ihl;     // 4 bit�İ汾��Ϣ + 4 bits�ı�ͷ�� 
	u_char tos;         // TOS����  
	u_short tlen;       // �ܳ��� 
	u_short identification; // Identification����ʶ�� 
	u_short flags_fo;     // Flags (3 bits��־) +Fragment offset (13 bitsƬƫ��) 
	u_char ttl;         // ������ 
	u_char proto;       // �����Э����Ϣ 
	u_short crc;         // ͷ��У��� 
	ip_address saddr;     // ԴIP 
	ip_address daddr;     // Ŀ��IP 
	u_int   op_pad;       // Option + Padding��ѡ��+����� 
}ip_header; 

/* UDP header*/ 
typedef struct udp_header
{ 
	u_short sport;       // Source port��Դ�˿ںţ� 
	u_short dport;       // Destination port��Ŀ�Ķ˿ںţ� 
	u_short len;         // Datagram length���ܳ��ȣ� 
	u_short crc;         // Checksum��У��ͣ�
}udp_header; 

/* ˵����������Ļص����� */ 
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data); 

int main() 
{ 
	pcap_if_t *alldevs; 
	pcap_if_t *d; 
	int inum; 
	int i=0; 
	pcap_t *adhandle; 
	char errbuf[PCAP_ERRBUF_SIZE]; 
	u_int netmask; 
	char packet_filter[] = "ip and udp"; 
	struct bpf_program fcode; 
	
	/* Retrieve the device list */ 
	if (pcap_findalldevs(&alldevs, errbuf) == -1) 
	{ 	fprintf(stderr,"Error in pcap_findalldevs: %s\n", errbuf); 
		exit(1); 
	} 
	
	/* Print the list */ 
	for(d=alldevs; d; d=d->next) 
	{ 
		printf("%d. %s", ++i, d->name); 
		if (d->description) printf(" (%s)\n", d->description); 
		else printf(" (No description available)\n"); 
	} 
	
	if(i==0) 
	{ printf("\nNo interfaces found! Make sure WinPcap is installed.\n"); 
	  return -1; 
	} 
	
	printf("Enter the interface number (1-%d):",i); 
	scanf("%d", &inum); 
	
	if(inum < 1 || inum > i) 
	{ printf("\nInterface number out of range.\n"); 
	  pcap_freealldevs(alldevs); 	/* Free the device list */ 
	  return -1; 
	} 
	
	/* Jump to the selected adapter */ 
	for(d=alldevs, i=0; i< inum-1 ;d=d->next, i++); 
	
	/* Open the adapter */ 
	if ( (adhandle= pcap_open_live(d->name, //name of the device 
		65536,   // portion of the packet to capture.  
		// 65536 grants that the whole packet will be captured on all the MACs. 
		1,       // promiscuous mode 
		1000,     // read timeout 
		errbuf   // error buffer 
		) ) == NULL) 
	{ fprintf(stderr,"\nUnable to open the adapter. %s is not supported by WinPcap\n"); 
	  pcap_freealldevs(alldevs); 	/* Free the device list */ 
	  return -1; 
	} 
	
	/* Check the link layer. We support only Ethernet for simplicity. */ 
	if(pcap_datalink(adhandle) != DLT_EN10MB) 
	{ 	fprintf(stderr,"\nThis program works only on Ethernet networks.\n"); 
		pcap_freealldevs(alldevs); /* Free the device list */
		return -1; 
	} 
	
	if(d->addresses != NULL) 
	/* Retrieve the mask of the first address of the interface */ 
	netmask=((struct sockaddr_in *)(d->addresses->netmask))->sin_addr.S_un.S_addr; 
	
	else /* If the interface is without addresses we suppose to be in a C class network */ 
	netmask=0xffffff; 	
	
	//compile the filter 
	if(pcap_compile(adhandle, &fcode, packet_filter, 1, netmask) <0 )
	{ 	fprintf(stderr,"\nUnable to compile the packet filter. Check the syntax.\n"); 
		pcap_freealldevs(alldevs); 	/* Free the device list */ 
		return -1; 
	} 
	
	//set the filter 
	if(pcap_setfilter(adhandle, &fcode)<0)
	{ 	fprintf(stderr,"\nError setting the filter.\n"); 
		pcap_freealldevs(alldevs); 	/* Free the device list */ 
		return -1; 
	} 
	
	printf("\nlistening on %s...\n", d->description); 
	
	/* At this point, we don't need any more the device list. Free it */ 
	pcap_freealldevs(alldevs); 
	
	/* start the capture */ 
	pcap_loop(adhandle, 0, packet_handler, NULL); 
	
	return 0; 
} 
								
  /* Callback function invoked by libpcap for every incoming packet */ 
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data) 
{ 
	struct tm *ltime; 
	char timestr[16]; 
	ip_header *ih; 
	udp_header *uh; 
	u_int ip_len; 
	u_short sport,dport; 
	
	/* convert the timestamp to readable format */ 
	
	ltime=localtime(&header->ts.tv_sec); 
	strftime( timestr, sizeof timestr, "%H:%M:%S", ltime); 
	
	/* print timestamp and length of the packet */ 
	
	printf("%s.%.6d len:%d ", timestr, header->ts.tv_usec, header->len); 
	
	/* �ҵ�IPͷ��λ�� */ 
	ih = (ip_header *) (pkt_data +14); //14Ϊ��̫ͷ�ĳ��� 
	
	/* �ҵ�UDP��λ�� */ 
	ip_len = (ih->ver_ihl & 0xf) * 4; 
	uh = (udp_header *) ((u_char*)ih + ip_len); 
	
	/* ���˿���Ϣ��������ת��Ϊ����˳�� */ 
	sport = ntohs( uh->sport ); 
	dport = ntohs( uh->dport ); 
	
	/* print ip addresses and udp ports */ 
	printf("%d.%d.%d.%d.%d -> %d.%d.%d.%d.%d\n", 
		ih->saddr.byte1, 
		ih->saddr.byte2, 
		ih->saddr.byte3, 
		ih->saddr.byte4, 
		sport, 
		ih->daddr.byte1, 
		ih->daddr.byte2, 
		ih->daddr.byte3, 
		ih->daddr.byte4, 
		dport); 
} 
