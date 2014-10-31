/**
******************************************************************************
* @file     zc_common.c
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief   
******************************************************************************
*/

#include <zc_common.h>
#include <zc_protocol_controller.h>
#include <zc_sec_engine.h>
extern 	char *optarg; 

u32 g_u32TraceSwitch = 0;
u32  g_u32LoopFlag = 0;
u32  g_u32SecSwitch = 1;

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
        ZC_Printf("0x%02x, 0x%02x, 0x%02x, 0x%02x,\n",
            pData[Index],
            pData[Index + 1],
            pData[Index + 2],
            pData[Index + 3]);
    }
    
    for (; Index < Len - 1; Index++)
    {
        ZC_Printf("0x%02x, ", pData[Index]);
    }
    ZC_Printf("0x%02x", pData[Index]);

    ZC_Printf("\n++++++++++++++++++++++++++++++++++++++++++++++++\n");
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
            break;

		default:
			break;
		}
		opt = getopt(argc, argv, opString);
	}
#endif
}

