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


u8 g_u8cloud[] = {
0xb3,0xfa,0x85,0x5a,
0xb7,0x5b,0xf0,0xbd,
0xc8,0x5d,0xa9,0xa8,
0x93,0x09,0xdc,0x6f,
0x6a,0x46,0x2b,0x37,
0xaf,0xe0,0x7c,0xf4,
0x97,0x7c,0x78,0x0b,
0x4b,0x29,0x25,0xe0,
0x9d,0x6d,0xc3,0x05,
0x7b,0x21,0x52,0x71,
0x58,0xf5,0x06,0x22,
0x72,0xcd,0x98,0xfc,
0xc5,0xc2,0x63,0x3a,
0xf0,0x76,0xef,0x09,
0x7b,0x22,0x99,0x6e,
0xeb,0x85,0xa5,0x38,
0x71,0xde,0xd0,0x2c,
0x1b,0x6f,0x5c,0x87,
0x28,0x37,0xc4,0x82,
0xda,0xb9,0x97,0xde,
0x1e,0xb5,0xaf,0xdb,
0xb7,0x3c,0x71,0xe3,
0x1a,0x78,0xfa,0x67,
0x33,0x56,0xc1,0x4b,
0x74,0xc9,0xa8,0x7f,
0xfd,0x30,0xec,0x9a,
0x77,0xc6,0xe9,0xe0,
0x3a,0xe9,0x32,0xd8,
0xc0,0x24,0x40,0xce,
0xef,0x43,0xcc,0x7c,
0xa8,0xa5,0x7a,0x58,
0xe0,0xa9,0x41,0x13,
0x01,0x00,0x01    
};

u8 g_moudleKey[] = {                    
0xb3,0xfa,0x85,0x5a,
0xb7,0x5b,0xf0,0xbd,
0xc8,0x5d,0xa9,0xa8,
0x93,0x09,0xdc,0x6f,
0x6a,0x46,0x2b,0x37,
0xaf,0xe0,0x7c,0xf4,
0x97,0x7c,0x78,0x0b,
0x4b,0x29,0x25,0xe0,
0x9d,0x6d,0xc3,0x05,
0x7b,0x21,0x52,0x71,
0x58,0xf5,0x06,0x22,
0x72,0xcd,0x98,0xfc,
0xc5,0xc2,0x63,0x3a,
0xf0,0x76,0xef,0x09,
0x7b,0x22,0x99,0x6e,
0xeb,0x85,0xa5,0x38,
0x71,0xde,0xd0,0x2c,
0x1b,0x6f,0x5c,0x87,
0x28,0x37,0xc4,0x82,
0xda,0xb9,0x97,0xde,
0x1e,0xb5,0xaf,0xdb,
0xb7,0x3c,0x71,0xe3,
0x1a,0x78,0xfa,0x67,
0x33,0x56,0xc1,0x4b,
0x74,0xc9,0xa8,0x7f,
0xfd,0x30,0xec,0x9a,
0x77,0xc6,0xe9,0xe0,
0x3a,0xe9,0x32,0xd8,
0xc0,0x24,0x40,0xce,
0xef,0x43,0xcc,0x7c,
0xa8,0xa5,0x7a,0x58,
0xe0,0xa9,0x41,0x13,
0xe0,0xbb,0x6a,0x4c,
0x96,0x01,0x6b,0xe5,
0x16,0x19,0x2f,0x53,
0x10,0x4b,0xb5,0xb2,
0x4a,0x9f,0x18,0xa8,
0xe6,0xec,0x43,0xba,
0x87,0xb6,0x33,0x32,
0x83,0x01,0x83,0x86,
0xd7,0x95,0xb4,0x50,
0x56,0xd0,0xdb,0x9b,
0x00,0xc0,0xaa,0xcd,
0x74,0xd7,0x9c,0x32,
0x0e,0x32,0xdb,0x62,
0x69,0xf0,0xb8,0xa4,
0x17,0xbd,0x87,0xc6,
0x1e,0x6f,0x08,0x85,
0xcd,0x05,0x0f,0x75,
0xb4,0xf6,0x15,0x49,
0x26,0xb9,0x6a,0xb8,
0xae,0x15,0x6c,0x55,
0x24,0x01,0x74,0x4e,
0x5e,0x19,0x29,0xb9,
0xc9,0xa7,0x55,0xbe,
0x0d,0x57,0x84,0x62,
0xc4,0xd0,0xaf,0x18,
0x37,0x8a,0x17,0x38,
0x1f,0xa8,0xb8,0x75,
0x34,0x91,0x4b,0x21,
0xd4,0x3a,0x15,0x64,
0xbb,0x7e,0x03,0xae,
0x3b,0x15,0xb1,0x40,
0x44,0x2e,0xa2,0xb7,
0xb3,0x48,0x70,0x9e,
0x16,0x91,0x9d,0xbc,
0xcd,0xab,0x5b,0xd8,
0xbd,0x48,0x58,0xf4,
0x9a,0x99,0x5f,0xb7,
0xe7,0x39,0x97,0x32,
0xa8,0xef,0xce,0xda,
0x3e,0x2f,0x27,0xf9,
0x9b,0xe5,0x5f,0x4b,
0x34,0x67,0x3c,0xba,
0xe4,0x97,0xcf,0xeb,
0x46,0x8a,0x77,0x9e,
0x6b,0x3b,0x61,0x4f,
0x9a,0xca,0xa2,0xd2,
0xad,0xe9,0xd2,0xf6,
0x57,0xf3,0x26,0x6d,
0x22,0x6e,0x1d,0x20,
0x89,0x59,0x62,0x4d,
0xbc,0xa1,0x1a,0xd9,
0x14,0xf3,0x28,0x14,
0x4c,0x4d,0xaa,0x38,
0x96,0xf8,0xe6,0xd6,
0xfa,0x94,0x8d,0x83,
0xb3,0x3d,0xe5,0x4f,
0xa8,0xf9,0xda,0xcd,
0xd8,0x55,0xea,0x7c,
0x54,0x64,0xc3,0x45,
0x7b,0xa4,0x82,0xd7,
0x95,0x8a,0x83,0x99,
0x3d,0x1b,0x76,0x86,
0xc6,0x09,0x68,0x91,
0x00,0x62,0x4c,0x95,
0x32,0xfa,0xbf,0xd7,
0xc6,0x6a,0x84,0x78,
0xdf,0xff,0xcd,0xc3,
0x75,0xf8,0xb1,0x8b,
0x80,0x2e,0x12,0x5d,
0x70,0x6c,0xdf,0x05,
0xec,0xbd,0x2a,0x88,
0x81,0x09,0xe9,0xe5,
0xd2,0x5e,0xb8,0x75,
0x32,0xcf,0x93,0x53,
0x3a,0xab,0xd8,0x82,
0xd2,0x5b,0xb7,0xe0,
0x26,0xe3,0x79,0xfa,
0x6b,0xf5,0x2f,0x09,
0xc9,0x2f,0x25,0xc1,
0x5e,0x2f,0xe2,0x2e
};



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
    u32RetVal = MSG_RecvDataFromCloud(pu8Data, u32DataLen);

    if (ZC_RET_OK == u32RetVal)
    {
        if (MSG_BUFFER_FULL == g_struRecvBuffer.u8Status)
        {
            u32RetVal = SEC_Decrypt((ZC_SecHead*)g_u8CiperBuffer, 
                g_u8CiperBuffer + sizeof(ZC_SecHead), g_struRecvBuffer.u8MsgBuffer);

            g_struRecvBuffer.u32Len -= sizeof(ZC_SecHead);
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
    *pu8Key = g_u8cloud;//IoTpAd.UsrCfg.CloudKey;
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
    *pu8Key = g_moudleKey;//IoTpAd.UsrCfg.ProductKey;

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
        uip_ipaddr(ip, 192, 168, 1, 111);
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


