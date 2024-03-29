
#include "r_cg_macrodriver.h"
#include "protocol.h"
#include "r_cg_rtc.h"
#include "device.h"
#include "string.h"
#include "math.h"
#include "net.h"
#include "irc.h"
#include "hum.h"
#include "press.h"
#include "24cxx.h"
#include "stdio.h"
#include "stdlib.h"
#include "rad.h"
#include "cs123x.h"


//#define static   
#define                     MD_SET_REPORT_TIME_MSGID              40
#define                     MD_GET_REPORT_TIME_MSGID              41
#define                     MD_SET_PALARM_THRESHOLD_MSGID         42
#define                     MD_GET_PALARM_THRESHOLD_MSGID         43  
#define                     MD_SET_TALARM_THRESHOLD_MSGID         44
#define                     MD_GET_TALARM_THRESHOLD_MSGID         45
#define                     MD_GET_DEVICE_INFO_MSGID              46
#define                     MD_SET_ACCESS_ADDR_MSGID              47 
#define                     MD_GET_ACCESS_ADDR_MSGID              48
#define                     MD_SET_DEVICE_OFFLINE_MSGID           49



static struct 
{ 
	unsigned char buf[312];
	unsigned char step;
	int event_index;
	int reRryTimes;
	 
	
}
protocolMisc=
{
	"",
	0 ,
	-1,
	3
};

static unsigned int generateCRC(unsigned char *buffer, unsigned int messageLength)
{
	unsigned int crc = 0xFFFF;
	int i, j = 0;
	for (i = 0;i < messageLength;i++)
	{
		crc ^= buffer[i];
		for (j = 8; j != 0; j--)
		{
			if ((crc & 0x0001) != 0)
			{
				crc >>= 1;
				crc ^= 0xA001;
			}
			else
			{
				crc >>= 1;
			}
		}
	}
	
	return crc;
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

////////return  lenght of Rcvbuf
//static unsigned short crc_16(unsigned char const*puchMsg, unsigned char *Rcvbuf ,unsigned int usDataLen)
//{
//	unsigned short wCRCin = 0x0000;
//	unsigned short wCPoly = 0x1021;
//	unsigned short wChar = 0;
//	int i=0;
//
//	while (usDataLen--) 	
//	{
//	wChar = *(puchMsg++);
//	wCRCin ^= (wChar << 8);
//	for(i = 0;i < 8;i++)
//	{
//	  if(wCRCin & 0x8000)
//	    wCRCin = (wCRCin << 1) ^ wCPoly;
//	  else
//	    wCRCin = wCRCin << 1;
//	}
//	}
//	Rcvbuf[0] = (wCRCin & 0xff00)>>8;//??????
//	Rcvbuf[1] = (wCRCin& 0x00ff);  //�̨�????
//	return 2;
//}
//
////return Return crc check result 0 ok 1,err
////static unsigned short check_crc_16(unsigned char const *Array, unsigned int crc_16,unsigned int Len)
////{
//       
//static unsigned short check_crc_16(unsigned char const*puchMsg, unsigned int crc_16,unsigned int usDataLen)
//{
//  unsigned short wCRCin = 0x0000;
//  unsigned short wCPoly = 0x1021;
//  unsigned short wChar = 0;
//  int i=0;
//  
//  while (usDataLen--) 	
//  {
//        wChar = *(puchMsg++);
//        wCRCin ^= (wChar << 8);
//        for(i = 0;i < 8;i++)
//        {
//          if(wCRCin & 0x8000)
//            wCRCin = (wCRCin << 1) ^ wCPoly;
//          else
//            wCRCin = wCRCin << 1;
//        }
//  }
//  return (wCRCin!=crc_16) ;
//}


static long pwr(int n)
{
    if(n==0)
    {
        return 1;
    }
    if(n==1)
    {
        return 10;
    }
    if(n==2)
    {
        return 100;
    }
    if(n==3)
    {
        return 1000;
    }
    if(n==4)
    {
        return 10000;
    }
    if(n==5)
    {
        return 100000;
    }
    return 1;
}

static unsigned char AddAddr(unsigned char *buf)
{
	unsigned char i=7;
    DataAdd30H(buf,&device_comps.device_addr.addr[0],i);
    return i=i*2;
}
static unsigned char  AddTp(unsigned char *buf)
{
	unsigned char i=7;
	unsigned char temp[7];
	temp[0]=0x20;
	temp[1]=rtc_valve.year;
	temp[2]=rtc_valve.month;
	temp[3]=rtc_valve.day;
	temp[4]=rtc_valve.hour;
    temp[5]=rtc_valve.min;
	temp[6]=rtc_valve.sec;
    DataAdd30H(buf,temp,i);
    return  i=i*2;
}

static unsigned int AddTimeDataSeg(unsigned char *buf)
{
	unsigned int Addr=0;
	unsigned int StartAddr=0;
	unsigned int Count=0;
	unsigned int m=0;
	long temp;
	long buffer[48];
	Addr=device_comps.TimeSegData.store_addr;
	if((Addr-MD_TIME_SEG_DATA_START_ADDR)/4>=48)
	{
		_24cxx_comps.read(Addr-(unsigned int)48*4,buffer,(unsigned int)48*4);
	}
	else
	{
		StartAddr=MD_TIME_SEG_DATA_END_ADDR-((48-(Addr-MD_TIME_SEG_DATA_START_ADDR)/4)*4);
		Count=(48-(Addr-MD_TIME_SEG_DATA_START_ADDR)/4)*4;
		_24cxx_comps.read(StartAddr,buffer,Count);
		_24cxx_comps.read(MD_TIME_SEG_DATA_START_ADDR,(unsigned char *)buffer+Count,(Addr-MD_TIME_SEG_DATA_START_ADDR)/4*4);
		
	}
	for(m=0;m<48;m++)
	{
        temp=formatData4fixDot(buffer[m],device_comps.calibration_param.dot );
        buf[m*4+0]=temp>>24;
        buf[m*4+1]=temp>>16;
        buf[m*4+2]=temp>>8;
        buf[m*4+3]=temp;
	}
	return (unsigned int)48*4;
}
unsigned int AddLastSamplTps(unsigned char *buf)
{
	unsigned char i=7;
    DataAdd30H(buf,&device_comps.TimeSegData.lastSampleTime[0],i);           
	return i=i*2;
}

static unsigned char VerifyMeterId(unsigned char const *buf, unsigned char const *meterId)
{
    unsigned char temp[14];
    DataAdd30H(temp, meterId,7);
    return memcmp(buf,temp,14);
}

//static int AddTriggerReportCode(unsigned char *const buf,int event)
//{
//	int i=0;
////	static unsigned char const event_table[][4]=//Triggering event report table E.2
////	{
////		{0x80,0x01,0x00,0x06},//low voltage
////		{0x80,0x01,0x00,0x07},
////		{0x80,0x01,0x00,0x09},
////		{0x80,0x01,0x00,0x10},
////		{0x80,0x01,0x00,0x11},
////		{0x80,0x01,0x00,0x12},
////		{0x80,0x01,0x00,0x13},
////		{0x00,0x00,0x00,0x00},
////		{0x00,0x00,0x00,0x01}
////	};
////	buf[i++]=event_table[event][0];//Trigger report event code 1
////	buf[i++]=event_table[event][1];//Trigger report event code 2
////	buf[i++]=event_table[event][2];//Trigger report event code 3
////	buf[i++]=event_table[event][3];//Trigger report event code 4
//	return i;
//}

static int encapsulate_zhian_pack(char *const buf,int event)
{
    int i=0;
    long temp=0;
    char msg[32]="";


    memset(msg,0,sizeof(msg));
    memcpy(msg,&device_comps.manufacturer_info.name,sizeof(device_comps.manufacturer_info.name));
    sprintf(&buf[i],"$%s$;",msg);
    i+=strlen(&buf[i]);
    
    memset(msg,0,sizeof(msg));
    memcpy(msg,&device_comps.device_info.type,sizeof(device_comps.device_info.type));
    sprintf(&buf[i],"DTYPE:%s;",msg);
    i+=strlen(&buf[i]);
    
    memset(msg,0,sizeof(msg));
    memcpy(msg,&device_comps.device_info.id,sizeof(device_comps.device_info.id));
    sprintf(&buf[i],"DSN:%s;",msg);
    i+=strlen(&buf[i]);
    
    sprintf(&buf[i],"START;");
    i+=strlen(&buf[i]);

    if(device_comps.gps.glng<0)
    {
        sprintf(&buf[i],"GLNG:%07ld;",device_comps.gps.glng);
    }
    else
    {
        sprintf(&buf[i],"GLNG:%06ld;",device_comps.gps.glng);
    }
    i+=strlen(&buf[i]);
    memmove(&buf[i]-5,&buf[i]-6,6);
    *(&buf[i]-6)='.',
    i++;
    
    if(device_comps.gps.glat<0)
    {
        sprintf(&buf[i],"GLAT:%07ld;",device_comps.gps.glat);
    }
    else
    {
        sprintf(&buf[i],"GLAT:%06ld;",device_comps.gps.glat);
    }
    i+=strlen(&buf[i]);
    memmove(&buf[i]-5,&buf[i]-6,6);
    *(&buf[i]-6)='.',
    i++;
    
    memset(msg,0,sizeof(msg));
    memcpy(msg,netComps.net_info.iccid,20);
    sprintf(&buf[i],"CCID:%s;",msg);
    i+=strlen(&buf[i]);
    

    temp=device_comps.batt;
    if(temp<31) {temp=0;}
    else if(temp>36) {temp=10;}
    else {temp=(temp-31)*2;}
    sprintf(&buf[i],"DBAT:%d;",temp);
    i+=strlen(&buf[i]);
    
    
    temp=(int)netComps.net_info.Csq*10/31;
    if(temp>10) {temp=0;}//Csq=99;
    sprintf(&buf[i],"CSQ:%d;",temp);
    i+=strlen(&buf[i]);
    

    //sprintf(&buf[i],"KD:%s;","");
    //i+=strlen(&buf[i]);
    
    sprintf(&buf[i],"KD:");
    i+=strlen(&buf[i]);
    i+=AddTp((unsigned char *)&buf[i]);
    buf[i++]=';';
    
/**************start measure data*****************/
    sprintf(&buf[i],"DQT:%s;","C_5_A");
    i+=strlen(&buf[i]);
    

    if(device_comps.calibration_param.unit&0x0f)
    {
        temp=device_comps.current_press;
    }
    else
    {
        temp=device_comps.current_press*1000;
    }
    
    
    memset(msg,0,sizeof(msg));
    memcpy(msg,&device_comps.sensor_info.id,sizeof(device_comps.sensor_info.id));
    sprintf(&buf[i],"DQ1:%s",msg);
    i+=strlen(&buf[i]);
   

    #ifdef MD_HIGH
     sprintf(&buf[i],"_%0.3fm",(float)device_comps.current_high/pwr(device_comps.high_calibration_param.dot));//
    #else
     if(device_comps.calibration_param.is_calibrated)
     {
        sprintf(&buf[i],"_%ld",(long)( (float)temp/pwr(device_comps.calibration_param.dot)*102.04) );//kpa->m  /9.8 *1000 mm
     }
     else
     {
     
 sprintf(&buf[i],"_00");
     }
    #endif
    i+=strlen(&buf[i]);

    sprintf(&buf[i],"_00.0;");//(float)device_comps.current_temp/10);
    i+=strlen(&buf[i]);
    

    /**************end measure data*****************/


    sprintf(&buf[i],"MD:");
    i+=strlen(&buf[i]);
    i+=AddTp((unsigned char *)&buf[i]);
    buf[i++]=';';

    sprintf(&buf[i],"END;");
    i+=strlen(&buf[i]);


   // sprintf(&buf[i],"report event:%d;",event);
   // i+=strlen(&buf[i]);

    return i;

}

static int encapsulate_self_pack(char *const buf,int event)
{
    int i=0;
    long temp=0;
    char msg[32]="";
//    buf[i++]=0;//mesID
//   i+=AddAddr(&buf[i]);//meter id
//
//    temp=formatData4fixDot(device_comps.calibration_param.y[3],device_comps.calibration_param.dot);
//    buf[i++]=temp>>24;
//    buf[i++]=temp>>16;
//    buf[i++]=temp>>8;
//    buf[i++]=temp;
//                                             //current p
//    temp=formatData4fixDot(device_comps.current_press,device_comps.calibration_param.dot);                                         
//    buf[i++]=temp>>24;
//    buf[i++]=temp>>16;
//    buf[i++]=temp>>8;
//    buf[i++]=temp;
//    
//    buf[i++]=(device_comps.calibration_param.unit&0x0f);//param unit
//
//    buf[i++]=device_comps.current_temp>>8;
//    buf[i++]=device_comps.current_temp;
//
//    buf[i++]=netComps.net_info.sina;
//    buf[i++]=netComps.net_info.rsrp;
//    buf[i++]=netComps.net_info.Csq;
//
//    buf[i++]=netComps.net_info.pci>>8;
//    buf[i++]=netComps.net_info.pci;
//
//    buf[i++]=netComps.net_info.earFcn>>24;
//    buf[i++]=netComps.net_info.earFcn>>16;
//    buf[i++]=netComps.net_info.earFcn>>8;
//    buf[i++]=netComps.net_info.earFcn;
//
//    buf[i++]=netComps.net_info.ecl;
//    buf[i++]=device_comps.sw._bit.batt_status;
//    buf[i++]=device_comps.batt;
//    buf[i++]=device_comps.sw._bit.data_mode;
//    i+=AddTp(&buf[i]);
//
//    buf[i++]=netComps.net_info.cellId>>24;
//    buf[i++]=netComps.net_info.cellId>>16;
//    buf[i++]=netComps.net_info.cellId>>8;
//    buf[i++]=netComps.net_info.cellId;
//    buf[i++]=event;
//
//    i+=AddTimeDataSeg(&buf[i]);
//    i+=AddLastSamplTps(&buf[i]);
//    
//    return i;

    memset(msg,0,sizeof(msg));
    memcpy(msg,&device_comps.manufacturer_info.name,sizeof(device_comps.manufacturer_info.name));
    sprintf(&buf[i],"$%s$;",msg);
    i+=strlen(&buf[i]);

    memset(msg,0,sizeof(msg));
    memcpy(msg,&device_comps.device_info.type,sizeof(device_comps.device_info.type));
    sprintf(&buf[i],"TYPE:%s;",msg);
    i+=strlen(&buf[i]);

    sprintf(&buf[i],"ID:");
    i+=strlen(&buf[i]);
    i+=AddAddr((unsigned char *)&buf[i]);
    buf[i++]=';';
    
  
   
    if(device_comps.gps.glng<0)
    {
        sprintf(&buf[i],"LNG:%07ld;",device_comps.gps.glng);
    }
    else
    {
        sprintf(&buf[i],"LNG:%06ld;",device_comps.gps.glng);
    }
    i+=strlen(&buf[i]);
    memmove(&buf[i]-5,&buf[i]-6,6);
    *(&buf[i]-6)='.',
    i++;
    
    if(device_comps.gps.glat<0)
    {
        sprintf(&buf[i],"LAT:%07ld;",device_comps.gps.glat);
    }
    else
    {
        sprintf(&buf[i],"LAT:%06ld;",device_comps.gps.glat);
    }
    i+=strlen(&buf[i]);
    memmove(&buf[i]-5,&buf[i]-6,6);
    *(&buf[i]-6)='.',
    i++;
    
    memset(msg,0,sizeof(msg));
    memcpy(msg,netComps.net_info.iccid,20);
    sprintf(&buf[i],"ICCID:%s;",msg);
    i+=strlen(&buf[i]);

    sprintf(&buf[i],"VBATT:%d.",(int)device_comps.batt/10);
    i+=strlen(&buf[i]);
    sprintf(&buf[i],"%dV;",(int)device_comps.batt%10);
    i+=strlen(&buf[i]);
    
    sprintf(&buf[i],"CSQ:%d;",(int)netComps.net_info.Csq);
    i+=strlen(&buf[i]);
    
    temp=formatData4fixDot(device_comps.calibration_param.y[3],device_comps.calibration_param.dot);
    sprintf(&buf[i],"PFS:%ld.",temp/10000);
    i+=strlen(&buf[i]);
    if(device_comps.calibration_param.unit&0x0f)
    {
        sprintf(&buf[i],"%dKPa;",temp%10000);
    }
    else
    {
        sprintf(&buf[i],"%dMPa;",temp%10000);
    }
    i+=strlen(&buf[i]);

    temp=formatData4fixDot(device_comps.current_press,device_comps.calibration_param.dot);
    sprintf(&buf[i],"PRES:%ld.",temp/10000);
    i+=strlen(&buf[i]);
    if(device_comps.calibration_param.unit&0x0f)
    {
        sprintf(&buf[i],"%dKPa;",temp%10000);
    }
    else
    {
        sprintf(&buf[i],"%dMPa;",temp%10000);
    }
    i+=strlen(&buf[i]);

    sprintf(&buf[i],"TEMP:%.1f;",device_comps.current_temp/10.);
    i+=strlen(&buf[i]);
//    sprintf(&buf[i],"%d;",device_comps.current_temp%10);
//    i+=strlen(&buf[i]);

    sprintf(&buf[i],"TIME:");
    i+=strlen(&buf[i]);
    i+=AddTp((unsigned char *)&buf[i]);
    buf[i++]=';';

    sprintf(&buf[i],"EVENT:%d;",event);
    i+=strlen(&buf[i]);

    sprintf(&buf[i],"END;");
    i+=strlen(&buf[i]);
    
    return i;

}


static int encapsulate_shenghuo_pack(char *const buf,int event,int No)
{
    int i=0;
    long temp=0;
    
    unsigned int crc;
    buf[i++]=0x7e;
    buf[i++]=0;//Length  h
    buf[i++]=0;//Length  l
    memcpy(&buf[i],&device_comps.device_info.id,12);
    i+=12;
    buf[i++]=No;//cmd
    if(No==1)
    {
        buf[i++]=0x00;//pack no h
        buf[i++]=0x01;//pack no l
        buf[i++]=0x00;//has more
        buf[i++]=netComps.net_info.Csq;
        buf[i++]=device_comps.sw._bit.batt_status;
        buf[i++]=(int)device_comps.batt*10>>8;
        buf[i++]=(int)device_comps.batt*10;
        buf[i++]=device_comps.report_param.min_Interval>>8;
        buf[i++]=device_comps.report_param.min_Interval;
        buf[i++]=(long)device_comps.alarm_param.temp_high*10>>8;
        buf[i++]=(long)device_comps.alarm_param.temp_high*10;
        buf[i++]=(long)device_comps.alarm_param.temp_low*10>>8;
        buf[i++]=(long)device_comps.alarm_param.temp_low*10;
        buf[i++]=(long)device_comps.alarm_param.press_high_lower/100>>8;
        buf[i++]=(long)device_comps.alarm_param.press_high_lower/100;
        buf[i++]=(long)device_comps.alarm_param.press_low_upper/100>>8;
        buf[i++]=(long)device_comps.alarm_param.press_low_upper/100;
        buf[i++]=device_comps.alarm_param.sw._bit.high_temp_alarm_en+(device_comps.alarm_param.sw._bit.low_temp_alarm_en<<1)
                +(device_comps.alarm_param.sw._bit.high_press_alarm_en<<2)+(device_comps.alarm_param.sw._bit.low_press_alarm_en<<3);

        
        buf[i++]=0x00;//record num h
        buf[i++]=0x01;//record num L
        buf[i++]=(rtc_valve.year);
        buf[i++]=(rtc_valve.month);
        buf[i++]=(rtc_valve.day);
        buf[i++]=(rtc_valve.hour);
        buf[i++]=(rtc_valve.min);
        buf[i++]=(rtc_valve.sec);
        buf[i++]=0x03;//type
        buf[i++]=0;  //psw h
        buf[i++]=device_comps.sw._bit.isTempNoConnect+(device_comps.sw._bit.isTLowLessTriggered<<1)
                +(device_comps.sw._bit.isTHighOverTriggered<<2)+(!device_comps.calibration_param.is_calibrated<<3)  //psw L
                +(device_comps.sw._bit.isPLowLessTriggered<<4)+(device_comps.sw._bit.isPHighOverTriggered<<5)
                +(device_comps.sw._bit.isOnline<<6);
                
        buf[i++]=(long)device_comps.current_temp*10>>8;
        buf[i++]=(long)device_comps.current_temp*10;
        temp=formatData4fixDot(device_comps.current_press,device_comps.calibration_param.dot);
        if(device_comps.calibration_param.unit&0x0f)
        {
            temp/=100000;
        }
        else
        {
            temp/=100;
        }
        buf[i++]=temp>>8;
        buf[i++]=temp;
    }
    if(No==4)
    {
    
  
    }
    if(No==6)
    {
    
   
    }
    buf[1]=(i+2)>>8;
    buf[2]=(i+2);
    crc=generateCRC((unsigned char *)buf,i);
    buf[i++]=crc;
    buf[i++]=crc>>8;
    return i;
}

static int encapsulate_sidi_info_pack(char *const buf,int event,unsigned int No)
{
    int i=0;
    long temp=0;
    
    unsigned int crc;
    buf[i++]=0x7b;
    buf[i++]=0;//ser_num
    buf[i++]=0;//Target addr h
    buf[i++]=0;//Target addr l
    buf[i++]=0;//source addr h
    buf[i++]=1;//source addr l
    buf[i++]=No>>8;//regist h
    buf[i++]=No;//regist l
    buf[i++]=1;//reserved 
    buf[i++]=1;//hw ver
    buf[i++]=1;//sf ver 
    buf[i++]=0;//reserved 
    buf[i++]=0;//reserved 
    buf[i++]=0;//reserved 
    buf[i++]=0;//reserved 
    memcpy(&buf[i],&device_comps.device_info.id,12);
    i+=12;
    
    if(No==0x0301)
    {
        buf[1]=0;//ser_num
        buf[i++]=0;//data len h
        buf[i++]=7;//data len l
        buf[i++]=0x20;
        buf[i++]=(rtc_valve.year);
        buf[i++]=(rtc_valve.month);
        buf[i++]=(rtc_valve.day);
        buf[i++]=(rtc_valve.hour);
        buf[i++]=(rtc_valve.min);
        buf[i++]=(rtc_valve.sec);
    }
    else if(No==0x0303)
    {
        buf[1]=1;//ser_num
        buf[i++]=0;//data len h
        buf[i++]=5*2+11;//data len l
        buf[i++]=1;//packet Num
        buf[i++]=1;//source addr
        buf[i++]=5*2+10;//datapacket data len 
        buf[i++]=5;//Ain nums
        buf[i++]=2;//fix
        buf[i++]=0;// Digital
        buf[i++]=0;// Digital
        buf[i++]=0;// Digital
        buf[i++]=0;//reserved 
        buf[i++]=0;//reserved 
        buf[i++]=(long)device_comps.current_temp*10>>8;
        buf[i++]=(long)device_comps.current_temp*10;
        temp=formatData4fixDot(device_comps.current_press,device_comps.calibration_param.dot);
        if(device_comps.calibration_param.unit&0x0f)
        {
            temp/=100000;
        }
        else
        {
            temp/=100;
        }
        buf[i++]=temp>>8;
        buf[i++]=temp;
        buf[i++]=(int)device_comps.batt*10>>8;
        buf[i++]=(int)device_comps.batt*10;
        buf[i++]=0;
        buf[i++]=netComps.net_info.Csq;
        buf[i++]=device_comps.report_param.min_Interval>>8;
        buf[i++]=device_comps.report_param.min_Interval;
        buf[i++]=0x0a;
    }
    
   
    buf[i++]=Check_Sum((unsigned char *)buf+1,i-1);
    buf[i++]=0x7d;
    return i;
}


static int Encapsulate_dataPush_package(char *const buf,int event)//6.4.3
{
    
    #if (APP_PROTOCOL_TYPE==APP_PROTOCOL_ZHIAN) 
     return encapsulate_zhian_pack(buf,event);
    #elif(APP_PROTOCOL_TYPE==APP_PROTOCOL_SELF) 
     return encapsulate_self_pack(buf,event);
    #elif(APP_PROTOCOL_TYPE==APP_PROTOCOL_SHENGHUO) 
      return encapsulate_shenghuo_pack(buf,event,1);
    #elif(APP_PROTOCOL_TYPE==APP_PROTOCOL_SIDI_INFO) 
      return encapsulate_sidi_info_pack(buf,event,0x0301);
    #endif
    
}

static int check_shenghuo_data(const unsigned char *buf,int length)
{
    int len;
    if(length<18)
    {
         return 0;
    }
    if(buf[0]!=0x7e)
    {
        return 1;
    }
    if(memcmp(&buf[3],device_comps.device_info.id,12))
    {
        return 1;
    }
    if(buf[15]==2)
    {
    
   len=18+2;
    }
    else if(buf[15]==3)
    {
        len=18+11;
    }
    else if(buf[15]==5)
    {
        len=18+6;
    }
    else
    {
        return 1;
    }
    if(length < len)
    {
        return 0;
    }
    if( ((unsigned int)buf[len-1]<<8)+buf[len-2]!=generateCRC((unsigned char *)buf, len-2))
    {
        return 1;
    }
    if(buf[15]==2)
    {
       len=18+2;
       netComps.St._bit.push_data_ok=1;
       return len;
    }
    return 2;
}

static int check_sidi_info_data(const unsigned char *buf,int length)
{
    int len;
    if(length<27)
    {
         return 0;
    }
    if(buf[0]!=0x7b)
    {
        return 1;
    }
    if(memcmp(&buf[15],device_comps.device_info.id,12))
    {
        return 1;
    }
    if(buf[7]==1)
    {
       len=27+2+7+2;
    }
    else if(buf[7]==3)
    {
        len=27+2+1+2;
    }
    else
    {
        return 1;
    }
    if(length < len)
    {
        return 0;
    }
    if(buf[len-2]!=Check_Sum((unsigned char *)buf+1, len-3))
    {
        return 1;
    }
    
    return 2;
}

static int Analysis_downstream_package(char * buf,unsigned int length,unsigned char *address_field)
{
        char *find;
        char *p;
        int len;
        int err;
     #if (APP_PROTOCOL_TYPE==APP_PROTOCOL_ZHIAN) 
        char  msg1[32]="";
        char   msg[32]="";
        find=strstr(buf,"DSN:");
        if(!find)
        {
            return 1;
        }
        find+=strlen("DSN:");
        p=strchr(find,';');    
        if(!p)
        {
             return 1;
        }
        len=p-find;
        if(len>sizeof(device_comps.device_info.id))
        {
            return 1;
        }
        strncpy(msg,find,len);
        memcpy(msg1,&device_comps.device_info.id,sizeof(device_comps.device_info.id));
        return strcmp(msg1,msg);
     #elif (APP_PROTOCOL_TYPE==APP_PROTOCOL_SELF) 
        find=strstr(buf,"ID:");
        if(!find)
        {
            return 1;
        }
        find+=strlen("ID:");
        p=strchr(find,';');    
        if(!p)
        {
             return 1;
        }
        len=p-find;
        if(len!=14)
        {
            return 1;
        }
        return VerifyMeterId((unsigned char *)find,device_comps.device_addr.addr);
     #elif (APP_PROTOCOL_TYPE==APP_PROTOCOL_SHENGHUO)
       do
       {
            err=check_shenghuo_data((unsigned char *)buf, length);
            if(err==0)
            {
                return 1;
            }
            if(err==2)
            {
                return 0;
            }
            memcpy(buf,buf+err,length-=err);
       }
       while(err>0);
       return 1;
     #elif (APP_PROTOCOL_TYPE==APP_PROTOCOL_SIDI_INFO)
       do
       {
            err=check_sidi_info_data((unsigned char *)buf, length);
            if(err==0)
            {
                return 1;
            }
            if(err==2)
            {
                return 0;
            }
            memcpy(buf,buf+err,length-=err);
       }
       while(err>0);
       return 1;
     #else
        return 1;
     #endif
}        


//static int  AddMethodName(unsigned char name,unsigned char *buf)
// {      
//	unsigned char i=0;
//	switch(name)
//	{
//        case MD_SET_REPORT_TIME_MSGID: 
//            buf[i++]=strlen("setReportTime");//
//            memcpy(&buf[i],"setReportTime",buf[i-1]);//
//            i+=buf[i-1]; 
//            return i;
//        case MD_GET_REPORT_TIME_MSGID: 
//            buf[i++]=strlen("getReportTime");//
//            memcpy(&buf[i],"getReportTime",buf[i-1]);//
//            i+=buf[i-1]; 
//            return i;
//        case MD_SET_PALARM_THRESHOLD_MSGID:
//            buf[i++]=strlen("setPAlarmThreshold");//
//            memcpy(&buf[i],"setPAlarmThreshold",buf[i-1]);//
//            i+=buf[i-1]; 
//            return i;
//        case MD_GET_PALARM_THRESHOLD_MSGID: 
//            buf[i++]=strlen("getPAlarmThreshold");//
//            memcpy(&buf[i],"getPAlarmThreshold",buf[i-1]);//
//            i+=buf[i-1]; 
//            return i;
//        case MD_SET_TALARM_THRESHOLD_MSGID:
//            buf[i++]=strlen("getTAlarmThreshold");//
//            memcpy(&buf[i],"getTAlarmThreshold",buf[i-1]);//
//            i+=buf[i-1]; 
//            return i;
//        case MD_GET_TALARM_THRESHOLD_MSGID: 
//            buf[i++]=strlen("getTAlarmThreshold");//
//            memcpy(&buf[i],"getTAlarmThreshold",buf[i-1]);//
//            i+=buf[i-1]; 
//            return i;
//        case MD_GET_DEVICE_INFO_MSGID:
//            buf[i++]=strlen("getDeviceInfo");//
//            memcpy(&buf[i],"getDeviceInfo",buf[i-1]);//
//            i+=buf[i-1]; 
//            return i;
//        case MD_SET_ACCESS_ADDR_MSGID:
//         buf[i++]=strlen("setAccessAddr");//
//            memcpy(&buf[i],"setAccessAddr",buf[i-1]);//
//            i+=buf[i-1]; 
//            return i;
//        case MD_GET_ACCESS_ADDR_MSGID: 
//           buf[i++]=strlen("getAccessAddr");//
//            memcpy(&buf[i],"getAccessAddr",buf[i-1]);//
//            i+=buf[i-1]; 
//            return i;
//        case MD_SET_DEVICE_OFFLINE_MSGID:  
//		    buf[i++]=strlen("setDeviceOffline");//
//            memcpy(&buf[i],"setDeviceOffline",buf[i-1]);//
//            i+=buf[i-1]; 
//            return i;
//		default:
//		        buf[i++]=0;//len
//			return i;
//	}
//	return i;
// }
 
//static void dealAckData(unsigned char address_fiel,unsigned int midId)
//{
   
//    unsigned int  i=0;
//	memset(protocolMisc.buf,0,sizeof(protocolMisc.buf));
//	protocolMisc.buf[i++]=address_fiel+0x20;//The same msgID
//	i+=AddMethodName(address_fiel,&protocolMisc.buf[i]);
//	protocolMisc.buf[i++]=0;//errCode
//	switch(address_fiel)
//	{
//        case  MD_SET_ACCESS_ADDR_MSGID://
//        case  MD_SET_REPORT_TIME_MSGID :   
//        case  MD_SET_DEVICE_OFFLINE_MSGID:
//        case  MD_SET_PALARM_THRESHOLD_MSGID:
//        case  MD_SET_TALARM_THRESHOLD_MSGID:
//              break;
//        case  MD_GET_REPORT_TIME_MSGID:
//            protocolMisc.buf[i++]=device_comps.report_param.min;
//            protocolMisc.buf[i++]=device_comps.report_param.hour;
//            protocolMisc.buf[i++]=device_comps.report_param.hour_Interval;
//            protocolMisc.buf[i++]=device_comps.report_param.disFactor>>8;
//            protocolMisc.buf[i++]=device_comps.report_param.disFactor;
//            break;

//        case  MD_GET_PALARM_THRESHOLD_MSGID:
//            protocolMisc.buf[i++]=device_comps.alarm_param.press_high_upper>>24;
//            protocolMisc.buf[i++]=device_comps.alarm_param.press_high_upper>>16;
//            protocolMisc.buf[i++]=device_comps.alarm_param.press_high_upper>>8;
//            protocolMisc.buf[i++]=device_comps.alarm_param.press_high_upper;

//            protocolMisc.buf[i++]=device_comps.alarm_param.press_high_lower>>24;
//            protocolMisc.buf[i++]=device_comps.alarm_param.press_high_lower>>16;
//            protocolMisc.buf[i++]=device_comps.alarm_param.press_high_lower>>8;
//            protocolMisc.buf[i++]=device_comps.alarm_param.press_high_lower;

//            protocolMisc.buf[i++]=device_comps.alarm_param.press_low_upper>>24;
//            protocolMisc.buf[i++]=device_comps.alarm_param.press_low_upper>>16;
//            protocolMisc.buf[i++]=device_comps.alarm_param.press_low_upper>>8;
//            protocolMisc.buf[i++]=device_comps.alarm_param.press_low_upper;

//            protocolMisc.buf[i++]=device_comps.alarm_param.press_low_lower>>24;
//            protocolMisc.buf[i++]=device_comps.alarm_param.press_low_lower>>16;
//            protocolMisc.buf[i++]=device_comps.alarm_param.press_low_lower>>8;
//            protocolMisc.buf[i++]=device_comps.alarm_param.press_low_lower;

//            protocolMisc.buf[i++]=device_comps.alarm_param.unit;
//            break;
//        case  MD_GET_TALARM_THRESHOLD_MSGID:
//            protocolMisc.buf[i++]=device_comps.alarm_param.temp_high>>8;
//			protocolMisc.buf[i++]=device_comps.alarm_param.temp_high;
//			protocolMisc.buf[i++]=device_comps.alarm_param.temp_low>>8;
//			protocolMisc.buf[i++]=device_comps.alarm_param.temp_low;
//            break;

//        case  MD_GET_DEVICE_INFO_MSGID:
//            i+=AddAddr(&protocolMisc.buf[i]);
//            protocolMisc.buf[i++]=MD_FL_VER;//FL_VER hdware ver
//            protocolMisc.buf[i++]=MD_FL_VER ;//FL_VER swware ver

//            protocolMisc.buf[i++]=strlen(netComps.net_info.firmhdVer)>sizeof(netComps.net_info.firmhdVer)?sizeof(netComps.net_info.firmhdVer):strlen(netComps.net_info.firmhdVer);//moudle swver len
//            memcpy(&protocolMisc.buf[i],netComps.net_info.firmhdVer,protocolMisc.buf[i-1]);//r
//            i+=protocolMisc.buf[i-1];

//            protocolMisc.buf[i++]=strlen(netComps.net_info.firmVer)>sizeof(netComps.net_info.firmVer)?sizeof(netComps.net_info.firmVer):strlen(netComps.net_info.firmVer);//moudle swver len
//            memcpy(&protocolMisc.buf[i],netComps.net_info.firmVer,protocolMisc.buf[i-1]);//
//            i+=protocolMisc.buf[i-1];

//            protocolMisc.buf[i++]=strlen(netComps.net_info.imei)>sizeof(netComps.net_info.imei)?sizeof(netComps.net_info.imei):strlen(netComps.net_info.imei);//moudle swver len
//            memcpy(&protocolMisc.buf[i],netComps.net_info.imei,protocolMisc.buf[i-1]);//
//            i+=protocolMisc.buf[i-1];

//            protocolMisc.buf[i++]=strlen(netComps.net_info.iccid)>sizeof(netComps.net_info.iccid)?sizeof(netComps.net_info.iccid):strlen(netComps.net_info.iccid);//moudle swver len
//            memcpy(&protocolMisc.buf[i],netComps.net_info.iccid,protocolMisc.buf[i-1]);//
//            i+=protocolMisc.buf[i-1];

//            protocolMisc.buf[i++]=strlen(netComps.net_info.imsi)>sizeof(netComps.net_info.imsi)?sizeof(netComps.net_info.imsi):strlen(netComps.net_info.imsi);//moudle swver len
//            memcpy(&protocolMisc.buf[i],netComps.net_info.imsi,protocolMisc.buf[i-1]);//
//            i+=protocolMisc.buf[i-1];
//            break;
//        case  MD_GET_ACCESS_ADDR_MSGID:
//            {
//                char IpNum[25]="";
                
//                sprintf(IpNum+strlen(IpNum),"%d.",device_comps.access_param.ip[0]);
//                sprintf(IpNum+strlen(IpNum),"%d.",device_comps.access_param.ip[1]);
//                sprintf(IpNum+strlen(IpNum),"%d.",device_comps.access_param.ip[2]);
//                sprintf(IpNum+strlen(IpNum),"%d,",device_comps.access_param.ip[3]);
//                sprintf(IpNum+strlen(IpNum),"%u",device_comps.access_param.port);
//                protocolMisc.buf[i++]=strlen(IpNum)>sizeof(IpNum)?sizeof(IpNum):strlen(IpNum);//
//                memcpy(&protocolMisc.buf[i],IpNum,protocolMisc.buf[i-1]);//New IP
//                i+=protocolMisc.buf[i-1];
//            }
            
//            break;
//       default:
//		          break;
//	}
//    protocolMisc.buf[i++]=midId>>8;//The same MIDidH
//	protocolMisc.buf[i++]=midId;//The same MIDidL
	
//	protocolComps.msg;//=protocolMisc.buf;
//	protocolComps.msgLen=i;
//	protocolComps.sw._bit.DataRdy=1;
//}


static int  DealDownCmd(char  * const buf,unsigned int Len)
{
//        unsigned int   i=15;
//        unsigned int   err=1;
//       
//        switch(buf[0])
//	    {
//	            case  MD_SET_ACCESS_ADDR_MSGID://
//    			  	if(!VerifyMeterId(&buf[1],&device_comps.device_addr.addr[0]))
//    				{
//    					char *ptsz;
//    					unsigned int port=0;
//    					device_comps.access_param.ip[0]=strtol(&buf[i],&ptsz,10);
//    					device_comps.access_param.ip[1]= strtol(ptsz+1,&ptsz,10);
//    					device_comps.access_param.ip[2]= strtol(ptsz+1,&ptsz,10);
//    					device_comps.access_param.ip[3]= strtol(ptsz+1,&ptsz,10);
//    					port= strtol(ptsz+1,(char **)0,10);//////////////////////bug
//    					device_comps.access_param.port=port;
//    					device_comps.access_param.cs=Check_Sum_5A(&device_comps.access_param, &device_comps.access_param.cs-(unsigned char *)&device_comps.access_param);
//    					device_comps.save_access_param(&device_comps.access_param,sizeof(device_comps.access_param));
//    				}
//    				else 
//    				{
//    					err=0;
//    				}
//				break;
//				
//		        case  MD_SET_REPORT_TIME_MSGID :   
//			        if(!VerifyMeterId(&buf[1],&device_comps.device_addr.addr[0]))
//				    {
//                        unsigned int tmpdat=0;
//                        device_comps.report_param.min=buf[i++];//min
//                        device_comps.report_param.hour=buf[i++];//hour
//                        device_comps.report_param.hour_Interval=buf[i++];//report intertime
//                        tmpdat=buf[i++];
//                        tmpdat=(tmpdat<<8)+buf[i++];
//                        if((tmpdat>59)&&(tmpdat<601))
//                        {
//                            device_comps.report_param.disFactor=tmpdat;
//                        }
//                       
//                        device_comps.report_interval_timer=0;
//                        device_comps.report_param.cs=Check_Sum_5A(&device_comps.report_param, &device_comps.report_param.cs-(unsigned char*)&device_comps.report_param);
//    				    device_comps.save_report_param(&device_comps.report_param,sizeof(device_comps.report_param));
//				    }
//				else 
//				{
//					err=0;
//				}
//				break;
//			case  MD_SET_DEVICE_OFFLINE_MSGID:
//			      if(!buf[i])//0: has no cache cmd  ,
//			      {
//				     protocolMisc.step++;
//				     protocolComps.sw._bit.cmd_shutDown=1;
//				     protocolComps.sw._bit.dataPushYet=1;
//				     
//			      }
//			      else
//			      {
//				       // CurrentReportHasCacheCmd=1;   
//			      }
//			      break;
//
//            case  MD_SET_PALARM_THRESHOLD_MSGID:
//                    if(!VerifyMeterId(&buf[1],&device_comps.device_addr.addr[0]))
//				    {
//                        //memcpy(&device_comps.alarm_param.press_high_upper,&buf[i],4*4);
//    				   // device_comps.alarm_param.unit=buf[i+=4*4];
//    				    //i++;
//    				    device_comps.alarm_param.press_high_upper=((unsigned long)buf[i   ]<<24)+((unsigned long)buf[i+ 1]<<16)+((unsigned long)buf[i+2 ]<<8)+buf[i+3];
//                        device_comps.alarm_param.press_high_lower=((unsigned long)buf[i+ 4]<<24)+((unsigned long)buf[i+ 5]<<16)+((unsigned long)buf[i+6 ]<<8)+buf[i+7];
//                        device_comps.alarm_param.press_low_upper =((unsigned long)buf[i+ 8]<<24)+((unsigned long)buf[i+ 9]<<16)+((unsigned long)buf[i+10]<<8)+buf[i+11];
//                        device_comps.alarm_param.press_low_lower =((unsigned long)buf[i+12]<<24)+((unsigned long)buf[i+13]<<16)+((unsigned long)buf[i+14]<<8)+buf[i+15];
//                        device_comps.alarm_param.unit=buf[i+=16];
//                        i++;
//                        device_comps.alarm_param.cs=Check_Sum_5A(&device_comps.alarm_param, &device_comps.alarm_param.cs-(unsigned char)&device_comps.alarm_param);
//    				    device_comps.save_alarm_param(&device_comps.alarm_param,sizeof(device_comps.alarm_param));
//				    }
//    				else 
//    				{
//    					err=0;
//    				}
//    				break;
//                  
//    				
//            case  MD_SET_TALARM_THRESHOLD_MSGID:
//                    if(!VerifyMeterId(&buf[1],&device_comps.device_addr.addr[0]))
//                    {
//                     //memcpy(&alarm_param_cpy.temp_high,&buf[13+4*4+1],2*2);
//                        device_comps.alarm_param.temp_high =((unsigned int)buf[i]<<8)+buf[i+1];
//                        device_comps.alarm_param.temp_low =((unsigned int)buf[i+2]<<8)+buf[i+3];
//                        i+=4;
//                        device_comps.alarm_param.cs=Check_Sum_5A(&device_comps.alarm_param, &device_comps.alarm_param.cs-(unsigned char *)&device_comps.alarm_param);
//                        device_comps.save_alarm_param(&device_comps.alarm_param,sizeof(device_comps.alarm_param));
//                    }
//                    else 
//                    {
//                        err=0;
//                    }
//                    break;
//                    
//            case  MD_GET_REPORT_TIME_MSGID:
//            case  MD_GET_PALARM_THRESHOLD_MSGID:
//            case  MD_GET_TALARM_THRESHOLD_MSGID:
//            case  MD_GET_DEVICE_INFO_MSGID:
//            case  MD_GET_ACCESS_ADDR_MSGID:
//                   break;
//		default:
//		           err=0;
//	}
//	
//   
//	if(err)
//	{
//        unsigned char address_fiel=buf[0];
//        unsigned int  midId=((unsigned int)buf[Len-2]<<8)+buf[Len-1];
//		dealAckData(buf[0],midId);
//	}
//	return err;
    int i=0;
    char *find;
    char  msg[32]="";
	unsigned char cmd;
#if ((APP_PROTOCOL_TYPE==APP_PROTOCOL_ZHIAN) || (APP_PROTOCOL_TYPE==APP_PROTOCOL_SELF))

     #if (APP_PROTOCOL_TYPE==APP_PROTOCOL_ZHIAN) 
     const char *sample_interval="HCJG:";
     const char *ip="SA:";
     const char *rport="PORT:";
     #elif(APP_PROTOCOL_TYPE==APP_PROTOCOL_SELF) 
     const char *sample_interval="INR:";
     const char *ip="RSA:";
     const char *rport="PORT:";
     #endif
		
    if(1)
    {
        find=strstr(buf,sample_interval);
        if(find)
        {
    	    long    hcjg;	
    	    find+=strlen(sample_interval);
    	    if(*find>='0'  && *find<='9')
    	    {
                hcjg=atol(find);
                if(hcjg<65536)
                {
                       device_comps.report_interval_timer=0;
                       device_comps.report_param.min_Interval=hcjg;
        			   device_comps.report_param.hour_Interval=hcjg;
        			   device_comps.report_param.cs=Check_Sum_5A((unsigned char *)&device_comps.report_param, &device_comps.report_param.cs-(unsigned char *)&device_comps.report_param);
        			   device_comps.save_report_param(&device_comps.report_param,sizeof(device_comps.report_param));
                }
            }
        }

        find=strstr(buf,rport);
        if(find)
        {
    	    unsigned int port;	
    	    find+=strlen(rport);
            port=atol(find);
            if(port>0)
            {
                    if(netComps.net_info.currentIP_No==EM_IP0)
                    {
                        device_comps.access_param.port=port;
                        device_comps.access_param.cs=Check_Sum_5A(&device_comps.access_param, &device_comps.access_param.cs-(unsigned char *)&device_comps.access_param);
                        device_comps.save_access_param(&device_comps.access_param,sizeof(device_comps.access_param));
    			   }
    			   else if(netComps.net_info.currentIP_No==EM_IP1)
    			   {
    			        device_comps.access_param.port1=port;
                        device_comps.access_param.cs=Check_Sum_5A(&device_comps.access_param, &device_comps.access_param.cs-(unsigned char *)&device_comps.access_param);
                        device_comps.save_access_param(&device_comps.access_param,sizeof(device_comps.access_param));
                    }
            }
        }

        find=strstr(buf,ip);
        if(find)
        {
    	    int len;	
    	    find+=strlen(ip);
    	    len=strstr(find,";")- find;
            
            if( len>6&& len<26)
            {
                  if(netComps.net_info.currentIP_No==EM_IP0)
                    {
                        memset(&device_comps.access_param.ip,0,sizeof(device_comps.access_param.ip));
                        memcpy(&device_comps.access_param.ip,find,len);
                        device_comps.access_param.cs=Check_Sum_5A(&device_comps.access_param, &device_comps.access_param.cs-(unsigned char *)&device_comps.access_param);
    			        device_comps.save_access_param(&device_comps.access_param,sizeof(device_comps.access_param));
                    }
                    else if(netComps.net_info.currentIP_No==EM_IP1)
                    {
                        memset(&device_comps.access_param.ip1,0,sizeof(device_comps.access_param.ip1));
                        memcpy(&device_comps.access_param.ip1,find,len);
                        device_comps.access_param.cs=Check_Sum_5A(&device_comps.access_param, &device_comps.access_param.cs-(unsigned char *)&device_comps.access_param);
    			        device_comps.save_access_param(&device_comps.access_param,sizeof(device_comps.access_param));
                    }
            }
        
        }
        
    }
    memset(protocolMisc.buf,0,sizeof(protocolMisc.buf));
    
   #if (APP_PROTOCOL_TYPE==APP_PROTOCOL_ZHIAN) 
    memset(msg,0,sizeof(msg));
    memcpy(msg,&device_comps.manufacturer_info.name,sizeof(device_comps.manufacturer_info.name));
    sprintf(&buf[i],"$%s$;",msg);
    i+=strlen(&buf[i]);
    
    memset(msg,0,sizeof(msg));
    memcpy(msg,&device_comps.device_info.type,sizeof(device_comps.device_info.type));
    sprintf(&buf[i],"DTYPE:%s;",msg);
    i+=strlen(&buf[i]);
    
    memset(msg,0,sizeof(msg));
    memcpy(msg,&device_comps.device_info.id,sizeof(device_comps.device_info.id));
    sprintf(&buf[i],"D_SN:%s;",msg);
    i+=strlen(&buf[i]);
    
    

    sprintf(&buf[i],"START;");
    i+=strlen(&buf[i]);

    //sprintf(&buf[i],"RCVD:%d;",Len);
    sprintf(&buf[i],"RCVD:0;");
    i+=strlen(&buf[i]);

    sprintf(&buf[i],"END;");
    i+=strlen(&buf[i]);
    
  #else
    memset(msg,0,sizeof(msg));
    memcpy(msg,&device_comps.manufacturer_info.name,sizeof(device_comps.manufacturer_info.name));
    sprintf(&buf[i],"$%s$;",msg);
    i+=strlen(&buf[i]);

    sprintf(&buf[i],"ID:");
    i+=strlen(&buf[i]);
    i+=AddAddr((unsigned char *)&buf[i]);
    buf[i++]=';';

    sprintf(&buf[i],"RCVD:%d;",Len);
    i+=strlen(&buf[i]);

    sprintf(&buf[i],"END;");
    i+=strlen(&buf[i]);
  #endif
#elif(APP_PROTOCOL_TYPE==APP_PROTOCOL_SHENGHUO)
    cmd=buf[15];
    switch(cmd)
    {
        case 2:
                netComps.St._bit.push_data_ok=1;
                protocolComps.AckTmr=10;
                break;
        case 3:
                device_comps.report_param.min_Interval=((unsigned int)buf[16]<<8)+buf[17];
			    device_comps.report_param.hour_Interval=device_comps.report_param.min_Interval;
			    device_comps.report_param.cs=Check_Sum_5A((unsigned char *)&device_comps.report_param, &device_comps.report_param.cs-(unsigned char *)&device_comps.report_param);
			    device_comps.save_report_param(&device_comps.report_param,sizeof(device_comps.report_param));

                device_comps.alarm_param.temp_high =(((unsigned int)buf[18]<<8)+buf[19])/10;
				device_comps.alarm_param.temp_low = (((unsigned int)buf[20]<<8)+buf[21])/10;
				device_comps.alarm_param.press_high_lower=(((unsigned long)buf[22]<<8)+buf[23])*100;
				device_comps.alarm_param.press_low_upper =(((unsigned long)buf[24]<<8)+buf[25])*100;
				device_comps.alarm_param.unit=0;
				device_comps.alarm_param.sw._bit.high_temp_alarm_en=buf[26]&1;
				device_comps.alarm_param.sw._bit.low_temp_alarm_en=(buf[26]&2)>>1;
				device_comps.alarm_param.sw._bit.high_press_alarm_en=(buf[26]&4)>>2;
				device_comps.alarm_param.sw._bit.low_press_alarm_en=(buf[26]&8)>>3;
				device_comps.alarm_param.cs=Check_Sum_5A(&device_comps.alarm_param, &device_comps.alarm_param.cs-(unsigned char *)&device_comps.alarm_param);
				device_comps.save_alarm_param(&device_comps.alarm_param,sizeof(device_comps.alarm_param));
				protocolComps.AckTmr=10;
    			break;
        case 5:
                 //protocolComps.sw._bit.cmd_shutDown=1;
                  protocolComps.AckTmr=10;
				  netComps.St._bit.push_data_ok=1;
				  break;
				     
        default:
                break;
    }
    memset(protocolMisc.buf,0,sizeof(protocolMisc.buf));
    if(cmd==3 || cmd==5)
    {
        i=encapsulate_shenghuo_pack(buf, 0, cmd+1);
    }
#elif(APP_PROTOCOL_TYPE==APP_PROTOCOL_SIDI_INFO)
    cmd=buf[7];
    switch(cmd)
    {
        case 1:
                
                protocolComps.AckTmr=10;
                break;
        case 3:
                if(buf[29]==1)
                {
                    netComps.St._bit.push_data_ok=1;
                }
                else
                {
                    netComps.St._bit.push_data_ok=0;
                }
				//protocolComps.sw._bit.cmd_shutDown=1;
                 protocolComps.AckTmr=2;
    			break;
      
				     
        default:
                break;
    }
    memset(protocolMisc.buf,0,sizeof(protocolMisc.buf));
    if(cmd==1)
    {
        i=encapsulate_sidi_info_pack(buf, 0, 0x0303);
    }    
#endif

    if(i>0)
    {
        protocolComps.msgLen=i;
    //    protocolComps.msg;//=protocolMisc.buf;
        protocolComps.sw._bit.DataRdy=1;
   }
	return 0;
}

static void Srv_Protol(int event)
{

	if(protocolMisc.step==0)//Encapsulate the registration package and send a message to esam encryption
	{
		if(netComps.St._bit.allow_data_send>0)
		{
            if(!protocolComps.sw._bit.DataRdy)
            {
                memset(protocolMisc.buf,0,sizeof(protocolMisc.buf));
    			protocolComps.msgLen=Encapsulate_dataPush_package((char *)protocolMisc.buf,event);
   // 			protocolComps.msg;//=protocolMisc.buf;	
    		    protocolComps.sw._bit.DataRdy=1; //send a message to bc35 send
    		    protocolComps.AckTmr=10;
                protocolMisc.step++;
            }
         }
	}
	else if(protocolMisc.step==1)//
	{
		if((!protocolComps.sw._bit.DataRdy)&&(netComps.St._bit.recvData))//////////////////
		{
			unsigned char address_field;
			netComps.St._bit.recvData=0;//TODO clr buf
			memset(protocolMisc.buf,0,sizeof(protocolMisc.buf));
			memcpy(protocolMisc.buf,netComps.msg,netComps.msgLen);
			protocolComps.msgLen=netComps.msgLen;
			if(!Analysis_downstream_package((char *)protocolMisc.buf,protocolComps.msgLen,&address_field))//Remove the link layer protocol (68, len crc etc)
			{
 //               netComps.op_window_tmr=5;
//                if(address_field>=40&&address_field<=49)
//                {
//                    DealDownCmd(protocolMisc.buf,protocolComps.msgLen);
//                    protocolComps.AckTmr=10;
//                }
//                else if(address_field==0xaa)
//                {
//                     protocolComps.sw._bit.dataPushYet=1;
//                }
//                else
//                {
//
//                }
                  DealDownCmd((char *)protocolMisc.buf,protocolComps.msgLen);
                  
            }
			else
			{
				//TODO ....
			}
		}
		else
		{
			if(!protocolComps.AckTmr)
			{
                if(!netComps.St._bit.push_data_ok)
                {
    				if(protocolMisc.reRryTimes>0)
    				{
    					protocolMisc.reRryTimes--;
    					
    					if(protocolMisc.reRryTimes==0)
    					{
    						protocolComps.sw._bit.AckTmrOut=1;
    						protocolMisc.step++;
    					}
    					else
    					{
    						protocolMisc.step=0;
    					}
    				}
				}
				else
				{
                    protocolComps.sw._bit.AckTmrOut=1;
                    protocolMisc.step++;
				}
				
				if(protocolComps.sw._bit.AckTmrOut)
				{
                     protocolComps.sw._bit.isAckTmrOut=1;
				}
		    }
		}
	}
	else if(protocolMisc.step==2)
	{
        
	}

    if(netComps.St._bit.noResponse||netComps.St._bit.windowTimeOut||netComps.St._bit.err||netComps.St._bit.noIP
	   ||protocolComps.sw._bit.cmd_shutDown||protocolComps.sw._bit.AckTmrOut)
	
	{
        netComps.St._bit.off=1;
        
        netComps.St._bit.noResponse=0;
        netComps.St._bit.windowTimeOut=0;
        netComps.St._bit.err=0;
        netComps.St._bit.noIP=0;
        protocolComps.sw._bit.cmd_shutDown=0;
        protocolComps.sw._bit.AckTmrOut=0;
        netComps.op_window_tmr=0;
        
     }
	
}

static void protocolComps_report_callback(void)
{
    #ifdef   MD_EXT_MEASUREMENT_MODULE
       #if (MD_EXT_MEASUREMENT_MODULE_TYPE==MD_RAD)
           
       #else
           pressComps.sw._bit.on=1;
       #endif 
	#endif
}
static void protocolComps_task_handle(void)
{
	if(protocolMisc.event_index<0)
	{
		int i=0;
		for(i=0;i<sizeof(protocolComps.triggerIrq.All)*8;i++)
		{
			if((protocolComps.triggerIrq.All>>i)&1)
			{
				protocolMisc.event_index=i;
				if(!protocolMisc.event_index)
				{
                    device_comps.gps.sw._bit.isActive=1;
				}
                protocolMisc.step=0;
				protocolMisc.reRryTimes=1;
                protocolComps.sw._bit.DataRdy=0;
	            protocolComps.sw._bit.dataPushYet=0;
                protocolComps.sw._bit.dataPushYet1=0;
                protocolComps.sw._bit.isAckTmrOut=0;
                protocolComps.triggerIrq.All&=~((unsigned int)1<<protocolMisc.event_index);
                protocolComps_report_callback();
                break;
			}
		}
	}
	
    #ifdef   MD_EXT_MEASUREMENT_MODULE
       #if (MD_EXT_MEASUREMENT_MODULE_TYPE==MD_RAD)
          
       #else
           if( pressComps.sw._bit.on || pressComps.sw._bit.window_running )
			{
			    return;
			}
       #endif 
    #endif
	
if(protocolMisc.event_index>-1)
	{
		if(!netComps.St._bit.running)
		{
			netComps.St._bit.on=1;
			cs123x_comps.stop();
			pressComps.stop();//
			radComps.stop();
		}
		else
		{
 
			Srv_Protol(protocolMisc.event_index);
		}
    }
}


protocolComps_t protocolComps=
{
    {0},
    {0},
    (unsigned char *)&protocolMisc.buf,
    0,
    30,
    &protocolMisc.step,
    &protocolMisc.event_index,//unsigned int * const event_index_pt;
    protocolComps_task_handle
};

