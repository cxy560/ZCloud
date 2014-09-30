/**
******************************************************************************
* @file     zc_sec_engine.c
* @authors  cxy
* @version  V1.0.0
* @date     10-Sep-2014
* @brief    Security module
******************************************************************************
*/
#include <zc_sec_engine.h>
#include <zc_protocol_controller.h>

/*************************************************
* Function: SEC_EncryptTextByRsa
* Description:
* Author: cxy
* Returns:
* Parameter:
* History:
*************************************************/
u32 SEC_EncryptTextByRsa(u8* pu8CiperBuf, u8 *pu8Plainbuf, u16 u16Len)
{
    rsa_context rsa;
    s32 s32Ret;
    PTC_ProtocolCon *pstruCon;
    u8 *pu8PublicKey;
    pstruCon = &g_struProtocolController;

    pstruCon->pstruMoudleFun->pfunGetCloudKey(&pu8PublicKey);
    
    SEC_InitRsaContextWithPublicKey(&rsa, pu8PublicKey);
    s32Ret = rsa_pkcs1_encrypt(&rsa, RSA_PUBLIC, u16Len, pu8Plainbuf, pu8CiperBuf);
    rsa_free(&rsa);
    
    if (s32Ret)
    {
        return ZC_RET_ERROR;
    }
    else
    {
        return ZC_RET_OK;
    }
}

/*************************************************
* Function: SEC_DecryptTextByRsa
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 SEC_DecryptTextByRsa(u8* pu8CiperBuf, u8 *pu8Plainbuf, u16 u16Len)
{
    rsa_context struRsa;
    s32 s32len;
    s32 s32Ret;
    PTC_ProtocolCon *pstruCon;
    u8 *pu8PrivateKey;

    pstruCon = &g_struProtocolController;

    pstruCon->pstruMoudleFun->pfunGetCloudKey(&pu8PrivateKey);


    SEC_InitRsaContextWithPrivateKey(&struRsa, pu8PrivateKey);

    s32Ret = rsa_pkcs1_decrypt(&struRsa, RSA_PRIVATE, &s32len, pu8CiperBuf,
        pu8Plainbuf, u16Len);
    rsa_free(&struRsa);
    if (s32Ret)
    {
        return ZC_RET_ERROR;
    }
    else
    {
        return ZC_RET_OK;
    }
}


/*************************************************
* Function: SEC_InitRsaContextWithPublicKey
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void SEC_InitRsaContextWithPublicKey(rsa_context *pstruRsa, const u8 *pu8Pubkey)
{
    rsa_init(pstruRsa, RSA_PKCS_V15, RSA_RAW, NULL, NULL);

    pstruRsa->len = 128;
    mpi_read_binary(&pstruRsa->N, pu8Pubkey, 128);
    mpi_read_binary(&pstruRsa->E, pu8Pubkey+128, 3);
}

/*************************************************
* Function: SEC_InitRsaContextWithPrivateKey
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void SEC_InitRsaContextWithPrivateKey(rsa_context *pstrRsa, const u8 *pu8PrivateKey)
{
    u8 u8Index;
    u16 u16StartPos;
    u8 u8BufLen[6] = {128,64,64,64,64,64};
    mpi *pstruMpi[6];
    rsa_init(pstrRsa, RSA_PKCS_V15, RSA_RAW, NULL, NULL);

    pstrRsa->len = 128;
      
    pstruMpi[0] = &pstrRsa->N;
    pstruMpi[1] = &pstrRsa->P;
    pstruMpi[2] = &pstrRsa->Q;
    pstruMpi[3] = &pstrRsa->DP;
    pstruMpi[4] = &pstrRsa->DQ;    
    pstruMpi[5] = &pstrRsa->QP;  

    u16StartPos = 0;
    for (u8Index = 0; u8Index < 6; u8Index++)
    {
        mpi_read_binary(pstruMpi[u8Index], pu8PrivateKey + u16StartPos, u8BufLen[u8Index]);
        u16StartPos += (u16)u8BufLen[u8Index];
    }
}

/*************************************************
* Function: SEC_AesEncrypt
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 SEC_AesEncrypt(u8* pu8CiperBuf, u8 *pu8Plainbuf, u16 u16Len)
{
    u32 u32Len;
    PTC_ProtocolCon *pstruCon;
    u32Len = u16Len;

    pstruCon = &g_struProtocolController;
    
    if (PCT_KEY_RECVED != pstruCon->u8keyRecv)
    {
        return ZC_RET_ERROR;            
    }
    
    AES_CBC_Encrypt(pu8Plainbuf, u16Len, 
        pstruCon->u8SessionKey, ZC_HS_SESSION_KEY_LEN, 
        pstruCon->IvSend, 16, 
        pu8CiperBuf, &u32Len);
    memcpy(pstruCon->IvSend, pu8CiperBuf, 16);
    
    return ZC_RET_OK;
}
/*************************************************
* Function: SEC_AesDecrypt
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:PTC_ProtocolCon *pstruCon, u8 *pu8Key, u8 *pu8IvRecv,
*************************************************/
u32 SEC_AesDecrypt(u8* pu8CiperBuf, u8 *pu8Plainbuf, u16 u16Len)
{
    u8 *pu8Key;
    u8 *pu8IvRecv;
    u8 u8NextIv[16];
    u32 u32Len;
    PTC_ProtocolCon *pstruCon;
    u32Len = u16Len;
    pstruCon = &g_struProtocolController;
    
    if (PCT_KEY_RECVED != pstruCon->u8keyRecv)
    {
        return ZC_RET_ERROR;            
    }
    
    pu8Key = pstruCon->u8SessionKey;
    pu8IvRecv = pstruCon->IvRecv;
    
    if (u16Len > 16)
    {
        memcpy(u8NextIv, pu8CiperBuf, 16);
    }
    else
    {
        /*padding if small than 16*/
        memcpy(u8NextIv, pu8CiperBuf, u16Len);
        memset(u8NextIv + u16Len, 0, 16 - u16Len);
    }
    
    AES_CBC_Decrypt(pu8CiperBuf, u16Len, pu8Key, ZC_HS_SESSION_KEY_LEN, pu8IvRecv, 16, pu8Plainbuf, &u32Len);
    memcpy(pstruCon->IvRecv, u8NextIv, 16);
    
    return ZC_RET_OK;
}

/*************************************************
* Function: SEC_Encrypt
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 SEC_Encrypt(ZC_SecHead *pstruSecHead, u8 *pu8CiperBuf, u8 *pu8PlainBuf)
{
    u32 u32RetVal = ZC_RET_ERROR;
    switch (pstruSecHead->u8SecType)
    {
        case ZC_SEC_ALG_NONE:
            memcpy(pu8CiperBuf, pu8PlainBuf, ZC_HTONS(pstruSecHead->u16TotalMsg));
            u32RetVal = ZC_RET_OK;
            break;
        case ZC_SEC_ALG_RSA:
            u32RetVal = SEC_EncryptTextByRsa(pu8CiperBuf, pu8PlainBuf, ZC_HTONS(pstruSecHead->u16TotalMsg));
            break;
        case ZC_SEC_ALG_AES:
            u32RetVal = SEC_AesEncrypt(pu8CiperBuf, pu8PlainBuf, ZC_HTONS(pstruSecHead->u16TotalMsg));
            break;                        
    }
    return u32RetVal;
}
/*************************************************
* Function: SEC_Decrypt
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 SEC_Decrypt(ZC_SecHead *pstruSecHead, u8 *pu8CiperBuf, u8 *pu8PlainBuf)
{
    u32 u32RetVal = ZC_RET_ERROR;
    switch (pstruSecHead->u8SecType)
    {
        case ZC_SEC_ALG_NONE:
            memcpy(pu8PlainBuf, pu8CiperBuf, ZC_HTONS(pstruSecHead->u16TotalMsg));
            u32RetVal = ZC_RET_OK;
            break;
        case ZC_SEC_ALG_RSA:
            u32RetVal = SEC_DecryptTextByRsa(pu8CiperBuf, pu8PlainBuf, ZC_HTONS(pstruSecHead->u16TotalMsg));
            break;
        case ZC_SEC_ALG_AES:
            u32RetVal = SEC_AesDecrypt(pu8CiperBuf, pu8PlainBuf, ZC_HTONS(pstruSecHead->u16TotalMsg));
            break;                        
    }
    return u32RetVal;
}
/******************************* FILE END ***********************************/
