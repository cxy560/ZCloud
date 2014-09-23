/**
******************************************************************************
* @file     zc_cloud_event.h
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief    dealevent
******************************************************************************
*/
#ifndef  __ZC_CLOUD_EVENT_H__ 
#define  __ZC_CLOUD_EVENT_H__

#include <zc_common.h>
#include <zc_protocol_controller.h>

#define      ZC_HELLO_MSG_LEN       (20)

#ifdef __cplusplus
extern "C" {
#endif


u32     EVENT_BuildHelloMsg(PTC_ProtocolCon *pstruCon, u8 *pu8Msg, u32 *pu32Len);



#ifdef __cplusplus
}
#endif

#endif
/******************************* FILE END ***********************************/

