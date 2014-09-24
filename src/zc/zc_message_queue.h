/**
******************************************************************************
* @file     zc_message_queue.h
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief    Message
******************************************************************************
*/

#ifndef  __ZC_MESSAGE_QUEUE_H__ 
#define  __ZC_MESSAGE_QUEUE_H__

#include <zc_common.h>

#define  MSG_QUEUE_MAXLEN  (10)

typedef struct struMSG_QueNode{
    struct struMSG_QueNode* pstruNext;
    u8 *pu8Msg;
}MSG_QueNode;

typedef struct{
    MSG_QueNode *pstruHead;
    MSG_QueNode *pstruTail;
    MSG_QueNode struQueue[MSG_QUEUE_MAXLEN+1];
}MSG_Queue;

#ifdef __cplusplus
extern "C" {
#endif

void MSG_InitQueue(MSG_Queue *pstruMsgQue);
u32 MSG_PushMsg(MSG_Queue *pstruMsgQueue, u8 *pu8Msg);
u8* MSG_PopMsg(MSG_Queue *pstruMsgQueue);

#ifdef __cplusplus
}
#endif

#endif
/******************************* FILE END ***********************************/

