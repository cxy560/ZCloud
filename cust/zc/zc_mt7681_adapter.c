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
#include <zc_protocol_interface.h>
#include <zc_message_queue.h>
#include <zc_cloud_event.h>

extern PTC_ProtocolCon  g_struProtocolController;
PTC_ModuleAdapter g_struMt7681Adapter;

u32 g_u32Timer = 0;

extern MSG_Buffer g_struRecvBuffer;
extern MSG_Queue  g_struRecvQueue;
extern MSG_Queue  g_struSendQueue;



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
    ZC_Message *pstruMsg = NULL;
    u16 u16DataLen; 
    pstruBuf = (MSG_Buffer *)MSG_PopMsg(&g_struSendQueue); 
    
    if (NULL == pstruBuf)
    {
        if (60 == g_u32Timer)
        {
            ZC_Message struHeart;
            EVENT_BuildHeartMsg(NULL, (u8*)&struHeart, (u32*)&u16DataLen);
            pstruMsg = &struHeart;
        }
        else
        {
            return;
        }
    }
    else
    {
        pstruMsg = (ZC_Message*)pstruBuf->u8MsgBuffer;
        u16DataLen = ZC_HTONS(pstruMsg->Payloadlen) + sizeof(ZC_Message); 
    }
    
    
    if (ZC_CONNECT_TYPE_TCP == pstruConnection->u8ConnectionType)
    {
        ZC_Printf("send msg\n");
        uip_send((u8*)pstruMsg, u16DataLen);
    }
    else
    {
        iot_udp_send(pstruConnection->u32Socket, 
            (u8*)pstruMsg, u16DataLen, pstruConnection->u8IpAddress, pstruConnection->u32Port);
    }
    
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
            u32RetVal = MSG_PushMsg(&g_struRecvQueue, (u8*)&g_struRecvBuffer);
        }
    }
    
    if (ZC_RET_ERROR == u32RetVal)
    {
        
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
u32 MT_FirmwareUpdate(u8 *pu8NewVerFile, u32 u32DataLen)
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
u32 MT_SendDataToMoudle(u8 *pu8Data, u32 u32DataLen)
{
    IoT_uart_output(pu8Data, u32DataLen);
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
u32 MT_RecvDataFromMoudle(u8 *pu8Data, u32 u32DataLen)
{
    PCT_HandleMoudleEvent(pu8Data, u32DataLen);
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
u32 MT_GetCloudKey(u8 *pu8Key)
{
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
u32 MT_GetPrivateKey(u8 *pu8Key)
{
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
u32 MT_GetVersion(u8 *pu8Version)
{
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
u32 MT_GetDeviceId(u8 *pu8DeviceId)
{
    return ZC_RET_OK;
}
/*************************************************
* Function: MT_GetCloudIp
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 MT_GetCloudIp(u8 *pu8CloudIp)
{
    pu8CloudIp[0] = 192;
    pu8CloudIp[1] = 168;
    pu8CloudIp[2] = 1;
    pu8CloudIp[3] = 111;    
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
    u32 u32Index;
    
    pu16Test = resolv_lookup("www.baidu.com");

    if(NULL == pu16Test)
    {
        return ZC_RET_ERROR;
    }
    
    for (u32Index = 0; u32Index < 2; u32Index++)
    {
        ZC_Printf("%02x ", pu16Test[u32Index]);
    }
    ZC_Printf("\n");    
    
    ZC_Printf("Connect \n");
    if (ZC_IPTYPE_IPV4 == pstruConnection->u8IpType)
    {
        uip_ipaddr(ip, pstruConnection->u8IpAddress[0],
            pstruConnection->u8IpAddress[1],
            pstruConnection->u8IpAddress[2],
            pstruConnection->u8IpAddress[3]);
    }
    else 
    {
        uip_ip6addr(ip, pstruConnection->u8IpAddress[0],
            pstruConnection->u8IpAddress[1],
            pstruConnection->u8IpAddress[2],
            pstruConnection->u8IpAddress[3],
            pstruConnection->u8IpAddress[4],
            pstruConnection->u8IpAddress[5],
            pstruConnection->u8IpAddress[6],
            pstruConnection->u8IpAddress[7]);

    }
    
    if (ZC_CONNECT_TYPE_TCP == pstruConnection->u8ConnectionType)
    {
      	conn = uip_connect(&ip, ZC_HTONS((u16_t)pstruConnection->u32Port));

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
        udp_conn = uip_udp_new(&ip, ZC_HTONS((u16_t)pstruConnection->u32Port));
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
    g_struMt7681Adapter.pfunGetCloudIP = MT_GetCloudIp;    
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
void MT_CloudAppCall()
{

    /*Connect Cloud */
    if(uip_connected()) 
    {
        /*Send Access to Cloud*/
        ZC_Printf("Connect to cloud\n");
        g_u32Timer = 0;        
        PCT_SendEmptyMsg();
        return;
    }

    /*Connect Time Out */
    if(uip_timedout()) 
    {
        ZC_Printf("Time Out\n");
        PCT_ConnectCloud(&g_struProtocolController);
    }

    if(uip_acked()) 
    {
        g_u32Timer = 0;
    }
    
    if(uip_newdata()) 
    {
        g_u32Timer = 0;
        MT_RecvDataFromCloud((char *)uip_appdata, uip_datalen());
    }
    
    if(uip_poll()) 
    {
        g_u32Timer++;
        ZC_Printf("Timer = %d\n", g_u32Timer);
        MT_SendDataToCloud(&g_struProtocolController.struCloudConnection);

        if (g_u32Timer == 60)
        {
            g_u32Timer = 0;
        }
    }

    if(uip_closed() || uip_aborted()) 
    {
        PCT_DisConnectCloud(&g_struProtocolController);
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


