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
#include <zc_protocol_interface.h>


extern MSG_Buffer g_struRecvBuffer;
extern MSG_Queue  g_struRecvQueue;

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

/*************************************************
* Function: MSG_RecvDataFromCloud
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 MSG_RecvDataFromCloud(u8 *pu8Data, u32 u32DataLen)
{
    ZC_Message *pstruMsg;
    u32 u32MsgLen;
    
    if (MSG_BUFFER_FULL == g_struRecvBuffer.u8Status)
    {
        return ZC_RET_ERROR;
    }
    if (MSG_BUFFER_IDLE == g_struRecvBuffer.u8Status)
    {

        if (u32DataLen < sizeof(ZC_Message))
        {
            memcpy(g_struRecvBuffer.u8MsgBuffer, pu8Data, u32DataLen);
            g_struRecvBuffer.u8Status = MSG_BUFFER_SEGMENT_NOHEAD;
            g_struRecvBuffer.u32Len = u32DataLen;
        }
        else
        {
            pstruMsg = (ZC_Message *)(pu8Data);
            u32MsgLen =  ZC_HTONS(pstruMsg->Payloadlen) + sizeof(ZC_Message);

            if (u32MsgLen > MSG_BUFFER_MAXLEN)
            {
                return ZC_RET_ERROR;
            }

            if (u32MsgLen <= u32DataLen)
            {
                memcpy(g_struRecvBuffer.u8MsgBuffer, pu8Data, u32MsgLen);
                g_struRecvBuffer.u8Status = MSG_BUFFER_FULL;
                g_struRecvBuffer.u32Len = u32MsgLen;
            }
            else
            {
                memcpy(g_struRecvBuffer.u8MsgBuffer, pu8Data, u32DataLen);
                g_struRecvBuffer.u8Status = MSG_BUFFER_SEGMENT_HEAD;
                g_struRecvBuffer.u32Len = u32DataLen;
            }

        }

        return ZC_RET_OK;

    }

    if (MSG_BUFFER_SEGMENT_HEAD == g_struRecvBuffer.u8Status)
    {
        pstruMsg = (ZC_Message *)(g_struRecvBuffer.u8MsgBuffer);
        u32MsgLen = ZC_HTONS(pstruMsg->Payloadlen) + sizeof(ZC_Message);

        if (u32MsgLen <= u32DataLen + g_struRecvBuffer.u32Len)
        {
            memcpy((g_struRecvBuffer.u8MsgBuffer + g_struRecvBuffer.u32Len), 
                pu8Data, 
                (u32MsgLen - g_struRecvBuffer.u32Len));

            g_struRecvBuffer.u8Status = MSG_BUFFER_FULL;
            g_struRecvBuffer.u32Len = u32MsgLen;
        }
        else
        {
            memcpy((g_struRecvBuffer.u8MsgBuffer + g_struRecvBuffer.u32Len), 
                pu8Data, 
                u32DataLen);
            g_struRecvBuffer.u32Len += u32DataLen;
            g_struRecvBuffer.u8Status = MSG_BUFFER_SEGMENT_HEAD;
        }

        return ZC_RET_OK;
    }

    if (MSG_BUFFER_SEGMENT_NOHEAD == g_struRecvBuffer.u8Status)
    {
        if ((g_struRecvBuffer.u32Len + u32DataLen) < sizeof(ZC_Message))
        {
            memcpy((g_struRecvBuffer.u8MsgBuffer + g_struRecvBuffer.u32Len), 
                pu8Data,
                u32DataLen);
            g_struRecvBuffer.u32Len += u32DataLen;
            g_struRecvBuffer.u8Status = MSG_BUFFER_SEGMENT_NOHEAD;
        }
        else
        {
            memcpy((g_struRecvBuffer.u8MsgBuffer + g_struRecvBuffer.u32Len), 
                pu8Data,
                (sizeof(ZC_Message) - g_struRecvBuffer.u32Len));

            pstruMsg = (ZC_Message *)(g_struRecvBuffer.u8MsgBuffer);
            u32MsgLen = ZC_HTONS(pstruMsg->Payloadlen) + sizeof(ZC_Message);

            if (u32MsgLen <= u32DataLen + g_struRecvBuffer.u32Len)
            {
                memcpy((g_struRecvBuffer.u8MsgBuffer + g_struRecvBuffer.u32Len), 
                    pu8Data,
                    u32MsgLen - g_struRecvBuffer.u32Len);
                g_struRecvBuffer.u8Status = MSG_BUFFER_FULL;
                g_struRecvBuffer.u32Len = u32MsgLen;

            }
            else
            {
                memcpy((g_struRecvBuffer.u8MsgBuffer + g_struRecvBuffer.u32Len), 
                    pu8Data,
                    u32DataLen);
                g_struRecvBuffer.u8Status = MSG_BUFFER_SEGMENT_HEAD;
                g_struRecvBuffer.u32Len += u32DataLen;
            }

        }

        return ZC_RET_OK;

    }
    
    return ZC_RET_ERROR;
    

}


/******************************* FILE END ***********************************/
