#include "r_cg_macrodriver.h"
#include "r_cg_userdefine.h"
#include "r_cg_lcd.h"
#include "r_cg_sau.h"
#include "r_cg_rtc.h"
#include "r_cg_adc.h"
#include "r_cg_tau.h"
#include "r_cg_intp.h"
#include "hum.h"
#include "lora.h"
#include "irc.h"
#include "device.h"
#include "24cxx.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "math.h"
#include "modbus.h"
#define  MD_DATA_ID_READ_MEASURE_INFO      0x901f
#define  MD_DATA_ID_READ_BASIC_INFO        0x9020
#define  MD_DATA_ID_READ_ALARM_PARAM       0x9021
#define  MD_DATA_ID_READ_RPORT_PARAM       0x9022

#define  MD_DATA_ID_READ_ACCESS_ADDR        0x9023
#define  MD_DATA_ID_READ_HIGH_INFO          0x9026
#define  MD_DATA_ID_READ_4_20MA_INFO        0x9027


#define  MD_DATA_ID_SET_DEVICE_ADDR         0x9018
#define  MD_DATA_ID_SET_ACCESS_ADDR         0x9005
#define  MD_DATA_ID_ENTER_CAL_MODE          0xfa55//
#define  MD_DATA_ID_ENTER_NORMAL_MODE       0xfa99//
#define  MD_DATA_ID_WRITE_ALARM_PARAM       0x9001//
#define  MD_DATA_ID_WRITE_CAL_DATA          0x9002//
#define  MD_DATA_ID_WRITE_REPORT_PARAM      0x9003//
#define  MD_DATA_ID_WRITE_DEVICE_TIME       0x9004

 


static struct 
{ 
    unsigned char send_buf[128];
	unsigned char recv_buf[64];
	unsigned char rec_pos;

	unsigned char const *send_addr;
	int                  send_len;

	unsigned int  dataId;
    yl701_info_t yl701_info;
}
loraMisc=
{
	{0},
	{0},
	 0,
	 
     0,
	 0,
	 0,
	{1,1,1,1,1}
    
};

static int read_yl701_info(void const *buf,int len )
{
     return _24cxx_comps.read(MD_LORA_PARAM_START_ADDR,buf,len);
}

static int save_yl701_info(void const *buf,int len )
{
     return _24cxx_comps.write(MD_LORA_PARAM_START_ADDR,buf,len);
}



static void write_lora(unsigned char * const buf,unsigned int len)
{
     loraMisc.send_addr=buf;
     loraMisc.send_len=len;
   
    loraComps.sw._bit.busy=1;
   
    if(MD_LORA_EN_PIN)
    {
        volatile int i=5000;
    	while(i>0)
    	{
    		NOP();
    		i--;
    	}
    	disable_lora();
        loraComps.sw._bit.runing=0;
        loraComps.sw._bit.sendReq=1;
        loraComps.count=2;//delay50-100ms
        MD_LORA_EN_PIN_RESET;
        if(loraComps.work_st.mode==EM_RUN)
        {
            MD_LORA_SET_PIN_RESET;
        }
        else if(loraComps.work_st.mode==EM_CFG)
        {
            MD_LORA_SET_PIN_SET;
        }
	}
    else
    {
        enable_lora();
        loraComps.sw._bit.runing=1;
        R_UART2_Send(loraMisc.send_addr,loraMisc.send_len);
        loraComps.work_st.dir=EM_SEND;
        
     }
     
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

static unsigned int  Check_Sum_back_2bytes(unsigned char const *Data,int Len)
{
	unsigned int Sum=0;
	int  i=0;
	for(i=0;i<Len;i++)
	{
		Sum+=Data[i];
	}
	return Sum;
}



static int add_measure_data(unsigned char Cmd,unsigned int DataId,unsigned char *buf,unsigned char rssi)
{
    int i=0;
    long temp=0;
    buf[i++]=loraMisc.yl701_info.NodeId>>8;
    buf[i++]=loraMisc.yl701_info.NodeId;
	buf[i++]=(Cmd|0x80);
	buf[i++]=0;//Length
	buf[i++]=DataId>>8;//dataID
	buf[i++]=DataId;
	                                        //add full range ...
	switch(DataId)
	{
        case MD_DATA_ID_READ_MEASURE_INFO:
        	temp=formatData4fixDot(device_comps.calibration_param.y[3],device_comps.calibration_param.dot);
        	buf[i++]=temp>>24;
        	buf[i++]=temp>>16;
        	buf[i++]=temp>>8;
        	buf[i++]=temp;
        	                                         //current p
        	temp=formatData4fixDot(device_comps.current_press,device_comps.calibration_param.dot);                                        
        	buf[i++]=temp>>24;
        	buf[i++]=temp>>16;
        	buf[i++]=temp>>8;
        	buf[i++]=temp;
        	
            buf[i++]=(device_comps.calibration_param.unit&0x0f);//param unit
            break;
      case MD_DATA_ID_READ_HIGH_INFO:
        	temp=formatData4fixDot(device_comps.high_calibration_param.y[1],device_comps.high_calibration_param.dot);
			buf[i++]=temp>>24;
			buf[i++]=temp>>16;
			buf[i++]=temp>>8;
			buf[i++]=temp;
			                                         
			temp=formatData4fixDot(device_comps.current_high,device_comps.high_calibration_param.dot);                                        
			buf[i++]=temp>>24;
			buf[i++]=temp>>16;
			buf[i++]=temp>>8;
			buf[i++]=temp; 
			
			temp=formatData4fixDot(device_comps.current_volume,device_comps.high_calibration_param.dot+2); //v=S*H H/=10,S:3 fix dot                                       
			buf[i++]=temp>>24;
			buf[i++]=temp>>16;
			buf[i++]=temp>>8;
			buf[i++]=temp;
            break;
      case MD_DATA_ID_READ_4_20MA_INFO:
        	temp=formatData4fixDot(device_comps.current_4_20ma,3);
			buf[i++]=temp>>24;
			buf[i++]=temp>>16;
			buf[i++]=temp>>8;
			buf[i++]=temp;
			                                         
			temp=formatData4fixDot(device_comps.current_4_20ma,3);                                        
			buf[i++]=temp>>24;
			buf[i++]=temp>>16;
			buf[i++]=temp>>8;
			buf[i++]=temp; 
			
			temp=formatData4fixDot(device_comps.current_4_20ma,3);                                     
			buf[i++]=temp>>24;
			buf[i++]=temp>>16;
			buf[i++]=temp>>8;
			buf[i++]=temp;
            break;      
      default:
            break;
   }
    
                    

    buf[i++]=device_comps.current_temp>>8;
    buf[i++]=device_comps.current_temp;

    buf[i++]=device_comps.batt;

    buf[i++]=rssi;//rssi
	
	
	///ADD Other Data
	buf[3]=i-4;
	buf[i++]=Check_Sum(buf,i);
	return i;
}
    
static unsigned char Check_Sum_whih_NodeId(unsigned int NodeId,unsigned char *Data,unsigned char Len)
{
	unsigned char Sum=(unsigned char)(NodeId>>8)+(unsigned char)NodeId;
	unsigned char i=0;
	for(i=0;i<Len;i++)
	{
		Sum+=Data[i];
	}
	return Sum;
}



static unsigned char Pro_lora(unsigned char Cmd,unsigned char *buf)
{
	unsigned char i=0;
	unsigned char k=0;
	unsigned long temp=0;
	unsigned char VerifyResult;
	
	unsigned int DataId=((unsigned int)buf[2]<<8)+buf[3];
	
	if(Cmd==0)//Read
	{
		
		switch(DataId)
		{
            /*
			case MD_DATA_ID_READ_BASIC_INFO:                //BasicInf
					loraMisc.send_buf[i++]=(buf[9]|0x80);
					loraMisc.send_buf[i++]=0;//Length
					loraMisc.send_buf[i++]=buf[11];//dataID
					loraMisc.send_buf[i++]=buf[12];
					memcpy(&loraMisc.send_buf[i],netComps.net_info.imei,16);//ADD IMEI
					i+=16;
					memcpy(&loraMisc.send_buf[i],netComps.net_info.imsi,16);//ADD IMSI
					i+=16;
					loraMisc.send_buf[i++]=MD_FL_VER;//device hwVer
					loraMisc.send_buf[i++]=MD_FL_VER;//device swVer
					loraMisc.send_buf[i++]=MD_FL_VER;//protocol swVer
				
					memcpy(&loraMisc.send_buf[i],netComps.net_info.firmVer,20);//ADD FireWave ver
					i+=20;
					memcpy(&loraMisc.send_buf[i],netComps.net_info.iccid,20);//ADD iccid
					i+=20;
					memcpy(&loraMisc.send_buf[i],device_comps.access_param.ip,4);//New IP
					i+=4;
					loraMisc.send_buf[i++]=device_comps.access_param.port>>8;
					loraMisc.send_buf[i++]=device_comps.access_param.port;
					
					///ADD Other Data
					loraMisc.send_buf[10]=i-11;
					loraMisc.send_buf[i++]=Check_Sum(loraMisc.send_buf,i);
					loraMisc.send_buf[i++]=0x16;
					VerifyResult=0;
					break;
			*/		
			case MD_DATA_ID_READ_ALARM_PARAM:                //alram param
			        loraMisc.send_buf[i++]=loraMisc.yl701_info.NodeId>>8;
                    loraMisc.send_buf[i++]=loraMisc.yl701_info.NodeId;
					loraMisc.send_buf[i++]=(Cmd|0x80);
					loraMisc.send_buf[i++]=0;//Length
					loraMisc.send_buf[i++]=DataId>>8;//dataID
					loraMisc.send_buf[i++]=DataId;
					
					loraMisc.send_buf[i++]=device_comps.alarm_param.press_high_upper>>24;
					loraMisc.send_buf[i++]=device_comps.alarm_param.press_high_upper>>16;
					loraMisc.send_buf[i++]=device_comps.alarm_param.press_high_upper>>8;
					loraMisc.send_buf[i++]=device_comps.alarm_param.press_high_upper;
					
					loraMisc.send_buf[i++]=device_comps.alarm_param.press_high_lower>>24;
					loraMisc.send_buf[i++]=device_comps.alarm_param.press_high_lower>>16;
					loraMisc.send_buf[i++]=device_comps.alarm_param.press_high_lower>>8;
					loraMisc.send_buf[i++]=device_comps.alarm_param.press_high_lower;

					loraMisc.send_buf[i++]=device_comps.alarm_param.press_low_upper>>24;
					loraMisc.send_buf[i++]=device_comps.alarm_param.press_low_upper>>16;
					loraMisc.send_buf[i++]=device_comps.alarm_param.press_low_upper>>8;
					loraMisc.send_buf[i++]=device_comps.alarm_param.press_low_upper;

					loraMisc.send_buf[i++]=device_comps.alarm_param.press_low_lower>>24;
					loraMisc.send_buf[i++]=device_comps.alarm_param.press_low_lower>>16;
					loraMisc.send_buf[i++]=device_comps.alarm_param.press_low_lower>>8;
					loraMisc.send_buf[i++]=device_comps.alarm_param.press_low_lower;
					
					loraMisc.send_buf[i++]=device_comps.alarm_param.unit;

					loraMisc.send_buf[i++]=device_comps.alarm_param.temp_high>>8;
					loraMisc.send_buf[i++]=device_comps.alarm_param.temp_high;
					loraMisc.send_buf[i++]=device_comps.alarm_param.temp_low>>8;
					loraMisc.send_buf[i++]=device_comps.alarm_param.temp_low;
					//memcpy(&loraMisc.send_buf[i],&device_comps.alarm_param.press_high_upper,
					//i+=&device_comps.alarm_param.temp_low-&device_comps.alarm_param.press_high_upper+sizeof(device_comps.alarm_param.temp_low));//New IP
				
					///ADD Other Data
					loraMisc.send_buf[3]=i-4;
					loraMisc.send_buf[i++]=Check_Sum(loraMisc.send_buf,i);
					
					
					break;
            /*
			case MD_DATA_ID_READ_RPORT_PARAM:                //report param
					loraMisc.send_buf[i++]=(buf[9]|0x80);
					loraMisc.send_buf[i++]=0;//Length
					loraMisc.send_buf[i++]=buf[11];//dataID
					loraMisc.send_buf[i++]=buf[12];
					
					loraMisc.send_buf[i++]=device_comps.report_param.min;
					loraMisc.send_buf[i++]=device_comps.report_param.hour;
					loraMisc.send_buf[i++]=device_comps.report_param.hour_Interval;
				    loraMisc.send_buf[i++]=device_comps.report_param.disFactor>>8;
				    loraMisc.send_buf[i++]=device_comps.report_param.disFactor;
					///ADD Other Data
					loraMisc.send_buf[10]=i-11;
					loraMisc.send_buf[i++]=Check_Sum(loraMisc.send_buf,i);
					loraMisc.send_buf[i++]=0x16;
					VerifyResult=0;
					break;		
			*/
			case MD_DATA_ID_READ_MEASURE_INFO:                //measure info
           case MD_DATA_ID_READ_HIGH_INFO:                //measure high_info
           case MD_DATA_ID_READ_4_20MA_INFO:                //measure high_info
					

                    if(!loraComps.sw._bit.reading_rssi)
                    {
                        int j=0;
                        loraComps.sw._bit.reading_rssi=1;
                         MD_LORA_INTP_DISABLE();
                        loraComps.work_st.mode=EM_CFG;
                        loraMisc.dataId=DataId;
                        
                        loraMisc.send_buf[j++]=0XAF;
                        loraMisc.send_buf[j++]=0XAF;
                        loraMisc.send_buf[j++]=0x00;
                        loraMisc.send_buf[j++]=0x00;
                        loraMisc.send_buf[j++]=0xAF;
                        loraMisc.send_buf[j++]=0x80; 
                        loraMisc.send_buf[j++]=0x06;
                        loraMisc.send_buf[j++]=0x02;
                        loraMisc.send_buf[j++]=0x00;
                        loraMisc.send_buf[j++]=0x00;
                        loraMisc.send_buf[j++]=0x95;
                        loraMisc.send_buf[j++]=0x0D;
                        loraMisc.send_buf[j++]=0x0A;
                        loraComps.write(loraMisc.send_buf,j);
                        return 3+buf[1];
                        
                    }
                    else if(loraComps.sw._bit.read_rssi_ok)
                    {
//                        loraComps.sw._bit.reading_rssi=0;
//                        loraComps.sw._bit.read_rssi_ok=0;
//                        loraComps.sw._bit.refresh_rssi=1;
//                        i+=add_measure_data(Cmd,DataId,&loraMisc.send_buf[i],loraMisc.yl701_info.rssi);
//                       
//                        loraComps.work_st.mode=EM_RUN;
//                        loraComps.write(loraMisc.send_buf,i);
//                        return 3+buf[1];
                    }
                    else if(loraComps.op_window_time<0)
                    {
//                        loraComps.sw._bit.reading_rssi=0;
//                        loraComps.sw._bit.read_rssi_ok=0;
//                        
//                        i+=add_measure_data(Cmd,DataId,&loraMisc.send_buf[i],0);
//                       
//                        loraComps.work_st.mode=EM_RUN;
//                        loraComps.write(loraMisc.send_buf,i);
//                        return 3+buf[1];
                    }
                   return 0;
					 
					
					break;
			/*	
			case MD_DATA_ID_READ_ACCESS_ADDR:                //read access addr
					loraMisc.send_buf[i++]=(buf[9]|0x80);
					loraMisc.send_buf[i++]=0;//Length
					loraMisc.send_buf[i++]=buf[11];//dataID
					loraMisc.send_buf[i++]=buf[12];
					memcpy(&loraMisc.send_buf[i],device_comps.access_param.ip,4);//New IP
					i+=4;
					loraMisc.send_buf[i++]=device_comps.access_param.port>>8;
					loraMisc.send_buf[i++]=device_comps.access_param.port;
					///ADD Other Data
					loraMisc.send_buf[10]=i-11;
					loraMisc.send_buf[i++]=Check_Sum(loraMisc.send_buf,i);
					loraMisc.send_buf[i++]=0x16;
					VerifyResult=0;
					break;
			*/
			default:       
					return 1;
			
		}
		loraComps.write(loraMisc.send_buf,i);
		return 3+buf[1];
		
	}
	else if(Cmd==1)//Write
	{
       if(hum_comps.current_mode==EM_CAL_MODIFY_MODE)
        {
            return 1;
        }
        switch(DataId)
	    {
	        /*
		    case MD_DATA_ID_ENTER_CAL_MODE:                //enter calmode
				
                if(device_comps.sw._bit.e2prom_driver_err)
                {
                    loraMisc.send_buf[i++]=(buf[9]|0x90);    
                }
                else 
                {
				    loraMisc.send_buf[i++]=(buf[9]|0x80);
				    enter_cal_modify_mode();
				   
				    cal_param_dot_bak=buf[13];
				    cal_param_unit_bak=buf[14];
				}
				
				loraMisc.send_buf[i++]=0;
				loraMisc.send_buf[i++]=buf[11];
				loraMisc.send_buf[i++]=buf[12];
				

				///ADD Other Data
				loraMisc.send_buf[10]=i-11;
				loraMisc.send_buf[i++]=Check_Sum(loraMisc.send_buf,i);
				loraMisc.send_buf[i++]=0x16;
				break;	
				
				
			case MD_DATA_ID_ENTER_NORMAL_MODE:                //exit cal mode
				
                loraMisc.send_buf[i++]=(buf[9]|0x80);
                enter_normal_mode();
				
			    loraMisc.send_buf[i++]=0;
				loraMisc.send_buf[i++]=buf[11];
				loraMisc.send_buf[i++]=buf[12];

				///ADD Other Data
				loraMisc.send_buf[10]=i-11;
				loraMisc.send_buf[i++]=Check_Sum(loraMisc.send_buf,i);
				loraMisc.send_buf[i++]=0x16;
				break;
						
				
			case MD_DATA_ID_SET_DEVICE_ADDR:                //write device addr
    			if(device_comps.sw._bit.e2prom_driver_err)
    			{
    				loraMisc.send_buf[i++]=(buf[9]|0x90);    
    			}
    			else 
    			{
    				device_addr_t addr_cpy;
    				memcpy(addr_cpy.addr,&buf[13],7);
    				addr_cpy.cs=Check_Sum_5A(&addr_cpy, &addr_cpy.cs-(unsigned char *)&addr_cpy);
    				if(device_comps.save_device_addr(&addr_cpy.addr,sizeof(addr_cpy)))
    				{
    				    loraMisc.send_buf[i++]=(buf[9]|0x90);    
    				}
    				else
    				{
    					memcpy(&device_comps.device_addr,&addr_cpy,sizeof(addr_cpy));
    					loraMisc.send_buf[i++]=(buf[9]|0x80);
						mode_comps[hum_comps.current_mode].dis_option=6;
                        hum_comps.dis_oper_mark._bit.refresh_option=1;
						hum_comps.dis_oper_mark._bit.refresh_device_id_low=1;
				    }
    			}
    			memcpy(loraMisc.send_buf+1,device_comps.device_addr.addr,7);
    		    loraMisc.send_buf[i++]=0;
    			loraMisc.send_buf[i++]=buf[11];
    			loraMisc.send_buf[i++]=buf[12];

    			///ADD Other Data
    			loraMisc.send_buf[10]=i-11;
    			loraMisc.send_buf[i++]=Check_Sum(loraMisc.send_buf,i);
    			loraMisc.send_buf[i++]=0x16;
			    break;
	      */
			case MD_DATA_ID_WRITE_ALARM_PARAM:                //write alarm Param
			    loraMisc.send_buf[i++]=loraMisc.yl701_info.NodeId>>8;
                loraMisc.send_buf[i++]=loraMisc.yl701_info.NodeId;
			    if(device_comps.sw._bit.e2prom_driver_err)
    			{
    				loraMisc.send_buf[i++]=(Cmd|0x90);    
    			}
    			else 
    			{
    				alarm_param_t alarm_param_cpy;
    				//memcpy(&alarm_param_cpy.press_high_upper,&buf[13],4*4);
    				alarm_param_cpy.press_high_upper=((unsigned long)buf[4]<<24)+((unsigned long)buf[5]<<16)+((unsigned long)buf[6]<<8)+buf[7];
    				alarm_param_cpy.press_high_lower=((unsigned long)buf[8]<<24)+((unsigned long)buf[9]<<16)+((unsigned long)buf[10]<<8)+buf[11];
    				alarm_param_cpy.press_low_upper =((unsigned long)buf[12]<<24)+((unsigned long)buf[13]<<16)+((unsigned long)buf[14]<<8)+buf[15];
    				alarm_param_cpy.press_low_lower =((unsigned long)buf[16]<<24)+((unsigned long)buf[17]<<16)+((unsigned long)buf[18]<<8)+buf[19];
    				alarm_param_cpy.unit=buf[20];
    				//memcpy(&alarm_param_cpy.temp_high,&buf[13+4*4+1],2*2);
    				alarm_param_cpy.temp_high =((unsigned int)buf[21]<<8)+buf[22];
    				alarm_param_cpy.temp_low =((unsigned int)buf[23]<<8)+buf[24];
    				alarm_param_cpy.cs=Check_Sum_5A(&alarm_param_cpy, &alarm_param_cpy.cs-(unsigned char *)&alarm_param_cpy);
    				if(device_comps.save_alarm_param(&alarm_param_cpy,sizeof(alarm_param_cpy)))
    				{
    				    loraMisc.send_buf[i++]=(Cmd|0x90);    
    				}
    				else
    				{
    					memcpy(&device_comps.alarm_param,&alarm_param_cpy,sizeof(alarm_param_cpy));
    					loraMisc.send_buf[i++]=(Cmd|0x80);
    				}
    			}	
    		    loraMisc.send_buf[i++]=0;
                loraMisc.send_buf[i++]=DataId>>8;//dataID
                loraMisc.send_buf[i++]=DataId;

    			///ADD Other Data
    			loraMisc.send_buf[3]=i-4;
    			loraMisc.send_buf[i++]=Check_Sum(loraMisc.send_buf,i);
    		    break;
    		 case MD_DATA_ID_WRITE_DEVICE_TIME:
    		    loraMisc.send_buf[i++]=loraMisc.yl701_info.NodeId>>8;
                loraMisc.send_buf[i++]=loraMisc.yl701_info.NodeId;
    		    if(device_comps.sw._bit.e2prom_driver_err)
    			{
    				loraMisc.send_buf[i++]=(Cmd|0x90);    
    			}
    			else 
    			{
                    /*
    				alarm_param_t alarm_param_cpy;
    				//memcpy(&alarm_param_cpy.press_high_upper,&buf[13],4*4);
    				alarm_param_cpy.press_high_upper=((unsigned long)buf[13]<<24)+((unsigned long)buf[14]<<16)+((unsigned long)buf[15]<<8)+buf[16];
    				alarm_param_cpy.press_high_lower=((unsigned long)buf[17]<<24)+((unsigned long)buf[18]<<16)+((unsigned long)buf[19]<<8)+buf[20];
    				alarm_param_cpy.press_low_upper =((unsigned long)buf[21]<<24)+((unsigned long)buf[22]<<16)+((unsigned long)buf[23]<<8)+buf[24];
    				alarm_param_cpy.press_low_lower =((unsigned long)buf[25]<<24)+((unsigned long)buf[26]<<16)+((unsigned long)buf[27]<<8)+buf[28];
    				alarm_param_cpy.unit=buf[29];
    				//memcpy(&alarm_param_cpy.temp_high,&buf[13+4*4+1],2*2);
    				alarm_param_cpy.temp_high =((unsigned int)buf[30]<<8)+buf[31];
    				alarm_param_cpy.temp_low =((unsigned int)buf[32]<<8)+buf[33];
    				alarm_param_cpy.cs=Check_Sum_5A(&alarm_param_cpy, &alarm_param_cpy.cs-(unsigned char *)&alarm_param_cpy);
    				
    				if(device_comps.save_alarm_param(&alarm_param_cpy,sizeof(alarm_param_cpy)))
    				{
    				    loraMisc.send_buf[i++]=(buf[1]|0x90);    
    				}
    				else
    				{
    					memcpy(&device_comps.alarm_param,&alarm_param_cpy,sizeof(alarm_param_cpy));
    					loraMisc.send_buf[i++]=(buf[1]|0x80);
    				}

                     */
                        system_time_t system_time_cpy;
                        system_time_cpy.year=rtc_valve.year=buf[4];
                        system_time_cpy.month=rtc_valve.month=buf[5];
                        system_time_cpy.day=rtc_valve.day=buf[6];
                        system_time_cpy.hour=rtc_valve.hour=buf[7];
                        system_time_cpy.min=rtc_valve.min=buf[8];
                        system_time_cpy.sec=rtc_valve.sec=buf[9];
                       system_time_cpy.cs==Check_Sum_5A(&system_time_cpy, &system_time_cpy.cs-(unsigned char *)&system_time_cpy);
                        R_RTC_Set_CounterValue(rtc_valve);	//ÉèÖÃÊ±¼ä//
                        if(device_comps.save_system_time(&system_time_cpy,sizeof(system_time_cpy)))
        				{
        				    loraMisc.send_buf[i++]=(Cmd|0x90);    
        				}
        				else
        				{
        					memcpy(&device_comps.system_time,&system_time_cpy,sizeof(system_time_cpy));
        					loraMisc.send_buf[i++]=(Cmd|0x80);
    				    }



    				
    			}	
    		    loraMisc.send_buf[i++]=0;
                loraMisc.send_buf[i++]=DataId>>8;//dataID
                loraMisc.send_buf[i++]=DataId;

    			///ADD Other Data
    			loraMisc.send_buf[3]=i-4;
    			loraMisc.send_buf[i++]=Check_Sum(loraMisc.send_buf,i);
    		    break;
    	/*
			case MD_DATA_ID_SET_ACCESS_ADDR :                //write access addr
			
				if(device_comps.sw._bit.e2prom_driver_err)
    			{
    				loraMisc.send_buf[i++]=(buf[9]|0x90);    
    			}
    			else 
    			{
					access_param_t access_param_cpy;
    				memcpy(&access_param_cpy.ip[0],&buf[13],4);
    				access_param_cpy.port=((unsigned int)buf[13+4]<<8)+buf[13+5];
    				access_param_cpy.cs=Check_Sum_5A(&access_param_cpy, &access_param_cpy.cs-(unsigned char *)&access_param_cpy);
    				if(device_comps.save_access_param(&access_param_cpy,sizeof(access_param_cpy)))
    				{
    				    loraMisc.send_buf[i++]=(buf[9]|0x90);    
    				}
    				else
    				{
    					memcpy(&device_comps.access_param,&access_param_cpy,sizeof(access_param_cpy));
    					loraMisc.send_buf[i++]=(buf[9]|0x80);
    				}
				}
			
				loraMisc.send_buf[i++]=0;
				loraMisc.send_buf[i++]=buf[11];
				loraMisc.send_buf[i++]=buf[12];

				///ADD Other Data
				loraMisc.send_buf[10]=i-11;
				loraMisc.send_buf[i++]=Check_Sum(loraMisc.send_buf,i);
				loraMisc.send_buf[i++]=0x16;
				break;
				
			case MD_DATA_ID_WRITE_REPORT_PARAM:  //report param
				if(device_comps.sw._bit.e2prom_driver_err)
    			{
    				loraMisc.send_buf[i++]=(buf[9]|0x90);    
    			}
    			else 
    			{
					report_param_t report_param_cpy;
					report_param_cpy.min=buf[13];
					report_param_cpy.hour=buf[14];
					report_param_cpy.hour_Interval=buf[15];
					report_param_cpy.disFactor=((unsigned int)buf[16]<<8)+buf[17];
					report_param_cpy.cs=Check_Sum_5A(&report_param_cpy, &report_param_cpy.cs-(unsigned char *)&report_param_cpy);
    				if(device_comps.save_report_param(&report_param_cpy,sizeof(report_param_cpy)))
    				{
    				    loraMisc.send_buf[i++]=(buf[9]|0x90);    
    				}
    				else
    				{
                        device_comps.report_interval_timer=0;
    					memcpy(&device_comps.report_param,&report_param_cpy,sizeof(report_param_cpy));
    					loraMisc.send_buf[i++]=(buf[9]|0x80);
    				}
				}
			
				loraMisc.send_buf[i++]=0;
				loraMisc.send_buf[i++]=buf[11];
				loraMisc.send_buf[i++]=buf[12];

				///ADD Other Data
				loraMisc.send_buf[10]=i-11;
				loraMisc.send_buf[i++]=Check_Sum(loraMisc.send_buf,i);
				loraMisc.send_buf[i++]=0x16;
				break;
				
			case MD_DATA_ID_WRITE_CAL_DATA:       //write cal data       
                if((device_comps.sw._bit.e2prom_driver_err)||(hum_comps.current_mode!=EM_CAL_MODIFY_MODE)
                  ||(mode_comps[hum_comps.current_mode].dis_option!=buf[13]))
    			{
    				loraMisc.send_buf[i++]=(buf[9]|0x90);    
    			}
    			else 
    			{
                    long param=((unsigned long)buf[14]<<24)+((unsigned long)buf[15]<<16)
                                +((unsigned long)buf[16]<<8)+buf[17];
                    loraMisc.send_buf[i++]=(buf[9]|0x80);           
                    switch(buf[13])
                    {
                        case 0:
                                    device_comps.calibration_param_bak.x[0]=device_comps.ad1_ad2_average_result;
                                    device_comps.calibration_param_bak.y[0]=param;
                                    mode_comps[hum_comps.current_mode].dis_option++;
                                    hum_comps.dis_oper_mark._bit.refresh_option=1;
                                    hum_comps.dis_oper_mark._bit.refresh_cal_param=1;
                                    break;
                     
                        case 1:
                                    device_comps.calibration_param_bak.x[1]=device_comps.ad1_ad2_average_result;
                                    device_comps.calibration_param_bak.y[1]=param;
                                    mode_comps[hum_comps.current_mode].dis_option++;
                                    hum_comps.dis_oper_mark._bit.refresh_option=1;
                                    hum_comps.dis_oper_mark._bit.refresh_cal_param=1;
                                    break;
                        case 2:
                                    device_comps.calibration_param_bak.x[2]=device_comps.ad1_ad2_average_result;
                                    device_comps.calibration_param_bak.y[2]=param;
                                    mode_comps[hum_comps.current_mode].dis_option++;
                                    hum_comps.dis_oper_mark._bit.refresh_option=1;
                                    hum_comps.dis_oper_mark._bit.refresh_cal_param=1;
                                    break;
                        case 3:     
                                    device_comps.calibration_param_bak.x[3]=device_comps.ad1_ad2_average_result;
                                    device_comps.calibration_param_bak.y[3]=param;
                                    device_comps.calibration_param_bak.is_calibrated=1;
                                    device_comps.calibration_param_bak.dot=cal_param_dot_bak;;
                                    device_comps.calibration_param_bak.unit=cal_param_unit_bak;
                                    device_comps.calibration_param_bak.cs=Check_Sum_5A(&device_comps.calibration_param_bak, & device_comps.calibration_param_bak.cs-£¨unsigned char *)&device_comps.calibration_param_bak);
                                    if(device_comps.save_calibration_param(&device_comps.calibration_param_bak,sizeof(device_comps.calibration_param_bak)))
                    				{
                    				    loraMisc.send_buf[i]=(buf[9]|0x90);
                                        mode_comps[hum_comps.current_mode].dis_option=0;
                                        hum_comps.dis_oper_mark._bit.refresh_option=1;
                                        hum_comps.dis_oper_mark._bit.refresh_cal_param=1;
                                        device_comps.calibration_param_bak.is_calibrated=0;
                    				}
                    				else
                    				{
                    					memcpy(&device_comps.calibration_param,&device_comps.calibration_param_bak,sizeof(device_comps.calibration_param_bak));
                    					loraMisc.send_buf[i]=(buf[9]|0x80);
                    					
                    					enter_normal_mode();
                    				}
                                    
                                    break;
                         
                        default:   return 1;
                        
                     }
                }
				
				loraMisc.send_buf[i++]=0;
				loraMisc.send_buf[i++]=buf[11];
				loraMisc.send_buf[i++]=buf[12];
				loraMisc.send_buf[i++]=buf[13];

				///ADD Other Data
				loraMisc.send_buf[10]=i-11;
				loraMisc.send_buf[i++]=Check_Sum(loraMisc.send_buf,i);
				loraMisc.send_buf[i++]=0x16;
				break;
				 */
			default:       
					return 1;	
		}
		loraComps.write(loraMisc.send_buf,i);
		return 3+buf[1];
		
	}
	else 
	{
		return 1;
	}
	return 1;
	
}

static unsigned char pro_lora_config(unsigned char cmd,unsigned char *buf,int len)
{
    int i=0;	
    switch(cmd)
    {
        case 0x01://write all param

             loraComps.sw._bit.init_ok=1;
             
        case 0x02://read_all param
            if(!loraComps.do_init)
            {
                loraComps.work_st.mode=EM_RUN;
                loraComps.op_window_time=0;
                enter_cad_mode();
                memcpy(modbusComps.send_cfg_base_pt,buf,len);
                modbusComps.write(modbusComps.send_cfg_base_pt,len);
             }
            return len;
             
        case 0x06://read rssi
             loraMisc.yl701_info.rssi=buf[8];
             //loraComps.sw._bit.read_rssi_ok=1;
	     loraComps.sw._bit.reading_rssi=0;
	     //loraComps.sw._bit.read_rssi_ok=0;
	     loraComps.sw._bit.refresh_rssi=1;
	     
	     i+=add_measure_data(0x80,loraMisc.dataId,&loraMisc.send_buf[i],loraMisc.yl701_info.rssi);
	     loraComps.work_st.mode=EM_RUN;
	     loraComps.write(loraMisc.send_buf,i);
	     return len;
        default:
              return 1;

     }
     return 1;
 }
static unsigned char Check_lora_Com(unsigned char *Rec_Data,unsigned char Rec_Pos)
{
	unsigned char i=0;
	if(loraComps.sw._bit.busy)
	{
	    return 0;
	}
	if(Rec_Pos<5)
	{
		return 0;
	}
	if(loraComps.work_st.mode==EM_CFG)
	{
    	if((Rec_Data[0]==0xaf)&&(Rec_Data[1]==0xaf)&&(Rec_Data[2]==0)&&
    	(Rec_Data[3]==0)&&(Rec_Data[4]==0xaf))
    	{
    	        if(Rec_Pos<8)
    	        {
    	            return 0;
    	        }
    	        if(Rec_Data[7]>0x0f)
    	        {
    	            return 1;
    	        }
    	        if(Rec_Pos<8+Rec_Data[7]+1+2)
    	        {
    	            return 0;
    	        }
    	        if((Rec_Data[8+Rec_Data[7]+1+2-2]!=0x0d)||(Rec_Data[8+Rec_Data[7]+1+2-1]!=0x0a))
    	        {
    	            return 1;
    	        }
    	        if(Rec_Data[8+Rec_Data[7]+1+2-3]!=Check_Sum(Rec_Data,8+Rec_Data[7]))
    	        {
    	            return 1;
    	        }
    	        return pro_lora_config(Rec_Data[6],Rec_Data,8+Rec_Data[7]+1+2);
    	}
    	
	}
	else if (loraComps.work_st.mode==EM_RUN)
	{
    	if((Rec_Data[0]==0x00)||(Rec_Data[0]==0x01)&&(Rec_Data[2]==0x90))
    	{
        
    	    	if((Rec_Data[3]!=0x21)&&(Rec_Data[3]!=0x26)&&(Rec_Data[3]!=0x27)&&(Rec_Data[3]!=0x1f)&&(Rec_Data[3]!=0x01)&&(Rec_Data[3]!=0x04))
    	    	{
    	    	    return 1;
    	    	}
    	    	if(Rec_Data[1]>0x20)
    	    	{
    	            return 1;
    	        }
    	    	if(Rec_Pos<Rec_Data[1]+3)
    	    	{
    	    		return 0;
    	    	}
    	    	
    	    	if(Rec_Data[Rec_Data[1]+3-1]!=Check_Sum_whih_NodeId(loraMisc.yl701_info.NodeId,Rec_Data,Rec_Data[1]+3-1))
    	    	{
    	    		return 1;
    	    	}
    	    	return Pro_lora(Rec_Data[0],Rec_Data);
        }
    }
   	else
    	{
        	return 1;
    	}
	return 1;
}

static void Deal_lora(void)
{
	unsigned char err=0;
 	do
	{
	    err=Check_lora_Com(loraMisc.recv_buf,loraMisc.rec_pos); 
		if(err>0)
		{
		    DI();
			memcpy(loraMisc.recv_buf,loraMisc.recv_buf+err,loraMisc.rec_pos-err);
			loraMisc.rec_pos-=err;
			EI();
    	}
	}
	while (err>0);
}

static unsigned char init_lora(loraComps_t *const this)
{
    int i=0;
    for(i=0;i<2;i++)
    {
        loraMisc.send_buf[i]=0xaf;
    }
    loraMisc.send_buf[i++]=0;
    loraMisc.send_buf[i++]=0;
    loraMisc.send_buf[i++]=0xaf;
    loraMisc.send_buf[i++]=0x80;
    loraMisc.send_buf[i++]=0x01;
    loraMisc.send_buf[i++]=0x0e;
    loraMisc.send_buf[i++]=loraMisc.yl701_info.rate;//9600
    loraMisc.send_buf[i++]=loraMisc.yl701_info.verify;//no even
    loraMisc.send_buf[i++]=loraMisc.yl701_info.freq>>16;
    loraMisc.send_buf[i++]=loraMisc.yl701_info.freq>>8;
    loraMisc.send_buf[i++]=loraMisc.yl701_info.freq;
    loraMisc.send_buf[i++]=loraMisc.yl701_info.sf;
    loraMisc.send_buf[i++]=loraMisc.yl701_info.workMode;
    loraMisc.send_buf[i++]=loraMisc.yl701_info.bw;
    loraMisc.send_buf[i++]=loraMisc.yl701_info.NodeId>>8;
    loraMisc.send_buf[i++]=loraMisc.yl701_info.NodeId;
    loraMisc.send_buf[i++]=loraMisc.yl701_info.netId;
    loraMisc.send_buf[i++]=loraMisc.yl701_info.power;
    loraMisc.send_buf[i++]=loraMisc.yl701_info.breathPeriod;
    loraMisc.send_buf[i++]=loraMisc.yl701_info.breathTime;
    loraMisc.send_buf[i++]=Check_Sum(loraMisc.send_buf, i);
    loraMisc.send_buf[i++]=0x0d;
    loraMisc.send_buf[i++]=0x0a;
    loraComps.write(loraMisc.send_buf,i);
    return 1;
}


 void enter_cad_mode(void)
{
    disable_lora();
    loraComps.sw._bit.runing=0;
    MD_LORA_EN_PIN_SET;
    MD_LORA_SET_PIN_SET;
    MD_LORA_INTP_ENABLE();
    loraComps.work_st.dir=EM_RECV;
}
static void  loraComps_task_handle(void)
{
    static int init_once=1;
    if(init_once)
    {
        MD_LORA_EN_PIN_SET;       
        MD_LORA_SET_PIN_SET;
        MD_LORA_INTP_DISABLE();
        loraComps.work_st.mode=EM_CFG;
        loraComps.read_yl701_info(&loraMisc.yl701_info,sizeof(loraMisc.yl701_info));
        if(loraMisc.yl701_info.cs!=Check_Sum_5A(&loraMisc.yl701_info, & loraMisc.yl701_info.cs-(unsigned char *)&loraMisc.yl701_info))
        {
            loraComps.sw._bit.noParameter=1;
            loraMisc.yl701_info.rate=4;//9.6k
            loraMisc.yl701_info.verify=0;//no 
            loraMisc.yl701_info.freq=((unsigned long)0x6c<<16)+((unsigned long)0x80<<8)+0x12;//434
            loraMisc.yl701_info.sf=0x0b;//2048
            loraMisc.yl701_info.workMode=2;//node mode
            loraMisc.yl701_info.bw=7;//125k
            loraMisc.yl701_info.NodeId=1;//nodeId
		loraMisc.yl701_info.netId=1;//netID 
		loraMisc.yl701_info.power=7;//buf[24];
		loraMisc.yl701_info.breathPeriod=3;//8s 
		loraMisc.yl701_info.breathTime=2;//8ms

        }
        init_once=0;
    }

   if(loraComps.do_init)
   {
        
            if(!loraComps.sw._bit.initing)
            {
                loraComps.sw._bit.initing=1;
                loraComps.op_window_time=10;
                loraComps.init(loraComps.this);
            }
            else if(!loraComps.op_window_time)
            {
                loraComps.sw._bit.initing=0;
                loraComps.re_try_times--;
                if(!loraComps.re_try_times)
                {
                    loraComps.do_init=0;
                    loraComps.sw._bit.initing=0;
                    loraComps.work_st.mode=EM_RUN;
                    enter_cad_mode();
                    
                }
            }
            else if(loraComps.sw._bit.init_ok)
            {
                loraComps.do_init=0;
                loraComps.sw._bit.initing=0;
                loraComps.work_st.mode=EM_RUN;
                enter_cad_mode();
              
            }
         
        
   }
  if(loraComps.sw._bit.runing)
   {
       Deal_lora();
   }
}



static void lora_comps_task_50ms(void)
{
    if(loraComps.op_window_time>0)
    {
     
        loraComps.op_window_time--;
        if(!loraComps.op_window_time)
        {
            if(!loraComps.do_init) 
            {
                loraComps.work_st.mode=EM_RUN;
                loraComps.sw._bit.reading_rssi=0; 
                loraComps.sw._bit.busy=0; 
                enter_cad_mode();
                NOP();
            }
        }
    }
    
    if(loraComps.sw._bit.sendReq)
    {
        if(loraComps.count>0)
        {
            loraComps.count--;
//            if(loraComps.count==1)
//            {
//                MD_LORA_EN_PIN_RESET;
//	            MD_LORA_SET_PIN_RESET;
//            }
            if(!loraComps.count)
            {
                 enable_lora();
                 loraComps.sw._bit.runing=1;
                 R_UART2_Send(loraMisc.send_addr,loraMisc.send_len);
                 loraComps.sw._bit.sendReq=0;
                 loraComps.work_st.dir=EM_SEND;
            }
        }  
    }
}
loraComps_t loraComps=
{
	"",       //char *desc;  
	&loraComps,//struct _LORA_COMPONENTS  *const this;
	0,//int   do_init  Whether to initialize,1:init 0,no init
	init_lora,      //unsigned char (*const init)(struct _LORA_COMPONENTS *);
	0,              //unsigned long count;


	loraMisc.recv_buf,
	loraMisc.send_buf,
	&loraMisc.rec_pos,
	30,

	3,
	&loraMisc.yl701_info,
	save_yl701_info,
	read_yl701_info,

	{EM_CFG,EM_SEND},//work_st

	{0},
	write_lora,
	loraComps_task_handle,
	lora_comps_task_50ms//void (*const task_send)(void);
};


