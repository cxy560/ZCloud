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

u8 g_u8DumpCloudMsg[102400];
extern MSG_Buffer g_struRecvBuffer;
extern MSG_Queue  g_struRecvQueue;
IOT_ADAPTER   	IoTpAd;

void SimRecvMsg(u8 u8Code, u8 *pu8msg, u16 u16Datalen)
{
    ZC_SecHead *pstruMsg;
    ZC_MessageHead *pstruHead;
    u32 u32Index;

    pstruMsg = (ZC_SecHead *)uip_appdata;
    pstruMsg->u8SecType = ZC_SEC_ALG_NONE;
    pstruMsg->u16TotalMsg = ZC_HTONS(u16Datalen + sizeof(ZC_MessageHead));
    
    pstruHead = (ZC_MessageHead*)(pstruMsg + 1);
    pstruHead->MsgCode = u8Code;

    pstruHead->Payloadlen = ZC_HTONS(u16Datalen);
    memcpy(pstruHead + 1, pu8msg, u16Datalen);
    
    uip_flags = UIP_NEWDATA;
    uip_len = u16Datalen + sizeof(ZC_MessageHead) + sizeof(ZC_SecHead);
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
    ZC_SecHead struHead;
    MT_Init();
    
    for (u32Index = 0; u32Index < 8; u32Index++)
    {
        g_u8DumpCloudMsg[u32Index] = u32Index;
    }
    struHead.u8SecType = ZC_SEC_ALG_NONE;
    struHead.u16TotalMsg = ZC_HTONS(8);
    PCT_SendMsgToCloud(&struHead, g_u8DumpCloudMsg);
  
    struHead.u8SecType = ZC_SEC_ALG_NONE;
    struHead.u16TotalMsg = ZC_HTONS(52);
    PCT_SendMsgToCloud(&struHead, g_u8DumpCloudMsg);

    MT_SendDataToCloud(&g_struProtocolController.struCloudConnection);
    MT_SendDataToCloud(&g_struProtocolController.struCloudConnection);

}

void testrecvbuffer()
{
    ZC_SecHead *pstruMsg;
    ZC_MessageHead *pstruHead;
    u32 u32Index;
    u16 u16Len;

    MT_Init();
    
    pstruMsg = (ZC_SecHead *)g_u8DumpCloudMsg;
    pstruHead = (ZC_MessageHead*)(pstruMsg+1);
    
    u16Len = 68;
    pstruMsg->u16TotalMsg  = ZC_HTONS(u16Len+sizeof(ZC_MessageHead));
    pstruHead->Payloadlen = ZC_HTONS(u16Len);
    
    for (u32Index = 0; u32Index < u16Len; u32Index++)
    {
        g_u8DumpCloudMsg[u32Index + sizeof(ZC_MessageHead) + sizeof(ZC_SecHead)] = u32Index;
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
    
    g_struProtocolController.u8keyRecv = 1;
    
    PCT_HandleEvent(&g_struProtocolController);
    
    while(1)
    {
        MT_TimerExpired();
        if (g_struProtocolController.u8ReSendMoudleNum == 2)
        {
            PCT_HandleMoudleEvent(0, 0, (u8 *)(g_u8DumpCloudMsg), 100);
        }
        
    }
}


void readcfg()
{
    u8 *pu8Test;
    MT_Init();
    testread();
    g_struProtocolController.pstruMoudleFun->pfunGetCloudKey(&pu8Test);
    g_struProtocolController.pstruMoudleFun->pfunGetDeviceId(&pu8Test);
    g_struProtocolController.pstruMoudleFun->pfunGetPrivateKey(&pu8Test);
    g_struProtocolController.pstruMoudleFun->pfunGetVersion(&pu8Test);
}
extern IOT_USR_CFG Usr_Cfg;
void testRecvAt()
{
    u8 u8At1[] = "AT#1";
    u8 u8At2[] = "AT#UPDATA";
    u8 u8At3[] = {0x41,0x54,0x23,0x57,0x50,0x44,0x41,0x54,0x41,0x45};
    MT_RecvDataFromMoudle(u8At1, sizeof(u8At1));
    MT_RecvDataFromMoudle(u8At2, sizeof(u8At2));
    MT_RecvDataFromMoudle(u8At3, sizeof(u8At3));
}
void main()
{
    u16 padding;
    MT_Init();
    memcpy(&IoTpAd.UsrCfg , &Usr_Cfg , sizeof(IOT_USR_CFG));

    testRecvAt();
}