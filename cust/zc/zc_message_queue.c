/**
******************************************************************************
* @file     zc_messgage_queue.c
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief    Msg Queue
******************************************************************************
*/

#include <zc_message_queue.h>


/*************************************************
* Function: MSG_InitQueue
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void MSG_InitQueue(MSG_Queue *pstruMsgQue)   
{
    u32 u32Index;
    MSG_QueNode *pstrNode = NULL;
    for (u32Index = 0; u32Index < MSG_QUEUE_MAXLEN + 1; u32Index++)
    {
        pstruMsgQue->struQueue[u32Index].pstruNext = pstrNode;
        pstruMsgQue->struQueue[u32Index].pu8Msg = NULL;
        pstrNode = &pstruMsgQue->struQueue[u32Index];
    }
    /*Init a loop queue*/
    pstruMsgQue->struQueue[0].pstruNext = &pstruMsgQue->struQueue[MSG_QUEUE_MAXLEN];
    
    pstruMsgQue->pstruHead = &pstruMsgQue->struQueue[0];
    pstruMsgQue->pstruTail = pstruMsgQue->pstruHead;
    return;
}

/*************************************************
* Function: MSG_PushMsg
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 MSG_PushMsg(MSG_Queue *pstruMsgQueue, u8 *pu8Msg)
{
    /*tail next == head, queue is full*/
    if (pstruMsgQueue->pstruTail->pstruNext == pstruMsgQueue->pstruHead)
    {
        return ZC_RET_ERROR;
    }
    pstruMsgQueue->pstruTail->pu8Msg = pu8Msg;
    pstruMsgQueue->pstruTail = pstruMsgQueue->pstruTail->pstruNext;
    return ZC_RET_OK;
}

/*************************************************
* Function: MSG_PopMsg
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u8* MSG_PopMsg(MSG_Queue *pstruMsgQueue)
{
    u8 *pu8Msg = NULL;
    
    /*head == tail, no msg*/
    if (pstruMsgQueue->pstruHead == pstruMsgQueue->pstruTail)
    {
        return pu8Msg;
    }
    
    pu8Msg = pstruMsgQueue->pstruHead->pu8Msg;
    pstruMsgQueue->pstruHead->pu8Msg = NULL;
    pstruMsgQueue->pstruHead = pstruMsgQueue->pstruHead->pstruNext;
    
    return pu8Msg;
}