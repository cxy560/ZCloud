#include <zc_common.h>
#include <uip.h>
#include <uiplib.h>
#include <zc_message_queue.h>
#include <iot_tcpip_interface.h>
#include <zc_protocol_interface.h>
#include <zc_protocol_controller.h>
#include <windows.h>
#include <zc_sec_engine.h>
#include "rsa_genkey.h"

u8 g_u8DumpCloudMsg[10240];
extern MSG_Buffer g_struRecvBuffer;
extern MSG_Queue  g_struRecvQueue;


void SimRecvMsg(u8 u8Code, u8 *pu8msg, u16 u16Datalen)
{
    ZC_Message *pstruMsg;
    u32 u32Index;

    pstruMsg = (ZC_Message *)uip_appdata;

    pstruMsg->MsgCode = u8Code;

    pstruMsg->Payloadlen = ZC_HTONS(u16Datalen);
    memcpy(pstruMsg->payload, pu8msg, u16Datalen);
    
    uip_flags = UIP_NEWDATA;
    uip_len = u16Datalen + sizeof(ZC_Message);
}

void logicTest()
{
    u32 u32AccessOk = 0;
    MT_Init();


    while(1)
    {
        ZC_Printf("u32Access status = %d\n", u32AccessOk);
        if (1 == u32AccessOk)
        {
            MT_WakeUp();
            
        }
        else if (0 == u32AccessOk)
        {
           
        }
        else
        {
            
        }
        MT_TimerExpired();
        
        if (5 == u32AccessOk)
        {
            uip_flags = UIP_TIMEDOUT;
        }
        /*else if (8 == u32AccessOk)
        {
            uip_flags = UIP_CONNECTED;
        }
        else if (u32AccessOk == 9)
        {
            SimRecvMsg(ZC_CODE_HANDSHAKE_2);
        }
        else if (u32AccessOk == 10)
        {
            SimRecvMsg(ZC_CODE_HANDSHAKE_4);
        }*/
        if (g_struProtocolController.u8MainState == PCT_STATE_WAIT_ACCESSRSP)
        {
            SimRecvMsg(ZC_CODE_HANDSHAKE_2, g_struProtocolController.RandMsg, sizeof(ZC_HandShakeMsg2));
        }
        if (g_struProtocolController.u8MainState == PCT_STATE_WAIT_MSG4)
        {
            //SimRecvMsg(ZC_CODE_HANDSHAKE_4, g_struProtocolController.RandMsg, sizeof(ZC_HandShakeMsg4));
        }
        MT_CloudAppCall();
        MT_Run();
        u32AccessOk++;
    }

}

void testqueue()
{
    u8 u8TestMsg[100];
    u32 u32Index;
    u32 u32RetVal;
    u8 *pu8Msg;
    
    MSG_Queue struTestqueue;
    
    for (u32Index = 0; u32Index < 100; u32Index++)
    {
        u8TestMsg[u32Index] = u32Index;
    }
    MSG_InitQueue(&struTestqueue);
    
    for (u32Index = 0; u32Index < 3; u32Index++)
    {
        pu8Msg = MSG_PopMsg(&struTestqueue);
        if (NULL == pu8Msg)
        {
            ZC_Printf("No Msg\n", u32Index);
        }
        else
        {
            ZC_Printf("msg %d is pop\n", *pu8Msg);
        }
    }
    
    for (u32Index = 0; u32Index < 1; u32Index++)
    {
        u32RetVal = MSG_PushMsg(&struTestqueue, &u8TestMsg[u32Index]);
        if (ZC_RET_ERROR == u32RetVal)
        {
            //Printf_High("Msg Queue is full, Num %d drop\n", u32Index);
        }
        else
        {
            ZC_Printf("msg %d is pushed\n", u32Index);
        }
    }
    
    for (u32Index = 0; u32Index < 2; u32Index++)
    {
        pu8Msg = MSG_PopMsg(&struTestqueue);
        if (NULL == pu8Msg)
        {
            //Printf_High("No Msg\n", u32Index);
        }
        else
        {
            ZC_Printf("msg %d is pop\n", *pu8Msg);
        }
    }
    
    for (u32Index = 0; u32Index < 100; u32Index++)
    {
        u32RetVal = MSG_PushMsg(&struTestqueue, &u8TestMsg[u32Index]);
        if (ZC_RET_ERROR == u32RetVal)
        {
            //Printf_High("Msg Queue is full, Num %d drop\n", u32Index);
        }
        else
        {
            ZC_Printf("msg %d is pushed\n", u32Index);
        }
    }

    for (u32Index = 0; u32Index < 3; u32Index++)
    {
        pu8Msg = MSG_PopMsg(&struTestqueue);
        if (NULL == pu8Msg)
        {
            //Printf_High("No Msg\n", u32Index);
        }
        else
        {
            ZC_Printf("msg %d is pop\n", *pu8Msg);
        }
    }
} 
void testsendcloud()
{
    u32 u32Index;
    MT_Init();
    
    for (u32Index = 0; u32Index < 1024; u32Index++)
    {
        g_u8DumpCloudMsg[u32Index] = u32Index;
    }
    PCT_SendMsgToCloud(g_u8DumpCloudMsg, 100);
    
    MT_SendDataToCloud(&g_struProtocolController.struCloudConnection);
    MT_SendDataToCloud(&g_struProtocolController.struCloudConnection);

}

void testrecvbuffer()
{
    ZC_Message *pstruMsg;
    u32 u32Index;
    u16 u16Len;

    MT_Init();
    
    pstruMsg = (ZC_Message *)g_u8DumpCloudMsg;
    
    u16Len = 100;
    pstruMsg->Payloadlen = ZC_HTONS(u16Len);
    for (u32Index = 0; u32Index < u16Len; u32Index++)
    {
        pstruMsg->payload[u32Index] = u32Index;
    }
    
    for (u32Index = 0; u32Index < 210; u32Index++)
    {
        MT_RecvDataFromCloud((u8 *)(g_u8DumpCloudMsg + 10 * u32Index), 1000);
    }
    
    ZC_Printf("status = %d, len = %d\n",g_struRecvBuffer.u8Status, g_struRecvBuffer.u32Len);
    
    for (u32Index = 0; u32Index < g_struRecvBuffer.u32Len; u32Index++)
    {
        ZC_Printf("%02X ", g_struRecvBuffer.u8MsgBuffer[u32Index]);
    }
    
    ZC_Printf("\n");
    
    
    PCT_HandleEvent(&g_struProtocolController);
    
    while(1)
    {
        MT_TimerExpired();
        if (g_struProtocolController.u8ReSendMoudleNum == 2)
        {
            PCT_HandleMoudleEvent((u8 *)(g_u8DumpCloudMsg), 100);
        }
        
    }
}




void main()
{
  newrsa();
}