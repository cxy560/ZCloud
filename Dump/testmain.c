#include <zc_common.h>
#include <uip.h>
#include <uiplib.h>
#include <zc_message_queue.h>
#include <iot_tcpip_interface.h>
#include <zc_protocol_interface.h>

u8 g_u8DumpCloudMsg[10240];
extern MSG_Buffer g_struRecvBuffer;
extern MSG_Queue  g_struRecvQueue;


void SimRecvMsg()
{
    ZC_Message *pstruMsg;
    u32 u32Index;
    u16 u16Len;

    pstruMsg = (ZC_Message *)uip_appdata;

    u16Len = 100;
    pstruMsg->Payloadlen = ZC_HTONS(u16Len);
    for (u32Index = 0; u32Index < u16Len; u32Index++)
    {
        pstruMsg->payload[u32Index] = u32Index;
    }
    
    uip_flags = UIP_NEWDATA;
    uip_len = u16Len + sizeof(ZC_Message);
}

void logicTest()
{
    u32 u32AccessOk = 0;
    MT_Init();


    while(1)
    {
        if (1 == u32AccessOk)
        {
            MT_WakeUp();
            
        }
        else if (0 == u32AccessOk)
        {
           
        }
        else
        {
            SimRecvMsg();
        }
        MT_CloudAppCall();
        MT_Run();
        uip_flags = UIP_POLL;
        MT_CloudAppCall();
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
}
void main()
{
  logicTest();
}