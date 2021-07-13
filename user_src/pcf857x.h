#ifndef PCF857X_H
#define PCF857X_H

typedef union 
{
    volatile char PX;
    struct
    {
        volatile int no0          :1;
        volatile int no1          :1;
        volatile int no2          :1;
        volatile int no3          :1;
        volatile int no4          :1;
        volatile int no5          :1;
        volatile int no6          :1;
        volatile int no7          :1;

    }_bit;
}
port_t;

typedef struct _PCF857X_COMPONENTS
{
	char *desc;
	struct  _PCF857X_COMPONENTS *this;
	port_t IN;
	port_t OUT;
    int  (*const Init )(void);
    int  (*const read )(struct  _PCF857X_COMPONENTS *this);
	int  (*const write)(struct  _PCF857X_COMPONENTS *this);
	
}pcf857x_comps_t;
extern pcf857x_comps_t pcf857x_comps;
#endif

