#ifndef __LORA_IPC_H__
#define __LORA_IPC_H__

typedef struct {
	long mtype;
	char mbuf[300];
}T_sMsgBuf;


extern int createMessageQueue(void);
extern int getMessageQueue(void);
extern int sendMessageData(int msgId,long msgType, char *data, int dataLen);
extern int recvMessageData(int msgId, long msgType, char *data, int recvLen);
extern int destroyMessageQueue(int msgId);

#endif