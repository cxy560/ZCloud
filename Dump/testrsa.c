#include <zc_common.h>
#include <uip.h>
#include <uiplib.h>
#include <zc_message_queue.h>
#include <iot_tcpip_interface.h>
#include <zc_protocol_interface.h>
#include <zc_protocol_controller.h>
#include <windows.h>
#include <zc_sec_engine.h>
#include "rsa_genkey.h"

u8 u8publicBuf[1024];

u8 u8TestBuf[1024];
u8 u8PlainBuf[1024];
u8 u8DeplainBuf[1024];

extern u8 g_u8DumpCloudMsg[10240];

void newrsa()
{
    int ret;
    rsa_context rsa;
    rsa_context newRsa;
    havege_state hs;
    u32 u32Index;
    s32 s32len;
    s32 sum = 0;
    s32 testlen;


    havege_init( &hs );

    printf( " ok\n  . Generating the RSA key [ %d-bit ]...\n", KEY_SIZE );

    rsa_init( &rsa, RSA_PKCS_V15, 0, havege_rand, &hs );
    if( ( ret = rsa_gen_key( &rsa, KEY_SIZE, EXPONENT ) ) != 0 )
    {
        printf( " failed\n  ! rsa_gen_key returned %d\n\n", ret );
    }
    
    s32len = mpi_size(&rsa.N);
    mpi_write_binary(&rsa.N, u8publicBuf+sum, s32len);
    sum +=s32len;

    s32len = mpi_size(&rsa.E);
    mpi_write_binary(&rsa.E, u8publicBuf+sum, s32len);
    sum +=s32len;

    sum = 0;
    s32len = mpi_size(&rsa.N);
    mpi_write_binary(&rsa.N, u8TestBuf+sum, s32len);
    sum +=s32len;

    s32len = mpi_size(&rsa.P);
    mpi_write_binary(&rsa.P, u8TestBuf+sum, s32len);
    sum +=s32len;

    s32len = mpi_size(&rsa.Q);
    mpi_write_binary(&rsa.Q, u8TestBuf+sum, s32len);
    sum +=s32len;

    s32len = mpi_size(&rsa.DP);
    mpi_write_binary(&rsa.DP, u8TestBuf+sum, s32len);
    sum +=s32len;

    s32len = mpi_size(&rsa.DQ);
    mpi_write_binary(&rsa.DQ, u8TestBuf+sum, s32len);
    sum +=s32len;

    s32len = mpi_size(&rsa.QP);
    mpi_write_binary(&rsa.QP, u8TestBuf+sum, s32len);
    sum +=s32len;

    testlen = 52;
    for (u32Index = 0; u32Index < testlen; u32Index++)
    {
        u8PlainBuf[u32Index] = u32Index;
    }

    for (u32Index = 0; u32Index < 2; u32Index++)
    {
        ZC_Printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

        ZC_TraceData(u8PlainBuf,testlen);
        ZC_Printf("+++++++++++++++++++++++++++++++++++\n");
        SEC_EncryptTextByRsa(u8publicBuf,u8PlainBuf,g_u8DumpCloudMsg,testlen);
          //rsa_pkcs1_encrypt(&rsa, RSA_PUBLIC, testlen, u8PlainBuf, g_u8DumpCloudMsg);

        ZC_TraceData(g_u8DumpCloudMsg,testlen);
        ZC_Printf("+++++++++++++++++++++++++++++++++++\n");
        SEC_DecipherTextByRsa(u8TestBuf, g_u8DumpCloudMsg, u8DeplainBuf,testlen);
        // rsa_pkcs1_decrypt(&rsa, RSA_PRIVATE, &s32len, g_u8DumpCloudMsg, u8DeplainBuf, testlen);


        ZC_TraceData(u8DeplainBuf,testlen);

        ZC_Printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
    }

}


