#include "pcap.h" 
#pragma comment(lib,"wpcap.lib")
void  main() 
 { 
   pcap_if_t *alldevs,*d; 
   int i=0; 
   char errbuf[PCAP_ERRBUF_SIZE];
   /* PCAP_ERRBUF_SIZE =256����pcap.h�ж��� */             
    
    if (pcap_findalldevs(&alldevs, errbuf) == -1) /* ���API��������������б� */
     {  fprintf(stderr,"Error in pcap_findalldevs: %s\n", errbuf); 
        return; 
     }  
     
	  /* ��ʾ�б����Ӧ�ֶε����� */ 
       for(d=alldevs;d;d=d->next) 
       {   printf("%d. %s", ++i, d->name); 
           if (d->description)  printf(" (%s)\n", d->description); 
           else  printf(" (No description available)\n"); 
	   } 
      
	   if(i==0) 
	   {printf("\nNo interfaces found! Make sure WinPcap is installed.\n"); 
        return; 
	   } 
  
	   /*We don't need any more the device list. Free it */ 
       pcap_freealldevs(alldevs); 
  }
