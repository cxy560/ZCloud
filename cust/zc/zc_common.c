#include <stdio.h>
#include "types.h"
#include "iot_api.h"

extern 	char *optarg; 

ULONG g_TraceSwitch = 0;
UCHAR g_TestSrcMac[] = {0xF8,0x01,0x13,0x48,0x32,0xF8};

void OutputData(UCHAR* pData, UINT Len)
{
    UINT Index;
    for (Index = 0; Index + 4 < Len; Index = Index + 4)
    {
        Printf_High("%02x %02x %02x %02x\n",
            pData[Index],
            pData[Index + 1],
            pData[Index + 2],
            pData[Index + 3]);
    }
    
    for (; Index < Len; Index++)
    {
        Printf_High("%02x ", pData[Index]);
    }
    
    Printf_High("\n");
}
void TraceData(RX_BLK* pRxBlk)
{

    if ((SUBTYPE_BEACON== pRxBlk->pHeader->FC.SubType)
    && (BTYPE_MGMT == pRxBlk->pHeader->FC.Type))
    {
        return;
    }
    
    Printf_High("Data Type = %d, Subtype = %d, DataSize = %d\n", 
        pRxBlk->pHeader->FC.Type, 
        pRxBlk->pHeader->FC.SubType,
        pRxBlk->DataSize);

    if (0 == g_TraceSwitch)
    {
        return;
    }

    if (BTYPE_MGMT == pRxBlk->pHeader->FC.Type)
    {
        //OutputData(pRxBlk->pData, pRxBlk->DataSize);
    }
    else if (BTYPE_DATA== pRxBlk->pHeader->FC.Type)
    {
        if ((!memcmp(pRxBlk->pHeader->Addr3, g_TestSrcMac,6))
        || (!memcmp(pRxBlk->pHeader->Addr1, g_TestSrcMac,6))
        || (!memcmp(pRxBlk->pHeader->Addr2, g_TestSrcMac,6)))
        {
            OutputData(pRxBlk->pData, 100);            
        }
        /*if (97 == pRxBlk->DataSize)
        {
            OutputData(pRxBlk->pData, 100);            
        }*/

    }
    else
    {
    }
    
    return;
        
}

VOID IoT_exec_AT_cmd_TraceSwitch(PCHAR pCmdBuf, INT16 at_cmd_len)
{
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
}

