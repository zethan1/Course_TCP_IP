typedef struct IP_HEAD //IPͷ���ṹ
{
	union
	{
		unsigned char Version;  //�汾���ֽ�ǰ4λ��
		unsigned char HeadLen;  //ͷ�����ȣ��ֽں�4λ��
	};
	unsigned char  ServiceType; //��������
	unsigned short TotalLen;    //�ܳ���
	unsigned short Identifier;  //��ʶ��
	union
	{
		unsigned short Flags;      //��־λ����ǰ3λ��
		unsigned short FragOffset; // Ƭƫ�ƣ��ֺ�13λ��
	};
	unsigned char TimeToLive;    //��������
	unsigned char Protocol;      //Э��
	unsigned short HeadChecksum; //ͷ��У���
	unsigned int   SourceAddr;   //ԴIP��ַ
	unsigned int   DestinAddr;   //Ŀ��IP��ַ
}ip_head;

typedef struct ICMP_HEAD      //ICMPͷ���ṹ
{
	unsigned char Type;     //����
	unsigned char Code;     //����
	unsigned short HeadChecksum; //ͷ��У���
	unsigned short Identifier;   //��ʶ��
	unsigned short Sequence;     //���
}icmp_head;

#define SIO_RCVALL  _WSAIOW(IOC_VENDOR,1)
#define IPV4_VERSION 4
#define IPV6_VERSION 6
#define ICMP_PACKET  1
#define IGMP_PACKET  2
#define TCP_PACKET   6
#define EGP_PACKET   8
#define UDP_PACKET   17
#define OSPF_PACKET  89