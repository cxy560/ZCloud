/**
******************************************************************************
* @file     zc_module_interface.h
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief    Security module
******************************************************************************
*/

#ifndef  __ZC_MODULE_INTERFACE_H__ 
#define  __ZC_MODULE_INTERFACE_H__
#include <zc_common.h>

typedef struct tag_RCTRL_STRU_MSGHEAD
{
    u32 MsgFlag;        /*byte0:0x76, byte1:0x81, byte2:0x43, byte3:0x50*/
    u8  MsgType;
    u8  MsgId;
    u16 MsgBodyLen;
}RCTRL_STRU_MSGHEAD;
#ifdef __cplusplus
}
#endif

#endif
/******************************* FILE END ***********************************/

