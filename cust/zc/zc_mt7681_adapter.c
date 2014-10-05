/**
******************************************************************************
* @file     MT7681Adapter.c
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief    Event
******************************************************************************
*/
#include <zc_protocol_controller.h>
#include <uiplib.h>
#include <uip.h>
#include <Random.h>
#include <xip_ovly.h>
#include <uip_timer.h>
#include <zc_timer.h>
#include <iot_api.h>

extern PTC_ProtocolCon  g_struProtocolController;
PTC_ModuleAdapter g_struMt7681Adapter;

MSG_Buffer g_struRecvBuffer;
MSG_Buffer g_struRetxBuffer;

MSG_Queue  g_struRecvQueue;
MSG_Buffer g_struSendBuffer[MSG_BUFFER_SEND_MAX_NUM];
MSG_Queue  g_struSendQueue;

u8 g_u8MsgBuildBuffer[MSG_BULID_BUFFER_MAXLEN];
u8 g_u8CiperBuffer[MSG_CIPER_BUFFER_MAXLEN];


struct timer g_struMtTimer[ZC_TIMER_MAX_NUM];

u16 g_u16TcpMss;
extern IOT_ADAPTER   	IoTpAd;
#ifndef ZC_OFF_LINETEST
/*************************************************
* Function: rand
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 rand()
{
    return apiRand();
}
#endif

/*************************************************
* Function: MT_TimerExpired
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void MT_TimerExpired()
{
    u8 u8Index;
    u8 u8Status;
    for (u8Index = 0; u8Index < ZC_TIMER_MAX_NUM; u8Index++)
    {   
        TIMER_GetTimerStatus(u8Index, &u8Status);
        if (ZC_TIMER_STATUS_USED == u8Status)
        {
            if (timer_expired(&g_struMtTimer[u8Index]))
            {
                TIMER_StopTimer(u8Index);
                TIMER_TimeoutAction(u8Index);
            }
        }
    }

    if ((g_struProtocolController.u8MainState >= PCT_STATE_WAIT_ACCESSRSP)
    && (PCT_INVAILD_SOCKET != g_struProtocolController.struCloudConnection.u32Socket))
    {
        uip_poll_conn(g_struProtocolController.struCloudConnection.u32Socket);
        if (uip_len > 0) 
        {
            ZC_Printf("pull have data %d\n", uip_len);
            uip_arp_out();
            mt76xx_dev_send();
        }
    }
    
}

/*************************************************
* Function: MT_SetTimer
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 MT_SetTimer(u8 u8Type, u32 u32Interval, u8 *pu8TimeIndex)
{
    u8 u8TimerIndex;
    u32 u32Retval;
    u32Retval = TIMER_FindIdleTimer(&u8TimerIndex);
    if (ZC_RET_OK == u32Retval)
    {
        TIMER_AllocateTimer(u8Type, u8TimerIndex, (u8*)&g_struMtTimer[u8TimerIndex]);
        timer_set(&g_struMtTimer[u8TimerIndex], u32Interval);
        *pu8TimeIndex = u8TimerIndex;
    }
    return u32Retval;
}


/*************************************************
* Function: MT_SendDataToCloud
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void MT_SendDataToCloud(PTC_Connection *pstruConnection)
{
    MSG_Buffer *pstruBuf = NULL;

    u16 u16DataLen; 
    pstruBuf = (MSG_Buffer *)MSG_PopMsg(&g_struSendQueue); 
    
    if (NULL == pstruBuf)
    {
        return;
    }
    
    u16DataLen = pstruBuf->u32Len; 
    uip_send((u8*)pstruBuf->u8MsgBuffer, u16DataLen);
    ZC_Printf("send data len = %d\n", u16DataLen);
    pstruBuf->u8Status = MSG_BUFFER_IDLE;
    pstruBuf->u32Len = 0;
    return;
}
/*************************************************
* Function: MT_RecvDataFromCloud
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void MT_RecvDataFromCloud(u8 *pu8Data, u32 u32DataLen)
{
    u32 u32RetVal;
    u16 u16PlainLen;
    u32RetVal = MSG_RecvDataFromCloud(pu8Data, u32DataLen);

    if (ZC_RET_OK == u32RetVal)
    {
        if (MSG_BUFFER_FULL == g_struRecvBuffer.u8Status)
        {
            u32RetVal = SEC_Decrypt((ZC_SecHead*)g_u8CiperBuffer, 
                g_u8CiperBuffer + sizeof(ZC_SecHead), g_struRecvBuffer.u8MsgBuffer, &u16PlainLen);

            g_struRecvBuffer.u32Len = u16PlainLen;
            if (ZC_RET_OK == u32RetVal)
            {
                u32RetVal = MSG_PushMsg(&g_struRecvQueue, (u8*)&g_struRecvBuffer);
            }
        }
    }
    
    return;
}
/*************************************************
* Function: MT_FirmwareUpdate
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 MT_FirmwareUpdate(u8 *pu8NewVerFile, u16 u16DataLen)
{
    return ZC_RET_OK;
}
/*************************************************
* Function: MT_SendDataToMoudle
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 MT_SendDataToMoudle(u8 *pu8Data, u16 u16DataLen)
{
    IoT_uart_output(pu8Data, u16DataLen);
    return ZC_RET_OK;
}
/*************************************************
* Function: MT_RecvDataFromMoudle
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 MT_RecvDataFromMoudle(u8 *pu8Data, u16 u16DataLen)
{
    PCT_HandleMoudleEvent(pu8Data, u16DataLen);
    return ZC_RET_OK;
}
/*************************************************
* Function: MT_GetCloudKey
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 MT_GetCloudKey(u8 **pu8Key)
{
    *pu8Key = IoTpAd.UsrCfg.CloudKey;
    return ZC_RET_OK;
}
/*************************************************
* Function: MT_GetPrivateKey
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 MT_GetPrivateKey(u8 **pu8Key)
{
    *pu8Key = IoTpAd.UsrCfg.ProductKey;

    return ZC_RET_OK;
}
/*************************************************
* Function: MT_GetVersion
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 MT_GetVersion(u8 **pu8Version)
{
    *pu8Version = IoTpAd.UsrCfg.ProductType;

    return ZC_RET_OK;
}
/*************************************************
* Function: MT_GetDeviceId
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 MT_GetDeviceId(u8 **pu8DeviceId)
{
    *pu8DeviceId = IoTpAd.UsrCfg.ProductName;
    return ZC_RET_OK;
}

/*************************************************
* Function: MT_ConnectToCloud
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 MT_ConnectToCloud(PTC_Connection *pstruConnection)
{
    struct uip_conn *conn=NULL;
    
	UIP_UDP_CONN *udp_conn=NULL;
    uip_ipaddr_t ip;

    u16 *pu16Test = NULL;
    
    pu16Test = resolv_lookup("www.baidu.com"/*IoTpAd.UsrCfg.CloudAddr*/);

    if(NULL == pu16Test)
    {
        return ZC_RET_ERROR;
    }
    
    ZC_Printf("Connect \n");
    if (ZC_IPTYPE_IPV4 == pstruConnection->u8IpType)
    {
        uip_ipaddr(ip, 192, 168, 1, 101);
    }
    else 
    {

    }

    if (ZC_CONNECT_TYPE_TCP == pstruConnection->u8ConnectionType)
    {
      	conn = uip_connect(&ip, ZC_HTONS((u16_t)pstruConnection->u16Port));

    	if (NULL == conn) 
    	{
    		return ZC_RET_ERROR;
    	}

        if (conn) {
            conn->lport = ZC_HTONS(ZC_MOUDLE_PORT);
        }

    	pstruConnection->u32Socket = conn->fd;

    	ZC_Printf("Connection Sokcet = %d\n",conn->fd);
    }
    else
    {
        udp_conn = uip_udp_new(&ip, ZC_HTONS((u16_t)pstruConnection->u16Port));
        if (NULL == udp_conn)
        {
            return ZC_RET_ERROR;
        }
        if (udp_conn) {
            uip_udp_bind(udp_conn, ZC_HTONS(ZC_MOUDLE_PORT));
        }

        pstruConnection->u32Socket = udp_conn->fd;
    }
    return ZC_RET_OK;
}
/*************************************************
* Function: MT_Init
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void MT_Init()
{
    ZC_Printf("MT Init\n");
    g_struMt7681Adapter.pfunConnectToCloud = MT_ConnectToCloud;
    g_struMt7681Adapter.pfunSendToCloud = MT_SendDataToCloud;   
    g_struMt7681Adapter.pfunUpdate = MT_FirmwareUpdate;        
    g_struMt7681Adapter.pfunSendToMoudle = MT_SendDataToMoudle;  
    g_struMt7681Adapter.pfunRecvFormMoudle = MT_RecvDataFromMoudle;
    g_struMt7681Adapter.pfunGetCloudKey = MT_GetCloudKey;   
    g_struMt7681Adapter.pfunGetPrivateKey = MT_GetPrivateKey; 
    g_struMt7681Adapter.pfunGetVersion = MT_GetVersion;    
    g_struMt7681Adapter.pfunGetDeviceId = MT_GetDeviceId;   
    g_struMt7681Adapter.pfunSetTimer = MT_SetTimer;   
    g_u16TcpMss = UIP_TCP_MSS;
    PCT_Init(&g_struMt7681Adapter);
}
/*************************************************
* Function: MT_CloudAppCall
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void MT_Rand(u8 *pu8Rand)
{
    u32 u32Rand;
    u32 u32Index; 
    for (u32Index = 0; u32Index < 10; u32Index++)
    {
        u32Rand = apiRand();
        memcpy((pu8Rand + 4 * u32Index), &u32Rand, 4);
    }
}

/*************************************************
* Function: MT_CloudAppCall
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void MT_CloudAppCall()
{

    /*Connect Cloud */
    if(uip_connected()) 
    {
        /*Send Access to Cloud*/
        ZC_Printf("Connect to cloud\n");
        MT_Rand(g_struProtocolController.RandMsg);
        PCT_SendCloudAccessMsg1(&g_struProtocolController);
        return;
    }

    /*Connect Time Out */
    if(uip_timedout() || uip_closed() || uip_aborted()) 
    {
        ZC_Printf("uip flag = %d, Close Connection\n",uip_flags);
        PCT_ReconnectCloud(&g_struProtocolController);
    }

    if(uip_acked()) 
    {
    }
    
    if(uip_newdata()) 
    {
        MT_RecvDataFromCloud((char *)uip_appdata, uip_datalen());
    }
    
    if(uip_poll()) 
    {
        if (PCT_STATE_DISCONNECT_CLOUD == g_struProtocolController.u8MainState)
        {
            ZC_Printf("disconnect\n", g_struProtocolController.u8MainState);
            uip_abort();
            PCT_ReconnectCloud(&g_struProtocolController);
        }
        else
        {
            ZC_Printf("poll \n");
            MT_SendDataToCloud(&g_struProtocolController.struCloudConnection);
        }
    }
}

/*************************************************
* Function: MT_WakeUp
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void MT_WakeUp()
{
    PCT_WakeUp();
}
/*************************************************
* Function: MT_Sleep
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void MT_Sleep()
{
    PCT_Sleep();
}

/*************************************************
* Function: MT_Run
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void MT_Run()
{
    PCT_Run();
}




/******************************* FILE END ***********************************/


