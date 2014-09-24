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

/*PCT Main State Machine*/
#define    PCT_STATE_SLEEP                  (0)
#define    PCT_STATE_INIT                   (1)
#define    PCT_STATE_ACCESS_NET             (2)
#define    PCT_STATE_WAIT_ACCESS            (3)
#define    PCT_STATE_WAIT_ACCESSRSP         (4)
#define    PCT_STATE_CONNECT_CLOUD          (5)

#define    PCT_MAX_BUF_LEN               (1000)


typedef struct
{
    u32 u32Socket;
    
    u8  u8IpType;             /*IP Type,IPV6,IPV4*/
    u8  u8ConnectionType;
    u8  u8Pad[2];
    
    u32 u32Port;
    u8  u8IpAddress[ZC_IPADDR_MAX_LEN];
}PTC_Connection;

typedef u32 (*pFunSendDataToCloud)(PTC_Connection *pstruConnection, u8 *pu8Data, u32 u32DataLen);
typedef u32 (*pFunRecvDataFromCloud)(u32 u32Socket,u8 *pu8Data, u32 u32DataLen);
typedef u32 (*pFunFirmwareUpdate)(u8 *pu8NewVerFile, u32 u32DataLen);
typedef u32 (*pFunSendDataToMoudle)(u8 *pu8Data, u32 u32DataLen);
typedef u32 (*pFunRecvDataFromMoudle)(u8 **pu8Data, u32 *pu32DataLen);
typedef u32 (*pFunGetCloudKey)(u8 *pu8Key);
typedef u32 (*pFunGetPrivateKey)(u8 *pu8Key);
typedef u32 (*pFunGetVersion)(u8 *pu8Version);
typedef u32 (*pFunGetDeviceId)(u8 *pu8DeviceId);
typedef u32 (*pFunGetCloudIp)(u8 *pu8CloudIp);
typedef u32 (*pFunConnectToCloud)(PTC_Connection *pstruConnection);

typedef struct
{
    /*action function*/
    pFunConnectToCloud          pfunConnectToCloud;
    pFunSendDataToCloud         pfunSendToCloud;
    pFunRecvDataFromCloud       pfunRecvFormCloud;
    pFunFirmwareUpdate          pfunUpdate;
    pFunSendDataToMoudle        pfunSendToMoudle;
    pFunRecvDataFromMoudle      pfunRecvFormMoudle;
    
    /*config function*/
    pFunGetCloudKey             pfunGetCloudKey;
    pFunGetPrivateKey           pfunGetPrivateKey;
    pFunGetVersion              pfunGetVersion;
    pFunGetDeviceId             pfunGetDeviceId;
    pFunGetCloudIp              pfunGetCloudIP;
}PTC_ModuleAdapter;



typedef struct
{
    u8   u8MainState;                         /*State*/
    u8   u8Pad[3];
    
/*
    u32  u32Port;
    u8   u8CloudIp[ZC_IPADDR_MAX_LEN];*/
    PTC_Connection struCloudConnection;
    
    u8   u8DeviceId[ZC_DEVICE_ID_MAX_LEN];      
    u8   u8Version[ZC_FIRMWARE_VER_MAX_LEN];
    u8   u8Msg1Rand[ZC_FIRMWARE_VER_MAX_LEN];
    
    u8   u8CloudPublicKey[ZC_CLOUD_PUBLIC_KEY_LEN];
    u8   u8MoudlePrivateKey[ZC_MOUDLE_PRIVATE_KEY_LEN];
  
    PTC_ModuleAdapter *pstruMoudleFun;      /*Communication With Cloud*/
}PTC_ProtocolCon;

#ifdef __cplusplus
extern "C" {
#endif



#ifdef __cplusplus
}
#endif
#endif
/******************************* FILE END ***********************************/

