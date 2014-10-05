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

u32 g_u32TraceSwitch = 0;
u32  g_u32LoopFlag = 0;
u32  g_u32SecSwitch = 0;

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
    if (0 == g_u32TraceSwitch)
    {
        return;
    }
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

void IoT_exec_AT_cmd_TestSwitch(u8 *pCmdBuf, u16 at_cmd_len)
{
    
#ifndef ZC_OFF_LINETEST
	u16 argc = 0;
	char *argv[15];
	char *opString = "t:s:l:r:?";
	char opt=0;
	char *endptr;
	
	
	memset(argv,0,4*15);
	
	split_string_cmd(pCmdBuf, at_cmd_len, &argc, argv);
	
	opt = getopt(argc, argv, opString);
	
	while (opt != -1)
	{
		switch (opt)
		{
    	case 't':
    		g_u32TraceSwitch = simple_strtol(optarg,&endptr,0);
    	    ZC_Printf("set Trace Switch to %d\n",g_u32TraceSwitch);    		
    		break;
        case 's':
            g_u32SecSwitch = simple_strtol(optarg,&endptr,0);
    	    ZC_Printf("set Security Switch to %d\n",g_u32SecSwitch);    		
            break;
        case 'l':
            g_u32LoopFlag = simple_strtol(optarg,&endptr,0);
    	    ZC_Printf("set Loop Switch to %d\n",g_u32LoopFlag);    		
            break;
        case 'r':
            TestSec();
            break;

		default:
			break;
		}
		opt = getopt(argc, argv, opString);
	}
#endif
}

