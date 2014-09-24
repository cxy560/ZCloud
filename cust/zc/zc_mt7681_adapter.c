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
#include <iot_tcpip_interface.h>
#include <uip.h>

extern PTC_ProtocolCon  g_struProtocolController;
PTC_ModuleAdapter g_struMt7681Adapter;

u32 g_u32Timer = 0;

/*************************************************
* Function: MT_SendDataToCloud
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 MT_SendDataToCloud(PTC_Connection *pstruConnection, u8 *pu8Data, u32 u32DataLen)
{
    u32 u32Ret = ZC_RET_OK;
    s32 s32CallRet = ZC_RET_OK;
    Printf_High("MT_SendDataToCloud type = %d\n",pstruConnection->u8ConnectionType);
    if (ZC_CONNECT_TYPE_TCP == pstruConnection->u8ConnectionType)
    {
        Printf_High("send msg\n");
        uip_send(pu8Data, u32DataLen);
        return ZC_RET_OK;
    }
    else
    {
        s32CallRet = iot_udp_send(pstruConnection->u32Socket, 
            pu8Data, u32DataLen, pstruConnection->u8IpAddress, pstruConnection->u32Port);
    }
}
/*************************************************
* Function: MT_SendDataToCloud
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 MT_RecvDataFromCloud(u32 u32Socket,u8 *pu8Data, u32 u32DataLen)
{
    
    return ZC_RET_OK;
}
/*************************************************
* Function: MT_SendDataToCloud
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
* Function: MT_SendDataToCloud
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
* Function: MT_SendDataToCloud
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
* Function: MT_SendDataToCloud
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
* Function: MT_SendDataToCloud
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
* Function: MT_SendDataToCloud
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
* Function: MT_SendDataToCloud
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
* Function: MT_SendDataToCloud
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
* Function: MT_SendDataToCloud
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
        Printf_High("%02x ", pu16Test[u32Index]);
    }
    Printf_High("\n");    
    
    Printf_High("Connect \n");
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
      	conn = uip_connect(&ip, htons((u16_t)pstruConnection->u32Port));

    	if (NULL == conn) 
    	{
    		return ZC_RET_ERROR;
    	}

        if (conn) {
            conn->lport = HTONS(ZC_MOUDLE_PORT);
        }

    	pstruConnection->u32Socket = conn->fd;

    	Printf_High("Connection Sokcet = %d\n",conn->fd);
    }
    else
    {
        udp_conn = uip_udp_new(&ip, HTONS((u16_t)pstruConnection->u32Port));
        if (NULL == udp_conn)
        {
            return ZC_RET_ERROR;
        }
        if (udp_conn) {
            uip_udp_bind(udp_conn, HTONS(ZC_MOUDLE_PORT));
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
    Printf_High("MT Init\n");
    g_struMt7681Adapter.pfunConnectToCloud = MT_ConnectToCloud;
    g_struMt7681Adapter.pfunSendToCloud = MT_SendDataToCloud;   
    g_struMt7681Adapter.pfunRecvFormCloud = MT_RecvDataFromCloud; 
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
u32 MT_CloudAppCall()
{

    Printf_High("uip flag = %d\n", uip_flags);

    /*Connect Cloud */
    if(uip_connected()) 
    {
        /*Send Access to Cloud*/
        Printf_High("Connect to cloud\n");
        PCT_SendCloudAccessMsg2(&g_struProtocolController);
        g_u32Timer = 0;        
        return;
    }

    /*Connect Time Out */
    if(uip_timedout()) 
    {
        Printf_High("Time Out\n");
        PCT_ConnectCloud(&g_struProtocolController);
    }


    if(uip_acked()) 
    {
        g_u32Timer = 0;
        //acked();
    }
    if(uip_newdata()) 
    {
        g_u32Timer = 0;
        PCT_HandleCloudEvent((char *)uip_appdata, uip_datalen());
    }
    
    if(uip_rexmit() ||
       uip_newdata() ||
       uip_acked()) 
    {
        /*senddata();*/
        g_u32Timer = 0;
    } 
    else if(uip_poll()) 
    {
        g_u32Timer++;
        Printf_High("Timer = %d\n", g_u32Timer);
        if (g_u32Timer == 60)
        {
            MT_SendDataToCloud(&g_struProtocolController.struCloudConnection,"h",sizeof("h"));
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
    //Printf_High("MT Wakeup\n");
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

/*************************************************
* Function: MT_DealTimer
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void MT_DealTimer()
{
    //PCT_ConnectCloud(PTC_ProtocolCon * pstruContoller)
}

/******************************* FILE END ***********************************/


