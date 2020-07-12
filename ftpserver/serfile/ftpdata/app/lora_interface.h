#ifndef __LORA_INTERFACE_H__
#define __LORA_INTERFACE_H__

/***********************************************************
 * 与loramac程序进行数据交互用数据结构
 * nodeId 此数据报文来自/发送哪一个LoRa终端
 * dataLen data数据的长度
 * data 结构体中包含的LoRa数据
 ***********************************************************/
typedef struct {
	unsigned int nodeId;
	char data[256];
}T_sLoraData;


typedef struct node {
	unsigned int nodeid;
	char data[1024];
	struct node * next;
}T_LoraData;

//定义队列（保存队首和队尾指针）
typedef struct queue_link {
	T_LoraData *front;
	T_LoraData *rear;
}que;


que * LoraSend_que_Init();
que * LoraRecv_que_Init();
void LoraSend_enque(que *q, unsigned int nodeid,  char* data);
void LoraRecv_enque(que *q, unsigned int nodeid,  char* data);
void LoraSend_deque(que *q, unsigned int* nodeid, char* data); 
void LoraRecv_deque(que *q, unsigned int* nodeid, char* data);
int LoraSend_queue_empty(que * q);        //传入队列
int LoraRecv_queue_empty(que * q);  









/***********************************************************
 * 函数名：
 * 功能描述：打开并初始化与loramac的交互通道
 * 参数：NULL
 * 返回值：1 成功  0 失败
 ***********************************************************/
int openLoraDevice(void);

/***********************************************************
 * 函数名：
 * 功能描述：向loramac发送数据
 * 参数：pLoraData 需要发送的数据
 * 返回值：1 成功  0 失败
 ***********************************************************/
int sendLoraData(T_sLoraData * pLoraData, unsigned char dataLen);

/***********************************************************
 * 函数名：
 * 功能描述：从loramac接收数据
 * 参数：pLoraData 用于存放将要接收到的数据
 * 返回值：成功 返回读取到的数据长度  失败 0
 ***********************************************************/
int recvLoraData(T_sLoraData * pLoraData);

/***********************************************************
 * 函数名：
 * 功能描述：查询loramac是否接收到lora数据
 * 参数：NULL
 * 返回值：1 有数据  0 无数据  -1 错误
 ***********************************************************/
//int inquiryRecvLoraData(void);

/***********************************************************
 * 函数名：
 * 功能描述：关闭与loramac的交互通道
 * 参数：NULL
 * 返回值：1 成功  0 失败
 ***********************************************************/
int closeLoraDevice(void);



#endif
