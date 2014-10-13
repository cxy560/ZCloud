/**
******************************************************************************
* @file     handshake.h
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief    HANDSHAKE
******************************************************************************
*/

#ifndef  __ZC_COMMON_H__ 
#define  __ZC_COMMON_H__

#include <string.h>

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
#else 
#define ZC_HTONS(n) (u16)((((u16) (n)) << 8) | (((u16) (n)) >> 8))
#endif 

#if ZC_DEBUG
#ifdef ZC_OFF_LINETEST
#define ZC_Printf(format, ...) printf("\1\2\3\4"format"File:%s, Line:%d, Function:%s\n", ##__VA_ARGS__, __FILE__, __LINE__ , __FUNCTION__)
#else
#define ZC_Printf(format, ...) Printf_High("\1\2\3\4"format"File:%s, Line:%d, Function:%s\n", ##__VA_ARGS__, __FILE__, __LINE__ , __FUNCTION__)
#endif 
#else
#ifdef ZC_OFF_LINETEST
#define ZC_Printf(format, ...) printf("\1\2\3\4"format"", ##__VA_ARGS__)
#else
#define ZC_Printf(format, ...) Printf_High("\1\2\3\4"format"", ##__VA_ARGS__)
#endif 
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

