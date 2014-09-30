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

u32 SEC_EncryptTextByRsa(u8* pu8CiperBuf, u8 *pu8Plainbuf, u16 u16Len);
u32 SEC_DecryptTextByRsa(u8* pu8CiperBuf, u8 *pu8Plainbuf, u16 u16Len);
u32 SEC_AesEncrypt(u8* pu8CiperBuf, u8 *pu8Plainbuf, u16 u16Len);
u32 SEC_AesDecrypt(u8* pu8CiperBuf, u8 *pu8Plainbuf, u16 u16Len);
u32 SEC_Encrypt(ZC_SecHead *pstruSecHead, u8 *pu8CiperBuf, u8 *pu8PlainBuf);
u32 SEC_Decrypt(ZC_SecHead *pstruSecHead, u8 *pu8CiperBuf, u8 *pu8PlainBuf);
void SEC_InitRsaContextWithPublicKey(rsa_context *pstruRsa, const u8 *pu8Pubkey);
void SEC_InitRsaContextWithPrivateKey(rsa_context *pstrRsa, const u8 *pu8PrivateKey);

#ifdef __cplusplus
}
#endif

#endif
/******************************* FILE END ***********************************/

