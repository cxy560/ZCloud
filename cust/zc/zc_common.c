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
    
    ZC_Printf("\n");
}
void IoT_exec_AT_cmd_TraceSwitch(u8 *pCmdBuf, u16 at_cmd_len)
{
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

