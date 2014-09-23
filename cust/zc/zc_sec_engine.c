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
* Function: SEC_CipherTextByRsa
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
s32 SEC_CipherTextByRsa(const u8 *pu8Plaintext, 
                        const u32 u32Len,
                        const u8 *pu8PublicKey,
                        u8 *pu8CipherText)
{
    rsa_context struRsa;
    s32 s32Ret; 

    SEC_InitRsaContextWithPublicKey(&struRsa, pu8PublicKey);

    s32Ret = rsa_pkcs1_encrypt(&struRsa, RSA_PUBLIC, u32Len, pu8Plaintext, pu8CipherText);
    rsa_free(&struRsa);
    return s32Ret;
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
                          const u8 *pu8CipherText,
                          u8 *pu8PlainText,
                          u32 u32PlainTextLen)
{
    rsa_context struRsa;
    s32 s32len = 128;
    s32 s32Ret;
    SEC_InitRsaContextWithPrivateKey(&struRsa, pu8PrivateKey);

    s32Ret = rsa_pkcs1_decrypt(&struRsa, RSA_PRIVATE, &s32len, pu8CipherText,
        pu8PlainText, u32PlainTextLen);
    rsa_free(&struRsa);
    return s32Ret;
}

/*************************************************
* Function: SEC_CalculateHmac
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
void SEC_CalculateHmac(const u8 *pu8Text,
                       const u8 *pu8Key,
                       u8 *pu8hmac)
{
    sha1_hmac(pu8Key, 40, pu8Text, 128, pu8hmac);
}

/*************************************************
* Function: SEC_VerifySignature
* Description: 
* Author: cxy 
* Returns: 
* Parameter: 
* History:
*************************************************/
s32 SEC_VerifySignature(const u8 *pu8Signature,
                     const u8 *pu8PubKey,
                     const u8 *pu8ExpectedHmac)
{
    rsa_context struRsa;
    s32 s32Ret;
    SEC_InitRsaContextWithPublicKey(&struRsa, pu8PubKey);

    s32Ret = rsa_pkcs1_verify(&struRsa, RSA_PUBLIC, RSA_RAW, 20,
        pu8ExpectedHmac, pu8Signature);
    rsa_free(&struRsa);
    return s32Ret;
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
/******************************* FILE END ***********************************/