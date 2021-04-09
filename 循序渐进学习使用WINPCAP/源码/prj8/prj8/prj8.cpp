 #include  "pcap.h" 
#pragma comment(lib,"wpcap")
int main(int argc, char **argv) 
{ 
	pcap_t *fp; 
	char error[PCAP_ERRBUF_SIZE]; 
	u_char packet[100]; 
	int i=0,inum; 
	pcap_if_t *alldevs,*d; 

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
		return -1; 
	} 
	
	printf("Enter the interface number (1-%d):",i); 
	scanf("%d", &inum);//输入要选择打开的网卡号 
	
	if(inum < 1 || inum > i) //判断号的合法性 
	{   printf("\nInterface number out of range.\n"); 
		pcap_freealldevs(alldevs); /* Free the device list */ 
		return -1; 
	} 
	
	/* 找到要选择的网卡结构 */ 
	for(d=alldevs, i=0; i< inum-1 ;d=d->next, i++); 
	
	/* 打开选择的网卡 */ 
	if((fp = pcap_open_live(d->name, 100, 1, 1000, error) ) == NULL) 
	{ 
		fprintf(stderr,"\nError opening adapter: %s\n", error); 
		return -1; 
	} 
	
	/* 假设网络环境为ethernet，我门把目的MAC设为1:1:1:1:1:1*/ 
	packet[0]=1; packet[1]=1; packet[2]=1; 
	packet[3]=1; packet[4]=1; packet[5]=1; 
	
	/* 假设源MAC为 2:2:2:2:2:2 */ 
	packet[6]=2; packet[7]=2;  packet[8]=2; 
	packet[9]=2; packet[10]=2; packet[11]=2; 
	
	/* 填充发送包的剩余部分 */ 
	for(i=12;i<100;i++){ packet[i]=i%256;} 
	
	/* 发送包 */ 
	pcap_sendpacket(fp,packet, 100); 
	
	return 0; 
}
