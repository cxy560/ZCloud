/**
******************************************************************************
* @file     zc_cloud_event.c
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief    Event
******************************************************************************
*/
#include <zc_cloud_event.h>
#include <zc_protocol_interface.h>
#include <uiplib.h>
#include <uip.h>
/*************************************************
* Function: EVENT_BuildAccessMsg
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32  EVENT_BuildAccessMsg2(PTC_ProtocolCon *pstruCon, u8 *pu8Msg, u32 *pu32Len)
{
    ZC_Message *pstruMsg = NULL;
    ZC_HandShakeMsg2 *pstruMsg2 = NULL;
    pstruMsg = (ZC_Message *)pu8Msg;
    pstruMsg->MsgCode = ZC_CODE_HANDSHAKE_2;
    pstruMsg->MsgId = 1;     /*ToDo*/
    pstruMsg->Payloadlen = HTONS(sizeof(ZC_HandShakeMsg2));
    pstruMsg->Version = ZC_VERSION;

    pstruMsg2 = (ZC_HandShakeMsg2 *)pstruMsg->payload;
    memcpy(pstruMsg2->DeviceId, pstruCon->u8DeviceId, ZC_HS_DEVICE_ID_LEN);
    memcpy(pstruMsg2->RandMsg, pstruCon->u8Msg1Rand, ZC_HS_MSG_LEN);

    *pu32Len = sizeof(ZC_Message) + pstruMsg->Payloadlen;
    return ZC_RET_OK;
}

/*************************************************
* Function: EVENT_BuildErrorMsg
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32  EVENT_BuildErrorMsg(PTC_ProtocolCon *pstruCon, u8 u8MsgId, u8 u8ErrorCode, u8 *pu8Error, u16 u16ErrorLen,
    u8 *pu8Msg, u32 *pu32Len)
{
    ZC_Message *pstruMsg = NULL;
    ZC_ErrorMsg *pstruErrorMsg = NULL;
    pstruMsg = (ZC_Message *)pu8Msg;
    pstruMsg->MsgCode = ZC_CODE_ERR;
    pstruMsg->MsgId = u8MsgId;  
    pstruMsg->Payloadlen = HTONS(sizeof(ZC_ErrorMsg) + u16ErrorLen);
    pstruMsg->Version = ZC_VERSION;

    pstruErrorMsg = (ZC_ErrorMsg *)pstruMsg->payload;
    pstruErrorMsg->ErrorCode = u8ErrorCode;
    memcpy(pstruErrorMsg->ErrorMsg, pu8Error, u16ErrorLen);
    
    *pu32Len = sizeof(ZC_Message) + pstruMsg->Payloadlen;
    return ZC_RET_OK;
}
/*************************************************
* Function: EVENT_BuildErrorMsg
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32  EVENT_BuildEmptyMsg(PTC_ProtocolCon *pstruCon, u8 *pu8Msg, u32 *pu32Len)
{
    ZC_Message *pstruMsg = NULL;
    pstruMsg = (ZC_Message *)pu8Msg;
    pstruMsg->MsgCode = ZC_CODE_EMPTY;
    pstruMsg->MsgId = 0;  
    pstruMsg->Payloadlen = 0;
    pstruMsg->Version = ZC_VERSION;


    *pu32Len = sizeof(ZC_Message) + pstruMsg->Payloadlen;
    return ZC_RET_OK;
}

/*************************************************
* Function: EVENT_BuildHeartMsg
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32  EVENT_BuildHeartMsg(PTC_ProtocolCon *pstruCon, u8 *pu8Msg, u32 *pu32Len)
{
    ZC_Message *pstruMsg = NULL;
    pstruMsg = (ZC_Message *)pu8Msg;
    pstruMsg->MsgCode = ZC_CODE_HEARTBEAT;
    pstruMsg->MsgId = 0;  
    pstruMsg->Payloadlen = 0;
    pstruMsg->Version = ZC_VERSION;

    *pu32Len = sizeof(ZC_Message) + pstruMsg->Payloadlen;
    return ZC_RET_OK;
}
/******************************* FILE END ***********************************/
