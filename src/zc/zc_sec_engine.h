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
#include <sha1.h>
#include <zc_common.h>

#ifdef __cplusplus
extern "C" {
#endif

s32 SEC_CipherTextByRsa(const u8 *pu8Plaintext, 
    const u32 u32Len,
    const u8 *pu8PublicKey,
    u8 *pu8CipherText);
    
s32 SEC_DecipherTextByRsa(const u8 *pu8PrivateKey,
    const u8 *pu8CipherText,
    u8 *pu8PlainText,
    u32 u32PlainTextLen);
    
void SEC_CalculateHmac(const u8 *pu8Text,
    const u8 *pu8Key,
    u8 *pu8hmac);
    
s32 SEC_VerifySignature(const u8 *pu8Signature,
    const u8 *pu8PubKey,
    const u8 *pu8ExpectedHmac);
    
void SEC_InitRsaContextWithPublicKey(rsa_context *pstruRsa,
    const u8 *pu8Pubkey);
    
void SEC_InitRsaContextWithPrivateKey(rsa_context *pstrRsa,
    const u8 *pu8PrivateKey);
#ifdef __cplusplus
}
#endif

#endif
/******************************* FILE END ***********************************/

