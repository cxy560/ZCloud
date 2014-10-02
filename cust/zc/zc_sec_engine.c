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
u32 SEC_EncryptTextByRsa(u8* pu8CiperBuf, u8 *pu8Plainbuf, u16 u16Len, u16 *pu16CiperLen)
{
    s32 s32Ret;
    u8 *pu8PublicKey;
    u32 u32OutLen;
    
    g_struProtocolController.pstruMoudleFun->pfunGetCloudKey(&pu8PublicKey);
    
    
    s32Ret = RSAPublicEncrypt(pu8CiperBuf, &u32OutLen, pu8Plainbuf, u16Len, (R_RSA_PUBLIC_KEY*)pu8PublicKey, 0);
    
    *pu16CiperLen = (u16)u32OutLen;
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
u32 SEC_DecryptTextByRsa(u8* pu8CiperBuf, u8 *pu8Plainbuf, u16 u16Len, u16 *pu16PlainLen)
{
    s32 s32Ret;
    u8 *pu8PrivateKey;
    u32 u32OutLen;

    g_struProtocolController.pstruMoudleFun->pfunGetPrivateKey(&pu8PrivateKey);


    s32Ret = RSAPrivateDecrypt(pu8Plainbuf, &u32OutLen, pu8CiperBuf, u16Len, (R_RSA_PRIVATE_KEY*)pu8PrivateKey);
    *pu16PlainLen = (u16)u32OutLen;

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
* Function: SEC_AesEncrypt
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 SEC_AesEncrypt(u8* pu8CiperBuf, u8 *pu8Plainbuf, u16 u16Len, u16 *pu16CiperLen)
{
    u16 u16OutLen;
    PTC_ProtocolCon *pstruCon;

    /*must assign the outlen*/
    u16OutLen = u16Len;

    pstruCon = &g_struProtocolController;
    
    if (PCT_KEY_RECVED != pstruCon->u8keyRecv)
    {
        return ZC_RET_ERROR;            
    }
    
    AES_CBC_Encrypt(pu8Plainbuf, u16Len, 
        pstruCon->u8SessionKey, ZC_HS_SESSION_KEY_LEN, 
        pstruCon->IvSend, 16, 
        pu8CiperBuf, &u16OutLen);
    *pu16CiperLen = u16OutLen;  
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
u32 SEC_AesDecrypt(u8* pu8CiperBuf, u8 *pu8Plainbuf, u16 u16Len, u16 *pu16PlainLen)
{
    u8 *pu8Key;
    u8 *pu8IvRecv;
    u8 u8NextIv[16];
    u16 u16OutLen;
    PTC_ProtocolCon *pstruCon;
    
    /*must assign the outlen*/
    u16OutLen = u16Len;
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
    
    AES_CBC_Decrypt(pu8CiperBuf, u16Len, pu8Key, ZC_HS_SESSION_KEY_LEN, pu8IvRecv, 16, pu8Plainbuf, &u16OutLen);
    *pu16PlainLen = u16OutLen;
    memcpy(pstruCon->IvRecv, u8NextIv, 16);
    
    return ZC_RET_OK;
}
/*************************************************
* Function: SEC_PaddingCheck
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
u32 SEC_PaddingCheck(u8 u8SecType, u16 u16PlainLen, u16 *u16PaddingLen)
{
    u16 LastBlockSize = 0;
    *u16PaddingLen = 0;
    
    switch(u8SecType)
    {
        case ZC_SEC_ALG_AES:
        {
            LastBlockSize = u16PlainLen % ZC_SEC_AES_BLOCK_SIZE;
            if(LastBlockSize > 0)
                *u16PaddingLen = ZC_SEC_AES_BLOCK_SIZE - LastBlockSize;
            else
                *u16PaddingLen = 0;
            break;
        }
        case ZC_SEC_ALG_RSA:
        {
            if (u16PlainLen + 11 > ZC_SEC_RSA_KEY_LEN)
            {
               return ZC_RET_ERROR; 
            }
            else
            {
                *u16PaddingLen = ZC_HS_SESSION_KEY_LEN - u16PlainLen; 
            } 
            
            break;
        }
    }

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
u32 SEC_Encrypt(ZC_SecHead *pstruSecHead, u8 *pu8CiperBuf, u8 *pu8PlainBuf, u16 *pu16CiperLen)
{
    u32 u32RetVal = ZC_RET_ERROR;
    switch (pstruSecHead->u8SecType)
    {
        case ZC_SEC_ALG_NONE:
            memcpy(pu8CiperBuf, pu8PlainBuf, ZC_HTONS(pstruSecHead->u16TotalMsg));
            u32RetVal = ZC_RET_OK;
            *pu16CiperLen = ZC_HTONS(pstruSecHead->u16TotalMsg);
            break;
        case ZC_SEC_ALG_RSA:
            u32RetVal = SEC_EncryptTextByRsa(pu8CiperBuf, pu8PlainBuf, ZC_HTONS(pstruSecHead->u16TotalMsg), pu16CiperLen);
            break;
        case ZC_SEC_ALG_AES:
            u32RetVal = SEC_AesEncrypt(pu8CiperBuf, pu8PlainBuf, ZC_HTONS(pstruSecHead->u16TotalMsg), pu16CiperLen);
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
u32 SEC_Decrypt(ZC_SecHead *pstruSecHead, u8 *pu8CiperBuf, u8 *pu8PlainBuf, u16 *pu16PlainLen)
{
    u32 u32RetVal = ZC_RET_ERROR;
    switch (pstruSecHead->u8SecType)
    {
        case ZC_SEC_ALG_NONE:
            memcpy(pu8PlainBuf, pu8CiperBuf, ZC_HTONS(pstruSecHead->u16TotalMsg));
            u32RetVal = ZC_RET_OK;
            *pu16PlainLen = ZC_HTONS(pstruSecHead->u16TotalMsg);
            break;
        case ZC_SEC_ALG_RSA:
            u32RetVal = SEC_DecryptTextByRsa(pu8CiperBuf, pu8PlainBuf, ZC_HTONS(pstruSecHead->u16TotalMsg), pu16PlainLen);
            break;
        case ZC_SEC_ALG_AES:
            u32RetVal = SEC_AesDecrypt(pu8CiperBuf, pu8PlainBuf, ZC_HTONS(pstruSecHead->u16TotalMsg), pu16PlainLen);
            break;                        
    }
    return u32RetVal;
}
/******************************* FILE END ***********************************/
