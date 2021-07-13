#include "r_cg_macrodriver.h"
#include "r_cg_userdefine.h"
#include "r_cg_sau.h"
#include "r_cg_wdt.h"
#include "r_cg_rtc.h"
#include "r_cg_adc.h"
#include "r_cg_tau.h"
#include "r_cg_pclbuz.h"
#include "device.h"
#include "system.h"
#include "net.h"
#include "protocol.h"
#include "hum.h"
#include "lora.h"
#include "irc.h"
#include "24cxx.h"

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "cs123x.h"
#include "rad.h"
#include "press.h"

  
			
/////////////////////////////////////////get temperature////////////////////////////////////
/////////////////////////////////////////get temperature////////////////////////////////////

static const long temp_tab[] = 
{

	583542,  554647, 526968,  500480, 475159, 450974, 427897, 405892, 384927,  364967, //-55...-46
	345975,  327915, 310751,  294448, 278969, 264279, 250344, 237130, 224603,  212733, //-45...-36
	201487,  190836, 180750,  171201, 162163, 153610, 145516, 137858, 130614,  123761,//-35....-26

	117280,  111149, 105351,  99867 , 94681,   89776,  85137,  80750,  76600,   72676,  // -25 .........-16
	68963 ,  65451 ,  62129,  58986 , 56012,   53198,  50539,  48013,  45627,   43368,   // -15.......... -6
	41229 ,  39204 ,  37285,  35468 , 33747,   32116,  30570,  29105,  27716,   26399,   // -5............4
	25150 ,  23965 ,  22842,  21776 , 20764,   19783,  18892,  18026,  17204,   16423,   //  5.......... 14
	15681 ,  14976 ,  14306,  13669 , 13063,   12487,  11939,  11418,  10921,   10449,    //  15...........24
	10000 ,  9571  ,   9164,  8775  ,  8405,   8052 ,  7716,    7396,   7090,    6798,     // 25...........34
	6520  ,   6255 ,   6002,  5760  ,  5529,    5309,  5098,    4897,   4704,    4521,     //  35..........44
	4345  ,   4177 ,   4016,  3863  ,  3716,    3588,  3440,    3311,   3188,    3069,    // 45...........54
	2956  ,   2848 ,   2744,  2644  ,  2548,    2457,  2369,    2284,   2204,    2126,2051, //  55..........65
	1980  ,    1911,   1845,  1782  ,  1721,    1663,  1606,    1552,   1500, 1450,  //66-75
	1402  ,    1356,   1312,  1269  ,  1228,    1188,  1150,    1113,   1078, 1044,  //76-85
	1011  ,     979,    948,  919   ,   891,     863,   837,     811,    787,  763,   //86-95
	740   ,     718,    697,  676   ,   657,     637,   619,     601,    584,  567,   //96-105
	551   ,     535,    520,  505   ,   491,     477,   464,     451,    439,  427,   //106-115
	415   ,     404,    393,  383   ,   373,     363,   353,     344,    335,  326   //116-125

};

static const int pt100_tab[]=
{   
    
    7633 ,7673 ,7713 ,7752 ,7792 ,7832 ,7872 ,7911 ,7951 ,7991, //-60....-51
    8031 ,8070 ,8110 ,8150 ,8189 ,8229 ,8269 ,8308 ,8348 ,8388, 
    8427 ,8467 ,8506 ,8546 ,8585 ,8625 ,8664 ,8704 ,8743 ,8783, 
    8822 ,8862 ,8901 ,8940 ,8980 ,9019 ,9059 ,9093 ,9137 ,9177, 
    9216 ,9255 ,9295 ,9334 ,9373 ,9412 ,9452 ,9491 ,9530 ,9569, 
    9609 ,9648 ,9687 ,9726 ,9765 ,9804 ,9844 ,9883 ,9922 ,9961,//-10....-1
    
    10000,10039,10078,10117,10156,10195,10234,10273,10312,10351,//0......9
    10390,10429,10468,10507,10546,10585,10624,10663,10702,10740,
    10779,10818,10875,10896,10935,10973,11012,11051,11090,11128,
    11167,11206,11245,11283,11322,11361,11499,11438,11477,11515,
    11554,11593,11631,11670,11708,11747,11785,11824,11862,11901,
    11940,11978,12016,12055,12093,12132,12170,12209,12247,12286,
    12324,12362,12401,12439,12477,12516,12554,12592,12631,12669,
    12707,12745,12784,12822,12860,12898,12937,12975,13013,13051,
    13089,13127,13166,13204,13242,13280,13318,13356,13394,13432,
    13470,13508,13546,13584,13622,13660,13698,13736,13774,13812,
    13850,13888,13926,13964,14002,14039,14077,14115,14153,14191,
    14229,14266,14304,14342,14380,14417,14455,14493,14531,14568,
    14606,14644,14681,14719,14757,14794,14832,14870,14907,14945,
    14982,15020,15057,15095,15133,15170,15208,15245,15283,15320,
    15358,15395,15432,15470,15507,15545,15582,15619,15657,15694,
    15731,15769,15806,15843,15881,15918,15955,15993,16030,16067,
    16104,16142,16179,16216,16253,16290,16327,16365,16402,16439,
    16476,16513,16550,16587,16624,16661,16698,16735,16772,16809,
    16846,16883,16920,16957,16994,17031,17068,17105,17142,17179,
    17216,17253,17290,17326,17363,17400,17437,17474,17510,17547,
    17584,17621,17657,17694,17731,17768,17804,17841,17878,17914,
    17951,17988,18024,18061,18097,18134,18171,18207,18244,18280,
    18317,18353,18390,18426,18463,18499,18536,18572,18609,18645,
    18632,18718,18754,18791,18827,18863,18900,18936,18972,19009,
    19045,19081,19118,19154,19190,19226,19263,19299,19335,19371,
    19407,19444,19480,19516,19552,19588,19624,19660,19696,19733,
    19769,19805,19841,19877,19913,19949,19985,20021,20057,20093,
    20129,20165,20201,20236,20272,20308,20344,20380,20416,20452,
    20488,20523,20559,20595,20631,20667,20702,20738,20774,20810,
    20845,20881,20917,20952,20988,21024,21059,21095,21131,21166,
    21202,21237,21273,21309,21344,21380,21415,21451,21486,21522,
    21557,21593,21628,21664,21799,21735,21770,21805,21841,21876,
    21912,21947,21982,22018,22053,22088,22124,22159,22194,22229,
    22265,22300,22335,22370,22406,22441,22476,22511,22546,22581,
    22617,22652,22687,22722,22757,22792,22827,22862,22897,22932,
    22962,23002,23042,23082,23122,23162,23202,23242,23282,23322,
    23317,23352,23387,23422,23456,23491,23526,23561,23596,23631,
    23665,23700,23735,23770,23804,23839,23874,23909,23945,23978,
    24013,24047,24082,24117,24151,24186,24220,24255,24290,24324,
    24359,24393,24428,24462,24497,24531,24566,24600,24635,24669,
    24704,24738,24773,24807,24841,24876,24910,24945,24979,25013,
    25048,25082,25116,25150,25185,25219,25253,25255,25322,25356,
    25390,25424,25459,25493,25527,25561,25595,25629,25664,25698,
    25732,25766,25800,25834,25868,25902,25936,25970,26004,26038,
    26072,26106,26140,26174,26208,26242,26276,26310,26343,26377,
    26411,26445,26479,26513,26547,26580,26614,26648,26682,26715,
    26749,26783,26817,26850,26884,26918,26951,26985,27019,27052,
    27086,27120,27153,27187,27220,27254,27288,27321,27355,27388,
    27422,27455,27489,27522,27556,27589,27623,27656,27689,27723,
    27756,27790,27823,27856,27890,27923,27956,27990,28023,28056,
    28090,28123,28156,28189,28223,28256,28289,28322,28355,28389,
    28422,28455,28488,28521,28554,28587,28621,28654,28687,28720,
    28753,28786,28819,28852,28885,28918,28951,28984,29017,29050,
    29083,29116,29149,29181,29214,29247,29280,29313,29346,29379,
    29411,29444,29477,29510,29543,29575,29608,29641,29674,29706,
    29739,29772,29804,29837,29870,29902,29935,29968,30000,30033,
    30065,30098,30131,30163,30196,30228,30261,30293,30326,30358,
    30391,30423,30456,30488,30520,30553,30585,30618,30650,30682,
    30715,30747,30779,30812,30844,30876,30909,30941,30973,31005,
    31038,31070,31102,31134,31167,31199,31231,31263,31295,31327,//590...599

};

static void delay_us(int t)
{
	while(t>0)
	{
		NOP();NOP();
		t--;
	}
}



int read_time_seg_data_param(void *buf,int len )
{
    return _24cxx_comps.read(MD_TIME_SEG_DATA_PARAM_START_ADDR,buf,len);
}

int save_time_seg_data_param(void const *buf,int len )
{
     return _24cxx_comps.write(MD_TIME_SEG_DATA_PARAM_START_ADDR,buf,len);
}

int read_system_time(void *buf,int len )
{
    return _24cxx_comps.read(MD_SYSTEM_TIME_START_ADDR,buf,len);
}

int save_system_time(void const *buf,int len )
{
     return _24cxx_comps.write(MD_SYSTEM_TIME_START_ADDR,buf,len);
}


int read_calibration_param(void *buf,int len )
{
    return _24cxx_comps.read(MD_CALIBRATION_PARAM_START_ADDR,buf,len);
}
int save_calibration_param(void const *buf,int len )
{
    return _24cxx_comps.write(MD_CALIBRATION_PARAM_START_ADDR,buf,len);
}

int read_res_calibration_param(void *buf,int len )
{
    return _24cxx_comps.read(MD_RES_CALIBRATION_PARAM_START_ADDR,buf,len);
}
int save_res_calibration_param(void const *buf,int len )
{
    return _24cxx_comps.write(MD_RES_CALIBRATION_PARAM_START_ADDR,buf,len);
}

int read_high_calibration_param(void *buf,int len )
{
    return _24cxx_comps.read(MD_HIGH_CALIBRATION_PARAM_START_ADDR,buf,len);
}
int save_high_calibration_param(void const *buf,int len )
{
    return _24cxx_comps.write(MD_HIGH_CALIBRATION_PARAM_START_ADDR,buf,len);
}


static int read_device_coe(void *buf,int len )
{
    return _24cxx_comps.read(MD_DEVICE_COE_START_ADDR,buf,len);
}

int save_device_coe(void const *buf,int len )
{
    return _24cxx_comps.write(MD_DEVICE_COE_START_ADDR,buf,len);
}


static int read_gps_loc(void *buf,int len )
{
    return _24cxx_comps.read(MD_GPS_INFO_START_ADDR,buf,len);
}

static int save_gps_loc(void const *buf,int len )
{
    return _24cxx_comps.write(MD_GPS_INFO_START_ADDR,buf,len);
}

static int read_manufacturer_info(void *buf,int len )
{
    return _24cxx_comps.read(MD_MANUFACTURER_INFO_START_ADDR,buf,len);
}

static int save_manufacturer_info(void const *buf,int len )
{
    return _24cxx_comps.write(MD_MANUFACTURER_INFO_START_ADDR,buf,len);
}

static int read_device_info(void *buf,int len )
{
    return _24cxx_comps.read(MD_DEVICE_INFO_START_ADDR,buf,len);
}

static int save_device_info(void const *buf,int len )
{
    return _24cxx_comps.write(MD_DEVICE_INFO_START_ADDR,buf,len);
}

static int read_sensor_info(void *buf,int len )
{
    return _24cxx_comps.read(MD_SENSOR_INFO_START_ADDR,buf,len);
}

static int save_sensor_info(void const *buf,int len )
{
    return _24cxx_comps.write(MD_SENSOR_INFO_START_ADDR,buf,len);
}




int read_device_sn(void *buf,int len )
{
    return _24cxx_comps.read(MD_DEVICE_SN_START_ADDR,buf,len);
}

int save_device_sn(void const*buf,int len )
{
    return _24cxx_comps.write(MD_DEVICE_SN_START_ADDR,buf,len);
}
int read_level_param(void *buf,int len )
{
    return _24cxx_comps.read(MD_LEVEL_PARAM_START_ADDR           ,buf,len);
}

int save_level_param(void const *buf,int len )
{
    return _24cxx_comps.write(MD_LEVEL_PARAM_START_ADDR           ,buf,len);
}

int read_report_param(void *buf,int len)
{
     return _24cxx_comps.read(MD_REPORT_PARAM_START_ADDR,buf,len);
}
int save_report_param(void const *buf,int len)
{
    return _24cxx_comps.write(MD_REPORT_PARAM_START_ADDR,buf,len);
}

     
int read_access_param(void *buf,int len)
{
     return _24cxx_comps.read(MD_ACCESS_PARAM_START_ADDR,buf,len);
}
int save_access_param(void const *buf,int len)
{
     return _24cxx_comps.write(MD_ACCESS_PARAM_START_ADDR,buf,len);
}


int read_alarm_param(void *buf,int len)
{
    return _24cxx_comps.read(MD_ALARM_PARAM_START_ADDR,buf,len);
}
 
int save_alarm_param(void const *buf,int len)
{
    return _24cxx_comps.write(MD_ALARM_PARAM_START_ADDR,buf,len);
}

  
int read_device_addr(void *buf,int len)
 {
     return _24cxx_comps.read(MD_DEVICE_ADDR_START_ADDR,buf,len);
 }
 int save_device_addr(void const *buf,int len)
 {
    return _24cxx_comps.write(MD_DEVICE_ADDR_START_ADDR,buf,len);
 }


int read_device_sensor(void *buf,int len)
{
return _24cxx_comps.read(MD_DEVICE_SENSOR_START_ADDR,buf,len);
}
int save_device_sensor(void const *buf,int len)
{
return _24cxx_comps.write(MD_DEVICE_SENSOR_START_ADDR,buf,len);
}

static void start_buzzer(int timer)
{
    device_comps.buzzer.timer=timer;
    device_comps.buzzer.sw._bit.on=1;
    device_comps.buzzer.sw._bit.running=1;
    
}
static void stop_buzzer(void)
{
    P0 &= 0xFBU;
    PM0 |= ~0xFBU;
    R_PCLBUZ0_Stop();
}
 static void get_batt(void)
 {
     unsigned int adc;
     MD_SET_BAT_CTL_ON;
     delay_us(30);
     ADCE=0;
     delay_us(2);
     ADM2|=0x80;//switch internal vref
     ADM2&=~0x40;  //1.45V
     ADS = _10_AD_INPUT_CHANNEL_16;
     delay_us(10);
     ADCE=1;
     delay_us(2);
     
 
     device_comps.sw._bit.adc_busy=1;
     R_ADC_Start();
     while(device_comps.sw._bit.adc_busy);
     R_ADC_Get_Result((uint16_t *)&adc);
     MD_SET_BAT_CTL_OFF;
     ADCE=0;
     
     ADCE=0;       //switch extern vref
     delay_us(2);
     ADM2&=~0x80;
     ADM2|=0x40;
     delay_us(2);
		       
     
     
   #if(MD_DEVICE_BATT ==MD_3_6V)
     device_comps.batt=(unsigned long)435*adc/10230;//0.1V  20k 10k
   #elif(MD_DEVICE_BATT ==MD_4_5V)
     device_comps.batt=(unsigned long)580*adc/10230;//0.1V  30k 10k
   #endif
   hum_comps.dis_oper_mark._bit.refresh_batt_symbol=1;
 }



/////start ntc temp

static  long calc_ad3_average(device_comps_t *const this)
{
    int   i=0;
    const int count=this->ad3_pos;
    unsigned long average=0;
    if(!count)
    {
        return 0;
    }
	for(i=0;i<count;i++)
	{
		average+=this-> ad3_convert_result[i];
	}
    average=average*10/count;	
    //free (period);
	return average;
}

static long calc_ntc_valve(device_comps_t *const this)
{
	long  r_std=10000;
	long  ad3_average=this->ad3_average_result;
	long  ntc_valve=(r_std*ad3_average)/(10231-ad3_average);
	return ntc_valve;
}

static int calc_temp(device_comps_t  *const this)
{
    long Tem;
    long LowRValue;
    long HighRValue;        
    long   Tem0;
    int i;
    int  cBottom, cTop;
    int limit=sizeof(temp_tab)/sizeof(temp_tab[0])-1;
    

    if (this->ntc_valve >temp_tab[0])                // 电阻值小于表格最值低于量程下限。
    {
         return -550;
    }
    if (this->ntc_valve < temp_tab[limit])        // 电阻值大于表格最大值，超出量程上限 。
    {
        return 1250;
    }
    cBottom = 0; 
    cTop    = limit;
     for (i=limit/2; (cTop-cBottom)!=1; )        // 2分法查表。
    {

       if (this->ntc_valve > temp_tab[i])
        {
            cTop = i;
            i = (cTop + cBottom) / 2;
        }
        else if (this->ntc_valve < temp_tab[i])
        {
            cBottom = i;
            i = (cTop + cBottom) / 2;
        }
        else
        {
            Tem0 = i * 10 - 550;
            Tem = Tem0;
            goto ret;
        }
    }
    Tem0 = i * 10 - 550;
    LowRValue  =temp_tab[cBottom];
    HighRValue = temp_tab[cTop];
    Tem = ( ((this->ntc_valve - LowRValue)*10) / (HighRValue - LowRValue) ) + Tem0;        // 表格是以5度为一步的。
ret:
    Tem=Tem*(this->coe.temp/10000.);
    return Tem;
}


////////////////////////////////end get ntc temperature////////////////////////////////////
////////////////////////////////end get ntc temperature////////////////////////////////////



static long calc_temp_p_temp_n_average(device_comps_t *const this)
{
    int   i=0;
    const int count=this->temp_p_pos;
    long average=0;
	long difference[32]={0};//=malloc(count*sizeof(float));
	if(!count)
	{
	    return 0;
	}
	for(i=0;i<count;i++)
	{
		difference[i]=this-> temp_p_convert_result[i]-this-> temp_n_convert_result[i];
	}
    /*
    for(i=0;i<count-1;i++)
    {
        for(j=0;j<count-1-i;j++)
        {
        	if(difference[j]>difference[j+1])
        	{
        		temp_var=difference[j+1];
        		difference[j+1]=difference[j];
        		difference[j]=temp_var;
        	}
        }
    }
    */
    for(i=0;i<count;i++)
	{
		average+=difference[i];
	}	
	average=(average/(count));
	//free (period);
	return average;

}

static long calc_pt_valve(device_comps_t *const this)
{
    
	long delta_y=this->res_calibration_param.y[1]-this->res_calibration_param.y[0];
	long delta_x=this->res_calibration_param.x[1]-this->res_calibration_param.x[0];
    long pt_valve=(long long)delta_y*(this->temp_p_temp_n_average_result-this->res_calibration_param.x[0])/delta_x+this->res_calibration_param.y[0];
	return pt_valve;

}

static int calc_pt_temp(device_comps_t  *const this)
{
    long Tem;
    long LowRValue;
    long HighRValue;        
    long   Tem0;
    int i;
    int  cBottom, cTop;
    int  limit=sizeof(pt100_tab)/sizeof(pt100_tab[0])-1;
    

    if (this->pt_valve <pt100_tab[0])                // 电阻值小于表格最值低于量程下限。
    {
         return -600;
    }
    if (this->pt_valve >pt100_tab[limit])        // 电阻值大于表格最大值，超出量程上限 。
    {
        return 5990;
    }
    cBottom = 0; 
    cTop    = limit;
     for (i=limit/2; (cTop-cBottom)!=1; )        // 2分法查表。
    {

       if (this->pt_valve <pt100_tab[i])
        {
            cTop = i;
            i = (cTop + cBottom) / 2;
        }
        else if (this->pt_valve > pt100_tab[i])
        {
            cBottom = i;
            i = (cTop + cBottom) / 2;
        }
        else
        {
            Tem0 = i * 10 - 600;
            Tem = Tem0;
            goto ret;
        }
    }
    Tem0 = i * 10 - 600;
    LowRValue  =pt100_tab[cBottom];
    HighRValue = pt100_tab[cTop];
    Tem = ( ((this->pt_valve - LowRValue)*10) / (HighRValue - LowRValue) ) + Tem0;        // 表格是以5度为一步的。
ret:
    Tem=Tem*this->coe.temp/10000;
    return Tem;


}



static long calc_high_p_high_n_average(device_comps_t *const this)
{
    int   i=0;
    const int count=this->high_p_pos;
    long average=0;
	long difference[32]={0};//=malloc(count*sizeof(float));
	if(!count)
    {
        return 0;
    }
	for(i=0;i<count;i++)
	{
		difference[i]=this-> high_p_convert_result[i]-this-> high_n_convert_result[i];
	}
    /*
    for(i=0;i<count-1;i++)
    {
        for(j=0;j<count-1-i;j++)
        {
        	if(difference[j]>difference[j+1])
        	{
        		high_var=difference[j+1];
        		difference[j+1]=difference[j];
        		difference[j]=high_var;
        	}
        }
    }
    */
    for(i=0;i<count;i++)
	{
		average+=difference[i];
	}	
	average=(average/(count));
	//free (period);
	return average;

}


static long calc_current_high(device_comps_t *const this)
{
    
	long delta_y=this->high_calibration_param.y[1]-this->high_calibration_param.y[0];
	long delta_x=this->high_calibration_param.x[1]-this->high_calibration_param.x[0];
	long high=(long)((long long)delta_y*(this->high_p_high_n_average_result-this->high_calibration_param.x[0])/delta_x)+this->high_calibration_param.y[0];
	if(high<0) 
	{
		high=0;
	}
	return high;
}

static long calc_current_volume(device_comps_t *const this)
{
    
	return this->current_high/10 *this->level_param.bottom_s;
}


/////////////////////////////////////////get press////////////////////////////////////

static long  calc_ad1_ad2_average(device_comps_t *const this)
{
    int   i=0;
    const int count=this->ad1_pos;
    long average=0;
	long difference[32]={0};//=malloc(count*sizeof(float));
	if(!count)
    {
        return 0;
    }
	for(i=0;i<count;i++)
	{
		difference[i]=this-> ad1_convert_result[i]-this-> ad2_convert_result[i];
	}
    /*for(i=0;i<count-1;i++)
    {
        for(j=0;j<count-1-i;j++)
        {
        	if(difference[j]>difference[j+1])
        	{
        		temp_var=difference[j+1];
        		difference[j+1]=difference[j];
        		difference[j]=temp_var;
        	}
        }
    }
    */
    for(i=0;i<count;i++)
	{
		average+=difference[i];
	}	
	average=(average/(count));
	//free (period);
	return average;

}

static long calc_current_4_20ma(device_comps_t *const this)
{
    long current=this->ad1_ad2_average_result/65535.*1500/47*1000;//3fix dot
         current=current*this->coe.current/10000;
         if(current<0)
         {
            current=0;
         }
         return current;

}
static  long calc_current_press(device_comps_t *const this)
{
    long delta_v= this->ad1_ad2_average_result;
	long press=0;

    long  LowTValue;
    long  HighTValue;        
    int   i;
    int  Bottom=0;
    int  Top=sizeof(this->calibration_param.x)/sizeof(this->calibration_param.x[0])-1; //cal 4 points 0-3
    if (delta_v<=device_comps.calibration_param.x[Bottom])               
    {
       // Top=Bottom+1;
       // goto insert_calc;//jmp 2 points insert_calc code
        return press=device_comps.calibration_param.y[Bottom];
    }
    else if (delta_v>device_comps.calibration_param.x[Top])       
    {
        Bottom=Top-1;
        goto insert_calc;
    }
	i=Top/2;
	while(Top-Bottom>1)
	{
        if (delta_v<device_comps.calibration_param.x[i])
        {
            Top = i;
            i = (Top + Bottom) / 2;
        }
        else if (delta_v >device_comps.calibration_param.x[i])
        {
            Bottom = i;
            i = (Top + Bottom) / 2;
        }
        else
        {
            press = device_comps.calibration_param.y[i];
            goto  ret;
        }
    }
insert_calc:
{
    
    LowTValue  = device_comps.calibration_param.x[Bottom];
    HighTValue = device_comps.calibration_param.x[Top];
    press =(long)(
    ((long long) (delta_v-LowTValue)*(device_comps.calibration_param.y[Top]-device_comps.calibration_param.y[Bottom]))
    /(HighTValue - LowTValue)
    )
    +device_comps.calibration_param.y[Bottom];
}
ret:
    press-=this->coe.press_clr_value;
	if(press>this->calibration_param.y[3]*1/100)//>fs*1/100.
	{
	
	}
	else
	{
		press=0;
	}
	return press*(this->coe.press/10000.);
}


//Algorithm: The actual measured value is less than 4% of full scale and can be cleared. 
//The actual value = current value + cleared value. 
//Because the current value = actual value-clear value

static int clr_press(void)
{
    long actual_value=device_comps.current_press+device_comps.coe.press_clr_value;
    int ret=0;
    if(actual_value>device_comps.calibration_param.y[3]*4/100)//
    {
        ret=1;
    }
    else
    {
        device_comps.coe.press_clr_value=actual_value;
        device_comps.coe.cs=Check_Sum_5A(&device_comps.coe, &device_comps.coe.cs-(unsigned char *)&device_comps.coe);
        device_comps.save_coe(&device_comps.coe,sizeof(device_comps.coe));
    }
    return ret;
}
////////////////////////////////end get press////////////////////////////////////



static void device_comps_output_debug_info(device_comps_t const *const this)
{
//	static int line_num=0;
//	int tx_num=0;
//	memset(this->debug_info,0,sizeof(this->debug_info));
//	if(line_num==0)
//	{
		
//		//start output attribute name(title)
//		sprintf(this->debug_info+strlen(this->debug_info),"AD1-AD2\t\t");//
//		sprintf(this->debug_info+strlen(this->debug_info),"ad2_pos\t\t");//
//		sprintf(this->debug_info+strlen(this->debug_info),"AD3\t\t");//signal_ferq_from_timer_ch0 extern event counter 
//		sprintf(this->debug_info+strlen(this->debug_info),"ad3_pos\r\n");//signal_freq
//		//end output attribute name(title)
//	}
//	else
//	{
//		sprintf(this->debug_info+strlen(this->debug_info),"%05ld\t\t",this->ad1_ad2_average_result);
//		sprintf(this->debug_info+strlen(this->debug_info),"%05ld\t\t",this->ad2_pos);
//		sprintf(this->debug_info+strlen(this->debug_info),"%05ld\t\t",this->ad3_average_result);
//		sprintf(this->debug_info+strlen(this->debug_info),"%05ld\r\n",this->ad3_pos);
		
//	}
//	line_num++;
//	if(line_num>=10)//Output attribute name(title) every 50 lines
//	{
//		line_num=0; 
//	}
//	tx_num=strlen(this->debug_info);
//	if(tx_num>=sizeof(this->debug_info))
//	{
//		memset(this->debug_info,0,sizeof(this->debug_info));
//		sprintf(this->debug_info,"Write sensor debug output buffer overflow\r\n"); 
//		tx_num=strlen(this->debug_info);
//	}
//	ircComps.write(this->debug_info,tx_num);
	
	
}


int get_gps_info_from_net(char const *loc)
{
    char *endptr;
    long  glat;
    long  glng;
    
    glat=strtol(loc,     &endptr,10)*100000;
    if(glat<0)
    {
       glat=glat+(-strtol(endptr+1,&endptr,10));
    }
    else
    {
        glat=glat+strtol(endptr+1,&endptr,10);
    }
    
    glng=strtol(endptr+1,&endptr,10)*100000;
    if(glng<0)
    {
        glng=glng+(-strtol(endptr+1,&endptr,10));
    }
    else
    {
        glng=glng+strtol(endptr+1,&endptr,10);
    }
    if(glat==0 && glng==0)
    {
        return 1;
    }
    device_comps.gps.loc_times++;
    if(device_comps.gps.loc_times<2)
    {
        return 1;
	}
	device_comps.gps.glat=glat;
	device_comps.gps.glng=glng;
	device_comps.gps.cs=Check_Sum_5A(&device_comps.gps, &device_comps.gps.cs-(unsigned char *)&device_comps.gps);
	device_comps.save_gps_loc(&device_comps.gps,sizeof(device_comps.gps));
	device_comps.gps.loc_times=0;
	return 0;
}


static unsigned char device_comps_init(device_comps_t *const this)
{
	/**************START E2PROM TEST********************/
	if(device_comps.sw._bit.e2prom_driver_err)	
	{
		char msg[32]="";// ep2rom low 32bytes use test
		_24cxx_comps.write(MD_MEMORY_TEST_START_ADDR,"this is a e2prom driver test",strlen("this is a e2prom driver test"));
		_24cxx_comps.read (MD_MEMORY_TEST_START_ADDR,msg,strlen("this is a e2prom driver test"));
		if(!strcmp(msg,"this is a e2prom driver test"))
		{
			device_comps.sw._bit.e2prom_driver_err=0;	
		}
		else
		{
			device_comps.sw._bit.e2prom_driver_err=1;
		}
	}
	if(device_comps.sw._bit.cs123x_driver_err)	
	{
        unsigned char cfg=0x70;
        cs123x_comps.current_channel=cs123x_comps.init_channel;
        if(cs123x_comps.current_channel==0)
        {
            cfg=0x60|MD_CH0_GAIN;
        }
        else if(cs123x_comps.current_channel==1)
        {
            cfg=0x61|MD_CH1_GAIN;
        }
		if(!cs123x_comps.Init(cfg))
		{
            device_comps.sw._bit.cs123x_driver_err=0;
		}
		else
		{
            device_comps.sw._bit.cs123x_driver_err=1;
		}
	}

	if(device_comps.sw._bit.lora_module_err)	
	{
	    if(loraComps.sw._bit.init_ok)	
    	{
    	    device_comps.sw._bit.lora_module_err=0;
    	}
	}

    if(device_comps.sw._bit.rtc_module_err)	
	{
	    if(systemComps.sw._bit.is_xt1_running)	
    	{
    	    device_comps.sw._bit.rtc_module_err=0;
    	}
	}
          
		if(!(device_comps.sw.All&0x00ff))
    	{
    		return 0;
    	}
    	else
    	{
    		return 1;
    	}


}


//static void check_air_param()
//{
//    if(device_comps.4g_param.unit>3)
//    {
//        device_comps.4g_param.unit=0;
//    }
//    if(device_comps.4g_param.Constant_pressure_value>99999)
//    {
//        device_comps.4g_param.Constant_pressure_value=0;
//    }
//     if(device_comps.4g_param.buck_delta_value>99999)
//    {
//        device_comps.4g_param.buck_delta_value=0;
//    }
//    if(device_comps.4g_param.buck_percent>999)
//    {
//        device_comps.4g_param.buck_percent=0;
//    }
//    if(device_comps.4g_param.work_mode>2)
//    {
//        device_comps.4g_param.work_mode=0;
//    }
//     if(device_comps.4g_param.count_direction>1)
//    {
//        device_comps.4g_param.count_direction=0;
//    }
//    if(device_comps.4g_param.delay_timer>9999)
//    {
//        device_comps.4g_param.delay_timer=0;
//    }
//    if(device_comps.4g_param.timer>9999)
//    {
//        device_comps.4g_param.timer=0;
//    }
//}

static void read_all_param(struct _DEVICE_COMPONENTS  *const this)
{
    if(!device_comps.sw._bit.e2prom_driver_err)
    {
        if(!read_system_time(&device_comps.system_time,sizeof(device_comps.system_time)))
        {
            if(device_comps.system_time.cs!=Check_Sum_5A(&device_comps.system_time, &device_comps.system_time.cs-(unsigned char *)&device_comps.system_time))
            {
                device_comps.system_time.year=0x20;
                device_comps.system_time.month=1;
                device_comps.system_time.day=1;
                device_comps.system_time.hour=0;
                device_comps.system_time.min=0;
                device_comps.system_time.sec=0;
	    }
	    
	    rtc_valve.year=device_comps.system_time.year;
            rtc_valve.month=device_comps.system_time.month;
            rtc_valve.day=device_comps.system_time.day;
            rtc_valve.hour=device_comps.system_time.hour;
            rtc_valve.min=device_comps.system_time.min;
            rtc_valve.sec=device_comps.system_time.sec;
            R_RTC_Set_CounterValue(rtc_valve);	
        }
        
        if(!read_time_seg_data_param(&device_comps.TimeSegData,sizeof(device_comps.TimeSegData)))
        {
            if(device_comps.TimeSegData.cs!=Check_Sum_5A(&device_comps.TimeSegData, &device_comps.TimeSegData.cs-(unsigned char *)&device_comps.TimeSegData))
            {
                device_comps.TimeSegData.store_addr=MD_TIME_SEG_DATA_START_ADDR;
            }
        }

        if(!read_calibration_param(&device_comps.calibration_param,sizeof(device_comps.calibration_param)))
        {
            if(device_comps.calibration_param.cs!=Check_Sum_5A(&device_comps.calibration_param, & device_comps.calibration_param.cs-(unsigned char *)&device_comps.calibration_param))
            {
                device_comps.calibration_param.is_calibrated=0;
                memset(&device_comps.calibration_param,0,sizeof(device_comps.calibration_param));
            }
        }
        if(!read_res_calibration_param(&device_comps.res_calibration_param,sizeof(device_comps.res_calibration_param)))
        {
            if(device_comps.res_calibration_param.cs!=Check_Sum_5A(&device_comps.res_calibration_param, & device_comps.res_calibration_param.cs-(unsigned char *)&device_comps.res_calibration_param))
            {
                device_comps.res_calibration_param.is_calibrated=0;
                memset(&device_comps.res_calibration_param,0,sizeof(device_comps.res_calibration_param));
            }
        }
        if(!read_high_calibration_param(&device_comps.high_calibration_param,sizeof(device_comps.high_calibration_param)))
        {
            if(device_comps.high_calibration_param.cs!=Check_Sum_5A(&device_comps.high_calibration_param, & device_comps.high_calibration_param.cs-(unsigned char *)&device_comps.high_calibration_param))
            {
                device_comps.high_calibration_param.is_calibrated=0;
                memset(&device_comps.high_calibration_param,0,sizeof(device_comps.high_calibration_param));
            }
        }
//        if(!read_device_sn(&device_comps.4g_param,sizeof(device_comps.4g_param)))
//        {
//            if(device_comps.4g_param.cs!=Check_Sum_5A(&device_comps.4g_param, & device_comps.4g_param.cs-(unsigned char *)&device_comps.4g_param))
//            {
//                memset(&device_comps.4g_param,0,sizeof(device_comps.4g_param));
//            }
//            check_air_param();
//           
//        }
        if(!read_level_param(&device_comps.level_param,sizeof(device_comps.level_param)))
        {
            if(device_comps.level_param.cs!=Check_Sum_5A(&device_comps.level_param, & device_comps.level_param.cs-(unsigned char *)&device_comps.level_param))
            {
                device_comps.level_param.bottom_s=1000;//1.000
                device_comps.level_param.sample_interval_value=10;//
            }
        }
        if(!device_comps.read_coe(&device_comps.coe,sizeof(device_comps.coe)))
        {
            if(device_comps.coe.cs!=Check_Sum_5A(&device_comps.coe, &device_comps.coe.cs-(unsigned char *)&device_comps.coe))
            {
                device_comps.coe.press=10000;
                device_comps.coe.temp=10000;
                device_comps.coe.high=10000;
                device_comps.coe.current=10000;
                device_comps.coe.press_clr_value=0;
            }
            if(device_comps.coe.press<7000||device_comps.coe.press>13000)
            {
                device_comps.coe.press=10000;
            }
             if(device_comps.coe.temp<7000||device_comps.coe.temp>13000)
            {
                device_comps.coe.temp=10000;
            }
            if(device_comps.coe.press_clr_value>device_comps.calibration_param.y[3]*4/100)
            {
                device_comps.coe.press_clr_value=0;
            }
        }
        if(!read_alarm_param(&device_comps.alarm_param,sizeof(device_comps.alarm_param)))
        {
            if(device_comps.alarm_param.cs!=Check_Sum_5A(&device_comps.alarm_param, & device_comps.alarm_param.cs-(unsigned char *)&device_comps.alarm_param))
            {
                memset(&device_comps.alarm_param,0,sizeof(device_comps.alarm_param));
            }
        }
        
        if(!read_report_param(&device_comps.report_param,sizeof(device_comps.report_param)))
        {
            if(device_comps.report_param.cs!=Check_Sum_5A(&device_comps.report_param, &device_comps.report_param.cs-(unsigned char *)&device_comps.report_param))
            {
                device_comps.report_param.min=0;
                device_comps.report_param.hour=0;
                 device_comps.report_param.min_Interval=1440;
                device_comps.report_param.hour_Interval=25;
                device_comps.report_param.disFactor=60;
                device_comps.report_param.triggerTimes=0;
            }
        }

        if(!read_device_addr(&device_comps.device_addr,sizeof(device_comps.device_addr)))
        {
            if(device_comps.device_addr.cs!=Check_Sum_5A(&device_comps.device_addr, & device_comps.device_addr.cs-(unsigned char *)&device_comps.device_addr))
            {
                const unsigned char default_addr[7]={0x61,0x05,0x01,0x00,0x00,0x00,0x00};
                memcpy(&device_comps.device_addr.addr[0],default_addr,sizeof(device_comps.device_addr.addr));
            }
        }
        if(!read_device_sensor(&device_comps.device_sensor,sizeof(device_comps.device_sensor)))
        {
            if(device_comps.device_sensor.cs!=Check_Sum_5A(&device_comps.device_sensor, & device_comps.device_sensor.cs-(unsigned char *)&device_comps.device_sensor))
            {
                memset(&device_comps.device_sensor.ser_num[0],0,sizeof(device_comps.device_sensor.ser_num));
            }
        }
        if(!read_access_param(&device_comps.access_param,sizeof(device_comps.access_param)))
        {
            if(device_comps.access_param.cs!=Check_Sum_5A(&device_comps.access_param, &device_comps.access_param.cs-(unsigned char *)&device_comps.access_param))
            {
                memset(&device_comps.access_param,0,sizeof(device_comps.access_param));
            }
        }
        if(!read_gps_loc(&device_comps.gps,sizeof(device_comps.gps)))
        {
            if(device_comps.gps.cs!=Check_Sum_5A(&device_comps.gps, &device_comps.gps.cs-(unsigned char *)&device_comps.gps))
            {
                memset(&device_comps.gps,0,sizeof(device_comps.gps));
            }
        }
        if(!read_manufacturer_info(&device_comps.manufacturer_info,sizeof(device_comps.manufacturer_info)))
        {
            if(device_comps.manufacturer_info.cs!=Check_Sum_5A(&device_comps.manufacturer_info, &device_comps.manufacturer_info.cs-(unsigned char *)&device_comps.manufacturer_info))
            {
                memset(&device_comps.manufacturer_info,0,sizeof(device_comps.manufacturer_info));
            }
        }
        if(!read_device_info(&device_comps.device_info,sizeof(device_comps.device_info)))
        {
            if(device_comps.device_info.cs!=Check_Sum_5A(&device_comps.device_info, &device_comps.device_info.cs-(unsigned char *)&device_comps.device_info))
            {
                memset(&device_comps.device_info,0,sizeof(device_comps.device_info));
            }
        }
        if(!read_sensor_info(&device_comps.sensor_info,sizeof(device_comps.sensor_info)))
        {
            if(device_comps.sensor_info.cs!=Check_Sum_5A(&device_comps.sensor_info, &device_comps.sensor_info.cs-(unsigned char *)&device_comps.sensor_info))
            {
                memset(&device_comps.sensor_info,0,sizeof(device_comps.sensor_info));
            }
        }
         //device_comps.access_param.domain_name[48]=0;//add '\0'
    }
    else
    {
        device_comps.calibration_param.is_calibrated=0;
        device_comps.res_calibration_param.is_calibrated=0;
        device_comps.high_calibration_param.is_calibrated=0;
    }
    
}

//only use the highlow and low_upper 
static void pressOverloadReport(struct _DEVICE_COMPONENTS  *const this )
{
    long temp=0;
    if((this->calibration_param.unit&0x0f)==this->alarm_param.unit)
    {
        if(this->alarm_param.press_high_lower>0)
        {
            temp=formatData4fixDot(this->current_press,device_comps.calibration_param.dot);
            if(temp>this->alarm_param.press_high_lower)
            {
                this->PHihgOverTimer++;
                this->PHihgRealseTimer=0;
                if( this->PHihgOverTimer>=40)
                {
                    if(!this->sw._bit.isPHighOverTriggered)
                    {
                        if((device_comps.device_addr.addr[4]!=0)&&(device_comps.device_addr.addr[3]!=0))
            	        {
                            if(device_comps.batt>30)
                            {
                                protocolComps.triggerIrq._bit.PHighOver=1;
                                
                            }
            	        }
            	        this->sw._bit.isPHighRealseTriggered=0;
                        this->sw._bit.isPHighOverTriggered=1;
                    }
                    this->PHihgOverTimer=0;
                }
            }
            else if((temp<this->alarm_param.press_high_lower-this->alarm_param.press_high_lower/10)&&(this->sw._bit.isPHighOverTriggered))//dealt 10%
            {
                this->PHihgOverTimer=0;
                this->PHihgRealseTimer++;
                if( this->PHihgRealseTimer>=40)
                {
                    if(!this->sw._bit.isPHighRealseTriggered)
                    {
                        if((device_comps.device_addr.addr[4]!=0)&&(device_comps.device_addr.addr[3]!=0))
            	        {
                            if(device_comps.batt>30)
                            {
                              protocolComps.triggerIrq._bit.PHighRealse=1;
                               
                            }
            	        }
            	        this->sw._bit.isPHighRealseTriggered=1;
                        this->sw._bit.isPHighOverTriggered=0;
                    }
                   
                    this->PHihgRealseTimer=0;
                }
            }
        }

       if(this->alarm_param.press_low_upper>0)
       {
           temp=formatData4fixDot(this->current_press,device_comps.calibration_param.dot);
           if(temp<this->alarm_param.press_low_upper)
           {
               this->PLowLessTimer++;
               this->PLowRealseTimer=0;
               if( this->PLowLessTimer>=40)
               {
                   if(!this->sw._bit.isPLowLessTriggered)
                   {
                       if((device_comps.device_addr.addr[4]!=0)&&(device_comps.device_addr.addr[3]!=0))
                       {
                           if(device_comps.batt>30)
                           {
                               protocolComps.triggerIrq._bit.PLowLess=1;
                               
                           }
                       }
                       this->sw._bit.isPLowRealseTriggered=0;
                       this->sw._bit.isPLowLessTriggered=1;
                   }
                   this->PLowLessTimer=0;
               }
           }
           else if((temp>this->alarm_param.press_low_upper+this->alarm_param.press_low_upper/10)&&(this->sw._bit.isPLowLessTriggered))//dealt 10%
           {
               this->PLowLessTimer=0;
               this->PLowRealseTimer++;
               if(this->PLowRealseTimer>=40)
               {
                   if(!this->sw._bit.isPLowRealseTriggered)
                   {
                       if((device_comps.device_addr.addr[4]!=0)&&(device_comps.device_addr.addr[3]!=0))
                       {
                           if(device_comps.batt>30)
                           {
                              protocolComps.triggerIrq._bit.PLowRealse=1;
                              
                           }
                       }
                        this->sw._bit.isPLowRealseTriggered=1;
                        this->sw._bit.isPLowLessTriggered=0;
                   }
                   this->PLowRealseTimer=0;
               }
           }
       }
        
   }

}
static void device_comps_task_handle(void)//Execution interval is 200 ms
{
	
	device_comps_t *this=device_comps.this;
	if(this->do_init==1)
	{
		if(this->count<40)
		{
			if(!this->init(this))
			{
				this->do_init=0;
			}
			else
			{
				this->count++;
				hum_comps.dis_oper_mark._bit.refresh_err_code=1;
			}
			
		}
		else
		{
		    #ifdef  MD_IGNORE_ALL_ERR
			    this->do_init=0;
			#else
			    WDTE='R';
			#endif
		}
		if(this->do_init==0)
		{
           
			this->count=0;
			get_batt();
		   
			this->read_all_param(this);
			//TODO
		}
	}
	if(this->do_init==0&&hum_comps.dis_oper_mark._bit.test_ok)
	{
	    static int init_once=1;
	    if(init_once)
	    {
           
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
    		  init_once=0;
			 
		  }
		
    }
	
	//if((this->do_init==0)&&(!loraComps.sw._bit.runing))
	if( (this->do_init==0)&&(!netComps.St._bit.running)&&(!radComps.sw._bit.running))//&&(!loraComps.sw._bit.runing))
	{
	    int sample_en=0;
		if(this->count==5)//every 1s calc press and temperature
		{
            long ad1_ad3_temp=0;
			this->count=0;
			
            

			//NTC TEMP
			//ad1_ad3_temp=this->ad3_average_result;
            //this->ad3_average_result=this->calc_ad3_average(this);
            //this->ntc_valve=this->calc_ntc_valve(this);
           // this->current_temp_n_2=this->current_temp_n_1;
           // this->current_temp_n_1=this->current_temp;
           // this->current_temp=this->calc_temp(this);
            //ad1_ad3_temp-=this->ad3_average_result;


            #ifndef   MD_EXT_MEASUREMENT_MODULE
            //PT100 TEMP
            ad1_ad3_temp=this->temp_p_temp_n_average_result;
            this->temp_p_temp_n_average_result=this->calc_temp_p_temp_n_average(this);
            this->pt_valve=this->calc_pt_valve(this);
            this->current_temp_n_2=this->current_temp_n_1;
            this->current_temp_n_1=this->current_temp;
            this->current_temp=this->calc_pt_temp(this);
            ad1_ad3_temp-=this->temp_p_temp_n_average_result;
			
			if(ad1_ad3_temp<0)
			{
                ad1_ad3_temp=-ad1_ad3_temp;
			}
			if(ad1_ad3_temp>2)
			{
                 device_comps.sw._bit.temp_adc_stb=0;
			}
			else
			{
                 device_comps.sw._bit.temp_adc_stb=1;
			}
		    hum_comps.dis_oper_mark._bit.refresh_temp=1;
			hum_comps.dis_oper_mark._bit.refresh_temp_adc=1;
			

          
			ad1_ad3_temp=this->high_p_high_n_average_result;
            this->high_p_high_n_average_result=this->calc_high_p_high_n_average(this);
            this->current_high_n_2=this->current_high_n_1;
            this->current_high_n_1=this->current_high;
            this->current_high=this->calc_current_high(this);
            this->current_volume=calc_current_volume(this);
            ad1_ad3_temp-=this->high_p_high_n_average_result;
		    if(ad1_ad3_temp<0)
			{
                ad1_ad3_temp=-ad1_ad3_temp;
			}
			if(ad1_ad3_temp>2)
			{
                 device_comps.sw._bit.high_adc_stb=0;
			}
			else
			{
                 device_comps.sw._bit.high_adc_stb=1;
			}
			if(this->current_high>this->high_calibration_param.y[1])
			{
				device_comps.sw._bit.high_over_range=1;
			}
			else
			{
                device_comps.sw._bit.high_over_range=0;
			}
		    hum_comps.dis_oper_mark._bit.refresh_high=1;
		    hum_comps.dis_oper_mark._bit.refresh_volume=1;
			hum_comps.dis_oper_mark._bit.refresh_high_adc=1;
			
        

            ad1_ad3_temp=this->ad1_ad2_average_result;
			this->ad1_ad2_average_result=this->calc_ad1_ad2_average(this);
			this->ad1_ad2_average_result=this->ad1_convert_result[0];
			ad1_ad3_temp-=this->ad1_ad2_average_result;
			if(ad1_ad3_temp<0)
			{
                ad1_ad3_temp=-ad1_ad3_temp;
			}
			if(ad1_ad3_temp>2)
			{
                 device_comps.sw._bit.adc_stb=0;
                 
                 
			}
			else
			{
                 device_comps.sw._bit.adc_stb=1;
            }
			this->current_press_n_2=this->current_press_n_1;
			this->current_press_n_1=this->current_press;
			this->current_press=this->calc_current_press(this);
			if(this->current_press>this->calibration_param.y[3])
			{
				device_comps.sw._bit.over_range=1;
			}
			else
			{
                device_comps.sw._bit.over_range=0;
			}
		    hum_comps.dis_oper_mark._bit.refresh_press=1;
			hum_comps.dis_oper_mark._bit.refresh_press_adc=1;
			this->current_4_20ma=calc_current_4_20ma(this);
			
          #endif
			if(this->current_press>this->max_press)
			{
                this->max_press=this->current_press;
                hum_comps.dis_oper_mark._bit.refresh_press_max=1;
			}
            pressOverloadReport(this);
			
            this->ad3_pos=0;
			this->ad1_pos=0;
			this->ad2_pos=0;
			this->temp_p_pos=0;
			this->temp_n_pos=0;
			this->high_p_pos=0;
			this->high_n_pos=0;
			
	    }
			
		 
		 if(hum_comps.current_mode==EM_CAL_MODIFY_MODE)
		 {
             sample_en=1;
		 }  
         else
         {
            if(this->count==2 || this->count==4)
            {
                sample_en=1;
            }
           
         }
         if(sample_en)
         {


           #ifdef   MD_EXT_MEASUREMENT_MODULE
             #if (MD_EXT_MEASUREMENT_MODULE_TYPE==MD_RAD)
              radComps.start();
             #else
              pressComps.start();
             #endif   
		   #else 
			   MD_SET_REF_3030C_ON;
			   delay_us(400);//400
			   cs123x_comps.restart();
	           cs123x_comps.sw._bit.running=1;
	           cs123x_comps.enable_interrupt();
             
	       #endif
		   
		   //             
//	        
            
//            cs123x_comps.restart();
//            NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
//            while(MD_CS123X_DATA);
//           // cs123x_comps.write_reg(&this->ad1_convert_result[this->ad1_pos++],0x7d);
//           cs123x_comps.write_reg(&this->high_p_convert_result[this->high_p_pos++],0x7d);
//           this->ad1_convert_result[this->ad1_pos++]=this->high_p_convert_result[this->high_p_pos-1];
//            //cs123x_comps.read_adc(&this->ad1_convert_result[this->ad1_pos++]);
//            NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
//            while(MD_CS123X_DATA);
//            cs123x_comps.write_reg(&this->temp_p_convert_result[this->temp_p_pos++],0x70);
//            cs123x_comps.power_down();


//        	R_ADC_Set_OperationOff();
//	        delay_us(40);
//	        ADM2&=~0x80;
//	        ADM2|=0x40;
//	        delay_us(40);
		
//            ADS = _01_AD_INPUT_CHANNEL_1;
//	        R_ADC_Set_OperationOn();
//	        delay_us(2);
//		
//		device_comps.sw._bit.adc_busy=1;
//		R_ADC_Start();
//		while(device_comps.sw._bit.adc_busy);
//		R_ADC_Get_Result(&this->ad3_convert_result[this->ad3_pos++]);

		
	/*
		ADS = _10_AD_INPUT_CHANNEL_16;
		delay_us(2);
		device_comps.sw._bit.adc_busy=1;
		R_ADC_Start();
		while(device_comps.sw._bit.adc_busy);
		R_ADC_Get_Result(&this->ad1_convert_result[this->ad1_pos++]);

		
		
		ADS = _14_AD_INPUT_CHANNEL_20;
		delay_us(2);
		device_comps.sw._bit.adc_busy=1;
		R_ADC_Start();
		while(device_comps.sw._bit.adc_busy);
        	R_ADC_Get_Result(&this->ad2_convert_result[this->ad2_pos++]);


	        ADS = _10_AD_INPUT_CHANNEL_16;
		delay_us(2);
		device_comps.sw._bit.adc_busy=1;
		R_ADC_Start();
		while(device_comps.sw._bit.adc_busy);
		R_ADC_Get_Result(&this->temp_p_convert_result[this->temp_p_pos++]);

		
		
		ADS = _14_AD_INPUT_CHANNEL_20;
		delay_us(2);
		device_comps.sw._bit.adc_busy=1;
		R_ADC_Start();
		while(device_comps.sw._bit.adc_busy);
		R_ADC_Get_Result(&this->temp_n_convert_result[this->temp_n_pos++]);
        
      */ 
	//R_ADC_Set_OperationOff();
     //  MD_SET_REF_3030C_OFF;
		}
        this->count++;
	}
}



device_comps_t device_comps=
{
	"",                //char *desc;
    &device_comps,     //struct _DEVICE_COMPONENTS  *const this;
    1,//   int   do_init;//Whether to initialize,1:init 0,no init
    device_comps_init,//    unsigned char (*const init)(struct _DEVICE_COMPONENTS *);
    0,//    unsigned long count;            //Called counter


    {MD_E2PROM_DRIVER_ERR+MD_CS123X_DRIVER_ERR+MD_LORA_MODULE_ERR+MD_RTC_MODULE_ERR},  //union sw;  e2prom fault +cs123x err }, 
    
   0, //int PHihgOverTimer;
   0,// int PHihgRealseTimer;
   0, //int PLowLessTimer;
   0,// int PLowRealseTimer;

    {0},//    unsigned int  ad1_convert_result[MD_ADC_MAX_POS];
    0,//   unsigned int  ad1_pos;
    {0},//   unsigned int  ad2_convert_result[MD_ADC_MAX_POS];
    0,//   unsigned int  ad2_pos;
    0,//long          ad1_ad2_average_result;//(ad1-ad2)/ad1_pos

   {0}, //unsigned int  temp_p_convert_result[MD_ADC_MAX_POS];
	0,//unsigned int  temp_p_pos;
	{0},//unsigned int  temp_n_convert_result[MD_ADC_MAX_POS];
	0,//unsigned int  temp_n_pos;
	0,//long          temp_p_temp_n_average_result;//(temp_p_convert_result-temp_n_convert_result)/temp_p_pos

   {0},  //long  high_p_convert_result[MD_ADC_MAX_POS];
  0, // unsigned int  high_p_pos;
   {0}, //long  high_n_convert_result[MD_ADC_MAX_POS];
   0, //unsigned int  high_n_pos;
   0,// long          high_p_high_n_average_result;//(high_p_convert_result-high_n_convert_result)/high_p_pos

    {0}, //  unsigned int  ad3_convert_result[MD_ADC_MAX_POS];
    0, // unsigned int  ad3_pos;
    0,//unsigned long ad3_average_result

    0,//long current_4_20ma;


    10000,//     float ntc_valve;
    calc_ad3_average,//unsigned long ( * const calc_ad3_average)(struct _DEVICE_COMPONENTS  *const);
    calc_ntc_valve,    //float ( * const calc_ntc_valve)(struct _DEVICE_COMPONENTS  *const);//point to function calc_pt1000( device_comps_t *const this) whith param adc_result[][] 
    calc_temp,//      long  ( * const calc_temp)  (struct _DEVICE_COMPONENTS     *const);//point to function calc_temp( sensor_comp_t *const this)with param pt1000_valve

   20000,
    calc_temp_p_temp_n_average,
    calc_pt_valve,
    calc_pt_temp,
    0,//     long  current_temp;
    0,//     long  current_temp_n_1;
    0,//     long  current_temp_n_2;

    calc_high_p_high_n_average,//long ( *const calc_high_p_high_n_average)(struct _DEVICE_COMPONENTS *const ); //point to calc_signal_period(sensor_comp_t *const this)
    calc_current_high,//long ( *const calc_current_high)(struct _DEVICE_COMPONENTS *const ); //point to calc_signal_period(sensor_comp_t *const this)
    0,//long  current_volume
    0,//long  current_high;  //Yn
    0,//long  current_high_n_1;//Yn-1
    0,//long  current_high_n_2;//Yn-2    

   
	calc_ad1_ad2_average,//long ( *const calc_ad1_ad2_average)(struct _DEVICE_COMPONENTS *const ); //point to calc_signal_period(sensor_comp_t *const this)
	calc_current_press,//     unsigned long ( *const calc_current_press)(struct _DEVICE_COMPONENTS *const ); //point to calc_signal_period(sensor_comp_t *const this)
    0,//     long  current_press;  //Yn
    0,//long  max_press;
    0,//     long  current_press_n_1;//Yn-1
    0,//     long  current_press_n_2;//Yn-2

    {           //struct 
                //{ int press;
               //  int temp;  
        0        //   long press_clr_value 
    },         //}coe;
    read_device_coe,
    save_device_coe,  
    clr_press,

    {         //     struct 
            //    {
            //        union 
            //        {
            //        	unsigned char All;
       0     //        	struct
            //        	{
            //                unsigned char isLocSuc      :1;
            //                unsigned char isActive      :1;
            //            }_bit;
            //        }sw;
            //        long glng;
            //        long glat;
            //        unsigned char loc_times;
            //        unsigned char cs;
    },        //    }gps;
      read_gps_loc, //      int (*read_gps_loc)(void *,int );
      save_gps_loc,   //    int (*save_gps_loc)(void const *,int);
      get_gps_info_from_net,//int (*get_gps_info_from_net)(char const *);

     {     //struct 
           //{
      ""      //   char name[26];
            //   unsigned char cs;
     },      //}manufacturer_info;
     read_manufacturer_info,     //int (*read_manufacturer_info)(void *,int );
     save_manufacturer_info,      //int (*save_manufacturer_info)(void const *,int);
        
      {     //struct 
           //{
      ""      //   char type[26];
             //  char id[26];
             //  unsigned char cs;
      },     //}device_info;
      read_device_info,     //int (*read_device_info)(void *,int );
      save_device_info,     //int (*save_device_info)(void const *,int);
        
      {     //struct 
          // {
       ""     //   char id[26];
            //   unsigned char cs;
      },     //}sensor_info;
      read_sensor_info,     //int (*read_sensor_info)(void *,int );
      save_sensor_info,     //int (*save_sensor_info)(void const *,int);


     {           //             struct
                //             {
                //                 union 
                //                 {
                //                     unsigned char All;
                //                     struct
                //                     {
                //                         unsigned char on      :1;
                //                         unsigned char running :1;
                //                     }_bit;
    {0},        //                  }sw;
     0,         //                  int timer;
    start_buzzer,//                 void (*start)(int timer);
    stop_buzzer, //                 void (*stop)(void);
    },             //             }buzzer;
    

 
    0,//     unsigned int batt;//batt voltage

    {{0},{0},{0},0,0,0,0}, //struct calibration_param 
    {{0},{0},{0},0,0,0,0},//struct calibration_param_bak 
 	EM_CAL_PRESS,// cal_type
 	read_calibration_param,//     int (*read_calibration_param)(void *,int );
    save_calibration_param,//     int (*save_calibration_param)(void const *,int);


    {{0},{0},{0},0,0,0,0}, //struct res_calibration_param 
    {{0},{0},{0},0,0,0,0},//struct res_calibration_param_bak 
    read_res_calibration_param,//     int (*read_res_calibration_param)(void *,int );
    save_res_calibration_param,//     int (*save_res_calibration_param)(void const *,int);

    {{0},{0},{0},0,0,0,0}, //high_calibration_param_t  high_calibration_param;
    {{0},{0},{0},0,0,0,0},//high_calibration_param_t  high_calibration_param_bak;
    read_high_calibration_param,//int (*read_high_calibration_param)(void *,int );
     save_high_calibration_param,//  int (*save_high_calibration_param)(void const *,int);
    

    {0},   //struct report_parm;
    read_report_param,//  int (*read_report_param)(void *,int);
    save_report_param,//  int (*save_report_param)(void const *,int);

    {{0},0,0},   //struct access_parm;
    read_access_param,//   int (*read_access_param)(void *,int);
    save_access_param,//   int (*save_access_param)(void const *,int);


    {0},// struct alarm_param;
    read_alarm_param,// int (*read_alarm_param)(void *,int);
    save_alarm_param,//int (*save_alarm_param)(void const *,int);

    {{0},0},//unsigned char addr[7];
    read_device_addr,// int (*read_device_addr)(void *,int);
    save_device_addr,//int (*save_device_addr)(void const *,int);

    {{0},0},//device_sensor_t device_sensor;
    read_device_sensor,//int (*read_device_sensor)(void *,int);
	save_device_sensor,//int (*save_device_sensor)(void const *,int);

    { MD_TIME_SEG_DATA_START_ADDR,"",0},//TimeSegData_t TimeSegData;
    read_time_seg_data_param,
    save_time_seg_data_param,

    {0},
    read_system_time,
    save_system_time,

    {0},
    read_device_sn,
    save_device_sn,

    {0},//level_param_t  level_param;
	read_level_param,// int(*read_level_param)(void * , int);
	save_level_param,//int (*save_level_param)(void const * , int);

    
    0,//int report_interval_timer;
    0,//_1s_timr_acc

    read_all_param,//void (*read_all_param)(struct _DEVICE_COMPONENTS  *const);

	device_comps_output_debug_info,//void  ( *const output_debug_info)(struct _DEVICE_COMPONENTS   *const);//point to sensor_comps_output_debug_info(device_comps_t const *const this)
	"",                     //char debug_info[128];
	device_comps_task_handle,//void  ( *const task_handle)(void);//point to device_comps_task_handle
};




void CalcReportTime(unsigned char *hur,unsigned char *min,unsigned char *sec)
{
	unsigned long  DisPerTimeTotalSecond=(unsigned long)((device_comps.device_addr.addr[6]>>4)*10+(device_comps.device_addr.addr[6]&0x0f))* device_comps.report_param.disFactor;
	unsigned long  Min=DisPerTimeTotalSecond/60;
	unsigned long  DisPerTimehur=Min/60;
	unsigned long  DisPerTimeMin=Min%60;//min
	
	
	*sec=DisPerTimeTotalSecond%60;
	*min=(device_comps.report_param.min+DisPerTimeMin)%60;
	*hur=(device_comps.report_param.hour+DisPerTimehur+(device_comps.report_param.min+DisPerTimeMin)/60)%24;
	
	*sec=(*sec/10<<4)+(*sec%10);
	*min=(*min/10<<4)+(*min%10);
	*hur=(*hur/10<<4)+(*hur%10);
}

void GSMReturnTimeChk(unsigned char RHur,unsigned char RMin,unsigned char RSec)
{
	//unsigned int cmp=device_comps.report_param.hour_Interval;
	unsigned int cmp=device_comps.report_param.min_Interval;
	if(!cmp)
	{
        device_comps.report_interval_timer=0;
	    return ;
	}
    if(cmp<5)
	{
		cmp=5;
	}
	//if(device_comps.report_interval_timer>=(unsigned long)cmp*60)
	if(device_comps.report_interval_timer>=cmp)
    {
//		unsigned char reltSec=((RSec>>4)*10+(RSec&0x0f)) +60-((rtc_valve.sec>>4)*10+(rtc_valve.sec&0x0f));
//		unsigned char reltMin=((RMin>>4)*10+(RMin&0x0f)) +60-1-((rtc_valve.min>>4)*10+(rtc_valve.min&0x0f));
//		unsigned char reltHur=((RHur>>4)*10+(RHur&0x0f)) +24-1-((rtc_valve.hour>>4)*10+(rtc_valve.hour&0x0f));
//		unsigned char equSec= reltSec%60;
//		unsigned char equMin=(reltMin+(reltSec/60))%60;
//		unsigned char equHur=(reltHur+((reltMin+(reltSec/60))/60))%24;
//		unsigned long InterTimer=(unsigned long)equHur*3600+   (unsigned long)equMin*60    +(unsigned long)equSec;
//	     if(InterTimer>(unsigned long)5*60) 
        if(1)
		{
			 if((device_comps.device_addr.addr[4]!=0)&&(device_comps.device_addr.addr[3]!=0))
	        {
                if(device_comps.batt>30)
                {
                    protocolComps.triggerIrq._bit.intervalTime=1;
                }
	        }
	    }
	    device_comps.report_interval_timer=0;
	}
}

void  Timing_interval_report(void)
{
    unsigned char ReportHur;
	unsigned char ReportMin;
	unsigned char ReportSec;
	unsigned char ReportMin_30;
	unsigned char temp[7];
	
    CalcReportTime(&ReportHur,&ReportMin,&ReportSec);
    if(rtc_valve.sec == ReportSec)
	{	
		device_comps.report_interval_timer++;
        if((ReportHur==rtc_valve.hour)&&(ReportMin==rtc_valve.min))
        {
	        device_comps.report_interval_timer=0;
	        
	        if((device_comps.device_addr.addr[4]!=0)&&(device_comps.device_addr.addr[3]!=0))
	        {
                if(device_comps.batt>30)
                {
                    if(device_comps.report_param.hour && device_comps.report_param.min )
                    {
                        protocolComps.triggerIrq._bit.timeAuto=1;
                    }
                }
                
	        }
	        if(!device_comps.report_interval_timer && !device_comps.report_param.hour && !device_comps.report_param.min )
	        {
               if(rtc_valve.day==1 && rtc_valve.day==0x11 && rtc_valve.day==0x21)
               {
                    protocolComps.triggerIrq._bit.batteryBlunt=1;
               }
	        }
	     }
		GSMReturnTimeChk(ReportHur,ReportMin,ReportSec); //检查上报时间，如果到达指定上报时间，则系统开始上报数据//	

        ReportMin_30=ReportMin+0x30;
        if(ReportMin_30>=0x60)
        {
            ReportMin_30-=0x60;
        }
		if(rtc_valve.min == ReportMin  || rtc_valve.min == ReportMin_30)
		{
			if(device_comps.calibration_param.is_calibrated)
			{
				unsigned int StoreAddr=device_comps.TimeSegData.store_addr;
				
				if((StoreAddr>= MD_TIME_SEG_DATA_START_ADDR )&&(StoreAddr<=MD_TIME_SEG_DATA_END_ADDR-4))
				{
                    _24cxx_comps.write(StoreAddr,&device_comps.current_press,4);
					
					StoreAddr+=4;
					if(StoreAddr>=MD_TIME_SEG_DATA_END_ADDR)
					{
					        StoreAddr=MD_TIME_SEG_DATA_START_ADDR;
					}
					device_comps.TimeSegData.store_addr=StoreAddr;

					
                	temp[0]=0x20;
                	temp[1]=rtc_valve.year;
                	temp[2]=rtc_valve.month;
                	temp[3]=rtc_valve.day;
                	temp[4]=rtc_valve.hour;
                    temp[5]=rtc_valve.min;
                	temp[6]=rtc_valve.sec;
					
					memcpy(device_comps.TimeSegData.lastSampleTime,&temp,sizeof(device_comps.TimeSegData.lastSampleTime));
					device_comps.TimeSegData.cs=Check_Sum_5A(&device_comps.TimeSegData, &device_comps.TimeSegData.cs-(unsigned char *)&device_comps.TimeSegData);
					device_comps.save_time_seg_data_param(&device_comps.TimeSegData,sizeof(device_comps.TimeSegData));
				}
				else
				{
					StoreAddr=MD_TIME_SEG_DATA_START_ADDR;
					device_comps.TimeSegData.store_addr=StoreAddr;
					device_comps.TimeSegData.cs=Check_Sum_5A(&device_comps.TimeSegData, &device_comps.TimeSegData.cs-(unsigned char *)&device_comps.TimeSegData);
					device_comps.save_time_seg_data_param(&device_comps.TimeSegData,sizeof(device_comps.TimeSegData));
				}
			}
		}
	}
}



void _0_5s_task_handle(void)
{
    R_WDT_Restart();
    if(hum_comps.current_mode!=EM_SELF_TEST_MODE)
    {
        device_comps._0_5s_timr_acc++;
    	if(device_comps._0_5s_timr_acc>1)
    	{
            if( (ircComps.op_window_time>0)&&(hum_comps.current_mode!=EM_CAL_MODIFY_MODE))
    		{
    			ircComps.op_window_time--;
    			if(ircComps.op_window_time==0)
    			{
    				ircComps.stop();
    				
    			}
    		}
    		
       	    if(hum_comps.current_mode==EM_LORA_MODE||hum_comps.current_mode==EM_4G_MODE
	       ||hum_comps.current_mode==EM_NORMAL_MODE)
    	    {
    	        if(mode_comps[hum_comps.current_mode].displayTimer>0&&mode_comps[hum_comps.current_mode].dis_option!=0)
    	        {
    	            mode_comps[hum_comps.current_mode].displayTimer--;
    	            if(!mode_comps[hum_comps.current_mode].displayTimer)
    	            {
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
                                //enter_normal_mode();
                		  }
                		  #endif
    	            }
    	        }
    	    }

	        if(hum_comps.current_mode==EM_DEBUG_MODE)
    	    {
                if(mode_comps[hum_comps.current_mode].displayTimer>0)
    	        {
                    mode_comps[hum_comps.current_mode].displayTimer--;
                    if(!mode_comps[hum_comps.current_mode].displayTimer)
                    {
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
    	        }
    	    }
    	    
            if(netComps.op_window_tmr>0)
            {
                netComps.op_window_tmr--;
                if(!netComps.op_window_tmr)
                {
                    netComps.St._bit.windowTimeOut=1;
                }
            }
            if(protocolComps.AckTmr>0)
            {
                protocolComps.AckTmr--;
            }
            get_batt();
            

            R_RTC_Get_CounterValue(&rtc_valve);
        #if(MD_PRODUCT_NAME==MD_4G)
            if(hum_comps.current_mode==EM_4G_MODE)
            {
                Timing_interval_report();
            }
        #endif
            if(rtc_valve.hour==0x21&&rtc_valve.min==0x21&&rtc_valve.sec==0x21)
            {
                 //save systemtime();
				 device_comps.gps.sw._bit.isActive=1;
            }
            hum_comps.dis_oper_mark._bit.refresh_time=1;
            hum_comps.dis_oper_mark._bit.refresh_date=1; 


            if(radComps.op_window_time>0)
            {
                radComps.op_window_time--;
                if(!radComps.op_window_time)
                {
                    radComps.stop();
					device_comps.high_calibration_param.is_calibrated=0;
                   // R_TAU0_Channel1_Stop();
                }
                
            }

            
            device_comps._0_5s_timr_acc=0;
    	}

      /////////////////////////////////
        if(netComps.AckTmr>0)
        {
            netComps.AckTmr--;
        }

       
    }
}

void _50ms_task_handle(void)
{
    if(pressComps.op_window_time>0)
    {
        if(pressComps.op_window_time==3)//delay time=timer-20
        {
            MD_IR_VCM_ON;
		}
		if(pressComps.op_window_time==2)//delay time=timer-20
        {
           pressComps.write(0x901f);
        }
        pressComps.op_window_time--;
        if(!pressComps.op_window_time)
        {
            pressComps.stop();
            device_comps.calibration_param.is_calibrated=0;
        }
     }
     
    if(device_comps.buzzer.timer>0)
    {
       if(device_comps.buzzer.sw._bit.on)
       {
           R_PCLBUZ0_Start();
           P0 &= 0xFBU;
           PM0 &= 0xFBU;
           device_comps.buzzer.sw._bit.on=0;
       }
       device_comps.buzzer.timer--;
       if(!device_comps.buzzer.timer)
       {
            device_comps.buzzer.stop();
       }
    }
     
}

