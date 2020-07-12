/*
 * The contents of this file are subject to the Mozilla Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy
 * of the License at http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an
 * "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
 * The Original Code is State Machine Compiler (SMC).
 *
 * The Initial Developer of the Original Code is Charles W. Rapp.
 * Portions created by Charles W. Rapp are
 * Copyright (C) 2000 - 2003 Charles W. Rapp.
 * All Rights Reserved.
 *
 * Contributor(s):
 *      Port to C by Francois Perrad, francois.perrad@gadz.org
 *
 * Function
 *  Main
 *
 * Description
 *  This routine starts the finite state machine running.
 *
 * RCS ID
 * $Id: main.c,v 1.3 2014/07/12 10:41:45 fperrad Exp $
 *
 * CHANGE LOG
 * $Log: main.c,v $
 * Revision 1.3  2014/07/12 10:41:45  fperrad
 * remove _rcs_id
 *
 * Revision 1.2  2007/08/05 13:36:32  cwrapp
 * Version 5.0.1 check-in. See net/sf/smc/CODE_README.txt for more information.
 *
 * Revision 1.1  2005/06/16 18:00:15  fperrad
 * Added C examples 1 - 4
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <termios.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <syslog.h>
#include <string.h>


#include "app_common.h"
#include "socket.h"
#include "lora_interface.h"
#include "MQTTPacket.h"
#include "mqtt.h"
#include "mqtt_pal.h"
#include "zte_web_cjson.h"

#include "my_type.h"

#define WAITACK  1
#define RESPON   0
#define WAITINIT 3

#ifndef NO_NVRAM
extern int cfg_get_item(char *name, char* buf, int bufLen);
#define CFG_STR_SIZE  128
#endif

extern int cfg_set(const char * name,const char * value);
extern int cfg_save(void);


int awaitack = WAITINIT;
T_sLoraData ldack = {0};
T_sLoraData ldWrite = {0};
T_sLoraData ldRead = {0};
que * q;

UINT8 tempdata[1024] ="";

void returnsystem(char * str, int *num, char b[])//传入三个参数 第一个传入需要解析字符串， 第二个传入返回整形 ，第三个传入返回字符串
{
	char a[100];

	int r = 0;
	int p, q, acount = 0;
	int bcount = 0;
	while (1)
	{
		while (str[r] && (str[r]<'0' || str[r]>'9'))
			r++;
		if (str[r])
		{
			p = r;
			q = r + 1;
			a[acount] = str[r];
			acount++;
			while (str[q] >= '0'&&str[q] <= '9')
			{
				a[acount] = str[q];
				acount++;
				q++;
			}
		}
		break;
	}

	while (str[q] != ':')
		q++;
	bcount = 0;
	q = q + 1;
	b[bcount] = str[q];
	bcount++;
	q++;
	while (str[q] != '\0')
	{
		b[bcount] = str[q];
		bcount++;
		q++;
	}
	b[bcount] = '\0';
	a[acount] = '\0';


	*num = atoi(a);
}

void printf_x(const UINT8 *buf, UINT16 len){
	UINT8 i = 0;
	for(i = 0;i < len;i++)
		{
		printf("0x%2x\t", buf[i]);
		}
	printf("\r\n");
	return ;
}

int appSendMsgToLoramac(T_sLoraData *ld,UINT8 dataLen)
{
	int ret = -1;
	printf("appMsgSend:%010d\r\n",ld->nodeId);
	printf_x(ld->data,dataLen);
	ret = sendLoraData(ld, dataLen);
	if(ret == 0)
	{
		printf("send to loramac fail");
	}
	return ret;

}




int app_test(int device_id)
{
	char gGatewayWriteCfgBuff[128] = {0};
	T_sLoraData ld = {0};
	int ret = 0;
	ret = openLoraDevice();
	if(ret == 0)
	{
		printf("openLoraDevice fail\n");
		return 0;
	}
	printf("nodeId : %d  data : %s\n", ldWrite.nodeId, ldWrite.data);
	UINT8 a = 0;
	UINT32 i = 0;
	sleep(30);
	#if 0
	for(;;)
	{
		//ÉèÖÃloraŽø¿í£¬ÉèÖÃÍêÐèÉèÖÃreloadLoraParamFlagÀŽ¿ØÖÆÉèÖÃÄÄžölora
		memset(gGatewayWriteCfgBuff,0,128);
		sprintf(gGatewayWriteCfgBuff,"%d",20);
		cfg_set("lora_send_power",gGatewayWriteCfgBuff);
		#if 0
		//ÉèÖÃloraÀ©ÆµÒò×Ó
		memset(gGatewayWriteCfgBuff,0,128);
		sprintf(gGatewayWriteCfgBuff,"%d",9);
		cfg_set("lora_spread_factor",gGatewayWriteCfgBuff);

		//ÉèÖÃloraŽø¿í
		memset(gGatewayWriteCfgBuff,0,128);
		sprintf(gGatewayWriteCfgBuff,"%d",9);
		cfg_set("lora_band_width",gGatewayWriteCfgBuff);
		#endif
		//loramacÖØÐÂÉèÖÃlora0/lora1/lora2µÄ²ÎÊý±êÖŸ,ÖµÎª0
		memset(gGatewayWriteCfgBuff,0,128);
		sprintf(gGatewayWriteCfgBuff,"%d",0);
		cfg_set("reloadLoraParamFlag",gGatewayWriteCfgBuff);
		cfg_save();
		sleep(40);
	}

	
	#endif
	for(;;i++)
	{	
		ret = recvLoraData(&ldRead);  //save gateway data
		if(ret > 0){
			printf("appMsgRcv:%010d\r\n",ldRead.nodeId);   //recvice node data
			printf_x(ldRead.data,ret);     //print LoRa data
			strcpy(tempdata,ldRead.data);
			printf("yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy");
			printf("Lora data transfer to tempdata:%s\r\n",tempdata);
		}
		#if 1
		ldWrite.nodeId = 00021701537;
		for(;a<128;a++){
			ldWrite.data[a] = a;
		}
		a = 0;
		printf("this funtion in app_test");
		printf("ldWrite.nodeId:%d\n,ldWrite.data:%s\n,strlen(ldwrite.data):%d\r\n",ldWrite.nodeId,ldWrite.data,strlen(ldWrite.data));
		appSendMsgToLoramac(&ldWrite,128);
		printf("send appmsg num %d\r\n",i);
		sleep(2);
		#endif
		
	}
}


int socketfd = -1;

#define MQTT_SERVER_ADDR "111.230.9.54"
#define MQTT_SERVER_PORT 1883 
#define SERVER_MSG_LEN_MAX 20480
static lora_gateway_st lora_gateway;
static lora_gateway_st send_gateway;

static MQTTString topicString = MQTTString_initializer;
static MQTTString topicsendString = MQTTString_initializer;


static int netUseFlag = -1;
static pthread_mutex_t lock_send;
static pthread_mutex_t mutex_queue;
static pthread_mutex_t lock_rw;
pthread_t thId_queue;
pthread_mutex_t lock_fifo;
pthread_t thId_fifo;
#define  SOCKET_DBG(...)   printf("[socket_dbg]:"__VA_ARGS__)
#define  MQTT_DBG(...)   printf("[mqtt]:"__VA_ARGS__)
#define  QUEUE_DBG(...)  printf("[queue]:"__VA_ARGS__)

//{"deviceId":"0002000199","timestamp":1234,"data":"asdfff","customMessageId":"test"}
char * mqttcJSONCreate(char *data)
{
	cJSON *root = cJSON_CreateObject();
	cJSON *object_array = NULL;
	cJSON *object_object = NULL;
	cJSON *object_string = NULL;
	char *buf = NULL;
	struct timeval timestamp;
	//char s_time[24] = {0};

	object_string = cJSON_CreateString("0002666243");
	cJSON_AddItemToObject(root, "deviceId", object_string);
	gettimeofday(&timestamp, NULL);
	//get_local_time(s_time);
	printf("time sec %ld ,usec %ld\n",timestamp.tv_sec,timestamp.tv_usec);
	printf("time is %lld\n",((long long)((long long)timestamp.tv_sec * 1000L - 8 * 60 * 60) + ((long long)timestamp.tv_usec / 1000)));
	object_string = cJSON_CreateNumber(((long long)((long long)timestamp.tv_sec * 1000L - 8 * 60 * 60) + ((long long)timestamp.tv_usec / 1000)));
	cJSON_AddItemToObject(root, "timestamp", object_string);

	object_string = cJSON_CreateString(data);
	cJSON_AddItemToObject(root, "data", object_string);

	object_string = cJSON_CreateString("test");
	cJSON_AddItemToObject(root, "customMessageId", object_string);
	
	buf = cJSON_PrintUnformatted(root);
	cJSON_Delete(root);
	printf("[gateways.c] json:\r\n%s\r\n", buf);
	return buf;
}

//==================================================================
 /* Function:       // sendToMqttServer
 * Description:     // Send data to the server
 * Input:           // �������˵��������ÿ����������
                    // �á�ȡֵ˵�����������ϵ��

 * Return:          // ��������ֵ��˵��
*/
//==================================================================




uint8_t sendToMqttServer(unsigned char *buf,int buflen,char *topic,unsigned char *payload)
{
	int len = -1;
	int rc = -1;
	topicString.cstring = topic;
	
	len = MQTTSerialize_publish(buf, buflen, 0, 1, 0,lora_gateway.MsgNum, topicString, payload,strlen(payload));
	rc = write(socketfd,buf, len);
	if(rc>0)lora_gateway.MsgNum++;
	printf("[mqtt] send message topic is %s, payload is %s!\n", topicString.cstring, payload);
	return rc;
}
	void *MessageQueue_thread(void  *arg)
	{
		char gGatewayWriteCfgBuff[128] = {0};
		T_sLoraData ld = {0};
		int ret = 0;
		int a = 0;
		int i = -1;
		char *payload = NULL;
		ret = openLoraDevice();
		if(ret == 0)
		{
			QUEUE_DBG("openLoraDevice fail\n");
			return 0;
		}

		while(1){
			usleep(100);
			ret = recvLoraData(&ldRead);
			if(ret > 0){
				QUEUE_DBG("appMsgRcv:%010d\r\n",ldRead.nodeId);
				printf_x(ldRead.data,ret);
				printf("YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY\n");
				strcpy(tempdata,ldRead.data);
				printf("LoRa data transfer to tempdata:%s\r\n",tempdata);
                LoraSend_enque(q,ldRead.nodeId,ldRead.data);     //Written to the queue cache
				//ldWrite.nodeId = 21701537;
				printf("enqueue success\n");
				
				printf("this funtion in MessageQueue time:2019-10-17 21-45\n");
				if(awaitack==WAITACK && ldack.nodeId==ldRead.nodeId){
					strcpy(ldack.data,ldRead.data);
					awaitack = RESPON;
					printf("-------------------responed ack---------------------\n");
				}
			//	printf("ldWrite.nodeId:%d\n,ldWrite.data:%s\n,strlen(ldwrite.data):%d\r\n",ldWrite.nodeId,ldWrite.data,strlen(ldWrite.data));
			//	i = appSendMsgToLoramac(&ldWrite,strlen(ldWrite.data));
				memset(ldRead.data,0,256);    //clear ldRead.data
				printf("send appmsg num %d\r\n",i);
			}
			
		}
	}

	int start_MessageQueue_thread(uint8_t cmd)
	{
		int ret = 0;
		pthread_mutex_init(&lock_fifo, NULL);
#ifndef NO_NVRAMqqqqqqqqqqqqqqqqq
		pthread_attr_t attr;
		pthread_attr_init (&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		ret = pthread_create(&thId_queue, NULL, MessageQueue_thread, &cmd);
#endif
		return ret;
	}

	void stop_MessageQueue_thread()
	{
		if(thId_fifo > 0)
		{
#ifndef ANDROID
			pthread_cancel(thId_queue);
#endif        
			pthread_join(thId_queue, NULL);
			thId_fifo = -1;
		}
		pthread_mutex_destroy(&lock_fifo);
		return;
	}

	int gateway_sub(MQTTString topicString)
	{
		int rc;
		int rs;
		char buf[200];
		int buflen = sizeof(buf);
		int msgid = 1;
		int req_qos = 1;
		int len = 0;
		len = MQTTSerialize_subscribe(buf, buflen, 0, msgid, 1, &topicString, &req_qos);  
		pthread_mutex_lock(&lora_gateway.lock_rw);
		rc = write(socketfd,buf,len);
		pthread_mutex_unlock(&lora_gateway.lock_rw);
		if(rc < 0){
			printf("ssl write error try mqtt send\r\n");
			rc = 0;
			return -1;
		}
		pthread_mutex_lock(&lora_gateway.lock_rw);
		rs = MQTTPacket_read(buf, buflen, getdata);
		pthread_mutex_unlock(&lora_gateway.lock_rw);
		if (rs == SUBACK)	/* wait for suback */
		{
			int submsgid;
			int subcount;
			int granted_qos;
			rc = MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos, buf, buflen);
			if (granted_qos != 0)
			{
				printf("[mqtt] granted qos != 0, %d\n", granted_qos);
				goto exit;
			}
		}
		else
		{
			goto exit;
		}
		return rc;
		exit:
		printf("[mqtt] entry exit: mqtt_sub !!!\n");
		//close(netfd);
		return -1;

	}
int main(int argc, char *argv[])
{
	printf("main start\r\n");
	sleep(2);
	int heartbeat = 0;
	int looptimes = 0;
	int i;
	unsigned int nodeId = 21701537;
	char LRdata[1024] = "this is LRdata init data";

	UINT32 device_id = 0;
	int device_type = -1;
	struct timeval tv;
	int retval;
	fd_set rfds;
	int rs = -1;
	int rc = -1;
	int maxfd = -1;
	int len = -1;
	char *payload = NULL;
	char topic[128] = "xiaofendui.Gateway_to_Server";              //fabuzhuti   
	char sendservertopic[128] = "xiaofendui.serverreceiver";
	static char buf[SERVER_MSG_LEN_MAX];           //tianchong
	time_t t1,t2,t3,t4;
	time(&t1);
	char *mqttBuf = NULL;

	pthread_mutex_init(&lora_gateway.lock_rw, NULL); //
	//pthread_mutex_init(&send_gateway.lock_rw, NULL); //rc

	q = LoraSend_que_Init();  //init queue  rc
	char strCfgGetItem[CFG_STR_SIZE] = {0};
	if (0 != cfg_get_item("lora_id", strCfgGetItem, sizeof(strCfgGetItem)))
	{
		printf("call cfg_get_item() error.\n");
		return 0;
	}
	device_id = atoi(strCfgGetItem); 

	printf("cfg_get_item device_id %d\n", device_id);
        //start lora thread
	if(start_MessageQueue_thread(1)){            //start lora receive        
		SOCKET_DBG("start_fifo_thread end\n");
		stop_MessageQueue_thread();
	}
	

	

	while(1){
		printf("a new loop start");
		if(socketfd > 0){
			close(socketfd);
		}
		socketfd = mqtt_CreatedTcpConn(MQTT_SERVER_ADDR,MQTT_SERVER_PORT);
		if(socketfd < 0){             
			perror("sOck_CreateTcpConn() error");
			SOCKET_DBG("socket created fail wait 30s looptimes %d\r\n",looptimes);
			looptimes++;
			sleep(30);
			if(looptimes >= 15)system("reboot");//
			continue;
		}
		looptimes = 0;
		if(-1 == mqtt_connect_info(&lora_gateway))
		{
			MQTT_DBG("server connect fail\r\n");
			pthread_mutex_lock(&lora_gateway.lock_rw);
			close(socketfd);
			pthread_mutex_unlock(&lora_gateway.lock_rw);
			continue;
		}
		topicString.cstring = "xiaofendui.Server_to_Gateway";
		//topicsendString.cstring = "xiaofendui.serversend"//rc
		gateway_sub(topicString);			// 
		//gateway_sub(topicsendString);		// rc

		if (socketfd > maxfd)maxfd = socketfd;

		for(;socketfd > 0;)
		{
			reReadsucc:
			FD_ZERO(&rfds);
			FD_SET(socketfd, &rfds);
			tv.tv_sec = POLL_CYCLE;
			tv.tv_usec = 0;
			retval = select(maxfd + 1, &rfds, NULL, NULL, &tv);
			time(&t2);
			printf("select: %d spend %d, %d\n", retval, t2-t1);
			if(t2-t1 > 20&&awaitack == RESPON)   		//send respond
			{
				len = MQTTSerialize_pingreq( buf, sizeof(buf));
				//printf("loop netfd %d",netfd);
				pthread_mutex_lock(&lora_gateway.lock_rw);
				rc = write(socketfd,buf,len);
				pthread_mutex_unlock(&lora_gateway.lock_rw);
				printf("-------------------start ack---------------------");
				printf("[mqtt] send PINGREQ message!\n");
				//sprintf(payload, "{\"HUBID\":\"%010d\"}", lora_gateway.device_id);
		       	printf("\r\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@the ldack.data send is :%s\r\n",ldack.data);	
				payload = mqttcJSONCreate(ldack.data);
			  	printf("\r\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^the payload is :%s\r\n",payload);
				rs = sendToMqttServer(buf,sizeof(buf),topic,payload);  // send to mqtt server

				awaitack = WAITINIT;	
				
				printf("****************sendToMqttServer success****************\n");
				free(mqttBuf);
				t1 = t2;
			}
			if (retval < 0) 
			{
					if (errno != EINTR)
					{
						SOCKET_DBG("select() error: %s", strerror(errno));
						pthread_mutex_lock(&lora_gateway.lock_rw);

						close(socketfd);
						netUseFlag = -1;
						pthread_mutex_unlock(&lora_gateway.lock_rw);

						break;
					}
			} 
			else if (retval)
			{
				SOCKET_DBG("########################get from server#########################\r\n");
				memset(buf,0,SERVER_MSG_LEN_MAX);
				if (FD_ISSET(socketfd, &rfds)) 
				{
					pthread_mutex_lock(&lora_gateway.lock_rw);
					rc = MQTTPacket_read(buf, sizeof(buf), getdata);
					printf("the pakeage : r= %d\n,");
					pthread_mutex_unlock(&lora_gateway.lock_rw);
					if(0 == rc)
					{
								heartbeat++;
								FD_CLR(socketfd,&rfds);
								SOCKET_DBG("socket error break rc %d\r\n",rc);
								goto reReadsucc;
								//break;
					}
					if(rc > 0)
					{
						switch(rc)      // Determine the type of MQTT package
						{
							
							case PUBACK:/
							{
								MQTT_DBG("receive PUBACK\r\n");
								printf("****************receive PUBACK success****************");
								heartbeat = 0;
								uint16_t packet_id;
								uint8_t packettype;
								int a = -1;
								uint8_t dup = 0;
								a = MQTTDeserialize_ack(&packettype,&dup,&packet_id,buf,sizeof(buf));
								MQTT_DBG("a is %d type %d dup %d id %d\r\n",a,packettype,dup,packet_id);
								//app_list_set_free_flag(&lora_gateway.MsgList,packet_id);
								
							}
							break;
							case PUBLISH:   //Processing server messages
							{
								MQTT_DBG("receive PUBLISH\n");
								printf("****************receive PUBLISH success****************");
								int a = -1;
								int dup;
								int qos;
								uint8_t packettype;
								int retained;
								int msgid;
								int flag;
								int payloadlen_in;
								char* payload_in;
								T_sLoraData ldWritedata = {0};


								MQTTString receivedTopic;
								rc = MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic, 
										&payload_in, &payloadlen_in, buf, sizeof(buf));
								MQTT_DBG("&&&&&&&&&&&&&&&&&&&&&receive publish topic is %.*s\n", receivedTopic.lenstring.len, receivedTopic.lenstring.data);
								MQTT_DBG("&&&&&&&&&&&&&&&&&&&&&[mqtt] payload is %.*s\n", payloadlen_in, payload_in);
								MQTT_DBG("&&&&&&&&&&&&&&&&&&&&&[mqtt] msgid is %d\n", msgid);
								printf("payload_in is %s\r\n :",payload_in);
								//memset(ldWritedata.data,0,2);	
								/*********************RChes**************************/	
                                 if(payload_in[0]=='#')printf("\n######read all message in the message queue 2019-10-17 21-45\n");
								else
								{	
									returnsystem(payload_in, &ldWrite.nodeId, ldWrite.data);  //unbind json
					            }
								printf("ldWrite.nodeId:%d and ldWrite.data is %s\r\n",ldWrite.nodeId,ldWrite.data);	
								//ldWritedata.nodeId = 21701573;
								//printf("ldWritedata.nodeId:%d and ldWritedata.data is %s\r\n\r\n",ldWritedata.nodeId,ldWritedata.data);
								/*
									��Ӧack磽�
								*/
								if(payload_in[0]=='#' || awaitack == RESPON)
								{
									printf("RRRRRRRRRRRRRRRRRRRRRR");
									if(t2-t1 > 20)
									{
										len = MQTTSerialize_pingreq( buf, sizeof(buf));
										//printf("loop netfd %d",netfd);
										pthread_mutex_lock(&lora_gateway.lock_rw);
										rc = write(socketfd,buf,len);
										pthread_mutex_unlock(&lora_gateway.lock_rw);
										printf("[mqtt] send PINGREQ message!\n");
										//sprintf(payload, "{\"HUBID\":\"%010d\"}", lora_gateway.device_id);
						 				
										if(awaitack == RESPON)
										{
											printf("\r\n!!!!!!!!!!!!!ldack.data is :%s\r\n",ldack.data);	
											
											//printf("\r\n@@@@@@@@@@@@@@@@@@the LRdata is :%s\r\n",ldack.data);	
											payload = mqttcJSONCreate(LRdata);
											printf("\r\n^^^^^^^^^^^^^^^^^^^the payload is :%s\r\n",payload);
											awaitack = WAITINIT;	
										}
									        else{
											printf("\n======================sendservertopic:%s=======================\n",sendservertopic);
											while(!LoraSend_queue_empty(q))
											{
												LoraSend_deque(q, &nodeId, LRdata);
												printf("nodeId : %d ,data: %s",nodeId, LRdata);

												printf("\r\n@@@@@@@@@@@@@@@@@@the LRdata is :%s\r\n",LRdata);	
												payload = mqttcJSONCreate(LRdata);
												
												printf("\r\n^^^^^^^^^^^^^^^^^^^the payload is :%s\r\n",payload);
												
												rs = sendToMqttServer(buf,sizeof(buf),sendservertopic,payload);
												
												printf("****************sendToMqttServer success****************");
											}
											printf("queue empty");
										}
										memset(ldWrite.data,0,256);
										

										free(mqttBuf);
										t1 = t2;				
									}
					
								}
								else{
									flag = appSendMsgToLoramac(&ldWrite,strlen(ldWrite.data));
									awaitack = WAITACK ;
									printf("-------------------await ack---------------------");
									strcpy(ldack.data,ldWrite.data);
									ldack.nodeId=ldWrite.nodeId;
									printf("send to %d appmsg flag %d\r\n",ldWritedata.nodeId,flag);
								}
								/*********************RChes**************************/
								packettype = PUBACK;
								
								a = MQTTSerialize_ack(buf,sizeof(buf),packettype,dup,msgid);
								
								pthread_mutex_lock(&lora_gateway.lock_rw);
								a = write(socketfd,buf,a);
								pthread_mutex_unlock(&lora_gateway.lock_rw);
							}
							break;
							case PINGRESP://
							{
								heartbeat = 0;
								
								MQTT_DBG("[mqtt] receive PINGRESP\n");
								printf("****************receive PINGRESP success****************");
							}
							break;
							case PUBREC://Qos2
							{
								heartbeat = 0;
								uint16_t packet_id;
								uint8_t packettype;
								int a = -1;
								uint8_t dup = 0;
								MQTT_DBG("[mqtt] receive PUBREC message\r\n");
								printf("****************receive PUBREC success****************");
								a = MQTTDeserialize_ack(&packettype,&dup,&packet_id,buf,sizeof(buf));
								MQTT_DBG("a is %d packettype %d dup %d packetid %d\r\n",a,packettype,dup,packet_id);
								packettype = PUBREL;
								a = MQTTSerialize_ack(buf,sizeof(buf),packettype,dup,packet_id);
								pthread_mutex_lock(&lora_gateway.lock_rw);
								a = write(socketfd,buf,a);
								pthread_mutex_unlock(&lora_gateway.lock_rw);
							}
							break;
							case PUBCOMP:
							{
								heartbeat = 0;
								MQTT_DBG("[mqtt] receive PUBCOMP\r\n");
								uint16_t packet_id;
								uint8_t packettype;
								int a = -1;
								uint8_t dup = 0;
								a = MQTTDeserialize_ack(&packettype,&dup,&packet_id,buf,sizeof(buf));
								MQTT_DBG("a is %d packettype %d dup %d packetid %d\r\n",a,packettype,dup,packet_id);
							}
							break;
							default :
							{
								MQTT_DBG("[mqtt] receive rc = %d\n", rc);
							}
							break;
						}
					}
					if(rc < 0)
					{
						SOCKET_DBG("socket error break rc %d\r\n",rc);
						FD_CLR(socketfd,&rfds);
						close(socketfd);
						break;
					}
			
				}
			}
		}
		printf("a new loop start");
	}
	
	return 0;
}

