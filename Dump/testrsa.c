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
    0x9b,0x6b,0x6e,0xbf,\
    0xcd,0x8f,0x73,0xef,\
    0x2d,0x92,0x1b,0x54,\
    0x85,0x02,0x2a,0xbe,\
    0x68,0x3d,0xd3,0xc0,\
    0x27,0x7f,0x52,0xd9,\
    0x44,0x52,0x5d,0xb1,\
    0x51,0xdc,0xa9,0x3c,\
    0xff,0x07,0xa6,0xec,\
    0x4a,0x27,0x11,0x53,\
    0xb6,0x40,0x83,0x28,\
    0xea,0x2d,0xed,0xfd,\
    0xc4,0x49,0x42,0x85,\
    0xcf,0x8c,0xc1,0x7f,\
    0xe0,0xa9,0x01,0xe0,\
    0x79,0xc8,0x62,0x1f,\
    0x01,0x00,0x01\
}
#define DEFAULT_IOT_PRIVATE_KEY {\
    0x9b,0x6b,0x6e,0xbf,\
    0xcd,0x8f,0x73,0xef,\
    0x2d,0x92,0x1b,0x54,\
    0x85,0x02,0x2a,0xbe,\
    0x68,0x3d,0xd3,0xc0,\
    0x27,0x7f,0x52,0xd9,\
    0x44,0x52,0x5d,0xb1,\
    0x51,0xdc,0xa9,0x3c,\
    0xff,0x07,0xa6,0xec,\
    0x4a,0x27,0x11,0x53,\
    0xb6,0x40,0x83,0x28,\
    0xea,0x2d,0xed,0xfd,\
    0xc4,0x49,0x42,0x85,\
    0xcf,0x8c,0xc1,0x7f,\
    0xe0,0xa9,0x01,0xe0,\
    0x79,0xc8,0x62,0x1f,\
    0xea,0xa7,0xcb,0x16,\
    0xad,0xd3,0x12,0x35,\
    0x6b,0xc2,0x80,0x68,\
    0xe0,0xca,0xf6,0x9b,\
    0xf6,0xb8,0xd9,0x61,\
    0xc5,0xdc,0x65,0x92,\
    0x4b,0xf5,0x24,0x30,\
    0x8c,0x8d,0xe8,0x29,\
    0xa9,0x8e,0x8c,0x4d,\
    0xea,0x39,0xa3,0x99,\
    0x13,0x72,0xea,0x90,\
    0xae,0x11,0x2e,0x5c,\
    0x32,0xbc,0xc9,0xef,\
    0x03,0x0d,0xc4,0x17,\
    0xa6,0x11,0x10,0x22,\
    0x39,0x63,0xe1,0x07,\
    0xe5,0x1b,0x0c,0xb3,\
    0xc8,0xe5,0x83,0x2a,\
    0xbb,0x5a,0xbd,0x2b,\
    0x29,0x07,0xa1,0x7e,\
    0x27,0x86,0xd5,0x5f,\
    0x9b,0xe2,0x93,0x1d,\
    0x22,0xa2,0x67,0x42,\
    0x2b,0x89,0x45,0xf9,\
    0x8a,0x0b,0x79,0xa5,\
    0x91,0xd5,0xfd,0xa1,\
    0x0a,0x59,0x9c,0x29,\
    0xd0,0xde,0x42,0x1d,\
    0x64,0x5d,0x9a,0x16,\
    0x93,0x28,0xe9,0x06,\
    0xcb,0xf8,0xc6,0x66,\
    0xe6,0xcc,0xaf,0x8f,\
    0x42,0x14,0xa5,0xf1,\
    0xe5,0x8a,0xa4,0xa0,\
    0x13,0x06,0x07,0x86,\
    0xc3,0x75,0xd8,0xc9,\
    0x32,0x6a,0xe0,0x02,\
    0x17,0x4e,0xcf,0x8f,\
    0x31,0x55,0x51,0x0f,\
    0x82,0xd4,0x5a,0x4a\
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