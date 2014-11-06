/**
******************************************************************************
* @file     zc_common.h
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief    
******************************************************************************
*/

#ifndef  __ZC_COMMON_H__ 
#define  __ZC_COMMON_H__

#include <string.h>
#include <zc_module_config.h>

typedef signed   char                       s8;                                 
typedef signed   short                      s16;                                
typedef signed   int                        s32;                                
typedef unsigned char                       u8;                                 
typedef unsigned short                      u16;                                
typedef unsigned int                        u32;                                
typedef volatile signed char                vs8;                                
typedef volatile signed short               vs16;                               
typedef volatile signed int                 vs32;                               
typedef volatile unsigned char              vu8;                                
typedef volatile unsigned short             vu16;                               
typedef volatile unsigned int               vu32;      

#define     ZC_CLOUD_PORT                                    (8384)
#define     ZC_MOUDLE_PORT                                   (7689)
#define     ZC_MOUDLE_BROADCAST_PORT                         (8689)


#define  ZC_IPADDR_MAX_LEN                  (16)

#define  ZC_RET_OK                          (0)
#define  ZC_RET_ERROR                       (1)

#define  ZC_IPTYPE_IPV4                     (0)                
#define  ZC_IPTYPE_IPV6                     (1)

#define  ZC_CONNECT_TYPE_UDP                (0)                
#define  ZC_CONNECT_TYPE_TCP                (1)

#ifndef NULL
#define NULL                ((void *)0)
#endif

#ifdef ZC_BIG_ENDIAN
#define ZC_HTONS(n) (n)
#define ZC_HTONL(n) (n)
#else 
#define ZC_HTONS(n) (u16)((((u16) (n)) << 8) | (((u16) (n)) >> 8))
#define ZC_HTONL(n)  ((((u32)(n) & 0xff000000) >> 24) | \
    (((u32)(n)& 0x00ff0000) >> 8) | \
    (((u32)(n)& 0x0000ff00) << 8) | \
    (((u32)(n)& 0x000000ff) << 24))

#endif 





#ifdef __cplusplus
extern "C" {
#endif

void ZC_TraceData(u8* pData, u32 Len);
#ifdef __cplusplus
}
#endif
#endif
/******************************* FILE END ***********************************/

