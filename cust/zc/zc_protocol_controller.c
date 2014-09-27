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
extern MSG_Buffer g_struRecvBuffer;
extern MSG_Queue  g_struRecvQueue;
extern MSG_Buffer g_struSendBuffer[MSG_BUFFER_SEND_MAX_NUM];
extern MSG_Queue  g_struSendQueue;

extern u8 g_u8MsgBuildBuffer[MSG_BUFFER_MAXLEN];
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
    EVENT_BuildEmptyMsg(&g_struProtocolController, (u8*)&struMsg, &u32Len);
    PCT_SendMsgToCloud((u8*)&struMsg, u32Len);
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
    u32 u32Len;
    EVENT_BuildMsg(&g_struProtocolController, ZC_CODE_ERR, u8MsgId, g_u8MsgBuildBuffer, &u32Len, 
        (u8*)&pu8Error, u16ErrorLen);
    PCT_SendMsgToCloud((u8*)&g_u8MsgBuildBuffer, u32Len);
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
    rsa_context rsa;
    s32 s32Ret;
    u32 u32Len;
    ZC_HandShakeMsg1 struMsg1;
    
    memcpy(struMsg1.RandMsg, pstruContoller->RandMsg, ZC_HS_MSG_LEN);
    memcpy(struMsg1.DeviceId, pstruContoller->u8DeviceId, ZC_HS_DEVICE_ID_LEN);

    SEC_InitRsaContextWithPublicKey(&rsa, pstruContoller->u8CloudPublicKey);
    s32Ret = rsa_pkcs1_encrypt(&rsa, RSA_PUBLIC, 52, (u8*)&struMsg1, (u8*)&struMsg1);
    rsa_free(&rsa);

    if (s32Ret)
    {
        return;
    } 
    EVENT_BuildMsg(pstruContoller, ZC_CODE_HANDSHAKE_1, 1, g_u8MsgBuildBuffer, &u32Len, 
        (u8*)&struMsg1, sizeof(ZC_HandShakeMsg1));
    
    PCT_SendMsgToCloud((u8*)&g_u8MsgBuildBuffer, u32Len);
    pstruContoller->u8MainState = PCT_STATE_WAIT_ACCESSRSP;
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
    u32 u32Len;
    ZC_HandShakeMsg3 struMsg3;
    
    memcpy(struMsg3.RandMsg, pstruContoller->RandMsg, ZC_HS_MSG_LEN);

    SEC_Encrypt(pstruContoller, 
        pstruContoller->u8SessionKey, 
        pstruContoller->IvSend,
        (u8*)&struMsg3,
        sizeof(ZC_HandShakeMsg3));
    
    EVENT_BuildMsg(pstruContoller, ZC_CODE_HANDSHAKE_3, 1, g_u8MsgBuildBuffer, &u32Len, 
        (u8*)&struMsg3, sizeof(ZC_HandShakeMsg3));
    
    PCT_SendMsgToCloud((u8*)&g_u8MsgBuildBuffer, u32Len);
    pstruContoller->u8MainState = PCT_STATE_WAIT_MSG4;
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
        s32RetVal = SEC_DecipherTextByRsa(pstruContoller->u8MoudlePrivateKey,
            pstruMsg->payload,
            (u8*)&struMsg2);    
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
    ZC_TraceData("status = %d\n", pstruContoller->u8MainState);
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
void PCT_SendMsgToCloud(u8 *pu8Msg, u32 u32Len)
{
    u32 u32Index;
    ZC_Message *pstruMsg;
    for (u32Index = 0; u32Index < MSG_BUFFER_SEND_MAX_NUM; u32Index++)
    {
        if (MSG_BUFFER_IDLE == g_struSendBuffer[u32Index].u8Status)
        {
            memcpy(g_struSendBuffer[u32Index].u8MsgBuffer, pu8Msg, u32Len);
            g_struSendBuffer[u32Index].u32Len = u32Len;
            g_struSendBuffer[u32Index].u8Status = MSG_BUFFER_FULL;
            pstruMsg = (ZC_Message*)pu8Msg;
            SEC_Encrypt(&g_struProtocolController, 
                g_struProtocolController.u8SessionKey, 
                g_struProtocolController.IvSend,
                pstruMsg->payload,
                ZC_HTONS(pstruMsg->Payloadlen));            
            MSG_PushMsg(&g_struSendQueue, (u8*)&g_struSendBuffer[u32Index]);
            break;
        }
    }
}


/******************************* FILE END ***********************************/

