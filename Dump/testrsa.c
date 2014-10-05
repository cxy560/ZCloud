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

extern u8 g_u8DumpCloudMsg[102400];
extern IOT_ADAPTER   	IoTpAd;
#define DEFAULT_VENDOR_NEME			"Mediatek"
#define DEFAULT_PRODUCT_TYPE    	"IoT 1"
#define DEFAULT_PRODUCT_NAME        "MT7681"
#define DEFAULT_CLOUD_ADDR          "www.baidu.com"

#define DEFAULT_IOT_CLOUD_KEY {\
    0x8d, 0xc2, 0xf2, 0xae,\
    0xbc, 0x7b, 0xdc, 0x93,\
    0x80, 0xde, 0xb9, 0x6c,\
    0xa0, 0xe3, 0x93, 0x5d,\
    0x89, 0x30, 0x7f, 0x1e,\
    0xec, 0x7f, 0xd3, 0x75,\
    0x3c, 0x3d, 0x4b, 0xd6,\
    0x59, 0x8a, 0xb3, 0x2b,\
    0x01, 0x00, 0x01\
}
#define DEFAULT_IOT_PRIVATE_KEY {\
    0x8d, 0xc2, 0xf2, 0xae,\
    0xbc, 0x7b, 0xdc, 0x93,\
    0x80, 0xde, 0xb9, 0x6c,\
    0xa0, 0xe3, 0x93, 0x5d,\
    0x89, 0x30, 0x7f, 0x1e,\
    0xec, 0x7f, 0xd3, 0x75,\
    0x3c, 0x3d, 0x4b, 0xd6,\
    0x59, 0x8a, 0xb3, 0x2b,\
    0xea, 0x76, 0xb5, 0x4d,\
    0x3a, 0xb8, 0x75, 0xfc,\
    0xbb, 0xc6, 0x49, 0xa3,\
    0x48, 0x5b, 0x89, 0x2f,\
    0x9a, 0xc8, 0x65, 0x68,\
    0x9a, 0x07, 0x15, 0x71,\
    0x03, 0x92, 0x10, 0x0e,\
    0x8c, 0x7b, 0x7e, 0xc5,\
    0x1e, 0x8b, 0x9c, 0x8f,\
    0x15, 0x29, 0x56, 0xe5,\
    0xb8, 0x28, 0xdb, 0x0b,\
    0x14, 0xb0, 0x28, 0x2d,\
    0x7d, 0x68, 0xd9, 0x21,\
    0x83, 0x8b, 0x31, 0xc8,\
    0x3b, 0xc8, 0x60, 0xd2,\
    0x28, 0xe8, 0xf6, 0xb1,\
    0xcb, 0x26, 0x19, 0x68,\
    0x7c, 0x69, 0x4a, 0x80,\
    0xa5, 0x80, 0xd8, 0x0c,\
    0x3f, 0x43, 0x8f, 0x95\
}

IOT_USR_CFG Usr_Cfg = {
    DEFAULT_VENDOR_NEME,
    DEFAULT_PRODUCT_TYPE,
    DEFAULT_PRODUCT_NAME,
    DEFAULT_IOT_PRIVATE_KEY,
    DEFAULT_IOT_CLOUD_KEY,
    DEFAULT_CLOUD_ADDR
};


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
    MT_Init();

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
    ZC_TraceData(u8publicBuf, sum);
    ZC_Printf("+++++++++++++++++++++++++++++++++++\n");
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
    ZC_TraceData(u8TestBuf, sum);
    ZC_Printf("+++++++++++++++++++++++++++++++++++\n");

    testlen = 20;
    for (u32Index = 0; u32Index < testlen; u32Index++)
    {
        u8PlainBuf[u32Index] = u32Index;
    }

    for (u32Index = 0; u32Index < 2; u32Index++)
    {
        ZC_Printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

        ZC_TraceData(u8PlainBuf,testlen);
        ZC_Printf("+++++++++++++++++++++++++++++++++++\n");
        //SEC_EncryptTextByRsa(u8publicBuf,u8PlainBuf,g_u8DumpCloudMsg,52);
        //SEC_EncryptTextByRsa(u8publicBuf,u8PlainBuf+10,g_u8DumpCloudMsg+10,42);
        rsa_pkcs1_encrypt(&rsa, RSA_PUBLIC, testlen, u8PlainBuf, g_u8DumpCloudMsg);

        ZC_TraceData(g_u8DumpCloudMsg,testlen);
        ZC_Printf("+++++++++++++++++++++++++++++++++++\n");
        // SEC_DecryptTextByRsa(u8TestBuf, g_u8DumpCloudMsg, u8DeplainBuf,testlen);
        rsa_pkcs1_decrypt(&rsa, RSA_PRIVATE, &s32len, g_u8DumpCloudMsg, u8DeplainBuf, testlen);


        ZC_TraceData(u8DeplainBuf,testlen);

        ZC_Printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
    }

}

void testcloud()
{
    mpi X;  
    s32 s32len;
    mpi_init(&X); 
    u8 u8Key[] = {"92843671885879468539136027873653378477792077665795012146250279466996190721073"};
    mpi_read_string(&X, 10, u8Key);

    s32len = mpi_size(&X);
    mpi_write_binary(&X, Usr_Cfg.CloudKey, s32len);
    memcpy(&IoTpAd.UsrCfg, &Usr_Cfg, sizeof(IOT_USR_CFG));
    TestSec();

}

BOOLEAN reset_usr_cfg(BOOLEAN bUpFlash)
{
    u16_t CfgLen;
    u16_t FlashLen;
    u16_t ReadLen;
    u32 u32FlashBase;

    memset(IoTpAd.flash_rw_buf ,0xff, sizeof(IoTpAd.flash_rw_buf));

    memcpy(&IoTpAd.UsrCfg , &Usr_Cfg , sizeof(IOT_USR_CFG));

    if(bUpFlash == TRUE)
    {
        IoTpAd.flash_rw_buf[FLASH_USR_CFG_PRODUCT_INFO_STORED] = PRODUCT_INFO_STORED;

        FlashLen = sizeof(IoTpAd.flash_rw_buf);
        ReadLen = 0;
        CfgLen = sizeof(IOT_USR_CFG);
        if ((CfgLen + FLASH_USR_CFG_PRODUCT_INFO_STORED_LEN) <= FlashLen)
        {
            memcpy( &IoTpAd.flash_rw_buf[FLASH_USR_CFG_VENDOR_NAME],    
                IoTpAd.UsrCfg.VendorName,
                CfgLen);
            spi_flash_write(FLASH_USR_CFG_BASE, IoTpAd.flash_rw_buf, FlashLen);
        }
        else
        {
            memcpy( &IoTpAd.flash_rw_buf[FLASH_USR_CFG_VENDOR_NAME],    
                IoTpAd.UsrCfg.VendorName,
                FlashLen - FLASH_USR_CFG_PRODUCT_INFO_STORED_LEN);

            spi_flash_write(FLASH_USR_CFG_BASE, IoTpAd.flash_rw_buf, FlashLen);

            ReadLen += FlashLen - FLASH_USR_CFG_PRODUCT_INFO_STORED_LEN;
            u32FlashBase = FLASH_USR_CFG_BASE + FlashLen;
            while (ReadLen + FlashLen <= CfgLen)
            {
                memcpy( IoTpAd.flash_rw_buf,    
                    IoTpAd.UsrCfg.VendorName + ReadLen,
                    FlashLen);

                spi_flash_write(u32FlashBase, IoTpAd.flash_rw_buf, FlashLen);

                ReadLen += FlashLen;
                u32FlashBase += FlashLen;
            }

            if (ReadLen < CfgLen)
            {
                memcpy(IoTpAd.flash_rw_buf,    
                    IoTpAd.UsrCfg.VendorName + ReadLen,
                    CfgLen - ReadLen);
                spi_flash_write(u32FlashBase, IoTpAd.flash_rw_buf, FlashLen);
            }
        }
    }

    return TRUE;
}

BOOLEAN load_usr_cfg(VOID)
{
    u16_t CfgLen;
    u16_t FlashLen;
    u16_t ReadLen;
    u32 u32FlashBase;
    spi_flash_read(FLASH_USR_CFG_BASE, IoTpAd.flash_rw_buf, sizeof(IoTpAd.flash_rw_buf));

    if (IoTpAd.flash_rw_buf[FLASH_USR_CFG_PRODUCT_INFO_STORED] == PRODUCT_INFO_STORED)
    {
        FlashLen = sizeof(IoTpAd.flash_rw_buf);
        ReadLen = 0;
        CfgLen = sizeof(IOT_USR_CFG);
        if ((CfgLen + FLASH_USR_CFG_PRODUCT_INFO_STORED_LEN) <= FlashLen)
        {
            memcpy((IoTpAd.UsrCfg.VendorName + ReadLen), 
                IoTpAd.flash_rw_buf + FLASH_USR_CFG_PRODUCT_INFO_STORED_LEN, 
                CfgLen);
        }
        else
        {
            memcpy((IoTpAd.UsrCfg.VendorName + ReadLen), 
                IoTpAd.flash_rw_buf + FLASH_USR_CFG_PRODUCT_INFO_STORED_LEN, 
                FlashLen - FLASH_USR_CFG_PRODUCT_INFO_STORED_LEN);

            ReadLen += FlashLen - FLASH_USR_CFG_PRODUCT_INFO_STORED_LEN;
            u32FlashBase = FLASH_USR_CFG_BASE + FlashLen;
            while (ReadLen + FlashLen <= CfgLen)
            {
                spi_flash_read(u32FlashBase, IoTpAd.flash_rw_buf, FlashLen);

                memcpy((IoTpAd.UsrCfg.VendorName + ReadLen), IoTpAd.flash_rw_buf, FlashLen);

                ReadLen += FlashLen;
                u32FlashBase += FlashLen;
            }

            if (ReadLen < CfgLen)
            {
                spi_flash_read(u32FlashBase, IoTpAd.flash_rw_buf, FlashLen);
                memcpy((IoTpAd.UsrCfg.VendorName + ReadLen), IoTpAd.flash_rw_buf, CfgLen - ReadLen);
            }
        }
    }
    else
    {
        reset_usr_cfg(TRUE);
    }


    return TRUE;
}


void testread()
{
    memset(g_u8DumpCloudMsg, 0xcc, 102400);
    load_usr_cfg();
    
    load_usr_cfg();
}