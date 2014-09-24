#include <zc_common.h>
#include <uip.h>
#include <uiplib.h>
u8 uip_appdata[1024];
struct uip_conn g_DumpConn;
UIP_UDP_CONN g_DmupUdpConn;
u8 uip_flags = 0;
u16 uip_len = 0;
u8 g_u8Ipaddr[4]={127,0,0,1};

int iot_send(u8 fd, u8 *buf, u16 len)
{
    uip_flags = UIP_NEWDATA;
    return len;
}

int iot_udp_send(u8 fd, u8 *buf, u16 len, u8 *rip, u16 rport)
{
    uip_flags = UIP_NEWDATA;
    return len;
}

int IoT_uart_output(u8 *msg, u16 count)
{
    return count;
}

struct uip_conn * uip_connect(uip_ipaddr_t *ripaddr, u16 rport)
{
    static unsigned int ConnectTime = 0;
    
    ConnectTime++;
    uip_flags = UIP_TIMEDOUT;    

    if (10 == ConnectTime)
    {
        uip_flags = UIP_CONNECTED;
    }
    
    return &g_DumpConn;
}

UIP_UDP_CONN *uip_udp_new(uip_ipaddr_t *ripaddr, u16 rport)
{
    return &g_DmupUdpConn;
}
void uip_send(const void *data, u16_t len)
{

}

u16 *resolv_lookup(char *name)
{
    return (u16*)g_u8Ipaddr;    
}
