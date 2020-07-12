#include<stdio.h>
#include<fcntl.h>
#include<termios.h>   //串口终端头文件
//以下3个头文件是socket头文件
#include<sys/socket.h>
#include<netdb.h>
#include<netinet/in.h>

//#define _USE_7520M_
#ifdef _USE_7520M_

#else
#include <ifaddrs.h>    // UC/Linux 没有这个头文件
#endif
#include<stdlib.h>
#include<unistd.h>    //系统调用必须包含的头文件
#include<string.h>    //字符串处理函数
#include<sys/types.h>
#include<sys/stat.h>
#include<errno.h>
#include<pthread.h>
#include <openssl/err.h>

#include <openssl/x509.h>
#include <openssl/bio.h>


#include "my_type.h"
#include "socket.h"
//#include "../inc/ext_at.h"
#include <pthread.h>

#define SOFTPATH "/cache/delta.package"
#define BROADCASTID "FFFFFFFFFFFFFF" 
#define true 1
#define false 0

#define KEEPALIVETIME 30
#define PINGTIME 10
#define PUBLISHDEVINFOTIME 20



#define SOCKET_DEBUG	1
#if SOCKET_DEBUG
#  define  SOCK_DBG(...)   fprintf(stdout, __VA_ARGS__)
#else
#  define  SOCK_DBG(...)   ((void)0)
#endif
BIO *in=NULL;
SSL *ssl;
SSL_CTX *ctx;

int myconnect(char *ipaddr, short port);
int myreceive(char *buf, size_t count);
int mysend(char *buf, size_t count);
int set_blocking(int flag, int timeout);
extern int cfg_get_item(char *name, char* buf, int bufLen);
char g_id[16] ={'F','F','F','F','F','F','F','F','F','F','F','F','F','F'};
char g_simImsi[32] = {'0','0','0','0','0','0','0','0','0','0','0','0','0','0','0'};
char g_localver[128] = {0};

lora_gateway_st *gateway;




typedef struct{
	int sockfd;
	int timeout;    // ms
	int blockflag;
	int (* connect)(char *ipaddr, short port);
	int (* receive)(char *buf, size_t count);
	int (* send)(char *buf, size_t count);
	int (* set_blocking)(int flag, int timeout);
}TCPSocketConnection;

TCPSocketConnection mysock = {
	.sockfd = -1,
	.timeout = 0,
	.blockflag = false,
	.connect = myconnect,
	.receive = myreceive,
	.send = mysend,
	.set_blocking = set_blocking
}; 

/*
void init(void)
{
	cfg_get_item("imei", g_id,sizeof(g_id) - 1);
	cfg_get_item("cr_version", g_localver, sizeof(g_localver) - 1);
	cfg_get_item("sim_imsi", g_simImsi, sizeof(g_simImsi) - 1);
}
*/
int myconnect(char *ipaddr, short port)
{
	int sockFd = -1;
    struct sockaddr_in sServer_addr;
    bzero(&sServer_addr, sizeof(struct sockaddr_in));
    sServer_addr.sin_family = AF_INET;
    sServer_addr.sin_addr.s_addr = inet_addr(ipaddr);
    sServer_addr.sin_port = htons(port);
    if((sockFd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("[mqtt] tcp socket is error! %s\n", strerror(errno));
        return false;
    }
    if(connect(sockFd, (struct sockaddr*)(&sServer_addr), sizeof(struct sockaddr)) == -1)
    {
        printf("[mqtt] tcp connet is error! %s\n", strerror(errno));
        close(sockFd);
        return false;
    }
    printf("[mqtt] tcp socket create successful! \n");
	mysock.sockfd = sockFd;
    return sockFd;
}
int get_mqttsocketfd(){
	return mysock.sockfd;
}


int mysend(char *data, size_t dataLen)
{
	unsigned short count = 0;
    unsigned int now = dataLen;
    do{
        count = send(mysock.sockfd, data ,now, 0);
        if(count<0)
        {
            return -1;
        }
        else if(count < now)
        {
            printf("now:%d\n", now);
            now = now - count;
            data = data + count;
        }
        else
        {
            break;
        }
      }while(now != 0);
	printf("[mqtt] send %d byte\n", dataLen);
    return dataLen;
}

int set_blocking(int flag, int timeout)
{
	mysock.blockflag = flag;
	mysock.timeout = timeout;
	return 0;
}

int myreceive(char *data, size_t dataLen)
{
	unsigned int fs_sel = 0;
    unsigned int rcvLen = -1;
    fd_set fs_read;
   	struct timeval  tv_timeout;
   	FD_ZERO(&fs_read);
   	FD_SET(mysock.sockfd, &fs_read);
	if(mysock.blockflag == true)
	{
    	tv_timeout.tv_sec = mysock.timeout / 1000;
    	tv_timeout.tv_usec = mysock.timeout % 1000 * 1000;
	}
	else
	{
		tv_timeout.tv_sec = 0;
    	tv_timeout.tv_usec = 5;
	}
   	fs_sel = select(mysock.sockfd + 1, &fs_read, NULL, NULL, &tv_timeout);
   	switch (fs_sel)
	{
   		case -1:
       		printf("[mqtt] select error!\n");
       		exit(0);
       		break;
   		case 0:
       		return 0;
       		break;
   		default:
       		if(FD_ISSET(mysock.sockfd, &fs_read))
 			{
				rcvLen = recv(mysock.sockfd, data, dataLen, 0);
				if (rcvLen > 0)
				{
            		printf("[mqtt] fs_sel readlen = %d\n",rcvLen);
             		return rcvLen;
          		}
          		else if(rcvLen < 0)
           		{
               		printf("[mqtt] read len = %d\n", rcvLen);
               		return -1;
           		}
        	}
        	return 0;
        	break;
    }
}

volatile T_TcpContext g_TcpContext[TCPCHID_MAX] = {0};
T_TcpContext * getTcpChContext(UINT8 chID)
{
	if(chID >= 0 || chID < TCPCHID_MAX)
	{
		return &g_TcpContext[chID];
	}
	else
	{
		return NULL;
	}
}

BOOL reMoveTcpContext(T_TcpContext *ptcpCtx)
{
	if(ptcpCtx->thId > 0)
	{
#ifndef ANDROID
		pthread_cancel(ptcpCtx->thId);
#endif        
		pthread_join(ptcpCtx->thId, NULL);
		ptcpCtx->thId = -1;
	}
	ptcpCtx->TcpchState = TCPCH_STATE_UNUSED;
	if(ptcpCtx->sockFd > 0)
	{
		close(ptcpCtx->sockFd);
		ptcpCtx->sockFd = -1;
	}
	pthread_mutex_destroy(&(ptcpCtx->lock));
	ptcpCtx->RxdataLen = 0;
	ptcpCtx->TxdataLen = 0;
	ptcpCtx->tcpChID = TCPCHID_INVALID;
	return TRUE;
}

SINT32 sOck_CreateTcpConn(UINT8 *ipaddr, UINT16 port)
{
	SINT32 sockFd = -1;
	struct sockaddr_in sServer_addr;
	char ipbuf[20];
	struct hostent *hostenta;
	hostenta = malloc(sizeof(struct hostent));
	hostenta->h_name="";
	hostenta = gethostbyname(hostenta->h_name);
	printf("hostenta is %s\r\n",hostenta);
	if(hostenta!=NULL)
	{	int n=0;
		for(;hostenta->h_addr_list[n];n++){
		inet_ntop(AF_INET,hostenta->h_addr_list[n],ipbuf,sizeof(ipbuf));
		printf(" ip is %s\r\n",ipbuf);
		}
	}
	else{
		printf("get ip fail1\r\n");
		return -1;
	}
	bzero(&sServer_addr, sizeof(struct sockaddr_in));
	sServer_addr.sin_family = AF_INET;
	sServer_addr.sin_addr.s_addr = inet_addr(ipaddr);
	sServer_addr.sin_port = htons(port);
	//fprintf(stdout, "sServer_addr.sin_addr.s_addr%s\n", sServer_addr.sin_addr.s_addr);
	if((sockFd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		fprintf(stdout, "tcp socket is error! %s\n", strerror(errno));
		return -1;
	}
	SOCK_DBG("sockFd %d\n", sockFd);
	if(connect(sockFd, (struct sockaddr*)(&sServer_addr), sizeof(struct sockaddr)) == -1)
	{
		fprintf(stdout, "tcp connet is error! %s\n", strerror(errno));
		close(sockFd);
		return -1;
	}
	fprintf(stdout, "tcp socket creat successful! \n");

	return sockFd;
}
#if 0
int sOck_checkTcpConn(SINT32 sockFd)
{
	struct tcp_info info; 
  	int len=sizeof(info); 
  	getsockopt(sockFd, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len); 
  	if((info.tcpi_state==TCP_ESTABLISHED))
		return 0;
	else
		return -1;
}
#endif
int getdata(char* buf, int count)
{	
	return read(mysock.sockfd, buf, (size_t)count);
}

int get_ssl_error(){

	int errcode = -1;
	errcode = SSL_get_error(ssl,-1);
	//read(ssl,buf,sizeof(buf));
	printf("ssl errcode %d\r\n",errcode);
	
	return errcode;
}

int mqtt_CreatedTcpConn(char *ipaddr, short port){
	int i = -1;
	char ipbuf[20];
	#if 0
	struct hostent *hostenta;
	hostenta = malloc(sizeof(struct hostent));



	hostenta->h_name = ipaddr;


	hostenta = gethostbyname(hostenta->h_name);
	
	printf("hostenta is %s\r\n",hostenta);
	
	if(hostenta!=NULL)
	{	int n=0;
		for(;hostenta->h_addr_list[n];n++)
		{
			inet_ntop(AF_INET,hostenta->h_addr_list[n],ipbuf,sizeof(ipbuf));
			printf("%s ip is %s\r\n",ipaddr,ipbuf);
			//printf("iothubjqqa.saic-gm.com ip is %s\r\n",ipbuf);
		}
	}
	else{
		printf("get ip fail1\r\n");
		free(hostenta);
		return -1;
	}
	if(hostenta != NULL)free(hostenta);
	#endif
	printf("ipaddr %s\r\n",ipaddr);
	i = mysock.connect(ipaddr, port);

		if(i < 0)
		{
			printf("[mqtt] connect MQTT SERVER fail, sleep(10s) try again\n");

		}
		if(i > 0){
			struct timeval ti;	 
			ti.tv_sec=1;  
			ti.tv_usec=0;  
			setsockopt(mysock.sockfd,SOL_SOCKET,SO_RCVTIMEO,&ti,sizeof(ti));
			setsockopt(mysock.sockfd,SOL_SOCKET,SO_SNDTIMEO,&ti,sizeof(ti));	
			 
		}
		if(i == 0)i=-1;
	return i;
}
SINT32 sOck_CreateUdpConn(UINT8 *ipaddr, UINT16 port)
{
	SINT32 sockFd = -1;
	struct sockaddr_in sServer_addr;
	bzero(&sServer_addr, sizeof(struct sockaddr_in));
	sServer_addr.sin_family = AF_INET;
	sServer_addr.sin_addr.s_addr = inet_addr(ipaddr);
	sServer_addr.sin_port = htons(port);
	if((sockFd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		fprintf(stdout, "udp socket is error! %s\n", strerror(errno));
		return -1;
	}
	if(connect(sockFd, (struct sockaddr*)(&sServer_addr), sizeof(struct sockaddr)) == -1)
	{
		fprintf(stdout, "tcp connet is error! %s\n", strerror(errno));
		close(sockFd);
		return -1;
	}
	SOCK_DBG("sockFd %d\n", sockFd);
	fprintf(stdout, "udp socket creat successful! \n");
	return sockFd;
}

SINT32 sOck_ReadSockData(SINT32 sockFd, UINT8 *data, UINT16 dataLen)
{
	SINT32 fs_sel = 0;
	SINT32 rcvLen = -1;
	fd_set fs_read;
	struct timeval  tv_timeout;

	FD_ZERO(&fs_read);
	FD_SET(sockFd, &fs_read);
	tv_timeout.tv_sec = 0;
	tv_timeout.tv_usec = 5000;
	fs_sel = select(sockFd + 1, &fs_read, NULL, NULL, &tv_timeout);
	switch (fs_sel) {
		case -1:
			SOCK_DBG("select error!\n");
			exit(0);
			break;
		case 0:
			return 0;
			break;
		default:
			if(FD_ISSET(sockFd, &fs_read))
			{
				rcvLen = recv(sockFd, data, dataLen, 0);
				if (rcvLen > 0)
				{
					SOCK_DBG("fs_sel readlen = %d\n",rcvLen);
					return rcvLen;
				}
				else
				{
					SOCK_DBG("read error len = %d\n", rcvLen);
					return rcvLen;
				}
			}
			return 0;
			break;
	}
}

SINT32 sOck_WriteSockData(SINT32 sockFd, UINT8 *data, UINT16 dataLen)
{
	UINT16 count = 0;
	SINT32 now = dataLen;
	do{
		count = send(sockFd, data ,now, 0);
		SOCK_DBG("send : %s \n", data);
		if(count<0)
		{
			return -1;
		}
		else if(count < now)
		{
			SOCK_DBG("now:%d\n", now);
			now = now - count;
			data = data + count;
		}
		else
		{
			break;
		}
	}while(now != 0);
	return dataLen;
}

#if 0
VOID readUdpBuf(T_UdpContext *pudpCtx, UINT8 *buf, UINT16 buflen)
{
	pthread_mutex_lock(&(pudpCtx->lock));
	SOCK_DBG("mutex lock\n");
	CHAR * udpBuf = pudpCtx->RXBUF;
	UINT16 dataLen = pudpCtx->RxdataLen;
	if((dataLen + buflen) < RXBUFLEN_MAX)
	{
		memcpy(udpBuf + dataLen, buf, buflen);
		pudpCtx->RxdataLen += buflen;
	}
	pthread_mutex_unlock(&(pudpCtx->lock));
	SOCK_DBG("mutex unlock\n");
}
#endif

VOID readTcpBuf(T_TcpContext *ptcpCtx, UINT8 *buf, UINT16 buflen)
{
	pthread_mutex_lock(&(ptcpCtx->lock));
	SOCK_DBG("mutex lock\n");
	CHAR * tcpBuf = ptcpCtx->RXBUF;
	UINT16 dataLen = ptcpCtx->RxdataLen;
	if((dataLen + buflen) < RXBUFLEN_MAX)
	{
		memcpy(tcpBuf + dataLen, buf, buflen);
		ptcpCtx->RxdataLen += buflen;
	}
	pthread_mutex_unlock(&(ptcpCtx->lock));
	SOCK_DBG("mutex unlock\n");
}

VOID listening_tcp(UINT8 chID)
{
	SINT32 ret = 0;
	T_TcpContext * pTcpContext = getTcpChContext(chID);
	if (NULL == pTcpContext)
	{
		fprintf(stdout,"get data form %d TCP channel failed!! ",chID);
		return ;
	}
	while(pTcpContext->TcpchState == TCPCH_STATE_USED)
	{

		if (pTcpContext->TxdataLen > 0)
		{
			pthread_mutex_lock(&(pTcpContext->lock));
			SOCK_DBG("mutex lock\n");
			SOCK_DBG("sockfd : %d\n", pTcpContext->sockFd);
			ret = sOck_WriteSockData(pTcpContext->sockFd, pTcpContext->TXBUF, pTcpContext->TxdataLen);
			SOCK_DBG("ret : %d   send socket : %s \n", ret, pTcpContext->TXBUF);
			if(ret > 0)
			{
				pTcpContext->TxdataLen = pTcpContext->TxdataLen - ret;
				if(pTcpContext->TxdataLen != 0)
				{
					CHAR buf[TXBUFLEN_MAX] = {0};
					memcpy(buf, pTcpContext->TXBUF + ret,pTcpContext->TxdataLen);
					memcpy(pTcpContext->TXBUF, buf, pTcpContext->TxdataLen);
					fprintf(stdout,"write %d data to server !!\n",pTcpContext->TxdataLen);
				}
			}
			pthread_mutex_unlock(&(pTcpContext->lock));
			SOCK_DBG("mutex unlock\n");
		}


		if(pTcpContext->RxdataLen < RXBUFLEN_MAX)
		{
			UINT8 rxdata[RXBUFLEN_MAX] = {0};
			ret = sOck_ReadSockData(pTcpContext->sockFd, rxdata, 100);
			if(ret > 0)
			{
				readTcpBuf(pTcpContext, rxdata, ret);
				fprintf(stdout, "read %d data from server !!\n", ret);
			}
			else if(ret == -1 && errno != EINTR)
			{
				break;
			}

		}
	}
	pTcpContext->tcpChID = -1;
	reMoveTcpContext(pTcpContext);
	SOCK_DBG("listen thread exit\n");
	return;
}

#if 0
VOID listening_udp(UINT8 chID)
{
	SINT32 ret = 0;
	T_UdpContext * pUdpContext = getUdpChContext(chID);
	if (NULL == pUdpContext)
	{
		fprintf(stdout,"get data form %d UDP channel failed!! ",chID);
		return ;
	}
	while(pUdpContext->UdpchState == UDPCH_STATE_USED)
	{

		if (pUdpContext->TxdataLen > 0)
		{
			pthread_mutex_lock(&(pUdpContext->lock));
			SOCK_DBG("mutex lock\n");
			SOCK_DBG("sockfd : %d\n", pUdpContext->sockFd);
			ret = sOck_WriteSockData(pUdpContext->sockFd, pUdpContext->TXBUF, pUdpContext->TxdataLen);
			SOCK_DBG("ret : %d   send socket : %s \n", ret, pUdpContext->TXBUF);
			if(ret > 0)
			{
				pUdpContext->TxdataLen = pUdpContext->TxdataLen - ret;
				if(pUdpContext->TxdataLen != 0)
				{
					CHAR buf[TXBUFLEN_MAX] = {0};
					memcpy(buf, pUdpContext->TXBUF + ret,pUdpContext->TxdataLen);
					memcpy(pUdpContext->TXBUF, buf, pUdpContext->TxdataLen);
					fprintf(stdout,"write %d data to server !!\n",pUdpContext->TxdataLen);
				}
			}
			pthread_mutex_unlock(&(pUdpContext->lock));
			SOCK_DBG("mutex unlock\n");
		}


		if(pUdpContext->RxdataLen < RXBUFLEN_MAX)
		{
			UINT8 rxdata[RXBUFLEN_MAX] = {0};
			ret = sOck_ReadSockData(pUdpContext->sockFd, rxdata, 100);
			if(ret > 0)
			{
				readUdpBuf(pUdpContext, rxdata, ret);
				fprintf(stdout, "read %d data from server !!\n");
			}

		}
	}

	SOCK_DBG("listen thread exit\n");
	return;
}

void *sOck_udplisten(void *arg)
{
	UINT8 udpChID = (UINT8)arg;
	listening_udp(udpChID);
	return NULL;
}
#endif

void *sOck_tcplisten(void *arg)
{
	UINT8 tcpChID = (UINT8)arg;

	SOCK_DBG("sOck_tcplisten\n");
	listening_tcp(tcpChID);
	return NULL;
}



void getLocalTime(char *buf)
{
    char year[5] = {0};
    char mon[3] = {0};
    char day[3] ={0};
    char hour[3] = {0};
    char min[3] = {0};
    char sec[3] = {0};
    int tmp = 0;
    time_t now;
    struct tm *timenow;
    time(&now);
    timenow = localtime(&now);
    sprintf(year, "%d", timenow->tm_year+1900);

    tmp = timenow->tm_mon + 1;
    if(tmp> 9)
    {
        sprintf(mon, "%d", tmp);
    }
    else
    {
        mon[0] = '0';
        mon[1] = '0' + tmp;
    }

    tmp = timenow->tm_mday;
    if(tmp> 9)
    {
        sprintf(day, "%d", tmp);
    }
    else
    {
        day[0] = '0';
        day[1] = '0' + tmp;
    }

    tmp = timenow->tm_hour;
    if(tmp> 9)
    {
        sprintf(hour, "%d", tmp);
    }
	else
	{
		hour[0] = '0';
		hour[1] = '0' + tmp;
	}
	
	tmp = timenow->tm_min;
	if(tmp> 9)
	{
		sprintf(min, "%d", tmp);
	}
	else
	{
		min[0] = '0';
		min[1] = '0' + tmp;
	}
	
	tmp = timenow->tm_sec;
	if(tmp> 9)
	{
		sprintf(sec, "%d", tmp);
	}
	else
	{
		sec[0] = '0';
		sec[1] = '0' + tmp;
	}
	sprintf(buf, "%s-%s-%s %s:%s:%s", year, mon, day,hour,min,sec);
}


int mqtt_connect_info(lora_gateway_st *lora_gateway){
	int len = 0;
	int rs = 0;
	int rc = 0;
	char buf[350];
	char bufRead[350];
    int buflen = sizeof(buf);
	int msgid = 1;
	int req_qos = 0;
	int connCount = 0;
	unsigned char sessionPresent = 0;
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    
    
    data.keepAliveInterval = KEEPALIVETIME;
    data.cleansession = 1;
	data.willFlag = 0;
	data.will.topicName.cstring = "will_TPC";  //遗嘱主题

	data.clientID.cstring = g_id;
	data.will.message.cstring = g_id;    //遗嘱内容
	data.username.cstring = "zhaodi";
	data.password.cstring = "053073";


	cfg_get_item("lora_id", g_id,sizeof(g_id) - 1);
	time_t t1, t2, t3, t4;
	time(&t1);
	time(&t3);
    char payload[512] = {0};
    int payloadlen = 0;
	int pingConut = 0;

	mysock.set_blocking(true, 300);  /* 3 second Timeout */
	len = MQTTSerialize_connect(buf, buflen, &data);

	do
	{
	printf("bk[mqtt conn] connTimes %d\n",connCount++);
	pthread_mutex_lock(&(lora_gateway->lock_rw));
	rs = write(mysock.sockfd,buf,len);
	pthread_mutex_unlock(&(lora_gateway->lock_rw));
	sleep(1);
	printf("bk[mqtt conn] send success\n");
	pthread_mutex_lock(&(lora_gateway->lock_rw));
	rc = MQTTPacket_read(bufRead, buflen, getdata);
	pthread_mutex_unlock(&(lora_gateway->lock_rw));

	/* wait for connack */
		if (rc == CONNACK)
		{
				int connack_rc = 0;
		
			if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, bufRead, buflen) != 1 || connack_rc != 0)
			{
				printf("bk[mqtt conn] Unable to connect, return code %d\n", connack_rc);
				goto exit;
			}
			else
			{
				printf("[mqtt conn] break");
				break;
			}
		}
		else
		{
			printf("[mqtt conn] no connack\n");
			//goto exit;
			
		}
	}while(connCount <= 20);
	if(connCount >= 20) return -1;
	gateway = lora_gateway;
	return 1;
	exit:
	printf("[mqtt] entry exit: mqtt_connect !!!\n");
	close(mysock.sockfd);
	return -1;
}

#if 0
int main(int argc, char **argv)
{
	if(argc < 3)
	{
		SOCK_DBG("./main ip port\n");
		exit(0);
	}
	short port = atoi(argv[2]);
	char * ip = argv[1];
	int fd = sOck_CreateTcpConn("139.196.201.191", 8888);
	if(fd < 0)
	{
		fprintf(stderr, "sOck_CreateTcpConn() error: %s\n", strerror(errno));
		exit(0);
	}
	char str[] = "hello";
	char buf[RXBUFLEN_MAX] ={0};
	sOck_WriteSockData(fd, str, strlen(str));
	while(!sOck_ReadSockData(fd, buf, RXBUFLEN_MAX))
	{

	}
	SOCK_DBG("buf : %s\n", buf);
	close(fd);
	return 0;

}
#endif
