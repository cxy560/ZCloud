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

/*************************************************
* Function: SEC_EncryptTextByRsa
* Description:
* Author: cxy
* Returns:
* Parameter:
* History:
*************************************************/
s32 SEC_EncryptTextByRsa(const u8 *pu8PublicKey,
    u8 *pu8PlainText,
    u8 *pu8CipherText,
    u16 u16DataLen)
{
    //rsa_context rsa;
    //s32 s32Ret;

    //SEC_InitRsaContextWithPublicKey(&rsa, pu8PublicKey);
    //s32Ret = rsa_pkcs1_encrypt(&rsa, RSA_PUBLIC, 52, pu8PlainText, pu8CipherText);
    //rsa_free(&rsa);

    //if (s32Ret)
    //{
    //    return;
    //}
    memcpy(pu8CipherText, pu8PlainText, u16DataLen);
    return 0;
}

/*************************************************
* Function: SEC_DecipherTextByRsa
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
s32 SEC_DecipherTextByRsa(const u8 *pu8PrivateKey,
                          u8 *pu8CipherText,
                          u8 *pu8PlainText,
                          u16 u16DataLen)
{
    //rsa_context struRsa;
    //s32 s32len = 128;
    //s32 s32Ret;
    //SEC_InitRsaContextWithPrivateKey(&struRsa, pu8PrivateKey);

    //s32Ret = rsa_pkcs1_decrypt(&struRsa, RSA_PRIVATE, &s32len, pu8CipherText,
    //    pu8PlainText, 40);
    //rsa_free(&struRsa);
    //return s32Ret;
    memcpy(pu8PlainText, pu8CipherText, u16DataLen);
    return 0;
}


/*************************************************
* Function: SEC_InitRsaContextWithPublicKey
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void SEC_InitRsaContextWithPublicKey(rsa_context *pstruRsa,
                                     const u8 *pu8Pubkey)
{
    rsa_init(pstruRsa, RSA_PKCS_V15, RSA_RAW, NULL, NULL);

    pstruRsa->len = 256;
    mpi_read_binary(&pstruRsa->N, pu8Pubkey + 33, 256);
    mpi_read_string(&pstruRsa->E, 16, "10001");
}

/*************************************************
* Function: SEC_InitRsaContextWithPrivateKey
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void SEC_InitRsaContextWithPrivateKey(rsa_context *pstrRsa,
                                      const u8 *pu8PrivateKey)
{
    s32 s32Index = 9;
    rsa_init(pstrRsa, RSA_PKCS_V15, RSA_RAW, NULL, NULL);


    pstrRsa->len = 128;

    
    if (pu8PrivateKey[s32Index] & 1)
    {
        // key contains an extra zero byte
        ++s32Index;
    }
    ++s32Index;

    mpi_read_binary(&pstrRsa->N, pu8PrivateKey + s32Index, 128);
    mpi_read_string(&pstrRsa->E, 16, "10001");

    s32Index = s32Index + 135;
    if (pu8PrivateKey[s32Index] & 1)
    {
        // key contains an extra zero byte
        ++s32Index;
    }
    ++s32Index;

    mpi_read_binary(&pstrRsa->D, pu8PrivateKey + s32Index, 128);

    s32Index = s32Index + 129;
    if (pu8PrivateKey[s32Index] & 1)
    {
        // key contains an extra zero byte
        ++s32Index;
    }
    ++s32Index;

    mpi_read_binary(&pstrRsa->P, pu8PrivateKey + s32Index, 64);

    s32Index = s32Index + 65;
    if (pu8PrivateKey[s32Index] & 1)
    {
        // key contains an extra zero byte
        ++s32Index;
    }
    ++s32Index;

    mpi_read_binary(&pstrRsa->Q, pu8PrivateKey + s32Index, 64);

    s32Index = s32Index + 65;
    if (pu8PrivateKey[s32Index] & 1)
    {
        // key contains an extra zero byte
        ++s32Index;
    }
    ++s32Index;

    mpi_read_binary(&pstrRsa->DP, pu8PrivateKey + s32Index, 64);

    s32Index = s32Index + 65;
    if (pu8PrivateKey[s32Index] & 1)
    {
        // key contains an extra zero byte
        ++s32Index;
    }
    ++s32Index;

    mpi_read_binary(&pstrRsa->DQ, pu8PrivateKey + s32Index, 64);

    s32Index = s32Index + 65;
    if (pu8PrivateKey[s32Index] & 1)
    {
        // key contains an extra zero byte
        ++s32Index;
    }
    ++s32Index;

    mpi_read_binary(&pstrRsa->QP, pu8PrivateKey + s32Index, 64);
}

/*************************************************
* Function: SEC_Encrypt
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void SEC_Encrypt(PTC_ProtocolCon *pstruCon, u8 *pu8Key, u8 *pu8IvSend, u8 *pu8buf, int length)
{
    u32 u32Len;
    AES_CBC_Encrypt(pu8buf, length, pu8Key, 128, pu8IvSend, 16, pu8buf, &u32Len);
    memcpy(pstruCon->IvSend, pu8buf, 16);
}
/*************************************************
* Function: SEC_Decrypt
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void SEC_Decrypt(PTC_ProtocolCon *pstruCon, u8 *pu8Key, u8 *pu8IvRecv, u8 *pu8buf, int length)
{
    unsigned char next_iv[16];
    u32 u32Len;
    memcpy(next_iv, pu8buf, 16);

    AES_CBC_Decrypt(pu8buf, length, pu8Key, 128, pu8IvRecv, 16, pu8buf, &u32Len);
    memcpy(pstruCon->IvRecv, next_iv, 16);
}
/******************************* FILE END ***********************************/
