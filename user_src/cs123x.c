#include "r_cg_macrodriver.h"
#include "r_cg_userdefine.h"
#include "r_cg_adc.h"
#include "r_cg_sau.h"
#include "hum.h"
#include "device.h"
#include "string.h"
#include "cs123x.h"
#include "r_cg_intp.h"






typedef unsigned char   uint8;
typedef long            int32;


static void Delay(int n)
{
   n*=1;
   while(n--);
}
static long formatAd(long addat)
{
   if(addat&((long)1<<23))
   {
     addat|=0xff000000;
   }
   addat>>=6;//24-6=18bit 2^17
   return (addat*10000/13108);
}

//static void cs1237_io_dir_in(void)
//{
//   MD_CS1237_DATA_DIR=1; 
//}
//static void cs1237_io_dir_out(void)
//{
//   MD_CS1237_DATA_DIR=0;   
//}
//static void cs1237_clk_high(void)
//{
//  MD_CS2137_CLK=1;
//}
//static void cs1237_clk_low(void)
//{
//  MD_CS2137_CLK=0;
//}
//static void cs1237_data_high(void)
//{
//  MD_CS1237_DATA=1;
//}
//static void cs1237_data_low(void)
//{
//  MD_CS1237_DATA=0;
//}
//static uint8 cs1237_read_io(void)
//{
//  return (uint8)MD_CS1237_DATA;
//}
//
//
//
//static void CS1237_Power_Down(void)
//{
//
//    cs1237_io_dir_in();
//	cs1237_clk_high();
//	Delay(2);//must big than 100us
//}
//
//static void CS1237_Restart(void)
//{
//	cs1237_io_dir_in();
//	cs1237_clk_low();
//	Delay(20);
//}
//
//
//
//static void CS1237_Clock(void)
//{
//    cs1237_clk_high();
//    NOP();NOP();NOP();NOP();NOP();
//    NOP();NOP();NOP();NOP();NOP();
//   
//    cs1237_clk_low();
//    NOP();NOP();NOP();NOP();NOP();
//    NOP();NOP();NOP();NOP();NOP();
//   
//}
//
//
//
//
//static uint8 CS1237_ReadReg(int32 *ad)
//{
//  uint8 i,command = 0x56;
//  
//  //for(i = 0; i < 27; i++)//clk1-clk27
//  //CS1237_Clock();
//
// // uint8 i;
//   int32 addat = 0;
//   for(i = 0; i < 24; i++) //
//   {
//     addat <<= 1;
//     CS1237_Clock();
//     if(cs1237_read_io())
//     addat ++;
//   }
//   CS1237_Clock();
//   CS1237_Clock();
//   CS1237_Clock(); //
//  
//
//  
// // cs1237_io_dir_out();//
//  CS1237_Clock();//clk28
//  CS1237_Clock();//clk29
//  cs1237_io_dir_out();///////////
//  for(i = 0; i < 7; i ++) //clk30-clk36
//  {
//    if(command & 0x40) //MSB -> LSB
//      cs1237_data_high();
//    else
//      cs1237_data_low();
//    command <<= 1;
//    CS1237_Clock();
//  }
//  cs1237_io_dir_in();//
//  CS1237_Clock();//clk37
//  command = 0;
//  for(i = 0; i < 8; i++) //clk38-clk45
//  {
//    command <<= 1;
//    CS1237_Clock();
//    if(cs1237_read_io())
//      command++;
//  }
//  CS1237_Clock();//clk46
//  *ad = formatAd(addat);
//  return command;
//}
//
//static void CS1237_WriteReg(int32 *ad ,uint8 adreg)
//{
//  uint8 i,command = 0x65;
//  
// // for(i = 0; i < 27; i++)//clk1-clk27
//  //CS1237_Clock();
//
//
//
//
//   int32 addat = 0;
//   for(i = 0; i < 24; i++) //
//   {
//     addat <<= 1;
//     CS1237_Clock();
//     if(cs1237_read_io())
//     addat ++;
//   }
//   CS1237_Clock();
//   CS1237_Clock();
//   CS1237_Clock(); //
//  
////cs1237_io_dir_out();//
//  CS1237_Clock();//clk28
//  CS1237_Clock();//clk29
//  cs1237_io_dir_out();//
//  for(i = 0; i < 7; i ++) //clk30-clk36
//  {
//    if(command & 0x40) //MSB -> LSB
//      cs1237_data_high();
//    else
//      cs1237_data_low();
//    command <<= 1;
//    CS1237_Clock();
//  }
//  CS1237_Clock();//clk37
//  
//  for(i = 0; i < 8; i++) //clk38-clk45
//  {
//    if(adreg & 0x80) //MSB -> LSB
//      cs1237_data_high();
//    else
//      cs1237_data_low();
//    adreg <<= 1;
//    CS1237_Clock();
//  }
//  cs1237_io_dir_in();//
//  CS1237_Clock();//clk46
//  *ad=formatAd(addat);
//}
//
//
//static int CS1237_ReadAD(int32 *ad)
//{
//  uint8 i;
//  int32 addat = 0;
//  for(i = 0; i < 24; i++) //
//  {
//    addat <<= 1;
//    CS1237_Clock();
//    if(cs1237_read_io())
//    addat ++;
//  }
//  CS1237_Clock();
//  CS1237_Clock();
//  CS1237_Clock(); //
// *ad = formatAd(addat);
//  return 1;
//}
//
//
//
//static int CS1237_Init(unsigned char dat)
//{
////      long ad;
////      unsigned char reg;
////      CS1237_Restart();
////      while(MD_CS1237_DATA);
////      CS1237_WriteReg(&ad,dat);        //
////      NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
////      
////      while(MD_CS1237_DATA);
////      reg=CS1237_ReadReg(&ad);
////      CS1237_Power_Down();
////      if(dat!=reg)
////      {
////        return 1;
////      }
////      else
////      {
////        return 0;
////      }
////      
//      long ad;
//      volatile long timer=150000;
//      unsigned char reg;
//      CS1237_Restart();
//      while(MD_CS1237_DATA)
//      {
//        timer--;
//        if(timer==0)
//        {
//            return 1;
//        }
//      }
//      CS1237_WriteReg(&ad,dat);        //
//      NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
//      
//      timer=150000;
//      while(MD_CS1237_DATA)
//      {
//        timer--;
//        if(timer==0)
//        {
//            return 1;
//        }
//      }
//      
//      reg=CS1237_ReadReg(&ad);
//      CS1237_Power_Down();
//      if(dat!=reg)
//      {
//        return 1;
//      }
//      else
//      {
//        return 0;
//      }
//}

//
//void cs1237_task_handle(void)
//{
//    long ad;
//    if(cs1237_comps.sw._bit.adc_updated)
//    {
//        if(device_comps.temp_p_pos<MD_ADC_MAX_POS)
//        {
//            if(hum_comps.current_mode==EM_CAL_MODIFY_MODE)
//            {
//                cs1237_comps.write_reg(&device_comps.temp_p_convert_result[device_comps.temp_p_pos++],0x5c);//40hze G=128 4c
//            }
//            else
//            {
//                cs1237_comps.write_reg(&device_comps.temp_p_convert_result[device_comps.temp_p_pos++],0x5c);//40hze G=128 4c
//            }
//        }
//        else
//        {
//            cs1237_comps.read_adc(&ad);
//        }
//        cs1237_comps.power_down();
//        cs1237_comps.sw._bit.adc_updated=0;
//        cs1237_comps.sw._bit.running=0;
//        if(!cs123x_comps.sw._bit.running)
//        {
//            MD_SET_REF_3030C_OFF;
//        }
//    }
//}
//
//
//
//
//cs1237_comps_t cs1237_comps=
//{
//    "",
//	&cs1237_comps,
//    0,   //	union sw;
//    CS1237_Power_Down,//void (*const power_down)(void); //
//	CS1237_Restart,//void (*const restart)(void);// 
//	CS1237_Init,//int (*const Init )(unsigned char dat);     
//	CS1237_ReadAD,//int (*const read_adc)(long *const p);   // 
//	CS1237_ReadReg, //unsigned char (*const read_reg )(int32 *ad);        
//	CS1237_WriteReg,//	void (*const write_reg )(long * ad,unsigned char dat); 
//	R_INTC1_Stop,//void (*const disale_interrup(void);
//	R_INTC1_Start,//void (*const enable_interrupt(void);
//	cs1237_task_handle //void  ( *const task_handle)(void);
//};
//
//
//





static void cs123x_io_dir_in(void)
{
   MD_CS123X_DATA_DIR=1; 
}
static void cs123x_io_dir_out(void)
{
   MD_CS123X_DATA_DIR=0;   
}
static void cs123x_clk_high(void)
{
  MD_CS213X_CLK=1;
}
static void cs123x_clk_low(void)
{
  MD_CS213X_CLK=0;
}
static void cs123x_data_high(void)
{
  MD_CS123X_DATA=1;
}
static void cs123x_data_low(void)
{
  MD_CS123X_DATA=0;
}
static uint8 cs123x_read_io(void)
{
  return (uint8)MD_CS123X_DATA;
}

static void CS123x_Power_Down(void)
{

    cs123x_io_dir_in();
	cs123x_clk_high();
	Delay(2);//must big than 100us
	
}
static void cs123x_stop(void)
{
	CS123x_Power_Down();
	cs123x_comps.sw._bit.running=0;
    MD_SET_REF_3030C_OFF;
}


static void CS123x_Restart(void)
{
	cs123x_io_dir_in();
	cs123x_clk_low();
	Delay(20);
}



static void CS123x_Clock(void)
{
    cs123x_clk_high();
    NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();
   
    cs123x_clk_low();
    NOP();NOP();NOP();NOP();NOP();
    NOP();NOP();NOP();NOP();NOP();
   
}




static uint8 CS123x_ReadReg(int32 *ad)
{
  uint8 i,command = 0x56;
  
  //for(i = 0; i < 27; i++)//clk1-clk27
  //CS123x_Clock();

 // uint8 i;
   int32 addat = 0;
   for(i = 0; i < 24; i++) //
   {
     addat <<= 1;
     CS123x_Clock();
     if(cs123x_read_io())
     addat ++;
   }
   CS123x_Clock();
   CS123x_Clock();
   CS123x_Clock(); //
  

  
 // cs123x_io_dir_out();//
  CS123x_Clock();//clk28
  CS123x_Clock();//clk29
  cs123x_io_dir_out();///////////
  for(i = 0; i < 7; i ++) //clk30-clk36
  {
    if(command & 0x40) //MSB -> LSB
      cs123x_data_high();
    else
      cs123x_data_low();
    command <<= 1;
    CS123x_Clock();
  }
  cs123x_io_dir_in();//
  CS123x_Clock();//clk37
  command = 0;
  for(i = 0; i < 8; i++) //clk38-clk45
  {
    command <<= 1;
    CS123x_Clock();
    if(cs123x_read_io())
      command++;
  }
    CS123x_Clock();//clk46
    *ad =formatAd(addat);
    return command;
}

static void CS123x_WriteReg(int32 *ad ,uint8 adreg)
{
  uint8 i,command = 0x65;
  
 // for(i = 0; i < 27; i++)//clk1-clk27
  //CS123x_Clock();




   int32 addat = 0;
   for(i = 0; i < 24; i++) //
   {
     addat <<= 1;
     CS123x_Clock();
     if(cs123x_read_io())
     addat ++;
   }
   CS123x_Clock();
   CS123x_Clock();
   CS123x_Clock(); //
  
//cs123x_io_dir_out();//
  CS123x_Clock();//clk28
  CS123x_Clock();//clk29
  cs123x_io_dir_out();//
  for(i = 0; i < 7; i ++) //clk30-clk36
  {
    if(command & 0x40) //MSB -> LSB
      cs123x_data_high();
    else
      cs123x_data_low();
    command <<= 1;
    CS123x_Clock();
  }
  CS123x_Clock();//clk37
  
  for(i = 0; i < 8; i++) //clk38-clk45
  {
    if(adreg & 0x80) //MSB -> LSB
      cs123x_data_high();
    else
      cs123x_data_low();
    adreg <<= 1;
    CS123x_Clock();
  }
  cs123x_io_dir_in();//
  CS123x_Clock();//clk46
  *ad = formatAd(addat);
}


static int CS123x_ReadAD(int32 *ad)
{
  uint8 i;
  int32 addat = 0;
  for(i = 0; i < 24; i++) //
  {
    addat <<= 1;
    CS123x_Clock();
    if(cs123x_read_io())
    addat ++;
  }
  CS123x_Clock();
  CS123x_Clock();
  CS123x_Clock(); //
  *ad = formatAd(addat);
  return 1;
}




static int CS123x_Init(unsigned char dat)
{
      long ad;
      volatile long timer=150000;
      unsigned char reg;
      CS123x_Restart();
      while(MD_CS123X_DATA)
      {
        timer--;
        if(timer==0)
        {
            return 1;
        }
      }
      CS123x_WriteReg(&ad,dat);        //
      NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
      
      timer=150000;
      while(MD_CS123X_DATA)
      {
        timer--;
        if(timer==0)
        {
            return 1;
        }
      }
      
      reg=CS123x_ReadReg(&ad);
      CS123x_Power_Down();
      if(dat!=reg)
      {
        return 1;
      }
      else
      {
        return 0;
      }
}


void cs123x_task_handle(void)
{
  long ad;
    if(cs123x_comps.sw._bit.adc_updated)
    {
        if(cs123x_comps.current_channel==0)
        {
            if(device_comps.ad1_pos<MD_ADC_MAX_POS)
            {
                if(hum_comps.current_mode==EM_CAL_MODIFY_MODE)
                {
                    cs123x_comps.write_reg(&device_comps.high_p_convert_result[device_comps.high_p_pos++],0x61|MD_CH1_GAIN);//
                    device_comps.ad1_convert_result[device_comps.ad1_pos++]=device_comps.high_p_convert_result[device_comps.high_p_pos-1];
                }
                else
                {
                    cs123x_comps.write_reg(&device_comps.high_p_convert_result[device_comps.high_p_pos++],0x61|MD_CH1_GAIN);//  
                    device_comps.ad1_convert_result[device_comps.ad1_pos++]=device_comps.high_p_convert_result[device_comps.high_p_pos-1];
                }
            }
            else
            {
                cs123x_comps.read_adc(&ad);
            }
            cs123x_comps.current_channel++;
            cs123x_comps.current_channel%=2;
            if(cs123x_comps.init_channel== cs123x_comps.current_channel)
            {
                cs123x_comps.stop();
               
            }
            else
            {
                cs123x_comps.enable_interrupt();
            }
        }
        else if(cs123x_comps.current_channel==1)
        {
            if(device_comps.temp_p_pos<MD_ADC_MAX_POS)
            {
                if(hum_comps.current_mode==EM_CAL_MODIFY_MODE)
                {
                    cs123x_comps.write_reg(&device_comps.temp_p_convert_result[device_comps.temp_p_pos++],0x60|MD_CH0_GAIN);//
                }
                else
                {
                    cs123x_comps.write_reg(&device_comps.temp_p_convert_result[device_comps.temp_p_pos++],0x60|MD_CH0_GAIN);//
                }
             }
             else
             {
                  cs123x_comps.read_adc(&ad);
             }
            cs123x_comps.current_channel++;
            cs123x_comps.current_channel%=2;
            if(cs123x_comps.init_channel== cs123x_comps.current_channel)
            {
                cs123x_comps.stop();
            }
            else
            {
                cs123x_comps.enable_interrupt();
            }
        }
        cs123x_comps.sw._bit.adc_updated=0;
    }
}



cs123x_comps_t cs123x_comps=
{
   	 "",
	&cs123x_comps,
	1,//const int init_channel;
	0,//int current_channel;
	0,   //	union sw;
	CS123x_Power_Down,//void (*const power_down)(void); //
	CS123x_Restart,//void (*const restart)(void);// 
	CS123x_Init,//int (*const Init )(unsigned char dat);     
	CS123x_ReadAD,//int (*const read_adc)(long *const p);   // 
	CS123x_ReadReg, //unsigned char (*const read_reg )(int32 *ad);        
	CS123x_WriteReg,//	void (*const write_reg )(long * ad,unsigned char dat); 
	R_INTC2_Stop,//void (*const disale_interrup(void);
	R_INTC2_Start,//void (*const enable_interrupt(void);
	cs123x_stop,
	cs123x_task_handle//void  ( *const task_handle)(void);
};


