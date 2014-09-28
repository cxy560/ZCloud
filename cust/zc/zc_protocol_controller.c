/**
******************************************************************************
* @file     zc_protocol_controller.c
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief    protocol controller
******************************************************************************
*/
#include <zc_protocol_controller.h>
#include <zc_sec_engine.h>
#include <zc_module_config.h>
#include <zc_cloud_event.h>
#include <zc_message_queue.h>
#include <zc_protocol_interface.h>
#include <zc_timer.h>



PTC_ProtocolCon  g_struProtocolController;
extern MSG_Buffer g_struRecvBuffer;
extern MSG_Queue  g_struRecvQueue;
extern MSG_Buffer g_struSendBuffer[MSG_BUFFER_SEND_MAX_NUM];
extern MSG_Queue  g_struSendQueue;
extern MSG_Buffer g_struRetxBuffer;

extern u8 g_u8MsgBuildBuffer[MSG_BUFFER_MAXLEN];
extern u16 g_u16TcpMss;
/*************************************************
* Function: PCT_Init
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_Init(PTC_ModuleAdapter *pstruAdapter)
{
    u32 u32Ret = 0;
    u32 u32Index = 0;
    g_struProtocolController.pstruMoudleFun = pstruAdapter;
    g_struProtocolController.struCloudConnection.u32Socket = PCT_INVAILD_SOCKET;

    /*initialize parameters*/
    u32Ret = g_struProtocolController.pstruMoudleFun->pfunGetCloudKey(g_struProtocolController.u8CloudPublicKey);
    u32Ret += g_struProtocolController.pstruMoudleFun->pfunGetDeviceId(g_struProtocolController.u8DeviceId);
    u32Ret += g_struProtocolController.pstruMoudleFun->pfunGetVersion(g_struProtocolController.u8Version);
    u32Ret += g_struProtocolController.pstruMoudleFun->pfunGetPrivateKey(g_struProtocolController.u8MoudlePrivateKey);
    u32Ret += g_struProtocolController.pstruMoudleFun->pfunGetCloudIP(g_struProtocolController.struCloudConnection.u8IpAddress);
    
    /*config connection type*/
    g_struProtocolController.struCloudConnection.u32Port = ZC_CLOUD_PORT;
    g_struProtocolController.struCloudConnection.u8IpType = ZC_IPTYPE_IPV4;
    g_struProtocolController.struCloudConnection.u8ConnectionType = ZC_CONNECT_TYPE_TCP;


    MSG_InitQueue(&g_struRecvQueue);
    MSG_InitQueue(&g_struSendQueue);



    g_struRecvBuffer.u32Len = 0;
    g_struRecvBuffer.u8Status = MSG_BUFFER_IDLE;    

    for (u32Index = 0; u32Index < MSG_BUFFER_SEND_MAX_NUM; u32Index++)
    {
        g_struSendBuffer[u32Index].u32Len = 0;
        g_struSendBuffer[u32Index].u8Status = MSG_BUFFER_IDLE;
    }

    /*init ok if all result is ok*/
    g_struProtocolController.u8keyRecv = PCT_KEY_UNRECVED;

    TIMER_Init();
    g_struProtocolController.u8ReconnectTimer = PCT_TIMER_INVAILD;
    g_struProtocolController.u8SendMoudleTimer = PCT_TIMER_INVAILD;

    g_struProtocolController.u8MainState = (0 == u32Ret) ? (PCT_STATE_INIT) : (PCT_STATE_SLEEP);
}
/*************************************************
* Function: PCT_SendEmptyMsg
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_SendEmptyMsg()
{
    ZC_Message struMsg;
    u16 u16Len = 0;
    EVENT_BuildEmptyMsg(&g_struProtocolController, (u8*)&struMsg, &u16Len);
    PCT_SendMsgToCloud((u8*)&struMsg, u16Len);
}
/*************************************************
* Function: PCT_SendErrorMsg
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_SendErrorMsg(u8 u8MsgId, u8 *pu8Error, u16 u16ErrorLen)
{
    u16 u16Len;
    EVENT_BuildMsg(&g_struProtocolController, ZC_CODE_ERR, u8MsgId, g_u8MsgBuildBuffer, &u16Len, 
        (u8*)&pu8Error, u16ErrorLen);
    PCT_SendMsgToCloud((u8*)g_u8MsgBuildBuffer, u16Len);
}

/*************************************************
* Function: PCT_SendCloudAccessMsg1
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_SendCloudAccessMsg1(PTC_ProtocolCon *pstruContoller)
{
    u16 u16Len;
    ZC_HandShakeMsg1 struMsg1;
    s32 s32RetVal;
    
    /*stop reconnection timer*/
    if (PCT_TIMER_INVAILD != pstruContoller->u8ReconnectTimer)
    {
        TIMER_StopTimer(pstruContoller->u8ReconnectTimer);
        pstruContoller->u8ReconnectTimer = PCT_TIMER_INVAILD;
    }

    memcpy(struMsg1.RandMsg, pstruContoller->RandMsg, ZC_HS_MSG_LEN);
    memcpy(struMsg1.DeviceId, pstruContoller->u8DeviceId, ZC_HS_DEVICE_ID_LEN);

    s32RetVal = SEC_EncryptTextByRsa(pstruContoller->u8CloudPublicKey,
        (u8*)&struMsg1,
        (u8*)&struMsg1,
        sizeof(struMsg1));
    if (s32RetVal)
    {
        PCT_DisConnectCloud(pstruContoller);
        return;
    }
    
    EVENT_BuildMsg(pstruContoller, ZC_CODE_HANDSHAKE_1, 1, g_u8MsgBuildBuffer, &u16Len, 
        (u8*)&struMsg1, sizeof(ZC_HandShakeMsg1));
    
    PCT_SendMsgToCloud((u8*)g_u8MsgBuildBuffer, u16Len);
    pstruContoller->u8MainState = PCT_STATE_WAIT_ACCESSRSP;

    
    pstruContoller->pstruMoudleFun->pfunSetTimer(PCT_TIMER_REACCESS, 
        PCT_TIMER_INTERVAL_RECONNECT, &pstruContoller->u8AccessTimer);
}

/*************************************************
* Function: PCT_SendCloudAccessMsg3
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_SendCloudAccessMsg3(PTC_ProtocolCon *pstruContoller)
{
    u16 u16Len;
    ZC_HandShakeMsg3 struMsg3;
    
    memcpy(struMsg3.RandMsg, pstruContoller->RandMsg, ZC_HS_MSG_LEN);

    EVENT_BuildMsg(pstruContoller, ZC_CODE_HANDSHAKE_3, 1, g_u8MsgBuildBuffer, &u16Len, 
        (u8*)&struMsg3, sizeof(ZC_HandShakeMsg3));

    g_struProtocolController.u8keyRecv = PCT_KEY_RECVED;
    
    PCT_SendMsgToCloud((u8*)g_u8MsgBuildBuffer, u16Len);

    pstruContoller->u8MainState = PCT_STATE_WAIT_MSG4;

    
    pstruContoller->pstruMoudleFun->pfunSetTimer(PCT_TIMER_REACCESS, 
        PCT_TIMER_INTERVAL_RECONNECT, &pstruContoller->u8AccessTimer);
    return;
}
/*************************************************
* Function: PCT_DisConnectCloud
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_DisConnectCloud(PTC_ProtocolCon *pstruContoller)
{
    pstruContoller->u8MainState = PCT_STATE_DISCONNECT_CLOUD;
    pstruContoller->u8keyRecv = PCT_KEY_UNRECVED;
}

/*************************************************
* Function: PCT_ConnectCloud
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_ConnectCloud(PTC_ProtocolCon *pstruContoller)
{
    u32 u32Ret = ZC_RET_OK;
    
    /*Connect*/
    u32Ret = pstruContoller->pstruMoudleFun->pfunConnectToCloud(&pstruContoller->struCloudConnection);
    if (ZC_RET_OK != u32Ret)
    {
        return;
    }
    /*change state to wait access*/
    pstruContoller->u8MainState = PCT_STATE_WAIT_ACCESS;
    pstruContoller->u8keyRecv = PCT_KEY_UNRECVED;
}
/*************************************************
* Function: PCT_ReconnectCloud
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_ReconnectCloud(PTC_ProtocolCon *pstruContoller)
{
    if (PCT_INVAILD_SOCKET == pstruContoller->struCloudConnection.u32Socket)
    {
        return;
    }
    
    pstruContoller->pstruMoudleFun->pfunSetTimer(PCT_TIMER_RECONNECT, 
        PCT_TIMER_INTERVAL_RECONNECT, &pstruContoller->u8ReconnectTimer);
    pstruContoller->struCloudConnection.u32Socket = PCT_INVAILD_SOCKET;
    pstruContoller->u8keyRecv = PCT_KEY_UNRECVED;    
}

/*************************************************
* Function: PCT_SendMoudleTimeout
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_SendMoudleTimeout(PTC_ProtocolCon *pstruProtocolController)
{
    MSG_Buffer *pstruBuffer;
    ZC_Message *pstruMsg;
    pstruBuffer = (MSG_Buffer *)pstruProtocolController->pu8SendMoudleBuffer;
    pstruMsg = (ZC_Message*)pstruBuffer->u8MsgBuffer;

    /*Send to Moudle*/
    pstruProtocolController->u8ReSendMoudleNum++;
    
    if (pstruProtocolController->u8ReSendMoudleNum > PCT_SENDMOUDLE_NUM)
    {
        pstruBuffer = (MSG_Buffer *)pstruProtocolController->pu8SendMoudleBuffer;
        pstruBuffer->u32Len = 0;
        pstruBuffer->u8Status = MSG_BUFFER_IDLE;
        pstruProtocolController->u8SendMoudleTimer = PCT_TIMER_INVAILD;
        pstruProtocolController->u8ReSendMoudleNum = 0;
        
        PCT_SendErrorMsg(pstruMsg->MsgId, NULL, 0);
    }
    else
    {
        pstruProtocolController->pstruMoudleFun->pfunSendToMoudle((u8*)pstruMsg, ZC_HTONS(pstruMsg->Payloadlen) + sizeof(ZC_Message));

        pstruProtocolController->pstruMoudleFun->pfunSetTimer(PCT_TIMER_SENDMOUDLE, 
            PCT_TIMER_INTERVAL_SENDMOUDLE, &pstruProtocolController->u8SendMoudleTimer);
    }

}
/*************************************************
* Function: PCT_HandleMoudleEvent
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_HandleMoudleEvent(u8 *pu8Msg, u16 u16DataLen)
{
    MSG_Buffer *pstruBuffer;

    if (PCT_TIMER_INVAILD != g_struProtocolController.u8SendMoudleTimer)
    {
        TIMER_StopTimer(g_struProtocolController.u8SendMoudleTimer);
        pstruBuffer = (MSG_Buffer *)g_struProtocolController.pu8SendMoudleBuffer;
        pstruBuffer->u32Len = 0;
        pstruBuffer->u8Status = MSG_BUFFER_IDLE;
        g_struProtocolController.u8SendMoudleTimer = PCT_TIMER_INVAILD;
        g_struProtocolController.u8ReSendMoudleNum = 0;
    }
    PCT_SendMsgToCloud(pu8Msg, u16DataLen);
    return;
}
/*************************************************
* Function: PCT_RecvAccessMsg2
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_RecvAccessMsg2(PTC_ProtocolCon *pstruContoller)
{
    MSG_Buffer *pstruBuffer;
    ZC_Message *pstruMsg;
    ZC_HandShakeMsg2 struMsg2;
    
    s32 s32RetVal;
    pstruBuffer = (MSG_Buffer *)MSG_PopMsg(&g_struRecvQueue);
    if (NULL == pstruBuffer)
    {
        return;
    }

    pstruMsg = (ZC_Message*)pstruBuffer->u8MsgBuffer;
    ZC_TraceData((u8*)pstruMsg, ZC_HTONS(pstruMsg->Payloadlen) + sizeof(ZC_Message));

    if (ZC_CODE_HANDSHAKE_2 == pstruMsg->MsgCode)
    {
        TIMER_StopTimer(pstruContoller->u8AccessTimer);
        s32RetVal = SEC_DecipherTextByRsa(pstruContoller->u8MoudlePrivateKey,
            pstruMsg->payload,
            (u8*)&struMsg2,
            sizeof(struMsg2));
        if (s32RetVal)
        {
            PCT_DisConnectCloud(pstruContoller);
        }
        else
        {
            if (0 == memcmp(struMsg2.RandMsg, pstruContoller->RandMsg, ZC_HS_MSG_LEN))
            {
                memcpy(pstruContoller->u8SessionKey, struMsg2.SessionKey, ZC_HS_SESSION_KEY_LEN);
                PCT_SendCloudAccessMsg3(pstruContoller);
            }
            else
            {
                PCT_DisConnectCloud(pstruContoller);
            }
        }
    }
    
    PCT_SendEmptyMsg();
    pstruBuffer->u32Len = 0;
    pstruBuffer->u8Status = MSG_BUFFER_IDLE;
}

/*************************************************
* Function: PCT_RecvAccessMsg4
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_RecvAccessMsg4(PTC_ProtocolCon *pstruContoller)
{
    MSG_Buffer *pstruBuffer;
    ZC_Message *pstruMsg;
    ZC_HandShakeMsg4 *pstruMsg4;
    pstruBuffer = (MSG_Buffer *)MSG_PopMsg(&g_struRecvQueue);
    if (NULL == pstruBuffer)
    {
        return;
    }
    
    pstruMsg = (ZC_Message*)pstruBuffer->u8MsgBuffer;
    ZC_TraceData((u8*)pstruMsg, ZC_HTONS(pstruMsg->Payloadlen) + sizeof(ZC_Message));
    if (ZC_CODE_HANDSHAKE_4 == pstruMsg->MsgCode)
    {
        TIMER_StopTimer(pstruContoller->u8AccessTimer);
        SEC_Decrypt(pstruContoller, 
            pstruContoller->u8SessionKey, 
            pstruContoller->IvRecv, 
            pstruMsg->payload, ZC_HTONS(pstruMsg->Payloadlen));
        pstruMsg4 = (ZC_HandShakeMsg4 *)pstruMsg->payload;
        if (0 == memcmp(pstruMsg4->RandMsg, pstruContoller->RandMsg, ZC_HS_MSG_LEN))
        {
            pstruContoller->u8MainState = PCT_STATE_CONNECT_CLOUD;            
        }
        else
        {
            PCT_DisConnectCloud(pstruContoller);
        }

    }
    PCT_SendEmptyMsg();
    pstruBuffer->u32Len = 0;
    pstruBuffer->u8Status = MSG_BUFFER_IDLE;
}

/*************************************************
* Function: PCT_HandleEvent
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_HandleEvent(PTC_ProtocolCon *pstruContoller)
{
    MSG_Buffer *pstruBuffer;
    ZC_Message *pstruMsg;
    
    if (PCT_TIMER_INVAILD != pstruContoller->u8SendMoudleTimer)
    {
        return;
    }
    
    pstruBuffer = (MSG_Buffer *)MSG_PopMsg(&g_struRecvQueue);
    if (NULL == pstruBuffer)
    {
        return;
    }
    
    pstruMsg = (ZC_Message*)pstruBuffer->u8MsgBuffer;

    ZC_TraceData((u8*)pstruMsg, ZC_HTONS(pstruMsg->Payloadlen) + sizeof(ZC_Message));
    SEC_Decrypt(&g_struProtocolController, 
        g_struProtocolController.u8SessionKey, 
        g_struProtocolController.IvRecv, pstruMsg->payload, ZC_HTONS(pstruMsg->Payloadlen));
    /*Send to Moudle*/
    pstruContoller->pstruMoudleFun->pfunSendToMoudle((u8*)pstruMsg, ZC_HTONS(pstruMsg->Payloadlen) + sizeof(ZC_Message));
    
    pstruContoller->pstruMoudleFun->pfunSetTimer(PCT_TIMER_SENDMOUDLE, 
        PCT_TIMER_INTERVAL_SENDMOUDLE, &pstruContoller->u8SendMoudleTimer);
    
    PCT_SendEmptyMsg();

    /*copy buffer*/
    memcpy((u8*)g_struRetxBuffer.u8MsgBuffer, pstruBuffer->u8MsgBuffer, 
        ZC_HTONS(pstruMsg->Payloadlen) + sizeof(ZC_Message));
    g_struRetxBuffer.u32Len = ZC_HTONS(pstruMsg->Payloadlen) + sizeof(ZC_Message);
    g_struRetxBuffer.u8Status = MSG_BUFFER_FULL;
    pstruContoller->pu8SendMoudleBuffer = (u8*)&g_struRetxBuffer;
    pstruContoller->u8ReSendMoudleNum = 0;

    pstruBuffer->u32Len = 0;
    pstruBuffer->u8Status = MSG_BUFFER_IDLE;
    
} 

/*************************************************
* Function: PCT_Run
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_Run()
{
    PTC_ProtocolCon *pstruContoller = &g_struProtocolController;
    switch(pstruContoller->u8MainState)
    {
        case PCT_STATE_SLEEP:
            break;
        case PCT_STATE_INIT:
            /*smart connection*/
            break;
        case PCT_STATE_ACCESS_NET:
            PCT_ConnectCloud(pstruContoller);
            break;
        case PCT_STATE_WAIT_ACCESS:
            break;
        case PCT_STATE_WAIT_ACCESSRSP:
            PCT_RecvAccessMsg2(pstruContoller);
            break;    
        case PCT_STATE_WAIT_MSG4:
            PCT_RecvAccessMsg4(pstruContoller);
            break;
        case PCT_STATE_CONNECT_CLOUD:
            PCT_HandleEvent(pstruContoller);
            break;                       
    }
    
}
/*************************************************
* Function: PCT_WakeUp
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_WakeUp()
{
    if (PCT_STATE_INIT == g_struProtocolController.u8MainState)
    {
        g_struProtocolController.u8MainState = PCT_STATE_ACCESS_NET;
    }
    
}

/*************************************************
* Function: PCT_Sleep
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_Sleep()
{
    g_struProtocolController.u8MainState = PCT_STATE_INIT;
}

/*************************************************
* Function: PCT_SendMsgToCloud
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_SendMsgToCloud(u8 *pu8Msg, u16 u16Len)
{
    u32 u32Index;
    u16 u16RemainLen;
    ZC_Message *pstruMsg;
    
    /*Check send buffer is enough*/
    u16RemainLen = 0;
    for (u32Index = 0; u32Index < MSG_BUFFER_SEND_MAX_NUM; u32Index++)
    {
        if (MSG_BUFFER_IDLE == g_struSendBuffer[u32Index].u8Status)
        {
            u16RemainLen += g_u16TcpMss; 
        }
    }
    
    if (u16Len > u16RemainLen)
    {
        return;
    }
    
    pstruMsg = (ZC_Message*)pu8Msg;

    if (PCT_KEY_RECVED == g_struProtocolController.u8keyRecv)
    {
        SEC_Encrypt(&g_struProtocolController, 
            g_struProtocolController.u8SessionKey, 
            g_struProtocolController.IvSend,
            pstruMsg->payload,
            ZC_HTONS(pstruMsg->Payloadlen));            
    }

    u16RemainLen = u16Len;
    
    for (u32Index = 0; u32Index < MSG_BUFFER_SEND_MAX_NUM; u32Index++)
    {
        if (MSG_BUFFER_IDLE == g_struSendBuffer[u32Index].u8Status)
        {
            if (u16RemainLen > g_u16TcpMss)
            {
                memcpy(g_struSendBuffer[u32Index].u8MsgBuffer, pu8Msg + (u16Len - u16RemainLen), g_u16TcpMss);
                g_struSendBuffer[u32Index].u32Len = g_u16TcpMss;
                g_struSendBuffer[u32Index].u8Status = MSG_BUFFER_FULL;
                MSG_PushMsg(&g_struSendQueue, (u8*)&g_struSendBuffer[u32Index]);
                u16RemainLen -= g_u16TcpMss;
            }
            else
            {
                memcpy(g_struSendBuffer[u32Index].u8MsgBuffer, pu8Msg + (u16Len - u16RemainLen), u16RemainLen);
                g_struSendBuffer[u32Index].u32Len = u16RemainLen;
                g_struSendBuffer[u32Index].u8Status = MSG_BUFFER_FULL;
                MSG_PushMsg(&g_struSendQueue, (u8*)&g_struSendBuffer[u32Index]);
                break;
            }
            
        }
    }
}


/******************************* FILE END ***********************************/

