#ifndef SOCKET_H
#define SOCKET_H

#include "my_type.h"
#include "MQTTPacket.h"
#include <openssl/ssl.h>
#include "app_common.h"

//SINT32 sOck_sendTcpBuf(UINT8 tcpChID, UINT8 *buf, UINT16 buflen);
SINT32 sOck_CreateTcpConn(UINT8 *ipaddr, UINT16 port);

SINT32 sOck_CreateUdpConn(UINT8 *ipaddr, UINT16 port);
VOID *sOck_tcplisten(void *arg);
VOID *sOck_udplisten(void *arg);
int mqtt_CreatedTcpConn(char *ipaddr, short port);
int getdata(char* buf, int count);

int get_mqttsocketfd();

int get_ssl_error(void);

int openssl_init(void);
int mqtt_connect_info(lora_gateway_st *lora_gateway);
void openssl_free(void);

#endif // SOCKET_H
