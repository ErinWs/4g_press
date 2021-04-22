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
    

static struct 
{ 
    unsigned char send_buf[128];
	unsigned char recv_buf[128];
	unsigned char rec_pos;
  
}
ircMisc=
{
	{0},
	{0},
	 0
    
};





//////////////////////////DealNfc

//unsigned char Leftloop(unsigned char len)
//{
//	if(len&0x80)
//	{
//		len = len * 2 + 1;
//	}
//	else
//	{
//		len = len * 2;
//	}
//	return len;
//}
//void Do_Imm(unsigned char *data)
//{
//	char i,j,k;
//	char n1;
//	char code[8]={'C','E','L','E','G','A','L','F'};
//	char cdata[8];
//	k = 0x5a;
//	for(i=0;i<8;i++)
//	{
//		cdata[i] = ~(*data);
//		*data++; 
//	}
//	for(i=0;i<8;i++)
//	{
//		for(j=0;j<8;j++)
//		{
//			n1 = cdata[j];
//			n1 = n1 - code[j];
//			k = k^n1;
//			k = Leftloop(k);
//		}
//		cdata[i]=k;
//		k ++;
//	}
//	for(i=0;i<8;i++)
//	{
//		*data--;
//		*data = ~cdata[7-i];
//	}
//}

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

unsigned char Check_Sum_5A(unsigned char const *Data,unsigned char Len)
{
    unsigned char Sum=0x5A;
    unsigned char i=0;
    for(i=0;i<Len;i++)
    {
        Sum+=Data[i];
    }
    return Sum;
}

long formatData4fixDot(long temp,int dot)
{
    if(dot==6)
    {
        temp/=100;
    }
    else if(dot==5)
    {
        temp/=10;
    }
    else if(dot==4)
    {
        temp/=1;
    }
    else if(dot==3)
    {
        temp*=10;
    }
    else if(dot==2)
    {
        temp*=100;
    }
    else if(dot==1)
    {
        temp*=1000;
    }
    else if(dot==0)
    {
        temp*=10000;
    }
    return temp;
}




#define  VERIFYEN                                   0
#define  MD_DATA_ID_READ_BASIC_INFO                 0x9020
#define  MD_DATA_ID_READ_ALARM_PARAM                0x9021
#define  MD_DATA_ID_READ_RPORT_PARAM                0x9022
#define  MD_DATA_ID_READ_MEASURE_INFO               0x901f
#define  MD_DATA_ID_READ_ACCESS_ADDR                0x9023
#define  MD_DATA_ID_READ_LORA_PARAM                 0x9024
#define  MD_DATA_ID_READ_HIGH_INFO                  0x9026
#define  MD_DATA_ID_READ_GPS_LOC_INFO               0x9028
#define  MD_DATA_ID_READ_MANUFACTURER_INFO          0x9029
#define  MD_DATA_ID_READ_DEVICE_TYPE_INFO           0x902a
#define  MD_DATA_ID_READ_DEVICE_ID_INFO             0x902b
#define  MD_DATA_ID_READ_SENSOR_ID_INFO             0x902c





#define  MD_DATA_ID_SET_DEVICE_ADDR             0x9018
#define  MD_DATA_ID_SET_ACCESS_ADDR             0x9005
#define  MD_DATA_ID_ENTER_CAL_MODE              0xfa55//
#define  MD_DATA_ID_ENTER_NORMAL_MODE           0xfa99//
#define  MD_DATA_ID_WRITE_ALARM_PARAM           0x9001//
#define  MD_DATA_ID_WRITE_CAL_DATA              0x9002//
#define  MD_DATA_ID_WRITE_REPORT_PARAM          0x9003//
#define  MD_DATA_ID_WRITE_DEVICE_TIME           0x9004
#define  MD_DATA_ID_WRITE_LORA_PARAM            0x9004
#define  MD_DATA_ID_WRITE_GPS_LOC_INFO          0x9007
#define  MD_DATA_ID_WRITE_MANUFACTURER_INFO     0x9008
#define  MD_DATA_ID_WRITE_DEVICE_TYPE_INFO      0x9009
#define  MD_DATA_ID_WRITE_DEVICE_ID_INFO        0x900a
#define  MD_DATA_ID_WRITE_SENSOR_INFO           0x900b






static unsigned char Pro_irc(unsigned char Cmd,unsigned char *buf)
{
	unsigned char i=9;
	unsigned char k=0;
	unsigned long temp=0;
	unsigned char VerifyResult;
	
	unsigned int DataId=((unsigned int)buf[11]<<8)+buf[12];
	if(Cmd==0)//Read
	{
		memcpy(ircMisc.send_buf,buf,9);
		memcpy(ircMisc.send_buf+1,device_comps.device_addr.addr,7);
		switch(DataId)
		{
			case MD_DATA_ID_READ_BASIC_INFO:                //BasicInf
					ircMisc.send_buf[i++]=(buf[9]|0x80);
					ircMisc.send_buf[i++]=0;//Length
					ircMisc.send_buf[i++]=buf[11];//dataID
					ircMisc.send_buf[i++]=buf[12];
					memcpy(&ircMisc.send_buf[i],netComps.net_info.imei,16);//ADD IMEI
					i+=16;
					memcpy(&ircMisc.send_buf[i],netComps.net_info.imsi,16);//ADD IMSI
					i+=16;
					ircMisc.send_buf[i++]=MD_FL_VER;//device hwVer
					ircMisc.send_buf[i++]=MD_FL_VER;//device swVer
					ircMisc.send_buf[i++]=MD_FL_VER;//protocol swVer
				
					memcpy(&ircMisc.send_buf[i],netComps.net_info.firmVer,20);//ADD FireWave ver
					i+=20;
					memcpy(&ircMisc.send_buf[i],netComps.net_info.iccid,20);//ADD iccid
					i+=20;
					memcpy(&ircMisc.send_buf[i],device_comps.access_param.ip,4);//New IP
					i+=4;
					ircMisc.send_buf[i++]=device_comps.access_param.port>>8;
					ircMisc.send_buf[i++]=device_comps.access_param.port;
					
					///ADD Other Data
					ircMisc.send_buf[10]=i-11;
					ircMisc.send_buf[i++]=Check_Sum(ircMisc.send_buf,i);
					ircMisc.send_buf[i++]=0x16;
					VerifyResult=0;
					break;
			case MD_DATA_ID_READ_ALARM_PARAM:                //alram param
					ircMisc.send_buf[i++]=(buf[9]|0x80);
					ircMisc.send_buf[i++]=0;//Length
					ircMisc.send_buf[i++]=buf[11];//dataID
					ircMisc.send_buf[i++]=buf[12];
					
					ircMisc.send_buf[i++]=device_comps.alarm_param.press_high_upper>>24;
					ircMisc.send_buf[i++]=device_comps.alarm_param.press_high_upper>>16;
					ircMisc.send_buf[i++]=device_comps.alarm_param.press_high_upper>>8;
					ircMisc.send_buf[i++]=device_comps.alarm_param.press_high_upper;
					
					ircMisc.send_buf[i++]=device_comps.alarm_param.press_high_lower>>24;
					ircMisc.send_buf[i++]=device_comps.alarm_param.press_high_lower>>16;
					ircMisc.send_buf[i++]=device_comps.alarm_param.press_high_lower>>8;
					ircMisc.send_buf[i++]=device_comps.alarm_param.press_high_lower;

					ircMisc.send_buf[i++]=device_comps.alarm_param.press_low_upper>>24;
					ircMisc.send_buf[i++]=device_comps.alarm_param.press_low_upper>>16;
					ircMisc.send_buf[i++]=device_comps.alarm_param.press_low_upper>>8;
					ircMisc.send_buf[i++]=device_comps.alarm_param.press_low_upper;

					ircMisc.send_buf[i++]=device_comps.alarm_param.press_low_lower>>24;
					ircMisc.send_buf[i++]=device_comps.alarm_param.press_low_lower>>16;
					ircMisc.send_buf[i++]=device_comps.alarm_param.press_low_lower>>8;
					ircMisc.send_buf[i++]=device_comps.alarm_param.press_low_lower;
					
					ircMisc.send_buf[i++]=device_comps.alarm_param.unit;

					ircMisc.send_buf[i++]=device_comps.alarm_param.temp_high>>8;
					ircMisc.send_buf[i++]=device_comps.alarm_param.temp_high;
					ircMisc.send_buf[i++]=device_comps.alarm_param.temp_low>>8;
					ircMisc.send_buf[i++]=device_comps.alarm_param.temp_low;
					//memcpy(&ircMisc.send_buf[i],&device_comps.alarm_param.press_high_upper,
					//i+=&device_comps.alarm_param.temp_low-&device_comps.alarm_param.press_high_upper+sizeof(device_comps.alarm_param.temp_low));//New IP
				
					///ADD Other Data
					ircMisc.send_buf[10]=i-11;
					ircMisc.send_buf[i++]=Check_Sum(ircMisc.send_buf,i);
					ircMisc.send_buf[i++]=0x16;
					VerifyResult=0;
					break;

			case MD_DATA_ID_READ_RPORT_PARAM:                //report param
					ircMisc.send_buf[i++]=(buf[9]|0x80);
					ircMisc.send_buf[i++]=0;//Length
					ircMisc.send_buf[i++]=buf[11];//dataID
					ircMisc.send_buf[i++]=buf[12];
					
					ircMisc.send_buf[i++]=device_comps.report_param.min;
					ircMisc.send_buf[i++]=device_comps.report_param.hour;
					ircMisc.send_buf[i++]=device_comps.report_param.min_Interval>>8;
					ircMisc.send_buf[i++]=device_comps.report_param.min_Interval;
					//ircMisc.send_buf[i++]=device_comps.report_param.hour_Interval;
				    ircMisc.send_buf[i++]=device_comps.report_param.disFactor>>8;
				    ircMisc.send_buf[i++]=device_comps.report_param.disFactor;

                    temp=device_comps.report_param.triggerTimes;                                       
					ircMisc.send_buf[i++]=temp>>24;
					ircMisc.send_buf[i++]=temp>>16;
					ircMisc.send_buf[i++]=temp>>8;
					ircMisc.send_buf[i++]=temp;
					

				    
					///ADD Other Data
					ircMisc.send_buf[10]=i-11;
					ircMisc.send_buf[i++]=Check_Sum(ircMisc.send_buf,i);
					ircMisc.send_buf[i++]=0x16;
					VerifyResult=0;
					break;	
		     case MD_DATA_ID_READ_LORA_PARAM:                //lora param
					ircMisc.send_buf[i++]=(buf[9]|0x80);
					ircMisc.send_buf[i++]=0;//Length
					ircMisc.send_buf[i++]=buf[11];//dataID
					ircMisc.send_buf[i++]=buf[12];
					
                    ircMisc.send_buf[i++]=loraComps.yl701_info_p->rate;//9600
                    ircMisc.send_buf[i++]=loraComps.yl701_info_p->verify;//no even
                    
                    ircMisc.send_buf[i++]=loraComps.yl701_info_p->freq>>16;
                    ircMisc.send_buf[i++]=loraComps.yl701_info_p->freq>>8;
                    ircMisc.send_buf[i++]=loraComps.yl701_info_p->freq;
                    ircMisc.send_buf[i++]=loraComps.yl701_info_p->sf;
                    ircMisc.send_buf[i++]=loraComps.yl701_info_p->workMode;
                    ircMisc.send_buf[i++]=loraComps.yl701_info_p->bw;
                    ircMisc.send_buf[i++]=loraComps.yl701_info_p->NodeId>>8;
                    ircMisc.send_buf[i++]=loraComps.yl701_info_p->NodeId;
                    ircMisc.send_buf[i++]=loraComps.yl701_info_p->netId;
                    ircMisc.send_buf[i++]=loraComps.yl701_info_p->power;
                    ircMisc.send_buf[i++]=loraComps.yl701_info_p->breathPeriod;
                    ircMisc.send_buf[i++]=loraComps.yl701_info_p->breathTime;
					///ADD Other Data
					ircMisc.send_buf[10]=i-11;
					ircMisc.send_buf[i++]=Check_Sum(ircMisc.send_buf,i);
					ircMisc.send_buf[i++]=0x16;
					VerifyResult=0;
					break;			
			
			case MD_DATA_ID_READ_MEASURE_INFO:                //measure info
					ircMisc.send_buf[i++]=(buf[9]|0x80);
					ircMisc.send_buf[i++]=0;//Length
					ircMisc.send_buf[i++]=buf[11];//dataID
					ircMisc.send_buf[i++]=buf[12];
					                                        //add full range ...
					
					temp=formatData4fixDot(device_comps.calibration_param.y[3],device_comps.calibration_param.dot);
					ircMisc.send_buf[i++]=temp>>24;
					ircMisc.send_buf[i++]=temp>>16;
					ircMisc.send_buf[i++]=temp>>8;
					ircMisc.send_buf[i++]=temp;
					                                         //current p
					temp=formatData4fixDot(device_comps.current_press,device_comps.calibration_param.dot);                                        
					ircMisc.send_buf[i++]=temp>>24;
					ircMisc.send_buf[i++]=temp>>16;
					ircMisc.send_buf[i++]=temp>>8;
					ircMisc.send_buf[i++]=temp;
					
		            ircMisc.send_buf[i++]=(device_comps.calibration_param.unit&0x0f);//param unit

		            
                                    

		            ircMisc.send_buf[i++]=device_comps.current_temp>>8;
		            ircMisc.send_buf[i++]=device_comps.current_temp;

		            ircMisc.send_buf[i++]=device_comps.batt;

		            ircMisc.send_buf[i++]=0;//device status
					
					
					///ADD Other Data
					ircMisc.send_buf[10]=i-11;
					ircMisc.send_buf[i++]=Check_Sum(ircMisc.send_buf,i);
					ircMisc.send_buf[i++]=0x16;
					VerifyResult=0;
					break;
			case MD_DATA_ID_READ_HIGH_INFO:                //HIGH info
					ircMisc.send_buf[i++]=(buf[9]|0x80);
					ircMisc.send_buf[i++]=0;//Length
					ircMisc.send_buf[i++]=buf[11];//dataID
					ircMisc.send_buf[i++]=buf[12];
					                                        //add full range ...
					
					temp=formatData4fixDot(device_comps.high_calibration_param.y[1],device_comps.high_calibration_param.dot);
					ircMisc.send_buf[i++]=temp>>24;
					ircMisc.send_buf[i++]=temp>>16;
					ircMisc.send_buf[i++]=temp>>8;
					ircMisc.send_buf[i++]=temp;
					                                         
					temp=formatData4fixDot(device_comps.current_high,device_comps.high_calibration_param.dot);                                        
					ircMisc.send_buf[i++]=temp>>24;
					ircMisc.send_buf[i++]=temp>>16;
					ircMisc.send_buf[i++]=temp>>8;
					ircMisc.send_buf[i++]=temp;
					temp=formatData4fixDot(device_comps.current_high,device_comps.high_calibration_param.dot+2); //v=S*H H/=10,S:3 fix dot                                       
					ircMisc.send_buf[i++]=temp>>24;
					ircMisc.send_buf[i++]=temp>>16;
					ircMisc.send_buf[i++]=temp>>8;
					ircMisc.send_buf[i++]=temp;
					
		             ircMisc.send_buf[i++]=device_comps.current_temp>>8;
		            ircMisc.send_buf[i++]=device_comps.current_temp;

		            ircMisc.send_buf[i++]=device_comps.batt;

		            ircMisc.send_buf[i++]=0;//device status
					
					
					///ADD Other Data
					ircMisc.send_buf[10]=i-11;
					ircMisc.send_buf[i++]=Check_Sum(ircMisc.send_buf,i);
					ircMisc.send_buf[i++]=0x16;
					VerifyResult=0;
					break;	
			case MD_DATA_ID_READ_ACCESS_ADDR:                //read access addr
					ircMisc.send_buf[i++]=(buf[9]|0x80);
					ircMisc.send_buf[i++]=0;//Length
					ircMisc.send_buf[i++]=buf[11];//dataID
					ircMisc.send_buf[i++]=buf[12];
					memcpy(&ircMisc.send_buf[i],device_comps.access_param.ip,25);
					i+=25;
					ircMisc.send_buf[i++]=device_comps.access_param.port>>8;
					ircMisc.send_buf[i++]=device_comps.access_param.port;

                    memcpy(&ircMisc.send_buf[i],device_comps.access_param.ip1,25);
					i+=25;
					ircMisc.send_buf[i++]=device_comps.access_param.port1>>8;
					ircMisc.send_buf[i++]=device_comps.access_param.port1;
                    ///ADD Other Data
					ircMisc.send_buf[10]=i-11;
					ircMisc.send_buf[i++]=Check_Sum(ircMisc.send_buf,i);
					ircMisc.send_buf[i++]=0x16;
					VerifyResult=0;
					break;

			case MD_DATA_ID_READ_GPS_LOC_INFO:                //
					ircMisc.send_buf[i++]=(buf[9]|0x80);
					ircMisc.send_buf[i++]=0;//Length
					ircMisc.send_buf[i++]=buf[11];//dataID
					ircMisc.send_buf[i++]=buf[12];
					temp=device_comps.gps.glng;                                        
					ircMisc.send_buf[i++]=temp>>24;
					ircMisc.send_buf[i++]=temp>>16;
					ircMisc.send_buf[i++]=temp>>8;
					ircMisc.send_buf[i++]=temp;
					temp=device_comps.gps.glat;                                        
					ircMisc.send_buf[i++]=temp>>24;
					ircMisc.send_buf[i++]=temp>>16;
					ircMisc.send_buf[i++]=temp>>8;
					ircMisc.send_buf[i++]=temp;
                    ///ADD Other Data
					ircMisc.send_buf[10]=i-11;
					ircMisc.send_buf[i++]=Check_Sum(ircMisc.send_buf,i);
					ircMisc.send_buf[i++]=0x16;
					VerifyResult=0;
					break;
			case MD_DATA_ID_READ_MANUFACTURER_INFO:                //
					ircMisc.send_buf[i++]=(buf[9]|0x80);
					ircMisc.send_buf[i++]=0;//Length
					ircMisc.send_buf[i++]=buf[11];//dataID
					ircMisc.send_buf[i++]=buf[12];
					memcpy(&ircMisc.send_buf[i],device_comps.manufacturer_info.name,25);
					i+=25;
	                ///ADD Other Data
					ircMisc.send_buf[10]=i-11;
					ircMisc.send_buf[i++]=Check_Sum(ircMisc.send_buf,i);
					ircMisc.send_buf[i++]=0x16;
					VerifyResult=0;
					break;
			case MD_DATA_ID_READ_DEVICE_TYPE_INFO:                //
					ircMisc.send_buf[i++]=(buf[9]|0x80);
					ircMisc.send_buf[i++]=0;//Length
					ircMisc.send_buf[i++]=buf[11];//dataID
					ircMisc.send_buf[i++]=buf[12];
					memcpy(&ircMisc.send_buf[i],device_comps.device_info.type,25);
					i+=25;
	                ///ADD Other Data
					ircMisc.send_buf[10]=i-11;
					ircMisc.send_buf[i++]=Check_Sum(ircMisc.send_buf,i);
					ircMisc.send_buf[i++]=0x16;
					VerifyResult=0;
					break;
			case MD_DATA_ID_READ_DEVICE_ID_INFO:                //
					ircMisc.send_buf[i++]=(buf[9]|0x80);
					ircMisc.send_buf[i++]=0;//Length
					ircMisc.send_buf[i++]=buf[11];//dataID
					ircMisc.send_buf[i++]=buf[12];
					memcpy(&ircMisc.send_buf[i],device_comps.device_info.id,25);
					i+=25;
	                ///ADD Other Data
					ircMisc.send_buf[10]=i-11;
					ircMisc.send_buf[i++]=Check_Sum(ircMisc.send_buf,i);
					ircMisc.send_buf[i++]=0x16;
					VerifyResult=0;
					break;
			case MD_DATA_ID_READ_SENSOR_ID_INFO:                //
					ircMisc.send_buf[i++]=(buf[9]|0x80);
					ircMisc.send_buf[i++]=0;//Length
					ircMisc.send_buf[i++]=buf[11];//dataID
					ircMisc.send_buf[i++]=buf[12];
					memcpy(&ircMisc.send_buf[i],device_comps.sensor_info.id,25);
					i+=25;
	                ///ADD Other Data
					ircMisc.send_buf[10]=i-11;
					ircMisc.send_buf[i++]=Check_Sum(ircMisc.send_buf,i);
					ircMisc.send_buf[i++]=0x16;
					VerifyResult=0;
					break;
			default:       
					return 1;
			
		}
		ircComps.write(ircMisc.send_buf,i);
		return 13+buf[10];
	}
	else if(Cmd==1)//Write
	{
        if((DataId==MD_DATA_ID_SET_DEVICE_ADDR)||(DataId==MD_DATA_ID_WRITE_ALARM_PARAM)
             ||(DataId==MD_DATA_ID_SET_ACCESS_ADDR)||(DataId==MD_DATA_ID_WRITE_REPORT_PARAM)||(DataId==MD_DATA_ID_ENTER_CAL_MODE))
        {
            if(hum_comps.current_mode==EM_CAL_MODIFY_MODE)
            {
                return 1;
            }

        }
        else if(DataId==MD_DATA_ID_WRITE_CAL_DATA)
        {
            if(hum_comps.current_mode!=EM_CAL_MODIFY_MODE)
            {
                return 1;
            }
        }
	    memcpy(ircMisc.send_buf,buf,9);
		memcpy(ircMisc.send_buf+1,device_comps.device_addr.addr,7);
		switch(DataId)
	    {
		    case MD_DATA_ID_ENTER_CAL_MODE:                //enter calmode
				
                if(device_comps.sw._bit.e2prom_driver_err)
                {
                    ircMisc.send_buf[i++]=(buf[9]|0x90);    
                }
                else 
                {
				    ircMisc.send_buf[i++]=(buf[9]|0x80);
				   device_comps.cal_type=(cal_type_t)(buf[13]>>4);
				   enter_cal_query_mode();
				   enter_cal_modify_mode();
				   if(device_comps.cal_type==EM_CAL_PRESS)
				   {
                        device_comps.calibration_param_bak.dot=buf[13]&0x0f;
                        device_comps.calibration_param_bak.unit=buf[14];
				   }
				   else if(device_comps.cal_type==EM_CAL_RES)
				   {
                        device_comps.res_calibration_param_bak.dot=buf[13]&0x0f;
                        device_comps.res_calibration_param_bak.unit=buf[14];
				   }
				   else if(device_comps.cal_type==EM_CAL_HIGH)
				   {
                        device_comps.high_calibration_param_bak.dot=buf[13]&0x0f;
                        device_comps.high_calibration_param_bak.unit=buf[14];
				   }
                }
				
				ircMisc.send_buf[i++]=0;
				ircMisc.send_buf[i++]=buf[11];
				ircMisc.send_buf[i++]=buf[12];
				

				///ADD Other Data
				ircMisc.send_buf[10]=i-11;
				ircMisc.send_buf[i++]=Check_Sum(ircMisc.send_buf,i);
				ircMisc.send_buf[i++]=0x16;
				break;	
				
				
			case MD_DATA_ID_ENTER_NORMAL_MODE:                //exit cal mode
				
                ircMisc.send_buf[i++]=(buf[9]|0x80);
          #if(MD_PRODUCT_NAME==MD_LORA)
		  {
			  enter_lora_mode();
		  }
          #elif (MD_PRODUCT_NAME==MD_4G)
          {
			  enter_4g_mode();
		  }
		  #elif (MD_PRODUCT_NAME==MD_NORMAL)
		  {
                enter_normal_mode();
		  }
		  #endif
				
			    ircMisc.send_buf[i++]=0;
				ircMisc.send_buf[i++]=buf[11];
				ircMisc.send_buf[i++]=buf[12];

				///ADD Other Data
				ircMisc.send_buf[10]=i-11;
				ircMisc.send_buf[i++]=Check_Sum(ircMisc.send_buf,i);
				ircMisc.send_buf[i++]=0x16;
				break;
						
				
			case MD_DATA_ID_SET_DEVICE_ADDR:                //write device addr
    			if(device_comps.sw._bit.e2prom_driver_err)
    			{
    				ircMisc.send_buf[i++]=(buf[9]|0x90);    
    			}
    			else 
    			{
    				device_addr_t addr_cpy;
    				memcpy(addr_cpy.addr,&buf[13],7);
    				addr_cpy.cs=Check_Sum_5A(&addr_cpy, &addr_cpy.cs-(unsigned char *)&addr_cpy);
    				if(device_comps.save_device_addr(&addr_cpy.addr,sizeof(addr_cpy)))
    				{
    				    ircMisc.send_buf[i++]=(buf[9]|0x90);    
    				}
    				else
    				{
    					memcpy(&device_comps.device_addr,&addr_cpy,sizeof(addr_cpy));
    					ircMisc.send_buf[i++]=(buf[9]|0x80);
						mode_comps[hum_comps.current_mode].dis_option=6;
                        hum_comps.dis_oper_mark._bit.refresh_option=1;
						hum_comps.dis_oper_mark._bit.refresh_device_id_low=1;
				    }
    			}
    			memcpy(ircMisc.send_buf+1,device_comps.device_addr.addr,7);
    		    ircMisc.send_buf[i++]=0;
    			ircMisc.send_buf[i++]=buf[11];
    			ircMisc.send_buf[i++]=buf[12];

    			///ADD Other Data
    			ircMisc.send_buf[10]=i-11;
    			ircMisc.send_buf[i++]=Check_Sum(ircMisc.send_buf,i);
    			ircMisc.send_buf[i++]=0x16;
			    break;
			case MD_DATA_ID_WRITE_ALARM_PARAM:                //write alarm Param
			    if(device_comps.sw._bit.e2prom_driver_err)
    			{
    				ircMisc.send_buf[i++]=(buf[9]|0x90);    
    			}
    			else 
    			{
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
    				    ircMisc.send_buf[i++]=(buf[9]|0x90);    
    				}
    				else
    				{
    					memcpy(&device_comps.alarm_param,&alarm_param_cpy,sizeof(alarm_param_cpy));
    					ircMisc.send_buf[i++]=(buf[9]|0x80);
    				}
    			}	
    		    ircMisc.send_buf[i++]=0;
    			ircMisc.send_buf[i++]=buf[11];
    			ircMisc.send_buf[i++]=buf[12];

    			///ADD Other Data
    			ircMisc.send_buf[10]=i-11;
    			ircMisc.send_buf[i++]=Check_Sum(ircMisc.send_buf,i);
    			ircMisc.send_buf[i++]=0x16;
			    break;
			case MD_DATA_ID_SET_ACCESS_ADDR :                //write access addr
			
				if(device_comps.sw._bit.e2prom_driver_err)
    			{
    				ircMisc.send_buf[i++]=(buf[9]|0x90);    
    			}
    			else 
    			{
					access_param_t access_param_cpy={0};
    				memcpy(&access_param_cpy.ip[0],&buf[13],25);
    				
    				access_param_cpy.port=((unsigned int)buf[13+25]<<8)+buf[13+26];
    				memcpy(&access_param_cpy.ip1[0],&buf[13+27],25);
    				
    				access_param_cpy.port1=((unsigned int)buf[13+27+25]<<8)+buf[13+27+26];
    				
    				access_param_cpy.cs=Check_Sum_5A(&access_param_cpy, &access_param_cpy.cs-(unsigned char *)&access_param_cpy);
    				if(device_comps.save_access_param(&access_param_cpy,sizeof(access_param_cpy)))
    				{
    				    ircMisc.send_buf[i++]=(buf[9]|0x90);    
    				}
    				else
    				{
    					memcpy(&device_comps.access_param,&access_param_cpy,sizeof(access_param_cpy));
    					ircMisc.send_buf[i++]=(buf[9]|0x80);
    				}
				}
			
				ircMisc.send_buf[i++]=0;
				ircMisc.send_buf[i++]=buf[11];
				ircMisc.send_buf[i++]=buf[12];

				///ADD Other Data
				ircMisc.send_buf[10]=i-11;
				ircMisc.send_buf[i++]=Check_Sum(ircMisc.send_buf,i);
				ircMisc.send_buf[i++]=0x16;
				break;
				
			case MD_DATA_ID_WRITE_GPS_LOC_INFO :                //write
			
				if(device_comps.sw._bit.e2prom_driver_err)
    			{
    				ircMisc.send_buf[i++]=(buf[9]|0x90);    
    			}
    			else 
    			{
				    device_comps.gps.glng=((unsigned long)buf[13]<<24)+((unsigned long)buf[14]<<16)+((unsigned long)buf[15]<<8)+buf[16];;
    				device_comps.gps.glat=((unsigned long)buf[17]<<24)+((unsigned long)buf[18]<<16)+((unsigned long)buf[19]<<8)+buf[20];
    	            device_comps.gps.cs=Check_Sum_5A(&device_comps.gps, &device_comps.gps.cs-(unsigned char *)&device_comps.gps);
    				if(device_comps.save_gps_loc(&device_comps.gps,sizeof(device_comps.gps)))
    				{
    				    ircMisc.send_buf[i++]=(buf[9]|0x90);    
    				}
    				else
    				{
    					ircMisc.send_buf[i++]=(buf[9]|0x80);
    				}
    				
				}
			
				ircMisc.send_buf[i++]=0;
				ircMisc.send_buf[i++]=buf[11];
				ircMisc.send_buf[i++]=buf[12];

				///ADD Other Data
				ircMisc.send_buf[10]=i-11;
				ircMisc.send_buf[i++]=Check_Sum(ircMisc.send_buf,i);
				ircMisc.send_buf[i++]=0x16;
				break;
			case MD_DATA_ID_WRITE_MANUFACTURER_INFO :                //write
			
				if(device_comps.sw._bit.e2prom_driver_err)
    			{
    				ircMisc.send_buf[i++]=(buf[9]|0x90);    
    			}
    			else 
    			{
    			    memcpy(device_comps.manufacturer_info.name,&buf[13],25);
    			    
		            device_comps.manufacturer_info.cs=Check_Sum_5A(&device_comps.manufacturer_info, &device_comps.manufacturer_info.cs-(unsigned char *)&device_comps.manufacturer_info);
    				if(device_comps.save_manufacturer_info(&device_comps.manufacturer_info,sizeof(device_comps.manufacturer_info)))
    				{
    				    ircMisc.send_buf[i++]=(buf[9]|0x90);    
    				}
    				else
    				{
    					ircMisc.send_buf[i++]=(buf[9]|0x80);
    				}
    				
				}
			
				ircMisc.send_buf[i++]=0;
				ircMisc.send_buf[i++]=buf[11];
				ircMisc.send_buf[i++]=buf[12];

				///ADD Other Data
				ircMisc.send_buf[10]=i-11;
				ircMisc.send_buf[i++]=Check_Sum(ircMisc.send_buf,i);
				ircMisc.send_buf[i++]=0x16;
				break;
			case MD_DATA_ID_WRITE_DEVICE_TYPE_INFO :                //write
			
				if(device_comps.sw._bit.e2prom_driver_err)
    			{
    				ircMisc.send_buf[i++]=(buf[9]|0x90);    
    			}
    			else 
    			{
    			    memcpy(device_comps.device_info.type,&buf[13],25);
    			    
		            device_comps.device_info.cs=Check_Sum_5A(&device_comps.device_info, &device_comps.device_info.cs-(unsigned char *)&device_comps.device_info);
    				if(device_comps.save_device_info(&device_comps.device_info,sizeof(device_comps.device_info)))
    				{
    				    ircMisc.send_buf[i++]=(buf[9]|0x90);    
    				}
    				else
    				{
    					ircMisc.send_buf[i++]=(buf[9]|0x80);
    				}
    				
				}
			
				ircMisc.send_buf[i++]=0;
				ircMisc.send_buf[i++]=buf[11];
				ircMisc.send_buf[i++]=buf[12];

				///ADD Other Data
				ircMisc.send_buf[10]=i-11;
				ircMisc.send_buf[i++]=Check_Sum(ircMisc.send_buf,i);
				ircMisc.send_buf[i++]=0x16;
				break;
			
			case MD_DATA_ID_WRITE_DEVICE_ID_INFO :                //write
			
				if(device_comps.sw._bit.e2prom_driver_err)
    			{
    				ircMisc.send_buf[i++]=(buf[9]|0x90);    
    			}
    			else 
    			{
    			    memcpy(device_comps.device_info.id,&buf[13],25);
    			    
		            device_comps.device_info.cs=Check_Sum_5A(&device_comps.device_info, &device_comps.device_info.cs-(unsigned char *)&device_comps.device_info);
    				if(device_comps.save_device_info(&device_comps.device_info,sizeof(device_comps.device_info)))
    				{
    				    ircMisc.send_buf[i++]=(buf[9]|0x90);    
    				}
    				else
    				{
    					ircMisc.send_buf[i++]=(buf[9]|0x80);
    				}
    				
				}
			
				ircMisc.send_buf[i++]=0;
				ircMisc.send_buf[i++]=buf[11];
				ircMisc.send_buf[i++]=buf[12];

				///ADD Other Data
				ircMisc.send_buf[10]=i-11;
				ircMisc.send_buf[i++]=Check_Sum(ircMisc.send_buf,i);
				ircMisc.send_buf[i++]=0x16;
				break;
			case MD_DATA_ID_WRITE_SENSOR_INFO :                //write
			
				if(device_comps.sw._bit.e2prom_driver_err)
    			{
    				ircMisc.send_buf[i++]=(buf[9]|0x90);    
    			}
    			else 
    			{
    			    memcpy(device_comps.sensor_info.id,&buf[13],25);
    			    
		            device_comps.sensor_info.cs=Check_Sum_5A(&device_comps.sensor_info, &device_comps.sensor_info.cs-(unsigned char *)&device_comps.sensor_info);
    				if(device_comps.save_sensor_info(&device_comps.sensor_info,sizeof(device_comps.sensor_info)))
    				{
    				    ircMisc.send_buf[i++]=(buf[9]|0x90);    
    				}
    				else
    				{
    					ircMisc.send_buf[i++]=(buf[9]|0x80);
    				}
    				
				}
			
				ircMisc.send_buf[i++]=0;
				ircMisc.send_buf[i++]=buf[11];
				ircMisc.send_buf[i++]=buf[12];

				///ADD Other Data
				ircMisc.send_buf[10]=i-11;
				ircMisc.send_buf[i++]=Check_Sum(ircMisc.send_buf,i);
				ircMisc.send_buf[i++]=0x16;
				break;	
			case MD_DATA_ID_WRITE_REPORT_PARAM:  //report param
				if(device_comps.sw._bit.e2prom_driver_err)
    			{
    				ircMisc.send_buf[i++]=(buf[9]|0x90);    
    			}
    			else 
    			{
					report_param_t report_param_cpy;
					report_param_cpy.min=buf[13];
					report_param_cpy.hour=buf[14];
					report_param_cpy.min_Interval=((unsigned int)buf[15]<<8)+buf[16];
					report_param_cpy.hour_Interval=buf[15];
                    report_param_cpy.disFactor=((unsigned int)buf[17]<<8)+buf[18];
                    report_param_cpy.triggerTimes=((unsigned long)buf[19]<<24)+((unsigned long)buf[20]<<16)+((unsigned long)buf[21]<<8)+buf[22];
					report_param_cpy.cs=Check_Sum_5A(&report_param_cpy, &report_param_cpy.cs-(unsigned char *)&report_param_cpy);
    				if(device_comps.save_report_param(&report_param_cpy,sizeof(report_param_cpy)))
    				{
    				    ircMisc.send_buf[i++]=(buf[9]|0x90);    
    				}
    				else
    				{
                        device_comps.report_interval_timer=0;
    					memcpy(&device_comps.report_param,&report_param_cpy,sizeof(report_param_cpy));
    					ircMisc.send_buf[i++]=(buf[9]|0x80);
    				}
				}
			
				ircMisc.send_buf[i++]=0;
				ircMisc.send_buf[i++]=buf[11];
				ircMisc.send_buf[i++]=buf[12];

				///ADD Other Data
				ircMisc.send_buf[10]=i-11;
				ircMisc.send_buf[i++]=Check_Sum(ircMisc.send_buf,i);
				ircMisc.send_buf[i++]=0x16;
				break;
			case MD_DATA_ID_WRITE_LORA_PARAM:  //lora param
				if(device_comps.sw._bit.e2prom_driver_err)
    			{
    				ircMisc.send_buf[i++]=(buf[9]|0x90);    
    			}
    			else 
    			{
					yl701_info_t yl701_info_cpy;
					yl701_info_cpy.rate=buf[13]=4;
					yl701_info_cpy.verify=buf[14]=0;
					
					yl701_info_cpy.freq=((unsigned long)buf[15]<<16)+((unsigned long)buf[16]<<8)+buf[17];
					yl701_info_cpy.sf=buf[18];
                    yl701_info_cpy.workMode=buf[19];
					yl701_info_cpy.bw=buf[20];
					yl701_info_cpy.NodeId=((unsigned long)buf[21]<<8)+buf[22];
					yl701_info_cpy.netId=buf[23];
					yl701_info_cpy.power=buf[24];
					yl701_info_cpy.breathPeriod=buf[25];
					yl701_info_cpy.breathTime=buf[26];

					
					yl701_info_cpy.cs=Check_Sum_5A(&yl701_info_cpy, &yl701_info_cpy.cs-(unsigned char *)&yl701_info_cpy);
    				if(loraComps.save_yl701_info(&yl701_info_cpy,sizeof(yl701_info_cpy)))
    				{
    				    ircMisc.send_buf[i++]=(buf[9]|0x90);    
    				}
    				else
    				{
                        memcpy(loraComps.yl701_info_p,&yl701_info_cpy,sizeof(yl701_info_cpy));
    					ircMisc.send_buf[i++]=(buf[9]|0x80);
    					loraComps.sw._bit.param_modified=1;
    				}
				}
			
				ircMisc.send_buf[i++]=0;
				ircMisc.send_buf[i++]=buf[11];
				ircMisc.send_buf[i++]=buf[12];

				///ADD Other Data
				ircMisc.send_buf[10]=i-11;
				ircMisc.send_buf[i++]=Check_Sum(ircMisc.send_buf,i);
				ircMisc.send_buf[i++]=0x16;
				break;	
			case MD_DATA_ID_WRITE_CAL_DATA:       //write cal data       
                if((device_comps.sw._bit.e2prom_driver_err)||(hum_comps.current_mode!=EM_CAL_MODIFY_MODE)
                  ||(mode_comps[hum_comps.current_mode].dis_option!=buf[13]))
    			{
    				ircMisc.send_buf[i++]=(buf[9]|0x90);    
    			}
    			else 
    			{
                    long param=((unsigned long)buf[14]<<24)+((unsigned long)buf[15]<<16)
                                +((unsigned long)buf[16]<<8)+buf[17];
                    ircMisc.send_buf[i++]=(buf[9]|0x80); 
                    if(device_comps.cal_type==EM_CAL_PRESS)
                    {
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
                                        device_comps.calibration_param_bak.cs=Check_Sum_5A(&device_comps.calibration_param_bak, & device_comps.calibration_param_bak.cs-(unsigned char *)&device_comps.calibration_param_bak);
                                        if(device_comps.save_calibration_param(&device_comps.calibration_param_bak,sizeof(device_comps.calibration_param_bak)))
                        				{
                        				    ircMisc.send_buf[i]=(buf[9]|0x90);
                                            mode_comps[hum_comps.current_mode].dis_option=0;
                                            hum_comps.dis_oper_mark._bit.refresh_option=1;
                                            hum_comps.dis_oper_mark._bit.refresh_cal_param=1;
                                            device_comps.calibration_param_bak.is_calibrated=0;
                        				}
                        				else
                        				{
                        					memcpy(&device_comps.calibration_param,&device_comps.calibration_param_bak,sizeof(device_comps.calibration_param_bak));
                        					ircMisc.send_buf[i]=(buf[9]|0x80);
                        					#if(MD_PRODUCT_NAME==MD_LORA)
                                    		  {
                                    			  enter_lora_mode();
                                    		  }
                                              #elif (MD_PRODUCT_NAME==MD_4G)
                                              {
                                    			  enter_4g_mode();
                                    		  }
                                    		  #elif (MD_PRODUCT_NAME==MD_NORMAL)
                                    		  {
                                                     device_comps.max_press=0;
                                                     enter_normal_mode();
                                    		  }
                                    		  #endif
                                         }
                                        
                                        break;
                            default:   return 1;
                            
                         }
                   }
                   
                   else if(device_comps.cal_type==EM_CAL_RES)
                   {
                        switch(buf[13])
                        {
                            case 0:
                                        device_comps.res_calibration_param_bak.x[0]=device_comps.temp_p_temp_n_average_result;
                                        device_comps.res_calibration_param_bak.y[0]=param;
                                        mode_comps[hum_comps.current_mode].dis_option++;
                                        hum_comps.dis_oper_mark._bit.refresh_option=1;
                                        hum_comps.dis_oper_mark._bit.refresh_res_cal_param=1;
                                        break;
                         
                            case 1:
                                        device_comps.res_calibration_param_bak.x[1]=device_comps.temp_p_temp_n_average_result;
                                        device_comps.res_calibration_param_bak.y[1]=param;
                                        mode_comps[hum_comps.current_mode].dis_option++;
                                        hum_comps.dis_oper_mark._bit.refresh_option=1;
                                        hum_comps.dis_oper_mark._bit.refresh_res_cal_param=1;
                                        device_comps.res_calibration_param_bak.cs=Check_Sum_5A(&device_comps.res_calibration_param_bak, & device_comps.res_calibration_param_bak.cs-(unsigned char *)&device_comps.res_calibration_param_bak);
                                        if(device_comps.save_res_calibration_param(&device_comps.res_calibration_param_bak,sizeof(device_comps.res_calibration_param_bak)))
                        				{
                        				    ircMisc.send_buf[i]=(buf[9]|0x90);
                                            mode_comps[hum_comps.current_mode].dis_option=0;
                                            hum_comps.dis_oper_mark._bit.refresh_option=1;
                                            hum_comps.dis_oper_mark._bit.refresh_res_cal_param=1;
                                            device_comps.res_calibration_param_bak.is_calibrated=0;
                        				}
                        				else
                        				{
                        					memcpy(&device_comps.res_calibration_param,&device_comps.res_calibration_param_bak,sizeof(device_comps.res_calibration_param_bak));
                        					ircMisc.send_buf[i]=(buf[9]|0x80);
                        					#if(MD_PRODUCT_NAME==MD_LORA)
                                    		  {
                                    			  enter_lora_mode();
                                    		  }
                                              #elif (MD_PRODUCT_NAME==MD_4G)
                                              {
                                    			  enter_4g_mode();
                                    		  }
                                    		  #elif (MD_PRODUCT_NAME==MD_NORMAL)
                                    		  {
                                                     device_comps.max_press=0;
                                                     enter_normal_mode();
                                    		  }
                                    		  #endif
                                         }
                                        
                                        break;
                            default:   return 1;
                            
                         }
                   }
                   else if(device_comps.cal_type==EM_CAL_HIGH)
                   {
                        switch(buf[13])
                        {
                            case 0:
                                        device_comps.high_calibration_param_bak.x[0]=device_comps.high_p_high_n_average_result;
                                        device_comps.high_calibration_param_bak.y[0]=param;
                                        mode_comps[hum_comps.current_mode].dis_option++;
                                        hum_comps.dis_oper_mark._bit.refresh_option=1;
         				hum_comps.dis_oper_mark._bit.refresh_high_cal_param=1;
                                        break;
                         
                            case 1:
                                        device_comps.high_calibration_param_bak.x[1]=device_comps.high_p_high_n_average_result;
                                        device_comps.high_calibration_param_bak.y[1]=param;
                                        mode_comps[hum_comps.current_mode].dis_option++;
                                        hum_comps.dis_oper_mark._bit.refresh_option=1;
                                        hum_comps.dis_oper_mark._bit.refresh_high_cal_param=1;
                                        device_comps.high_calibration_param_bak.cs=Check_Sum_5A(&device_comps.high_calibration_param_bak, & device_comps.high_calibration_param_bak.cs-(unsigned char *)&device_comps.high_calibration_param_bak);
                                        if(device_comps.save_high_calibration_param(&device_comps.high_calibration_param_bak,sizeof(device_comps.high_calibration_param_bak)))
                        				{
                        				    ircMisc.send_buf[i]=(buf[9]|0x90);
                                            mode_comps[hum_comps.current_mode].dis_option=0;
                                            hum_comps.dis_oper_mark._bit.refresh_option=1;
                                            hum_comps.dis_oper_mark._bit.refresh_high_cal_param=1;
                                            device_comps.high_calibration_param_bak.is_calibrated=0;
                        				}
                        				else
                        				{
                        					memcpy(&device_comps.high_calibration_param,&device_comps.high_calibration_param_bak,sizeof(device_comps.high_calibration_param_bak));
								ircMisc.send_buf[i]=(buf[9]|0x80);
                        					#if(MD_PRODUCT_NAME==MD_LORA)
                                    		  {
                                    			  enter_lora_mode();
                                    		  }
                                              #elif (MD_PRODUCT_NAME==MD_4G)
                                              {
                                    			  enter_4g_mode();
                                    		  }
                                    		  #elif (MD_PRODUCT_NAME==MD_NORMAL)
                                    		  {
                                                   enter_normal_mode();
                                    		  }
                                    		  #endif
                                         }
                                        
                                        break;
                            default:   return 1;
                            
                         }
                   }
                   else 
                   {
                        return 1;
                   }
                }
				
				ircMisc.send_buf[i++]=0;
				ircMisc.send_buf[i++]=buf[11];
				ircMisc.send_buf[i++]=buf[12];
				ircMisc.send_buf[i++]=buf[13];

				///ADD Other Data
				ircMisc.send_buf[10]=i-11;
				ircMisc.send_buf[i++]=Check_Sum(ircMisc.send_buf,i);
				ircMisc.send_buf[i++]=0x16;
				break;
				
			default:       
					return 1;	
		}
		ircComps.write(ircMisc.send_buf,i);
		return 13+buf[10];
		
	}
	else 
	{
		return 1;
	}
	return 1;
	
}
static unsigned char Check_irc_Com(unsigned char *Rec_Data,unsigned char Rec_Pos)
{
	unsigned char i=0;
	if(Rec_Pos<2)
	{
		return 0;
	}
	if(Rec_Data[0]!=0x68)
	{
		return 1;
	}
	if(Rec_Pos<11)
	{
		return 0;
	}
	if(Rec_Data[8]!=0x68)
    {
		return 1;
	}
    if((Rec_Data[9]!=0x00)&&(Rec_Data[9]!=0x01))
    {
		return 1;
	}
	if(Rec_Data[10]>60)
    {
		return 1;
	}
	if((Rec_Data[1]!=0x99)||(Rec_Data[2]!=0x99)||(Rec_Data[3]!=0x99)||(Rec_Data[4]!=0x99)
	&&(Rec_Data[5]!=0x99)||(Rec_Data[6]!=0x99)||(Rec_Data[7]!=0x99))
	{
		for(i=0;i<7;i++)
		{
			if(Rec_Data[1+i]!=device_comps.device_addr.addr[i])
			{
				return 1;
			}
		}
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
	
	ircComps.op_window_time=25;
    return Pro_irc(Rec_Data[9],Rec_Data);
      	
}

static void Deal_irc(void)
{
	unsigned char err=0;
 	do
	{
		
		err=Check_irc_Com(ircMisc.recv_buf,ircMisc.rec_pos); 
		DI();
		if(err>0)
		{
			memcpy(ircMisc.recv_buf,ircMisc.recv_buf+err,ircMisc.rec_pos-=err);
    	}
		EI();
	}
	while (err>0);
}

static void  ircComps_task_handle(void)
{

  #ifndef MD_MODBUS  
   if(ircComps.sw._bit.runing)
   {
        Deal_irc();
   }
  #else
  Deal_irc();
  #endif
}

static void write_irc(unsigned char * const buf,unsigned int len)
{
    if(len>0)
    {
        
        #ifndef MD_MODBUS 
        R_TAU0_Channel0_Start();
        enable_irc_send();
        disable_irc_receive(); 
        #else
        enable_modbus_send();
        disable_modbus_receive(); 
        #endif
        R_UART0_Send(buf,len);
    }
}

ircComps_t ircComps=
{
    ircMisc.recv_buf,
    &ircMisc.rec_pos,
   
   30,
    {0},
    write_irc,
    ircComps_task_handle
};



