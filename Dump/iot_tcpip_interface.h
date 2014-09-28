#ifndef  __ZC_IOT_TCPIP_INTERFACE_H__ 
#define  __ZC_IOT_TCPIP_INTERFACE_H__
//#define Printf_High printf
typedef int clock_time_t;
#define CLOCK_CONF_SECOND 1000

struct timer {
    clock_time_t start;
    clock_time_t interval;
};

void timer_set(struct timer *t, clock_time_t interval);
void timer_reset(struct timer *t);
void timer_restart(struct timer *t);
int timer_expired(struct timer *t);

#endif
