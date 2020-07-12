#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lora_ipc.h"
#include "lora_interface.h"

#define MSG_TYPE_SEND ((long)8011)
#define MSG_TYPE_RECV ((long)8012)

int msgId = -1;

//rches
//////////////////////////////////////////////
//////////////初始化函数///////////////////////

que * LoraSend_que_Init()
{
	que * q = (que*)malloc(sizeof(que));
	q->front = q->rear = NULL;
	return q;
}

que * LoraRecv_que_Init()
{
	que * q = (que*)malloc(sizeof(que));
	q->front = q->rear = NULL;
	return q;
}

////////////////////////////////////////////////
////////////////////入队函数///////////////////
//第一个传入队列，第二个传入需要储存的id,第三个存储需要储存的字符串
void LoraSend_enque(que *q, unsigned int nodeid,  char* data)          
{
	T_LoraData *n =(T_LoraData*)malloc(sizeof(T_LoraData));
	if (n == NULL)  //内存分配失败
	    return;
	n->nodeid = nodeid;
	strcpy(n->data,data);

	n->next = NULL;//尾插法，插入元素指向空
	if (q->rear == NULL)
	{
		q->front = n;
		q->rear = n;
	}
	else {
		q->rear->next = n;//让n成为当前的尾部节点下一节点
		q->rear = n;//尾部指针指向n
	}

}





////////////////////////////////////////////////////
////////////////////////出队函数////////////////////
 //第一个传入队列， 第二个传入&nodeid，用来保存第一个数据，第二个传入char型数组，用来存储char型数据
void LoraSend_deque(que *q, unsigned int *nodeid, char *data) 
{
	T_LoraData * n = q->front;
	if (q->front == NULL)//判断队列是否为空
		return;
	if (q->front == q->rear)//是否只有一个元素
	{
		*nodeid = (q->front->nodeid);
		strcpy(data, q->front->data);
		q->front = q->front->next;
		q->front = NULL;
		q->rear = NULL;
	}
	else {
		*nodeid = q->front->nodeid;
		strcpy(data, q->front->data);
		q->front = q->front->next;
		free(n);
	}
}



////////////////////////////////////////////////
///////////////////////判断队空/////////////////

int LoraSend_queue_empty(que *q)        //传入队列
{
	return q->front == NULL;
}

int LoraRecv_queue_empty(que *q)
{
	return q->front == NULL;
}

//rches
int openLoraDevice(void)
{
    msgId = getMessageQueue();
    if(msgId == -1)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int sendLoraData(T_sLoraData *pLoraData, unsigned char dataLen)
{
    int ret = -1;
    ret = sendMessageData(msgId,MSG_TYPE_SEND, (char *)pLoraData, dataLen + sizeof(int));
    if(ret == -1)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int recvLoraData(T_sLoraData *pLoraData)
{
    int ret = -1;
    ret = recvMessageData(msgId, MSG_TYPE_RECV, (char *)pLoraData, sizeof(T_sLoraData));
    if(ret <= sizeof(int))
    {
        return 0;
    }
    else
    {
        return ret - sizeof(int);
    }
}

int closeLoraDevice(void)
{
    return 1;
}
