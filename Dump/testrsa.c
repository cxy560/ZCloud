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
#define DEFAULT_MOUDLE_KEY          "private"
#define DEFAULT_CLOUD_KEY           "public"
#define DEFAULT_CLOUD_ADDR          "www.baidu.com"

IOT_USR_CFG Usr_Cfg = {
    DEFAULT_VENDOR_NEME,
    DEFAULT_PRODUCT_TYPE,
    DEFAULT_PRODUCT_NAME,
    DEFAULT_MOUDLE_KEY,
    DEFAULT_CLOUD_KEY,
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
        SEC_EncryptTextByRsa(u8publicBuf,u8PlainBuf,g_u8DumpCloudMsg,52);
        //SEC_EncryptTextByRsa(u8publicBuf,u8PlainBuf+10,g_u8DumpCloudMsg+10,42);
          //rsa_pkcs1_encrypt(&rsa, RSA_PUBLIC, testlen, u8PlainBuf, g_u8DumpCloudMsg);

        ZC_TraceData(g_u8DumpCloudMsg,testlen);
        ZC_Printf("+++++++++++++++++++++++++++++++++++\n");
        SEC_DecryptTextByRsa(u8TestBuf, g_u8DumpCloudMsg, u8DeplainBuf,testlen);
        // rsa_pkcs1_decrypt(&rsa, RSA_PRIVATE, &s32len, g_u8DumpCloudMsg, u8DeplainBuf, testlen);


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