#ifndef APP_COMMON_H
#define APP_COMMON_H

#include <pthread.h>
#include <sys/time.h>




typedef unsigned char   uint8_t;
typedef unsigned short  uint16_t;
typedef unsigned int    uint32_t;
typedef int             BOOL;


#define POLL_CYCLE                      1
typedef struct lora_location_st_d
{
	double  latitude;
	double  longitude;
} lora_location_st;



typedef struct lora_gateway_st_d
{
	uint32_t device_id;


	uint32_t MsgNum;

	lora_location_st    location;
	lora_location_st    f_location;
	lora_location_st    t_location;

	pthread_t thId_send;
	pthread_mutex_t lock_send;
	pthread_mutex_t mutex_queue;
	pthread_mutex_t lock_rw;


}lora_gateway_st;

#endif
