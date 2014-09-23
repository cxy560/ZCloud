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

/*************************************************
* Function: EVENT_BuildAccessMsg
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32  EVENT_BuildAccessMsg(PTC_ProtocolCon *pstruCon, u8 *pu8Msg, u32 *pu32Len)
{
    u32 u32Index;
    for (u32Index = 0; u32Index < ZC_HELLO_MSG_LEN; u32Index++)
    {
        pu8Msg[u32Index] = u32Index;
    }
    *pu32Len = ZC_HELLO_MSG_LEN;

    return ZC_RET_OK;
}


/******************************* FILE END ***********************************/
