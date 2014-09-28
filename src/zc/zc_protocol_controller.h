/**
******************************************************************************
* @file     zc_protocol_controller.h
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief    protocol controller
******************************************************************************
*/
#ifndef  __ZC_PROTOCOL_CONTROLLER_H__ 
#define  __ZC_PROTOCOL_CONTROLLER_H__

#include <zc_common.h>
#include <zc_module_config.h>
#include <zc_protocol_interface.h>
/*PCT Main State Machine*/
#define    PCT_STATE_SLEEP                  (0)
#define    PCT_STATE_INIT                   (1)
#define    PCT_STATE_ACCESS_NET             (2)
#define    PCT_STATE_WAIT_ACCESS            (3)
#define    PCT_STATE_WAIT_ACCESSRSP         (4)
#define    PCT_STATE_WAIT_MSG4              (5)
#define    PCT_STATE_CONNECT_CLOUD          (6)
#define    PCT_STATE_DISCONNECT_CLOUD       (7)

#define    PCT_INVAILD_SOCKET               (0xFFFFFFFF)

#define    PCT_TIMER_RECONNECT              (0)
#define    PCT_TIMER_REACCESS               (1)
#define    PCT_TIMER_SENDMOUDLE             (2)

#define    PCT_TIMER_INTERVAL_RECONNECT     (1000)
#define    PCT_TIMER_INTERVAL_SENDMOUDLE    (1000)

#define    PCT_KEY_UNRECVED     (0)
#define    PCT_KEY_RECVED       (1)

#define    PCT_TIMER_INVAILD        (0xFF)
#define    PCT_SENDMOUDLE_NUM       (4)
typedef struct
{
    u32 u32Socket;
    
    u8  u8IpType;             /*IP Type,IPV6,IPV4*/
    u8  u8ConnectionType;
    u8  u8Pad[2];
    
    u32 u32Port;
    u8  u8IpAddress[ZC_IPADDR_MAX_LEN];
}PTC_Connection;

typedef void (*pFunSendDataToCloud)(PTC_Connection *pstruConnection);
typedef u32 (*pFunFirmwareUpdate)(u8 *pu8NewVerFile, u16 u16DataLen);
typedef u32 (*pFunSendDataToMoudle)(u8 *pu8Data, u16 u16DataLen);
typedef u32 (*pFunRecvDataFromMoudle)(u8 *pu8Data, u16 u16DataLen);
typedef u32 (*pFunGetCloudKey)(u8 *pu8Key);
typedef u32 (*pFunGetPrivateKey)(u8 *pu8Key);
typedef u32 (*pFunGetVersion)(u8 *pu8Version);
typedef u32 (*pFunGetDeviceId)(u8 *pu8DeviceId);
typedef u32 (*pFunGetCloudIp)(u8 *pu8CloudIp);
typedef u32 (*pFunConnectToCloud)(PTC_Connection *pstruConnection);
typedef u32 (*pFunSetTimer)(u8 u8Type, u32 Interval, u8 *pu8Index);


typedef struct
{
    /*action function*/
    pFunConnectToCloud          pfunConnectToCloud;
    pFunSendDataToCloud         pfunSendToCloud;
    pFunFirmwareUpdate          pfunUpdate;
    pFunSendDataToMoudle        pfunSendToMoudle;
    pFunRecvDataFromMoudle      pfunRecvFormMoudle;
    
    /*config function*/
    pFunGetCloudKey             pfunGetCloudKey;
    pFunGetPrivateKey           pfunGetPrivateKey;
    pFunGetVersion              pfunGetVersion;
    pFunGetDeviceId             pfunGetDeviceId;
    pFunGetCloudIp              pfunGetCloudIP;
    pFunSetTimer                pfunSetTimer;
}PTC_ModuleAdapter;



typedef struct
{
    u8   u8MainState;                         /*State*/
    u8   u8keyRecv;
    u8   u8ReconnectTimer;
    u8   u8AccessTimer;
    
    u8   u8SendMoudleTimer;
    u8   u8ReSendMoudleNum;
    u8   u8Pad[2];
    
    u8   *pu8SendMoudleBuffer;
    
    PTC_Connection struCloudConnection;
    
    u8   u8DeviceId[ZC_HS_DEVICE_ID_LEN];      
    u8   u8Version[ZC_FIRMWARE_VER_MAX_LEN];
    
    u8   u8CloudPublicKey[ZC_CLOUD_PUBLIC_KEY_LEN];
    u8   u8MoudlePrivateKey[ZC_MOUDLE_PRIVATE_KEY_LEN];
    u8   u8SessionKey[ZC_HS_SESSION_KEY_LEN];
    u8   IvSend[16];
    u8   IvRecv[16];
    u8   RandMsg[ZC_HS_MSG_LEN];

    PTC_ModuleAdapter *pstruMoudleFun;      /*Communication With Cloud*/
}PTC_ProtocolCon;

extern PTC_ProtocolCon  g_struProtocolController;

#ifdef __cplusplus
extern "C" {
#endif
void PCT_Init(PTC_ModuleAdapter *pstruAdapter);
void PCT_SendEmptyMsg();
void PCT_SendCloudAccessMsg1(PTC_ProtocolCon *pstruContoller);
void  PCT_SendCloudAccessMsg3(PTC_ProtocolCon *pstruContoller);
void PCT_DisConnectCloud(PTC_ProtocolCon *pstruContoller);
void PCT_ConnectCloud(PTC_ProtocolCon *pstruContoller);
void PCT_HandleMoudleEvent(u8 *pu8Msg, u16 u16DataLen);
void PCT_RecvAccessMsg2(PTC_ProtocolCon *pstruContoller);
void PCT_RecvAccessMsg4(PTC_ProtocolCon *pstruContoller);
void PCT_HandleEvent(PTC_ProtocolCon *pstruContoller);
void PCT_Run();
void PCT_WakeUp();
void PCT_Sleep();
void PCT_SendMsgToCloud(u8 *pu8Msg, u16 u16Len);
void PCT_SendErrorMsg(u8 u8MsgId, u8 *pu8Error, u16 u16ErrorLen);
void PCT_ReconnectCloud(PTC_ProtocolCon *pstruContoller);
void PCT_SendMoudleTimeout(PTC_ProtocolCon *pstruProtocolController);


#ifdef __cplusplus
}
#endif
#endif
/******************************* FILE END ***********************************/

