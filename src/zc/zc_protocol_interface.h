/**
******************************************************************************
* @file     zc_protocol_interface.h
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief    Security module
******************************************************************************
*/
#ifndef  __ZC_PROTOCOL_INTERFACE_H__ 
#define  __ZC_PROTOCOL_INTERFACE_H__
#include <zc_common.h>

#define ZC_HS_MSG_LEN                       (40)
#define ZC_HS_DEVICE_ID_LEN                 (12)
#define ZC_HS_SESSION_KEY_LEN               (16)
#define ZC_HS_SESSION_IV_LEN                (16)

typedef struct
{
    u16 u16TotalMsg;
    u8  u8SecType;
    u8  u8AlgType;
}ZC_SecHeader;

/*ZCloud Message*/
typedef struct
{
    u8  Version;
    u8  MsgId;
    u8  MsgCode;		
    u8  Resver;
    
    u16 Payloadlen; 
    u8  TotalMsgCrc[2];
    
    u8  payload[0];
}ZC_Message;

/*ZCloud Message code*/
typedef enum 
{
    ZC_CODE_SMARTCONFIG_BEGIN = 0,
    ZC_CODE_SMARTCONFIG_DONE,
    ZC_CODE_WIFI_CONNECT,
    ZC_CODE_WIFI_DISCONNECT,
    ZC_CODE_CLOUD_CONNECT,
    ZC_CODE_CLOUD_DISCONNECT,
    ZC_CODE_LOCAL_HANDSHAKE,
    ZC_CODE_DESCRIBE,                 
    ZC_CODE_ZDESCRIBE,                

    /*HandShake Code*/
    ZC_CODE_HANDSHAKE_1,
    ZC_CODE_HANDSHAKE_2,
    ZC_CODE_HANDSHAKE_3,
    ZC_CODE_HANDSHAKE_4,

    /*Respone Code*/
    ZC_CODE_HEARTBEAT,   /*no payload*/
    ZC_CODE_EMPTY,		 /*no payload, send by moudle when can recv another msg*/
    ZC_CODE_ACK,         /*user define payload*/
    ZC_CODE_ERR,         /*use ZC_ErrorMsg*/

    /*OTA Code*/
    ZC_CODE_OTA_BEGIN,
    ZC_CODE_OTA_FILE_BEGIN,      /*file name, len, version*/
    ZC_CODE_OTA_FILE_CHUNK,
    ZC_CODE_OTA_FILE_END,
    ZC_CODE_OTA_END
}ZC_MsgCode;

/*Error Msg*/
typedef struct{
    u8 ErrorCode;
    u8 ErrorMsg[0];
}ZC_ErrorMsg;


/*first handshake msg, send by moudle to cloud
encrypt by cloud public key*/
typedef struct
{
    u8 RandMsg[ZC_HS_MSG_LEN];
    u8 DeviceId[ZC_HS_DEVICE_ID_LEN];
}ZC_HandShakeMsg1;

/*Second handshake msg, send by cloud to moudle, 
encrypt by moudle public key*/
typedef struct
{
    u8 RandMsg[ZC_HS_MSG_LEN];
    u8 SessionKey[ZC_HS_SESSION_KEY_LEN];
}ZC_HandShakeMsg2;

/*3rd handshake msg, send by moudle to cloud,
encrypt by session key*/
typedef struct
{
    u8 RandMsg[ZC_HS_MSG_LEN];
}ZC_HandShakeMsg3;


/*4th handshake msg, send by cloud to moudle,
encrypt by session key*/
typedef struct
{
    u8 RandMsg[ZC_HS_MSG_LEN];
}ZC_HandShakeMsg4;
#endif
/******************************* FILE END ***********************************/

