#ifndef  __ZC_UIPDMUP_H__ 
#define  __ZC_UIPDMUP_H__

typedef unsigned short u16_t;
struct uip_conn
{
    unsigned int fd;
    unsigned int lport;
};

typedef struct
{
    unsigned int fd;
    unsigned int lport;
}UIP_UDP_CONN;

typedef unsigned short uip_ip4addr_t[2];
typedef uip_ip4addr_t uip_ipaddr_t;

extern u8 uip_appdata[1024];
extern u8 uip_flags;
extern u16 uip_len;

#endif
