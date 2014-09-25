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

#define  MSG_QUEUE_MAXLEN               (5)
#define  MSG_BUFFER_MAXLEN              (1000)
#define  MSG_BUFFER_MAX_NUM             (5)

#define  MSG_BUFFER_IDLE                (0)
#define  MSG_BUFFER_FULL                (1)
#define  MSG_BUFFER_SEGMENT_NOHEAD      (2)
#define  MSG_BUFFER_SEGMENT_HEAD        (3)

#define  MSG_BUFFER_SEND_MAX_NUM        (3)

typedef struct struMSG_QueNode{
    struct struMSG_QueNode* pstruNext;
    u8 *pu8Msg;
}MSG_QueNode;

/*Msg Queue*/
typedef struct{
    MSG_QueNode *pstruHead;
    MSG_QueNode *pstruTail;
    MSG_QueNode struQueue[MSG_QUEUE_MAXLEN+1];
}MSG_Queue;

/*Msg Buffer*/
typedef struct
{
    u32 u32Len;  
    u8  u8Status;
    u8  u8Pad[3];
    u8  u8MsgBuffer[MSG_BUFFER_MAXLEN];
}MSG_Buffer;


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

