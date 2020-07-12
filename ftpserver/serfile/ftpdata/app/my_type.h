#ifndef MY_TYPE_H
#define MY_TYPE_H

#if defined(ANDROID)
#include <pthread.h>
#endif

#define VOID void



typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned long UINT32;
typedef unsigned long long UINT64;

typedef signed char SINT8;
typedef signed short SINT16;
typedef signed long SINT32;
typedef signed long long SINT64;
typedef float   FLOAT;
typedef double DOUBLE;
typedef int BOOL;
typedef char   CHAR;

#define SUCCESS (SINT32)0
#define ERROR (SINT32)-1
//#define NULL (CHAR *)0

#ifdef TRUE
#undef TRUE
#endif
#define TRUE (BOOL)1

#ifdef FALSE
#undef FALSE
#endif
#define FALSE (BOOL)0


#define DEBUG_ERROR(px) fprintf(stdout, "%s::%s:%s\n", __FUNCTION__, px, strerror(errno))
#define DEBUG_Printf(px,pstr) fprintf(stdout, "%s::%s:%s\n", __FUNCTION__, px, pstr)



#define MAX_CH_NUM 5
#define RXBUFLEN_MAX 1024  //SOCKET接收最大缓存区长度
#define TXBUFLEN_MAX 1024  //SOCKET发送最大缓存区长度
#define SENDDATALEN_MAX 1024  //单次最大发送字节数
#define READDATALEN_MAX 1024  //单次最大读取字节数

typedef enum {
	TCPCHID_0 = 0,
	TCPCHID_1,
	TCPCHID_2,
	TCPCHID_3,
	TCPCHID_4,
	TCPCHID_MAX,
	TCPCHID_INVALID = 0xFF
}T_TcpChid;

typedef enum{
	TCPCH_STATE_UNUSED = 0,
	TCPCH_STATE_USED
}T_TcpChState;

typedef struct {
	T_TcpChid tcpChID;
	SINT32 sockFd;
	pthread_t thId;
	pthread_mutex_t lock;
	CHAR TXBUF[TXBUFLEN_MAX];
	UINT16 TxdataLen;
	CHAR RXBUF[RXBUFLEN_MAX];
	UINT16 RxdataLen;
	T_TcpChState TcpchState;
}T_TcpContext;



#endif // MY_TYPE_H
