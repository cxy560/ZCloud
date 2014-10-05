/**
******************************************************************************
* @file     zc_cloud_event.c
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief    Event
******************************************************************************
*/

#include <zc_common.h>
#include <zc_protocol_controller.h>
#include <zc_sec_engine.h>
extern 	char *optarg; 

u32 g_TraceSwitch = 0;
/*************************************************
* Function: ZC_TraceData
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void ZC_TraceData(u8* pData, u32 Len)
{
    u32 Index;
    ZC_Printf("++++++++++++++++++++++++++++++++++++++++++++++++\n");
    for (Index = 0; Index + 4 < Len; Index = Index + 4)
    {
        ZC_Printf("%02x %02x %02x %02x\n",
            pData[Index],
            pData[Index + 1],
            pData[Index + 2],
            pData[Index + 3]);
    }
    
    for (; Index < Len; Index++)
    {
        ZC_Printf("%02x ", pData[Index]);
    }
    
    ZC_Printf("\n++++++++++++++++++++++++++++++++++++++++++++++++\n");
}

void TestSec()
{
    u32 u32Index;
    u16 u16CiperLen;
    u16 u16PlainLen;
    u16 u16InputLen;
    u32 u32Retval;
    MT_Init();
    
    u16InputLen = 64;
    for (u32Index = 0; u32Index < u16InputLen; u32Index++)
    {
        g_struRecvBuffer.u8MsgBuffer[u32Index] = u32Index;
    }
    
    ZC_Printf("+++++++++++++++++++RSA+++++++++++++++++++\n");
    u32Retval = SEC_EncryptTextByRsa(g_u8CiperBuffer, g_struRecvBuffer.u8MsgBuffer, u16InputLen, &u16CiperLen);
    if (ZC_RET_ERROR == u32Retval)
    {
        return;
    }
    
    ZC_TraceData(g_struRecvBuffer.u8MsgBuffer, u16InputLen);
    ZC_Printf("+++++++++++++\n");
    ZC_TraceData(g_u8CiperBuffer, u16CiperLen);
    ZC_Printf("+++++++++++++\n");
    u32Retval = SEC_DecryptTextByRsa(g_u8CiperBuffer, g_struRetxBuffer.u8MsgBuffer, u16CiperLen, &u16PlainLen);
    if (ZC_RET_ERROR == u32Retval)
    {
        return;
    }
    ZC_Printf("Ciperlen = %d, Plainlen = %d\n", u16CiperLen, u16PlainLen);
    ZC_TraceData(g_struRetxBuffer.u8MsgBuffer, u16PlainLen);    
    ZC_Printf("+++++++++++++++++++RSA+++++++++++++++++++\n");
}



void IoT_exec_AT_cmd_TraceSwitch(u8 *pCmdBuf, u16 at_cmd_len)
{
    TestSec();
#if 0
	INT16 argc = 0;
	char *argv[MAX_OPTION_COUNT];
	char *opString = "s:?";
	char opt=0;
	char *endptr;
	
	UINT8 switch_on=0;
	UINT8 content=0;
	
	memset(argv,0,4*MAX_OPTION_COUNT);
	
	split_string_cmd(pCmdBuf, at_cmd_len, &argc, argv);
	
	opt = getopt(argc, argv, opString);
	
	while (opt != -1)
	{
		switch (opt)
		{
    	case 's':
    		g_TraceSwitch = simple_strtol(optarg,&endptr,0);
    		break;

		default:
			break;
		}
		opt = getopt(argc, argv, opString);
	}
#endif
}

