typedef struct IP_HEAD //IP头部结构
{
	union
	{
		unsigned char Version;  //版本（字节前4位）
		unsigned char HeadLen;  //头部长度（字节后4位）
	};
	unsigned char  ServiceType; //服务类型
	unsigned short TotalLen;    //总长度
	unsigned short Identifier;  //标识符
	union
	{
		unsigned short Flags;      //标志位（字前3位）
		unsigned short FragOffset; // 片偏移（字后13位）
	};
	unsigned char TimeToLive;    //生存周期
	unsigned char Protocol;      //协议
	unsigned short HeadChecksum; //头部校验和
	unsigned int   SourceAddr;   //源IP地址
	unsigned int   DestinAddr;   //目的IP地址
}ip_head;

typedef struct ICMP_HEAD      //ICMP头部结构
{
	unsigned char Type;     //类型
	unsigned char Code;     //代码
	unsigned short HeadChecksum; //头部校验和
	unsigned short Identifier;   //标识符
	unsigned short Sequence;     //序号
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