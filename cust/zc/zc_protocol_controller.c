/**
******************************************************************************
* @file     zc_protocol_controller.c
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief    protocol controller
******************************************************************************
*/
#include <zc_protocol_controller.h>
#include <zc_sec_engine.h>
#include <zc_module_config.h>
#include <zc_cloud_event.h>


PTC_ProtocolCon  g_struProtocolController;
u8 g_u8Message[PCT_MAX_BUF_LEN];
/*************************************************
* Function: PCT_Init
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_Init(PTC_ModuleAdapter *pstruAdapter)
{
    u32 u32Ret = 0;
    g_struProtocolController.pstruMoudleFun = pstruAdapter;
    
    /*initialize parameters*/
    u32Ret = g_struProtocolController.pstruMoudleFun->pfunGetCloudKey(g_struProtocolController.u8CloudPublicKey);
    u32Ret += g_struProtocolController.pstruMoudleFun->pfunGetDeviceId(g_struProtocolController.u8DeviceId);
    u32Ret += g_struProtocolController.pstruMoudleFun->pfunGetVersion(g_struProtocolController.u8Version);
    u32Ret += g_struProtocolController.pstruMoudleFun->pfunGetPrivateKey(g_struProtocolController.u8MoudlePrivateKey);
    u32Ret += g_struProtocolController.pstruMoudleFun->pfunGetCloudIP(g_struProtocolController.struCloudConnection.u8IpAddress);
    
    /*config connection type*/
    g_struProtocolController.struCloudConnection.u32Port = ZC_CLOUD_PORT;
    g_struProtocolController.struCloudConnection.u8IpType = ZC_IPTYPE_IPV4;
    g_struProtocolController.struCloudConnection.u8ConnectionType = ZC_CONNECT_TYPE_TCP;
    
    /*init ok if all result is ok*/
    g_struProtocolController.u8MainState = (0 == u32Ret) ? (PCT_STATE_INIT) : (PCT_STATE_SLEEP);
}

/*************************************************
* Function: PCT_SendCloudAccessMsg
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_SendCloudAccessMsg2(PTC_ProtocolCon *pstruContoller)
{
    u32 u32Ret = ZC_RET_OK;
    u32 u32Len = 0;
    
    /*Connect*/
    EVENT_BuildAccessMsg2(pstruContoller, g_u8Message, &u32Len);
    
    pstruContoller->pstruMoudleFun->pfunSendToCloud(&pstruContoller->struCloudConnection, g_u8Message, u32Len);
    
}
/*************************************************
* Function: PCT_DisConnectCloud
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_DisConnectCloud(PTC_ProtocolCon *pstruContoller)
{
    pstruContoller->u8MainState = PCT_STATE_ACCESS_NET;
}

/*************************************************
* Function: PCT_ConnectCloud
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_ConnectCloud(PTC_ProtocolCon *pstruContoller)
{
    u32 u32Ret = ZC_RET_OK;
    
    /*Connect*/
    u32Ret = pstruContoller->pstruMoudleFun->pfunConnectToCloud(&pstruContoller->struCloudConnection);
    if (ZC_RET_OK != u32Ret)
    {
        return;
    }
    /*change state to wait access*/
    pstruContoller->u8MainState = PCT_STATE_WAIT_ACCESS;
}

/*************************************************
* Function: PCT_HandleCloudEvent
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_HandleCloudEvent(u8 *pu8Msg, u32 u32DataLen)
{
#if 0
    u32 u32Index;
    Printf_High("recv msg len = %d\n", u32DataLen);
    for (u32Index = 0; u32Index < u32DataLen; u32Index++)
    {
        Printf_High("%02x ", pu8Msg[u32Index]);
    }
    Printf_High("\n");
    return;
#endif
}
/*************************************************
* Function: PCT_HandleMoudleEvent
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_HandleMoudleEvent(u8 *pu8Msg, u32 u32DataLen)
{
    return;
}
/*************************************************
* Function: PCT_RecvAccessRsp
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_RecvAccessRsp(PTC_ProtocolCon *pstruContoller)
{
    u32 u32Ret = ZC_RET_OK;
    u8  *pu8Msg = NULL;
    u32 u32DataLen = 0;

    /*recv Access rsp*/
    u32Ret = pstruContoller->pstruMoudleFun->pfunRecvFormCloud(&pstruContoller->struCloudConnection,
        &pu8Msg, &u32DataLen);
    if (ZC_RET_OK != u32Ret)
    {
        return;
    }

    /*parser header get secure method and key*/
    PCT_HandleCloudEvent(pu8Msg, u32DataLen);
    
    /*change main state to PCT_STATE_CONNECT_CLOUD*/
    pstruContoller->u8MainState = PCT_STATE_CONNECT_CLOUD;
}

/*************************************************
* Function: PCT_HandleEvent
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_HandleEvent(PTC_ProtocolCon *pstruContoller)
{
    u32 u32Ret = ZC_RET_OK;
    u8  *pu8Msg = NULL;
    u32 u32DataLen = 0;

    /*recv msg*/
    u32Ret = pstruContoller->pstruMoudleFun->pfunRecvFormCloud(&pstruContoller->struCloudConnection,
        &pu8Msg, &u32DataLen);

    PCT_HandleCloudEvent(pu8Msg, u32DataLen);
    
    u32Ret = pstruContoller->pstruMoudleFun->pfunRecvFormMoudle(&pu8Msg, &u32DataLen);

    PCT_HandleMoudleEvent(pu8Msg, u32DataLen);
} 

/*************************************************
* Function: PCT_Run
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_Run()
{
    PTC_ProtocolCon *pstruContoller = &g_struProtocolController;
    switch(pstruContoller->u8MainState)
    {
        case PCT_STATE_SLEEP:
            break;
        case PCT_STATE_INIT:
            /*smart connection*/
            break;
        case PCT_STATE_ACCESS_NET:
            PCT_ConnectCloud(pstruContoller);
            break;
        case PCT_STATE_WAIT_ACCESS:
            //PCT_RecvAccessRsp(pstruContoller);
            break;
        case PCT_STATE_CONNECT_CLOUD:
            //PCT_HandleEvent(pstruContoller);
            break;                       
    }
    
}
/*************************************************
* Function: PCT_WakeUp
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_WakeUp()
{
    if (PCT_STATE_INIT == g_struProtocolController.u8MainState)
    {
        g_struProtocolController.u8MainState = PCT_STATE_ACCESS_NET;
    }
    
}

/*************************************************
* Function: PCT_Sleep
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void PCT_Sleep()
{
    g_struProtocolController.u8MainState = PCT_STATE_INIT;
}

/******************************* FILE END ***********************************/

