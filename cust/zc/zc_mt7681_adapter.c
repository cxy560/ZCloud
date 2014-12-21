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
#include <zc_module_interface.h>



extern PTC_ProtocolCon  g_struProtocolController;
PTC_ModuleAdapter g_struMt7681Adapter;

MSG_Buffer g_struRecvBuffer;
MSG_Buffer g_struRetxBuffer;
MSG_Buffer g_struClientBuffer;


MSG_Queue  g_struRecvQueue;
MSG_Buffer g_struSendBuffer[MSG_BUFFER_SEND_MAX_NUM];
MSG_Queue  g_struSendQueue;

u8 g_u8MsgBuildBuffer[MSG_BULID_BUFFER_MAXLEN];
u8 g_u8CiperBuffer[MSG_CIPER_BUFFER_MAXLEN];
u8 g_u8ClientCiperBuffer[MSG_CIPER_BUFFER_MAXLEN];
u8 g_u8ClientSendLen = 0;

struct timer g_struMtTimer[ZC_TIMER_MAX_NUM];

u16 g_u16TcpMss;
extern IOT_ADAPTER   	IoTpAd;
extern IOT_USR_CFG Usr_Cfg;

u16 g_u16LocalPort;
u16 g_u16LocalListenPort;
extern char ATCmdPrefixAT[];
extern MLME_STRUCT *pIoTMlme;

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
* Function: MT_GetLocalPortNum
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u16 MT_GetLocalPortNum()
{
    return g_u16LocalPort;
}
/*************************************************
* Function: MT_GetLocalListenPortNum
* Description: 
* Author: zw 
* Returns: 
* Parameter: 
* History:
*************************************************/
u16 MT_GetLocalListenPortNum()
{
    return g_u16LocalListenPort;
}

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
        uip_poll_conn(&uip_conns[g_struProtocolController.struCloudConnection.u32Socket]);
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
* Function: MT_SendDataToMoudle
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 MT_SendDataToMoudle(u8 *pu8Data, u16 u16DataLen)
{
    u8 u8MagicFlag[4] = {0x02,0x03,0x04,0x05};
    IoT_uart_output(u8MagicFlag, 4);
    IoT_uart_output(pu8Data, u16DataLen);
    return ZC_RET_OK;
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
* Function: MT_FirmwareUpdateFinish
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 MT_FirmwareUpdateFinish(u32 u32TotalLen)
{
    spi_flash_CopyApToSta(u32TotalLen);        
}
/*************************************************
* Function: MT_FirmwareUpdate
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 MT_FirmwareUpdate(u8 *pu8FileData, u32 u32Offset, u32 u32DataLen)
{
    u8 u8RetVal;
    u32 u32HeadLen = 128;
    u32 u32WritLen = 0;
    u32 u32DataStartOffset = 0;
    u32 u32FlashStartOffset = 0;
    /*use ap flash as backup*/
    if ((u32Offset + u32DataLen) <= u32HeadLen)
    {
        ZC_Printf("%d,%d,%d\n",u32Offset,u32DataLen,u32HeadLen);
        return ZC_RET_OK;
    }
    else if (((u32Offset + u32DataLen) > u32HeadLen) && (u32Offset < u32HeadLen))
    {
        ZC_Printf("1 %d,%d,%d\n",u32Offset,u32DataLen,u32HeadLen);
        u32WritLen =  u32DataLen - (u32HeadLen - u32Offset);  
        u32DataStartOffset = u32HeadLen - u32Offset;
        u32FlashStartOffset = 0;
    }
    else
    {
    
        ZC_Printf("2 %d,%d,%d\n",u32Offset,u32DataLen,u32HeadLen);
        u32WritLen = u32DataLen;  
        u32DataStartOffset = 0;
        u32FlashStartOffset = (u32Offset - u32HeadLen);
    }

    u32HeadLen = 128;
    while(u32WritLen >= u32HeadLen)
    {
        u8RetVal = spi_flash_update_fw(UART_FlASH_UPG_ID_AP_FW, u32FlashStartOffset, pu8FileData + u32DataStartOffset, u32HeadLen);

        ZC_Printf("1 ret = %d, %d, %d,\n", u8RetVal,u32FlashStartOffset,u32DataStartOffset);
        if (0 != u8RetVal)
        {
            return ZC_RET_ERROR;
        }
        
        u32WritLen -= u32HeadLen;
        u32DataStartOffset += u32HeadLen;
        u32FlashStartOffset += u32HeadLen;
    }

    if (u32WritLen > 0)
    {
        u8RetVal = spi_flash_update_fw(UART_FlASH_UPG_ID_AP_FW, u32FlashStartOffset, pu8FileData + u32DataStartOffset, u32WritLen);
        ZC_Printf("2 ret = %d, %d, %d,\n", u8RetVal,u32FlashStartOffset,u32DataStartOffset);
        if (0 != u8RetVal)
        {
            return ZC_RET_ERROR;
        }
    }
    
    return ZC_RET_OK;
}
/*************************************************
* Function: MT_Rest
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void MT_Rest()
{
    pIoTMlme->ATSetSmnt = TRUE;
    wifi_state_chg(WIFI_STATE_INIT, 0);                 
}
/*************************************************
* Function: MT_SendDataToNet
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void MT_SendDataToNet(u32 u32Fd, u8 *pu8Data, u16 u16DataLen, ZC_SendParam *pstruParam)
{
    if (0 == pstruParam->u8NeedPoll)
    {
        uip_send(pu8Data, u16DataLen);    
    }
    else
    {
        uip_poll_conn(&uip_conns[u32Fd]);

        if (uip_len > 0) 
        {
            ZC_Printf("pull have data %d to client\n", uip_len);
            uip_arp_out();
            mt76xx_dev_send();
        }
    }
}

/*************************************************
* Function: MT_StoreRegisterInfor
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 MT_StoreRegisterInfor(u8 u8Type, u8 *pu8Data, u16 u16DataLen)
{
    switch(u8Type)
    {
        case 0:
        {
            ZC_RegisterReq *pstruRegister;
            
            pstruRegister = (ZC_RegisterReq *)(pu8Data);
            
            memcpy(IoTpAd.UsrCfg.ProductKey, pstruRegister->u8ModuleKey, ZC_MODULE_KEY_LEN);
            memcpy(IoTpAd.UsrCfg.ProductName, pstruRegister->u8DeviceId, ZC_HS_DEVICE_ID_LEN);
            memcpy(IoTpAd.UsrCfg.ProductName + ZC_HS_DEVICE_ID_LEN, pstruRegister->u8Domain, ZC_DOMAIN_LEN);
            memcpy(IoTpAd.UsrCfg.ProductType, pstruRegister->u8EqVersion, ZC_EQVERSION_LEN);
        
            break;
        }
        case 1:
        {
            memcpy(&IoTpAd.UsrCfg.VendorName, pu8Data, u16DataLen);
            memcpy(&Usr_Cfg, &IoTpAd.UsrCfg, sizeof(IOT_USR_CFG));
            reset_usr_cfg(TRUE);
            break;        
        }
        default:
            break;
    }
    
    return ZC_RET_OK;
}

/*************************************************
* Function: MT_GetStoreInfor
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 MT_GetStoreInfor(u8 u8Type, u8 **pu8Data)
{
    switch(u8Type)
    {
        case ZC_GET_TYPE_CLOUDKEY:
            *pu8Data = IoTpAd.UsrCfg.CloudKey;
            break;
        case ZC_GET_TYPE_DEVICEID:
            *pu8Data = IoTpAd.UsrCfg.ProductName;
            break;
        case ZC_GET_TYPE_PRIVATEKEY:
            *pu8Data = IoTpAd.UsrCfg.ProductKey;
            break;
        case ZC_GET_TYPE_VESION:
            *pu8Data = IoTpAd.UsrCfg.ProductType;        
            break;
        case ZC_GET_TYPE_TOKENKEY:
            *pu8Data = IoTpAd.UsrCfg.VendorName;        
            break;
    }
    return ZC_RET_OK;
}

/*************************************************
* Function: MT_StopTimer
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void MT_StopTimer(u8 u8TimerIndex)
{
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
    
	//UIP_UDP_CONN *udp_conn=NULL;
    uip_ipaddr_t ip;
    //uip_ipaddr_t broadcastip;

    u16 *pu16Test = NULL;

    pu16Test = resolv_lookup(IoTpAd.UsrCfg.CloudAddr);

    if(NULL == pu16Test)
    {
        return ZC_RET_ERROR;
    }

    
    ZC_Printf("Connect \n");
    if (ZC_IPTYPE_IPV4 == pstruConnection->u8IpType)
    {
        uip_ipaddr(ip, 42,62,41,75);
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
            conn->lport = (u16)rand();
        }

    	pstruConnection->u32Socket = conn->fd;
    	g_u16LocalPort = ZC_HTONS(conn->lport);

    	ZC_Printf("Connection Sokcet = %d, conn->lport = %d\n",conn->fd, g_u16LocalPort);
    }
    else
    {

    }
#if 0    
    /*add broadcast ip*/
    uip_ipaddr(broadcastip, 255,255,255,255);
    udp_conn = uip_udp_new(&broadcastip, ZC_HTONS(ZC_MOUDLE_BROADCAST_PORT));
    if(udp_conn != NULL) {
        ZC_Printf("setup Bc channel\n");
        uip_udp_bind(udp_conn, ZC_HTONS(ZC_MOUDLE_PORT));
    }
#endif
    return ZC_RET_OK;
}
/*************************************************
* Function: MT_ListenClient
* Description: 
* Author: zw 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 MT_ListenClient(PTC_Connection *pstruConnection)
{
    
	UIP_UDP_CONN *udp_conn=NULL;
    uip_ipaddr_t broadcastip;
    
    ZC_Printf("Listen \n");
    
    if (ZC_CONNECT_TYPE_TCP == pstruConnection->u8ConnectionType)
    {
        uip_listen(ZC_HTONS(pstruConnection->u16Port));
        
    	g_u16LocalListenPort = pstruConnection->u16Port;

    	ZC_Printf("Tcp Listen Port = %d\n", pstruConnection->u16Port);


  	    uip_ipaddr(broadcastip, 255,255,255,255);
        udp_conn = uip_udp_new(&broadcastip, ZC_HTONS(ZC_MOUDLE_BROADCAST_PORT));
        if(udp_conn != NULL) {
            ZC_Printf("setup Bc channel\n");
            uip_udp_bind(udp_conn, ZC_HTONS(ZC_MOUDLE_PORT));
        }
    }
    else
    {

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
    g_struMt7681Adapter.pfunListenClient = MT_ListenClient;
    g_struMt7681Adapter.pfunSendToNet = MT_SendDataToNet;   
    g_struMt7681Adapter.pfunStoreInfo = MT_StoreRegisterInfor;
    g_struMt7681Adapter.pfunRest = MT_Rest;
    g_struMt7681Adapter.pfunUpdate = MT_FirmwareUpdate;  
    g_struMt7681Adapter.pfunUpdateFinish = MT_FirmwareUpdateFinish;
    g_struMt7681Adapter.pfunSendToMoudle = MT_SendDataToMoudle;  

    g_struMt7681Adapter.pfunSetTimer = MT_SetTimer;  
    g_struMt7681Adapter.pfunStopTimer = MT_StopTimer;
    g_struMt7681Adapter.pfunGetStoreInfo = MT_GetStoreInfor;
    
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
* Function: MT_BroadcastAppCall
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void MT_BroadcastAppCall()
{
    u16 u16Len;
    ZC_ClientQueryRsp struRsp;
    u8 *pu8DeviceId;
    
    if (uip_poll())
    {
    
        if (PCT_STATE_CONNECT_CLOUD  != g_struProtocolController.u8MainState)
        {
            return;
        }

        EVENT_BuildBcMsg(g_u8MsgBuildBuffer, &u16Len);

        if (g_struProtocolController.u16SendBcNum < PCT_SEND_BC_MAX_NUM)
        {   
            uip_send(g_u8MsgBuildBuffer, u16Len);
            g_struProtocolController.u16SendBcNum++;
        }
    }

    if (uip_newdata())
    {
        struRsp.addr[0] = uip_ipaddr1(uip_hostaddr);
        struRsp.addr[1] = uip_ipaddr2(uip_hostaddr);        
        struRsp.addr[2] = uip_ipaddr3(uip_hostaddr);
        struRsp.addr[3] = uip_ipaddr4(uip_hostaddr);
        
        g_struProtocolController.pstruMoudleFun->pfunGetStoreInfo(ZC_GET_TYPE_DEVICEID, &pu8DeviceId);

        memcpy(struRsp.DeviceId, pu8DeviceId, ZC_HS_DEVICE_ID_LEN);
        memcpy(struRsp.u8Domain, pu8DeviceId + ZC_HS_DEVICE_ID_LEN, ZC_DOMAIN_LEN);
        EVENT_BuildMsg(ZC_CODE_CLIENT_QUERY_RSP, 0, g_u8MsgBuildBuffer, &u16Len, &struRsp, sizeof(ZC_ClientQueryRsp));
        uip_send(g_u8MsgBuildBuffer, u16Len);
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
    u32 u32Timer = 0;

    u32Timer = rand();
    u32Timer = (PCT_TIMER_INTERVAL_RECONNECT) * (u32Timer % 10 + 1);
    
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
        ZC_Printf("uip flag = %d, timer = %d, Close Connection\n",uip_flags,u32Timer);
        PCT_ReconnectCloud(&g_struProtocolController, u32Timer);
        PCT_SendNotifyMsg(ZC_CODE_CLOUD_DISCONNECT);
        uip_abort();
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
            ZC_Printf("disconnect, timer = %d\n", g_struProtocolController.u8MainState, u32Timer);
            uip_abort();
            PCT_ReconnectCloud(&g_struProtocolController, u32Timer);
        }
        else
        {
            MT_SendDataToCloud(&g_struProtocolController.struCloudConnection);
        }
    }
}

/*************************************************
* Function: MT_ClientAppCall
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void MT_ClientAppCall()
{
    MSG_Buffer *pstruBuf;

    if(uip_connected()) 
    {
    }

    if(uip_newdata()) 
    {
        ZC_RecvDataFromClient(uip_conn->fd, (char *)uip_appdata, uip_datalen());
    }
    
    if(uip_poll()) 
    {
        if (g_u8ClientSendLen > 0)
        {
            uip_send(g_u8MsgBuildBuffer, g_u8ClientSendLen);
        }
    }

    if(uip_closed())
    {
        
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
    ZC_StartClientListen();
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


