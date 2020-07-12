#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#include "lora_ipc.h"

#define MSGPATH "/etc/"


int createMessageQueue(void)
{
	int ret = -1;
	int msgId;
	key_t key;
	char *msgpath = MSGPATH;
	key = ftok(msgpath, 'b');
	if(key != -1)
	{
		printf("create the key success\n");
	}
	else
	{
		printf("create the key fail\n");
		return 0;
	}
	msgId = msgget(key, IPC_CREAT | IPC_EXCL | 0666);
	if(-1 == msgId)
	{
		perror("create message queue error");
	}
	else
	{
		printf("build the message queue success\n");
	}
	return msgId;
}

int getMessageQueue(void)
{
	int ret = -1;
	int msgId;
	key_t key;
	char *msgpath = MSGPATH;
	key = ftok(msgpath, 'b');
	if(key != -1)
	{
		printf("app get the key success\n");
	}
	else
	{
		perror("ftok");
		printf("app get the key fail\n");
		return 0;
	}
	msgId = msgget(key, IPC_CREAT | 0666);
	if(-1 == msgId)
	{
		perror("app get message queue error");
	}
	else
	{
		printf("app build the message queue success\n");
	}
	return msgId;
}

int sendMessageData(int msgId,long msgType, char *data, int dataLen)
{
	int ret = -1;
	T_sMsgBuf msgBuf = {0};
	msgBuf.mtype = msgType;
	memcpy(msgBuf.mbuf, data, dataLen);
	ret = msgsnd(msgId, &msgBuf, dataLen, IPC_NOWAIT);
	if(-1 == ret)
	{
		printf("send the message failed\n");
	}
	else
	{
		printf("send the message success\n");
	}
	return ret;
}

int recvMessageData(int msgId, long msgType, char *data, int recvLen)
{
	int ret = -1;
	T_sMsgBuf msgBuf = {0};
	msgBuf.mtype = msgType;
	ret = msgrcv(msgId, &msgBuf, recvLen, msgType, IPC_NOWAIT | MSG_NOERROR);
	if(-1 == ret)
	{
		//printf("recv the message failed\n");
	}
	else
	{
		printf("recv the message success\n");
		memcpy(data, msgBuf.mbuf, ret);
	}
	return ret;
}

int destroyMessageQueue(int msgId)
{
	return msgctl(msgId, IPC_RMID, NULL);
}
















