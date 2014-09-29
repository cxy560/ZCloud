/**
******************************************************************************
* @file     zc_cloud_event.c
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief    Event
******************************************************************************
*/
#include <zc_timer.h>
#include <zc_protocol_controller.h>
ZC_Timer g_struTimer[ZC_TIMER_MAX_NUM];

/*************************************************
* Function: TIMER_Init
* Description:
* Author: cxy
* Returns:
* Parameter:
* History:
*************************************************/
void TIMER_Init()
{
    u32 u32Index;
    for (u32Index = 0; u32Index < ZC_TIMER_MAX_NUM; u32Index++)
    {
        g_struTimer[u32Index].u8Status = ZC_TIMER_STATUS_IDLE;
    }
}


/*************************************************
* Function: TIMER_FindIdleTimer
* Description:
* Author: cxy
* Returns:
* Parameter:
* History:
*************************************************/
u32 TIMER_FindIdleTimer(u8 *pu8TimerIndex)
{
    u8 u8Index;
    for (u8Index = 0; u8Index < ZC_TIMER_MAX_NUM; u8Index++)
    {
        if (ZC_TIMER_STATUS_IDLE == g_struTimer[u8Index].u8Status)
        {
            *pu8TimerIndex = u8Index;
            return ZC_RET_OK;
        }

    }
    return ZC_RET_ERROR;
}

/*************************************************
* Function: TIMER_AllocateTimer
* Description:
* Author: cxy
* Returns:
* Parameter:
* History:
*************************************************/
void TIMER_AllocateTimer(u8 u8Type, u8 u8TimerIndex, u8 *pu8Timer)
{
    g_struTimer[u8TimerIndex].u8Status = ZC_TIMER_STATUS_USED;
    g_struTimer[u8TimerIndex].u8Type = u8Type;
    g_struTimer[u8TimerIndex].pu8Timer = pu8Timer;
    return;
}
/*************************************************
* Function: TIMER_GetTimerStatus
* Description:
* Author: cxy
* Returns:
* Parameter:
* History:
*************************************************/
void TIMER_GetTimerStatus(u8 u8TimerIndex, u8 *pu8Status)
{
    *pu8Status = g_struTimer[u8TimerIndex].u8Status;
}
/*************************************************
* Function: TIMER_TimeoutAction
* Description:
* Author: cxy
* Returns:
* Parameter:
* History:
*************************************************/
void TIMER_TimeoutAction(u8 u8TimerIndex)
{
    switch (g_struTimer[u8TimerIndex].u8Type)
    {
        case PCT_TIMER_RECONNECT:
            PCT_ConnectCloud(&g_struProtocolController);
            break;
        case PCT_TIMER_REACCESS:
            PCT_DisConnectCloud(&g_struProtocolController);
            break;
        case PCT_TIMER_SENDMOUDLE:            
            PCT_SendMoudleTimeout(&g_struProtocolController);
            break;
    }
}

/*************************************************
* Function: TIMER_StopTimer
* Description:
* Author: cxy
* Returns:
* Parameter:
* History:
*************************************************/
void TIMER_StopTimer(u8 u8TimerIndex)
{
    g_struTimer[u8TimerIndex].u8Status = ZC_TIMER_STATUS_IDLE;
}


/******************************* FILE END ***********************************/

