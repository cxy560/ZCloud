/**
******************************************************************************
* @file     zc_sec_engine.h
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief    Security module
******************************************************************************
*/

#ifndef  __ZC_SEC_ENGINE_H__ 
#define  __ZC_SEC_ENGINE_H__
#include <rsa.h>
#include <zc_common.h>
#include <zc_protocol_controller.h>

#ifdef __cplusplus
extern "C" {
#endif


s32 SEC_DecipherTextByRsa(const u8 *pu8PrivateKey,
    const u8 *pu8CipherText,
    u8 *pu8PlainText);
    
    
void SEC_InitRsaContextWithPublicKey(rsa_context *pstruRsa,
    const u8 *pu8Pubkey);
    
void SEC_InitRsaContextWithPrivateKey(rsa_context *pstrRsa,
    const u8 *pu8PrivateKey);
void SEC_Encrypt(PTC_ProtocolCon *pstruCon, u8 *pu8Key, u8 *pu8IvSend, u8 *pu8buf, int length);
void SEC_Decrypt(PTC_ProtocolCon *pstruCon, u8 *pu8Key, u8 *pu8IvRecv, u8 *pu8buf, int length);

#ifdef __cplusplus
}
#endif

#endif
/******************************* FILE END ***********************************/

