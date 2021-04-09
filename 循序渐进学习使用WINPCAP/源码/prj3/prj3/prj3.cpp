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
	
	/* 获得网卡的列表 */ 
	if (pcap_findalldevs(&alldevs, errbuf) == -1) 
	{ 	fprintf(stderr,"Error in pcap_findalldevs: %s\n", errbuf); 
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
		return -1; 
	} 
	
	printf("Enter the interface number (1-%d):",i); 
	scanf("%d", &inum);//输入要选择打开的网卡号 
	
	if(inum < 1 || inum > i) //判断号的合法性 
	{  printf("\nInterface number out of range.\n"); 
		/* Free the device list */ 
		pcap_freealldevs(alldevs); 
		return -1; 
	} 
	
	/* 找到要选择的网卡结构 */ 
	for(d=alldevs, i=0; i< inum-1 ;d=d->next, i++); 
	
	/* 打开选择的网卡 */ 
	if ( (adhandle= pcap_open_live(d->name, // 设备名称 
		65536,   // portion of the packet to capture.  
		// 65536 grants that the whole packet will be captured on all the MACs. 
		1,       // 混杂模式 
		1000,     // 读超时为1秒 
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
	
	/* 开始捕获包 */
	//NULL为额外参数，改为"test"设置额外参数
	pcap_loop(adhandle, 0, packet_handler, "test"); //如果0改为1，则抓1个包
	return 0; 
} 
/* 对每一个到来的数据包调用该函数 */ 
void packet_handler(u_char *param, const struct 
					pcap_pkthdr *header, const u_char *pkt_data) 
{ 
	struct tm *ltime; 
	char timestr[16]; 
	
	/* 将时间戳转变为易读的标准格式*/ 
	//补充
	static int cnt = 0;
	cnt++;
	printf("第%d个包，参数=%s", cnt,param);
	
	
	ltime=localtime(&header->ts.tv_sec); 
	strftime( timestr, sizeof timestr, "%H:%M:%S", ltime); 
	printf("%s,%.6d len:%d\n", timestr, header->ts.tv_usec, header->len); 
}
