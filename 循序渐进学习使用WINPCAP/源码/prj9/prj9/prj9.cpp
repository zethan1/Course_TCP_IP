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
#pragma comment(lib, "wpcap")
void dispatcher_handler(u_char *,  const struct pcap_pkthdr *, const u_char *); 


void main(void) 
{ 
	pcap_t *fp; 
	char error[PCAP_ERRBUF_SIZE]; 
	struct timeval st_ts; 
	u_int netmask; 
	struct bpf_program fcode; 
	
     pcap_if_t *alldevs,*d;
     int i=0,inum; 

		/* 获得网卡的列表 */ 
	if (pcap_findalldevs(&alldevs, error) == -1) 
	{ 	fprintf(stderr,"Error in pcap_findalldevs: %s\n", error); 
		exit(1); 
	} 
	
	/* 打印网卡信息 */ 
	for(d=alldevs; d; d=d->next) 
	{ 	printf("%d. %s", ++i, d->name); 
		if (d->description) printf(" (%s)\n", d->description); 
		else printf(" (No description available)\n"); 
	} 
	
	if(i==0) 
	{   printf("\nNo interfaces found! Make sure WinPcap is installed.\n"); 
		return ; 
	} 
	
	printf("Enter the interface number (1-%d):",i); 
	scanf("%d", &inum);//输入要选择打开的网卡号 
	
	if(inum < 1 || inum > i) //判断号的合法性 
	{   printf("\nInterface number out of range.\n"); 
		pcap_freealldevs(alldevs); /* Free the device list */ 
		return ; 
	} 
	
	/* 找到要选择的网卡结构 */ 
	for(d=alldevs, i=0; i< inum-1 ;d=d->next, i++); 
	
	/* 打开选择的网卡 */ 
   	if((fp = pcap_open_live(d->name, 100, 1, 1000, error) ) == NULL) 
	{ 
		fprintf(stderr,"\nError opening adapter: %s\n", error); 
		return; 
	} 
	
	/* Don't care about netmask, it won't be used 
	for this filter */ 
	netmask=0xffffff;  
	
	//compile the filter 
	if(pcap_compile(fp, &fcode, "tcp", 1, netmask)<0 )
	{ 
		fprintf(stderr,"\nUnable to compile the packet filter. Check the syntax.\n"); 
		return; 
	} 
	
	//set the filter 
	if(pcap_setfilter(fp, &fcode)<0)
	{ 
		fprintf(stderr,"\nError setting the filter.\n"); 
		return; 
	} 
	

	pcap_setmode(fp, MODE_STAT); 	/* 将网卡设置为统计模式 */ 
	
	printf("TCP traffic summary:\n"); 
	
	/* Start the main loop */ 
	pcap_loop(fp, 0, dispatcher_handler,(PUCHAR)&st_ts); 
	return; 
} 

void dispatcher_handler(u_char *state, const struct pcap_pkthdr *header, 
						const u_char *pkt_data) 
{ 
	struct timeval *old_ts = (struct timeval *)state; 
	u_int delay; 
	LARGE_INTEGER Bps,Pps; 
	struct tm *ltime; 
	char timestr[16]; 
	
	/* 从最近一次的采样以微秒计算延迟时间 */ 
	/* This value is obtained from the timestamp 
	that the associated with the sample. */ 
	delay=(header->ts.tv_sec - old_ts->tv_sec) * 
		1000000 - old_ts->tv_usec + header->ts.tv_usec; 
	/* 获得每秒的比特数 */ 
	Bps.QuadPart=(((*(LONGLONG*)(pkt_data + 8)) * 
		8 * 1000000) / (delay)); 
		/*                             ^     ^ 
		|     | 
		|     |  
		
		  |     | 
		  converts bytes in bits --     | 
		  | 
		  delay is expressed in microseconds 
		  -- 
	*/ 
	
	/* 获得每秒的数据包数 */ 
	Pps.QuadPart=(((*(LONGLONG*)(pkt_data)) * 1000000) / (delay)); 
	
	/* 将时间戳转变为可读的标准格式 */ 
	ltime=localtime(&header->ts.tv_sec); 
	strftime( timestr, sizeof timestr, "%H:%M:%S", ltime); 
	
	printf("%s ", timestr); 	/* Print timestamp*/ 
	
	/* Print the samples */ 
	printf("BPS=%I64u ", Bps.QuadPart); 
	printf("PPS=%I64u\n", Pps.QuadPart); 
	
	//store current timestamp 
	old_ts->tv_sec=header->ts.tv_sec; 
	old_ts->tv_usec=header->ts.tv_usec; 
}
