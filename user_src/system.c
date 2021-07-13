#include "r_cg_macrodriver.h"
#include "r_cg_wdt.h"
#include "r_cg_it.h"
#include "r_cg_cgc.h"
#include "device.h"
#include "net.h"
#include "protocol.h"
#include "hum.h"
#include "irc.h"
#include "lora.h"
#include "rad.h"
#include "modbus.h"
#include "cs123x.h"
#include "system.h"
#include "press.h"



pfun const _hlaf_s_task_handle=_0_5s_task_handle;
pfun const delay_task_50ms=_50ms_task_handle;

typedef struct _TASK_COMPONENTS
{
    char *desc;
    struct _TASK_COMPONENTS *this;
    int             run_flag;
    volatile int    timer;          //50ms timer counter
    const int       interval_time;  //*50ms timer counter
    pfun * const    task_hook;
    char            isRealTime;//TRUE FALSE
    //void (* *const task_hook)(void);
}task_comps_t;
 
static void  enter_pwd_mode(void)
{
    if(!(loraComps.sw._bit.runing)&&(!ircComps.sw._bit.runing)&&(!modbusComps.sw._bit.runing)&&(!netComps.St._bit.running)&&(!radComps.sw._bit.running)&&(!pressComps.sw._bit.running)&&(!cs123x_comps.sw._bit.adc_updated))
    {
        STOP();
    }
}

pfun const pwd_mode_task=enter_pwd_mode;



static task_comps_t task_comps[]=//50ms
{
	
	{"",&task_comps[0] ,0,1       ,1 ,     (pfun *)&hum_comps.task_handle            ,'F'},//*50ms  hum_comps.task_handle
	{"",&task_comps[1] ,0,1       ,4 ,     (pfun *)&device_comps.task_handle         ,'F'},//delay 100*50ms
	{"",&task_comps[2] ,0,5       ,10,     (pfun *)&_hlaf_s_task_handle              ,'F'},
	{"",&task_comps[3] ,0,5       ,1 ,     (pfun *)&loraComps.task_50ms              ,'F'},
	
	{"",&task_comps[4] ,0,22      ,0 ,     (pfun *)&ircComps.task_handle             ,'T'},//fast exe
	{"",&task_comps[5] ,0,5       ,0 ,     (pfun *)&loraComps.task_handle            ,'T'},//fast exe
	{"",&task_comps[6] ,0,2       ,0 ,     (pfun *)&pwd_mode_task                    ,'T'},//fast exe
	{"",&task_comps[7] ,0,1       ,0 ,     (pfun *)&modbusComps.task_handle          ,'T'},//fast exe
	{"",&task_comps[8] ,0,5       ,0 ,     (pfun *)&protocolComps.task_handle        ,'T'},//fast exe
	{"",&task_comps[9] ,0,5       ,0 ,     (pfun *)&netComps.task_handle             ,'T'},//fast exe
	{"",&task_comps[9] ,0,5       ,0 ,     (pfun *)&netComps.task_handle             ,'T'},//fast exe
    {"",&task_comps[10],0,1       ,0 ,     (pfun *)&radComps.task_handle             ,'T'},//fast exe
	{"",&task_comps[11],0,1       ,0 ,     (pfun *)&pressComps.task_handle           ,'T'},//fast exe
    {"",&task_comps[12],0,1       ,1 ,     (pfun *)&delay_task_50ms                  ,'F'},
	{"",&task_comps[13],0,1       ,0 ,     (pfun *)&cs123x_comps.task_handle         ,'T'}//fast exe
	
	//...TODO......
};

void call_back_task_remarks(void)//millisecond interval timer interrupt callback si used to start task
{
	int i=0;
	for(i=0;i<sizeof(task_comps)/sizeof(task_comps_t);i++)
	{
		if(task_comps[i].timer>0)
		{
			task_comps[i].timer--;
			if(task_comps[i].timer==0)
			{
				task_comps[i].timer=task_comps[i].interval_time;
				task_comps[i].run_flag=1;
			}
		}
	}
}


void task_process(void)//Main loop call is used to execution tasks
{
	int i=0;
	for(i=0;i<sizeof(task_comps)/sizeof(task_comps_t);i++)
	{
		if(task_comps[i].run_flag)
		{
			(*task_comps[i].task_hook)();//Priority combination ()>.>*
			if(task_comps[i].isRealTime=='F')
			{
			    task_comps[i].run_flag=0;
			}
		}
	}
	// ircComps.task_handle();
	 //protocolComps.task_handle();
	 //netComps.task_handle();
	// loraComps.task_handle();
	// enter_pwd_mode();
	 
	//TODO other realtime task;
}
static void Modify_IT_Init_Value(void)
{
    TMKAEN = 1U;    /* supply IT clock */
    ITMC = _0000_IT_OPERATION_DISABLE;    /* disable IT operation */
    TMKAMK = 1U;    /* disable INTIT interrupt */
    TMKAIF = 0U;    /* clear INTIT interrupt flag */
    /* Set INTIT low priority */
    TMKAPR1 = 1U;
    TMKAPR0 = 1U;
    ITMC = 50*15;
}

void user_init(void)
{
    
    long timer=0;
    unsigned int w_count;
    while(!systemComps.sw._bit.test_xt1_run_flag)
    {
        timer++;
        if(timer>30000)
        {
              /* Set fSUB */
            XTSTOP = 1U;
             /* Change the waiting time according to the system */
            for (w_count = 0U; w_count <= CGC_SUBWAITTIME; w_count++)
            {
                NOP();
            }
            OSMC = _00_CGC_SUBINHALT_ON | _10_CGC_RTC_IT_LCD_CLK_FIL;


            R_IT_Stop();
            Modify_IT_Init_Value();
            R_IT_Start();

            break;
        }
    }
    if(timer<=30000)
    {
        systemComps.sw._bit.is_xt1_running=1;
    }
    
}
systemComps_t systemComps=
{
    {0},
    0x0e,//char rst_code;
    user_init,
    call_back_task_remarks,
    task_process,
};




