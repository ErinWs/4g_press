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
#include "press.h"
    
#define  MD_PRESS_VCM_ON      P0_bit.no5=1
#define  MD_PRESS_VCM_OFF     P0_bit.no5=0

 struct 
{ 
    unsigned char send_buf[32];
	unsigned char recv_buf[64];
	unsigned char rec_pos;

	int sample_interval_timer;
  
}
pressMisc=
{
	{0},
	{0},
	 0,

	 0
    
};

static void stop_press(void)
{
    MD_PRESS_VCM_OFF;
	MD_IR_VCM_OFF;
    disable_press();
    pressComps.op_window_time=0;
    pressComps.sw._bit.running=0;
    pressComps.sw._bit.window_running=0; 
    
}

static unsigned char Check_Sum(unsigned char *Data,unsigned char Len)
{
	unsigned char Sum=0;
	unsigned char i=0;
	for(i=0;i<Len;i++)
	{
		Sum+=Data[i];
	}
	return Sum;
}

static void write_press(unsigned int cmd)
{
    int i=0;
    enable_press();
    pressComps.sw._bit.running=1;
    pressMisc.send_buf[i++]=0x68;
    pressMisc.send_buf[i++]=0x99;
    pressMisc.send_buf[i++]=0x99;
    pressMisc.send_buf[i++]=0x99;
    pressMisc.send_buf[i++]=0x99;
    pressMisc.send_buf[i++]=0x99;
    pressMisc.send_buf[i++]=0x99;
    pressMisc.send_buf[i++]=0x99;
    pressMisc.send_buf[i++]=0x68;
    pressMisc.send_buf[i++]=0x00;
    pressMisc.send_buf[i++]=0x02;
    pressMisc.send_buf[i++]=cmd>>8;
    pressMisc.send_buf[i++]=cmd;
    pressMisc.send_buf[i++]=Check_Sum(pressMisc.send_buf,i);
    pressMisc.send_buf[i++]=0x16;
    R_UART0_Send(pressMisc.send_buf,i);
}



static void start_press(void)
{
    if(!ircComps.sw._bit.runing)
    {
        if(pressComps.sw._bit.on)
    	{
            if(!pressComps.sw._bit.window_running)
            {
                MD_PRESS_VCM_ON;
                press_modify_baud(3,0);//9600
                pressComps.op_window_time=70;
                pressComps.sw._bit.window_running=1; 
                
            }
            pressComps.sw._bit.on=0;
    	}
	}
     pressMisc.sample_interval_timer++;
     if(pressMisc.sample_interval_timer>=300)
     {
        pressMisc.sample_interval_timer=0;
        pressComps.sw._bit.on=1;
     }
}





static unsigned int pro_press_com(unsigned char Cmd,unsigned char *buf,int len)
{
   unsigned int DataId=((unsigned int)buf[11]<<8)+buf[12];
	if(Cmd==0x80)//Read
	{
		
		switch(DataId)
		{
		    case 0x901f:
		            device_comps.calibration_param.y[3]=((long)buf[13]<<24)+((long)buf[14]<<16)+((long)buf[15]<<8)+buf[16];
					device_comps.current_press=((long)buf[17]<<24)+((long)buf[18]<<16)+((long)buf[19]<<8)+buf[20];                                   
					device_comps.calibration_param.unit=buf[21];
		            device_comps.current_temp=((unsigned int)buf[22]<<8)+buf[23];
		           // device_comps.batt=buf[24];
		            device_comps.calibration_param.dot=4;
		            device_comps.calibration_param.is_calibrated=1;
		            hum_comps.dis_oper_mark._bit.refresh_press=1;
		            pressComps.stop();
                    break;
		   default:
		        break;
		}
		return len;
	}
	else if(Cmd==0x81)//write
	{
        return len;
	}
	return 1;
}	
  
static unsigned char Check_press_Com(unsigned char *Rec_Data,unsigned char Rec_Pos)
{
	
	int len;
	if(Rec_Pos<2)
	{
		return 0;
	}
	if(Rec_Data[0]!=0x68)
	{
		return 1;
	}
	if(Rec_Pos<13)
	{
		return 0;
	}
	if(Rec_Data[8]!=0x68)
    {
		return 1;
	}
    if((Rec_Data[9]!=0x80)&&(Rec_Data[9]!=0x81))
    {
		return 1;
	}
	if((Rec_Data[11]==0x90)&&(Rec_Data[12]==0x1f))
    {
		len=0x0f;
	}
	else
	{
	    return 1;
	}
	if(Rec_Data[10]!=len)
    {
		return 1;
	}
	
	if(Rec_Pos<13+Rec_Data[10])
	{
		return 0;
	}
	
	if(Rec_Data[13+Rec_Data[10]-1-1]!=Check_Sum(Rec_Data,13+Rec_Data[10]-2))
	{
		return 1;
	}
	if(Rec_Data[13+Rec_Data[10]-1]!=0x16)
	{
		return 1;
	}
    return pressComps.pro_press_com(Rec_Data[9],Rec_Data,13+Rec_Data[10]);
   
}



static void pressComps_task_handle(void)
{
   if(pressComps.sw._bit.running)
   {
        unsigned char err=0;
     	do
    	{
    	    DI();
    		err=Check_press_Com(pressMisc.recv_buf,pressMisc.rec_pos); 
    	    memcpy(pressMisc.recv_buf,pressMisc.recv_buf+err,pressMisc.rec_pos-=err);
        	EI();
	    }
	    while (err>0);
   }
}


pressComps_t pressComps=
{
    "",
    &pressComps,
    (unsigned char *)&pressMisc.recv_buf,
    &pressMisc.rec_pos,
    0,
    
   {0x04},//sw._bit.on=1
    write_press,
    start_press,
    stop_press,
   
	pro_press_com,//unsigned int (*get_high)(char *,int len);
	
	
    pressComps_task_handle
};



