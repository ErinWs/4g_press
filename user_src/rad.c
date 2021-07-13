#include "r_cg_macrodriver.h"
#include "r_cg_userdefine.h"
#include "r_cg_sau.h"
#include "r_cg_tau.h"
#include "device.h"
#include "hum.h"
#include "lora.h"
#include "net.h"
#include "irc.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "rad.h"
    
#define  MD_RAD_VCM_ON      P0_bit.no5=1
#define  MD_RAD_VCM_OFF     P0_bit.no5=0

static struct 
{ 
    unsigned char send_buf[64];
	unsigned char recv_buf[64];
	unsigned char rec_pos;

	int sample_interval_timer;
  
}
radMisc=
{
	{0},
	{0},
	 0,

	 0
    
};

static void stop_rad(void)
{
    MD_RAD_VCM_OFF;
    disable_rad();
    radComps.op_window_time=0;
    radComps.sw._bit.running=0;
    
}


static void start_rad(void)
{
    if(radMisc.sample_interval_timer==0)
    {
        if(!radComps.sw._bit.running)
        {
            MD_RAD_VCM_ON;
            enable_rad();
            radComps.op_window_time=6;
            radComps.sw._bit.running=1;
        }
     }
     radMisc.sample_interval_timer++;
     if(radMisc.sample_interval_timer>=10)
     {
        radMisc.sample_interval_timer=0;
     }
}

static void write_rad(unsigned char * const buf,unsigned int len)
{
   
}

static long calc_average_high_mm(struct  _RAD_COMPONENTS *this,const int count)
{
    long sum=0;
    int i=0;
    long average=0;
    for(i=0;i<count;i++)
    {
        sum=sum+this->high_cm[i];
    }
    average=sum*10/count;
	average=average*(device_comps.level_param.coe/10000.);
    return average;
}

static unsigned int get_high(unsigned char *high,int len)
{
    
   
    if(radComps.high_pos<10)
    {
        if(*high>0x20)
        {
    	    radComps.high_cm[radComps.high_pos]=atol((char *)high);
    	}
    	else
    	{
            radComps.high_cm[radComps.high_pos]=0;
    	}
    	radComps.high_pos++; 
    	radComps.high_pos%=10;
        radComps.average_high_mm=calc_average_high_mm(&radComps,10);
    
        radComps.average_high_cm=radComps.average_high_mm/10;
           
	    radComps.position_height_mm=device_comps.level_param.total_high_mm-radComps.average_high_mm;
	    radComps.position_height_cm=radComps.position_height_mm/10;
	    device_comps.current_high=radComps.position_height_mm;
		device_comps.high_calibration_param.dot=3;
        device_comps.high_calibration_param.is_calibrated=1;
        hum_comps.dis_oper_mark._bit.refresh_high=1;
        if(!radComps.high_pos)
        {
            if(hum_comps.current_mode==EM_DEBUG_MODE)
            {
                radComps.op_window_time=6;
            }
            else
            {
                radComps.stop();
            }
        }
    }
    else
    {
        radComps.high_pos=0;
    }
    return len;
}
static unsigned char Check_rad_Com(unsigned char *Rec_Data,unsigned char Rec_Pos)
{
	int i=0;
	if(Rec_Pos<3)
	{
		return 0;
	}
	if(Rec_Data[0]!='E' && Rec_Data[0]!='1')
	{
		return 1;
	}
	if(Rec_Data[1]!=':')
	{
		return 1;
	}
	if(Rec_Data[0]=='E' )
	{
        if(Rec_Data[2]=='\n')
        {
	        return radComps.get_high(&Rec_Data[2],3);
	    }
	    else
	    {
             return 1;
	    }
	}
	if(Rec_Pos<3+4)//4:"3000"
	{
		return 0;
	}
	for(i=3;i<7;i++)
	{
    	if(Rec_Data[i]=='\n')
    	{
             return radComps.get_high(&Rec_Data[2],i+1);
        }
    }
    return 1;
}



static void radComps_task_handle(void)
{
   if(radComps.sw._bit.running)
   {
        unsigned char err=0;
     	do
    	{
    	    DI();
    		err=Check_rad_Com(radMisc.recv_buf,radMisc.rec_pos); 
    	    if(err>0)
    		{
    			memcpy(radMisc.recv_buf,radMisc.recv_buf+err,radMisc.rec_pos-=err);
        	}
    		EI();
	    }
	    while (err>0);
   }
}


radComps_t radComps=
{
    "",
    &radComps,
    (unsigned char *)&radMisc.recv_buf,
    &radMisc.rec_pos,
    0,
    
   {0},
    write_rad,
    start_rad,
    stop_rad,
    {0},//unsigned int high_cm[20];
	0,  //int          high_pos;
	get_high,//unsigned int (*get_high)(char *,int len);
	
	0,//long  average_high_mm;
    0,//long  average_high_cm;
	0,//long  position_height_mm;
	0,//long  position_height_cm;
    radComps_task_handle
};


