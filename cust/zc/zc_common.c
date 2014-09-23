#include <zc_common.h>

extern 	char *optarg; 

u32 g_TraceSwitch = 0;


void OutputData(u8* pData, u32 Len)
{
#if 0
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
#endif
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

