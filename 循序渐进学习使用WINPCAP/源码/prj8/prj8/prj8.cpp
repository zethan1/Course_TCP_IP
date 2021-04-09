 #include  "pcap.h" 
#pragma comment(lib,"wpcap")
int main(int argc, char **argv) 
{ 
	pcap_t *fp; 
	char error[PCAP_ERRBUF_SIZE]; 
	u_char packet[100]; 
	int i=0,inum; 
	pcap_if_t *alldevs,*d; 

	/* ����������б� */ 
	if (pcap_findalldevs(&alldevs, error) == -1) 
	{ 	fprintf(stderr,"Error in pcap_findalldevs: %s\n", error); 
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
	{   printf("\nInterface number out of range.\n"); 
		pcap_freealldevs(alldevs); /* Free the device list */ 
		return -1; 
	} 
	
	/* �ҵ�Ҫѡ��������ṹ */ 
	for(d=alldevs, i=0; i< inum-1 ;d=d->next, i++); 
	
	/* ��ѡ������� */ 
	if((fp = pcap_open_live(d->name, 100, 1, 1000, error) ) == NULL) 
	{ 
		fprintf(stderr,"\nError opening adapter: %s\n", error); 
		return -1; 
	} 
	
	/* �������绷��Ϊethernet�����Ű�Ŀ��MAC��Ϊ1:1:1:1:1:1*/ 
	packet[0]=1; packet[1]=1; packet[2]=1; 
	packet[3]=1; packet[4]=1; packet[5]=1; 
	
	/* ����ԴMACΪ 2:2:2:2:2:2 */ 
	packet[6]=2; packet[7]=2;  packet[8]=2; 
	packet[9]=2; packet[10]=2; packet[11]=2; 
	
	/* ��䷢�Ͱ���ʣ�ಿ�� */ 
	for(i=12;i<100;i++){ packet[i]=i%256;} 
	
	/* ���Ͱ� */ 
	pcap_sendpacket(fp,packet, 100); 
	
	return 0; 
}
