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
#define ZC_MODULE_KEY_LEN                   (112)


#define ZC_SEC_ALG_NONE                     (0) 
#define ZC_SEC_ALG_RSA                      (1) 
#define ZC_SEC_ALG_AES                      (2) 


#define ZC_OTA_MAX_CHUNK_LEN                (300)




typedef struct
{
    u16 u16TotalMsg;
    u8  u8SecType;
    u8  u8Resver;
}ZC_SecHead;

/*ZCloud Message*/
typedef struct
{
    u8  Version;
    u8  MsgId;
    u8  MsgCode;		
    u8  Resver;
    
    u16 Payloadlen; 
    u8  TotalMsgCrc[2];
    
}ZC_MessageHead;

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
    ZC_CODE_OTA_END,
    
    /*OTA Wifi Moudle Code*/
    ZC_CODE_ZOTA_BEGIN,
    ZC_CODE_ZOTA_FILE_BEGIN,      /*file name, len, version*/
    ZC_CODE_ZOTA_FILE_CHUNK,
    ZC_CODE_ZOTA_FILE_END,
    ZC_CODE_ZOTA_END,

    ZC_CODE_BC_INFO
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

/*msg code: ZC_CODE_DESCRIBE*/
typedef struct 
{
	u8	u8WifiSwVersion;            
	u8	u8HwVersion;              
	u8	u8ArmSwVersion;          
	u8	u8ZigbeeSwVersion;       
    u8  u8ModuleKey[ZC_MODULE_KEY_LEN];
    u8  u8DeviceId[ZC_HS_DEVICE_ID_LEN];
    
}ZC_RegisterReq;

/*msg code: ZC_CODE_OTA_BEGIN*/
typedef struct
{
    u8 u8FileNum;
    u8 u8Pad[3];
    u8 u8FileType[0];
}ZC_OtaBeginReq;

/*msg code: ZC_CODE_OTA_FILE_BEGIN*/
typedef struct
{
    u8 u8FileType;
    u8 u8FileVersion;
    u8  u8TotalFileCrc[2];
    
    u32 u32FileTotalLen;
}ZC_OtaFileBeginReq;

/*msg code: ZC_CODE_OTA_FILE_CHUNK, max data len is ZC_OTA_MAX_CHUNK_LEN*/
typedef struct
{
    u32 u32Offset;
}ZC_OtaFileChunkReq;

/*BC info�� send after connect with cloud�� in PCT_SEND_BC_MAX_NUM*/
typedef struct
{
    u8 RandMsg[ZC_HS_MSG_LEN];
    u8 DeviceId[ZC_HS_DEVICE_ID_LEN];
}ZC_BroadCastInfo;
#endif
/******************************* FILE END ***********************************/

