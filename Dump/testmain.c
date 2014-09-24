#include <zc_common.h>
#include <uip.h>
#include <zc_message_queue.h>
#include <iot_tcpip_interface.h>

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
        else
        {
            u32AccessOk = 1;
        }
        MT_CloudAppCall();
        MT_Run();
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
            Printf_High("No Msg\n", u32Index);
        }
        else
        {
            Printf_High("msg %d is pop\n", *pu8Msg);
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
            Printf_High("msg %d is pushed\n", u32Index);
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
            Printf_High("msg %d is pop\n", *pu8Msg);
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
            Printf_High("msg %d is pushed\n", u32Index);
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
            Printf_High("msg %d is pop\n", *pu8Msg);
        }
    }
} 
void main()
{
  testqueue();
}