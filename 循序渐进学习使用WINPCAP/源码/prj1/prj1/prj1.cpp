#include "pcap.h" 
#pragma comment(lib,"wpcap.lib")
void  main() 
 { 
   pcap_if_t *alldevs,*d; 
   int i=0; 
   char errbuf[PCAP_ERRBUF_SIZE];
   /* PCAP_ERRBUF_SIZE =256，在pcap.h中定义 */             
    
	//1、找到所有网卡
    if (pcap_findalldevs(&alldevs, errbuf) == -1) /* 这个API用来获得网卡的列表 */
     {  fprintf(stderr,"Error in pcap_findalldevs: %s\n", errbuf); 
        return; 
     }  
     
	//2、显示找到的所有网卡信息
	  /* 显示列表的响应字段的内容 */ 
       for(d=alldevs;d;d=d->next) 
       {   printf("%d. %s", ++i, d->name); 
           if (d->description)  printf(" (%s)\n", d->description); 
           else  printf(" (No description available)\n"); 
	   } 
      
	   if(i==0) 
	   {printf("\nNo interfaces found! Make sure WinPcap is installed.\n"); 
        return; 
	   } 
  
	//3、释放所有网卡
	   /*We don't need any more the device list. Free it */ 
       pcap_freealldevs(alldevs); 
  }
