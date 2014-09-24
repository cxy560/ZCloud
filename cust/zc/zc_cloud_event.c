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
    pstruMsg->MsgId = HTONS(1);     /*ToDo*/
    pstruMsg->Payloadlen = sizeof(ZC_HandShakeMsg2);
    pstruMsg->Version = ZC_VERSION;

    pstruMsg2 = (ZC_HandShakeMsg2 *)pstruMsg->payload;
    memcpy(pstruMsg2->DeviceId, pstruCon->u8DeviceId, ZC_HS_DEVICE_ID_LEN);
    memcpy(pstruMsg2->RandMsg, pstruCon->u8Msg1Rand, ZC_HS_MSG_LEN);

    *pu32Len = sizeof(ZC_Message) + pstruMsg->Payloadlen;
    return ZC_RET_OK;
}


/******************************* FILE END ***********************************/
