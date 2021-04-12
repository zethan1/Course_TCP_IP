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
/* 4 BIT的IP地址定义 */
typedef struct ip_address 
{ 	u_char byte1; 
	u_char byte2; 
	u_char byte3; 
	u_char byte4; 
}ip_address; 

/* IPv4 头的定义 */ 
typedef struct ip_header
{ 	u_char ver_ihl;     // 4 bit的版本信息 + 4 bits的报头长 
	u_char tos;         // TOS类型  
	u_short tlen;       // 总长度 
	u_short identification; // Identification（标识） 
	u_short flags_fo;     // Flags (3 bits标志) +Fragment offset (13 bits片偏移) 
	u_char ttl;         // 生存期 
	u_char proto;       // 后面的协议信息 
	u_short crc;         // 头部校验和 
	ip_address saddr;     // 源IP 
	ip_address daddr;     // 目的IP 
	u_int   op_pad;       // Option + Padding（选项+填充域） 
}ip_header; 

/* UDP header*/ 
typedef struct udp_header
{ 
	u_short sport;       // Source port（源端口号） 
	u_short dport;       // Destination port（目的端口号） 
	u_short len;         // Datagram length（总长度） 
	u_short crc;         // Checksum（校验和）
}udp_header; 

/* 说明：处理包的回调函数 */ 
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
	char packet_filter[] = "ip and udp"; 	//抓udp和ip的包，如果要tcp加上就行
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
	pcap_loop(adhandle, 0, packet_handler, NULL); //抓几个包，就把0改成几
	
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
	
	/* 找到IP头的位置 */ 
	ih = (ip_header *) (pkt_data +14); //14为以太头的长度 
	
	/* 找到UDP的位置 */ 
	ip_len = (ih->ver_ihl & 0xf) * 4; //0100 0101 & 0000 1111得到0000 0101
	uh = (udp_header *) ((u_char*)ih + ip_len); 
	
	/* 将端口信息从网络型转变为主机顺序 */ 
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

//清除缓冲更好的抓包
//ipconfig /flushdns

