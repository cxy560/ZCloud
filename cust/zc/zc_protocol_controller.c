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

PTC_ProtocolCon  g_struProtocolController;
MSG_Buffer g_struRecvBuffer;
MSG_Queue  g_struRecvQueue;
MSG_Buffer g_struSendBuffer[MSG_BUFFER_SEND_MAX_NUM];
MSG_Queue  g_struSendQueue;
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
    g_struProtocolController.pstruMoudleFun = pstruAdapter;
    
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

    /*init ok if all result is ok*/
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
    u32 u32Len = 0;
    EVENT_BuildEmptyMsg(&g_struProtocolController, &struMsg, &u32Len);
    PCT_SendMsgToCloud((u8*)&struMsg, u32Len);
}

/*************************************************
* Function: PCT_SendCloudAccessMsg2
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_SendCloudAccessMsg2(PTC_ProtocolCon *pstruContoller)
{
    u32 u32Ret = ZC_RET_OK;
    u32 u32Len = 0;
    
   
}

/*************************************************
* Function: PCT_SendCloudAccessMsg4
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_SendCloudAccessMsg4(PTC_ProtocolCon *pstruContoller)
{
    u32 u32Ret = ZC_RET_OK;
    u32 u32Len = 0;


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
    pstruContoller->u8MainState = PCT_STATE_ACCESS_NET;
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
}

/*************************************************
* Function: PCT_HandleMoudleEvent
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_HandleMoudleEvent(u8 *pu8Msg, u32 u32DataLen)
{
    PCT_SendMsgToCloud(pu8Msg, u32DataLen);
    return;
}
/*************************************************
* Function: PCT_RecvAccessRsp
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_RecvAccessMsg1(PTC_ProtocolCon *pstruContoller)
{
    MSG_Buffer *pstruBuffer;
    ZC_Message *pstruMsg;
    pstruBuffer = (MSG_Buffer *)MSG_PopMsg(&g_struRecvQueue);
    pstruMsg = (ZC_Message*)pstruBuffer->u8MsgBuffer;
    if (ZC_CODE_HANDSHAKE_1 == pstruMsg->MsgCode)
    {
        
    }
    
    PCT_SendEmptyMsg();
    pstruBuffer->u32Len = 0;
    pstruBuffer->u8Status = MSG_BUFFER_IDLE;
}

/*************************************************
* Function: PCT_RecvAccessRsp
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_RecvAccessMsg3(PTC_ProtocolCon *pstruContoller)
{
    MSG_Buffer *pstruBuffer;
    ZC_Message *pstruMsg;
    pstruBuffer = (MSG_Buffer *)MSG_PopMsg(&g_struRecvQueue);
    pstruMsg = (ZC_Message*)pstruBuffer->u8MsgBuffer;
    if (ZC_CODE_HANDSHAKE_3 == pstruMsg->MsgCode)
    {
        
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
    pstruBuffer = (MSG_Buffer *)MSG_PopMsg(&g_struRecvQueue);
    pstruMsg = (ZC_Message*)pstruBuffer->u8MsgBuffer;

    PCT_SendEmptyMsg();
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
            PCT_RecvAccessMsg1(pstruContoller);
            break;
        case PCT_STATE_WAIT_ACCESSRSP:
            PCT_RecvAccessMsg3(pstruContoller);
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
void PCT_SendMsgToCloud(u8 *pu8Msg, u32 u32Len)
{
    u32 u32Index;
    for (u32Index = 0; u32Index < MSG_BUFFER_SEND_MAX_NUM; u32Index++)
    {
        if (MSG_BUFFER_IDLE == g_struSendBuffer[u32Index].u8Status)
        {
            memcpy(g_struSendBuffer[u32Index].u8MsgBuffer, pu8Msg, u32Len);
            g_struSendBuffer[u32Index].u32Len = u32Len;
            g_struSendBuffer[u32Index].u8Status = MSG_BUFFER_FULL;
            MSG_PushMsg(&g_struSendQueue, &g_struSendBuffer[u32Index]);
            break;
        }
    }
}


/******************************* FILE END ***********************************/

