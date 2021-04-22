#include "r_cg_macrodriver.h"
#include "r_cg_userdefine.h"
#include "r_cg_lcd.h"
#include "r_cg_sau.h"
#include "r_cg_rtc.h"
#include "r_cg_adc.h"
#include "system.h"
#include "hum.h"
#include "irc.h"
#include "lora.h"
#include "net.h"
#include "protocol.h"
#include "device.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "math.h"
//#define  static 
          //key_io

int       MD_KEY_VDD;
#define   MD_K1                 P4_bit.no1   
#define   MD_K2                 P12_bit.no2
#define   MD_K3                 P12_bit.no1
#define   MD_KEY_MASK           (int)0x07
#define   MD_KEY_DATA           ((MD_K1<<2)+(MD_K2<<1)+MD_K3)
#define   MD_S_KEY              0x03//011
#define   MD_M_KEY              0x05//101
#define   MD_J_KEY              0x06//110

//#define   MD_BACK_LED           P1_bit.no5
//#define   MD_BACK_LED_ON        MD_BACK_LED=1
//#define   MD_BACK_LED_OFF       MD_BACK_LED=0
//
//#define MD_GREEN_LIGHT    0
//#define MD_RED_LIGHT      0
//#define MD_SET_GREEN_LIGHT_ON    NOP()
//#define MD_SET_GREEN_LIGHT_OFF   NOP()
//#define MD_SET_RED_LIGHT_ON      NOP()
//#define MD_SET_RED_LIGHT_OFF     NOP()
//#define isLightOn(a)  (a)  


static void delay_us(int t)
{
	while(t>0)
	{
		NOP();NOP();
		t--;
	}
}

static void dis_all_unit()
{
    MD_DIS_T1_COMMUNICATION;
    MD_DIS_T2_KPA;
    MD_DIS_T3_PSI;
    MD_DIS_T4_BAR;
    MD_DIS_T5_MPA;
    MD_DIS_T6_CELSIUS;
    MD_DIS_T7_M;
    MD_DIS_T8_PA;
    MD_DIS_T9_KGF_CM_2;
    MD_DIS_T10_CELSIUS;
    MD_DIS_T11_MAX;
    MD_DIS_T12_MMH2O;
    MD_DIS_T13_MMHG;
    MD_DIS_T14_INH2O;
    MD_DIS_T15_INHG;
    MD_DIS_T16_V;
    MD_DIS_T17_MA;



    MD_DIS_P1_DOT;
    MD_DIS_P2_DOT;
    MD_DIS_P3_DOT;
    MD_DIS_P4_DOT;
    MD_DIS_P5_DOT;
    MD_DIS_P6_DOT;
    MD_DIS_P7_DOT;
    MD_DIS_P8_DOT;
    MD_DIS_P9_DOT;
    MD_DIS_P10_DOT;

    MD_DIS_COL;

    MD_DIS_D1_BAT;
    MD_DIS_D2_BAT;
    MD_DIS_D3_BAT;
    MD_DIS_D4_BAT;

    MD_DIS_N1_CSQ;
    MD_DIS_N2_CSQ;
    MD_DIS_N3_CSQ;
    MD_DIS_N4_CSQ;
    
}
static void hide_all_unit(void)
{
    MD_HIDE_T1_COMMUNICATION;
    MD_HIDE_T2_KPA;
    MD_HIDE_T3_PSI;
    MD_HIDE_T4_BAR;
    MD_HIDE_T5_MPA;
    MD_HIDE_T6_CELSIUS;
    MD_HIDE_T7_M;
    MD_HIDE_T8_PA;
    MD_HIDE_T9_KGF_CM_2;
    //MD_HIDE_T10_CELSIUS;
    MD_HIDE_T11_MAX;
    MD_HIDE_T12_MMH2O;
    MD_HIDE_T13_MMHG;
    MD_HIDE_T14_INH2O;
    MD_HIDE_T15_INHG;
    MD_HIDE_T16_V;
    MD_HIDE_T17_MA;

//    MD_HIDE_P1_DOT;
//    MD_HIDE_P2_DOT;
//    MD_HIDE_P3_DOT;
//    MD_HIDE_P4_DOT;
//    MD_HIDE_P5_DOT;
//    MD_HIDE_P6_DOT;
//    MD_HIDE_P7_DOT;
//    MD_HIDE_P8_DOT;
//    MD_HIDE_P9_DOT;
//    MD_HIDE_P10_DOT;
//
     MD_HIDE_COL;

    //MD_HIDE_D1_BAT;
    //MD_HIDE_D2_BAT;
    //MD_HIDE_D3_BAT;
    //MD_HIDE_D4_BAT;
    //MD_HIDE_N1_CSQ;
    //MD_HIDE_N2_CSQ;
    //MD_HIDE_N3_CSQ;
    //MD_HIDE_N4_CSQ;


}

void clr_lcd(void)
{
    MD_09_PIN_LCD_SEG04=0;
    MD_10_PIN_LCD_SEG05=0;   
    MD_11_PIN_LCD_SEG08=0;  
    MD_12_PIN_LCD_SEG09=0;   
    MD_13_PIN_LCD_SEG10=0;   
    MD_14_PIN_LCD_SEG11=0;   
    MD_15_PIN_LCD_SEG12=0;   
    MD_16_PIN_LCD_SEG13=0;   
    MD_17_PIN_LCD_SEG14=0;   
    MD_18_PIN_LCD_SEG15=0;   
    MD_19_PIN_LCD_SEG16=0;   
    MD_20_PIN_LCD_SEG17=0;   
    MD_21_PIN_LCD_SEG18=0;   
    MD_22_PIN_LCD_SEG19=0;   
    MD_23_PIN_LCD_SEG20=0;
}
static void MD_DISPLAY_WRITE_D0_0(unsigned char a)	//  D00  H   L 
{
    MD_09_PIN_LCD_SEG04&=0xf8;
    MD_HIDE_P10_DOT;
    MD_09_PIN_LCD_SEG04|=hum_comps.SEG_TAB[a]>>4;
   
    MD_20_PIN_LCD_SEG17&=0xf0;
    MD_20_PIN_LCD_SEG17|=hum_comps.SEG_TAB[a]&0x0f;
 
}
static void MD_DISPLAY_WRITE_D0_1(unsigned char a)//  D01  H   L 
{

    MD_21_PIN_LCD_SEG18&=0xf0;
    MD_21_PIN_LCD_SEG18|=hum_comps.SEG_TAB[a]>>4;
    if(hum_comps.dot0_pos==1)  MD_21_PIN_LCD_SEG18|=0x08;
    MD_22_PIN_LCD_SEG19&=0xf0;
    MD_22_PIN_LCD_SEG19|=hum_comps.SEG_TAB[a]&0x0f;
}



static void MD_DISPLAY_WRITE_D1_0(unsigned char a)	//  D02  H   L 
{
    MD_15_PIN_LCD_SEG12&=0xf8;
    MD_15_PIN_LCD_SEG12|=hum_comps.SEG_TAB[a]>>4;
    MD_16_PIN_LCD_SEG13&=0xf0;
    MD_16_PIN_LCD_SEG13|=hum_comps.SEG_TAB[a]&0x0f;
    
	
}
static void MD_DISPLAY_WRITE_D1_1(unsigned char a)	//  D03  H   L 
{
    MD_17_PIN_LCD_SEG14&=0xf0;
    MD_17_PIN_LCD_SEG14|=hum_comps.SEG_TAB[a]>>4;
    if(hum_comps.dot1_pos==1)  MD_17_PIN_LCD_SEG14|=0x08;
    MD_18_PIN_LCD_SEG15&=0xf0;
    MD_18_PIN_LCD_SEG15|=hum_comps.SEG_TAB[a]&0x0f;
}

static void MD_DISPLAY_WRITE_D1_2(unsigned char a)//  D04  H   L  
{

    MD_19_PIN_LCD_SEG16&=0xf0;
    MD_19_PIN_LCD_SEG16|=hum_comps.SEG_TAB[a]>>4;
    if(hum_comps.dot1_pos==2)  MD_19_PIN_LCD_SEG16|=0x08;
    MD_12_PIN_LCD_SEG09&=0xf0;
    MD_12_PIN_LCD_SEG09|=hum_comps.SEG_TAB[a]&0x0f;
	
}

                 //  D10  H   L  
 static void  MD_DISPLAY_WRITE_D1_3(unsigned char a)   
 {
    MD_11_PIN_LCD_SEG08&=0xf0;
    MD_11_PIN_LCD_SEG08|=hum_comps.SEG_TAB[a]>>4;
    if(hum_comps.dot1_pos==3)  MD_11_PIN_LCD_SEG08|=0x08;
    MD_10_PIN_LCD_SEG05&=0xf0;
    MD_10_PIN_LCD_SEG05|=hum_comps.SEG_TAB[a]&0x0f;
 }

 
 static void  MD_DISPLAY_WRITE_D2_0(unsigned char a) //  D11  H   L
 {
    MD_18_PIN_LCD_SEG15&=0x8f;
    MD_18_PIN_LCD_SEG15|=hum_comps.SEG_TAB[a]&0xf0;
    MD_17_PIN_LCD_SEG14&=0x0f;
    MD_17_PIN_LCD_SEG14|=hum_comps.SEG_TAB[a]<<4;
 }
 
 static void  MD_DISPLAY_WRITE_D2_1(unsigned char a) //  D12  H   L 
 {
    MD_16_PIN_LCD_SEG13&=0x0f;
    MD_16_PIN_LCD_SEG13|=hum_comps.SEG_TAB[a]&0xf0;
    if(hum_comps.dot2_pos==1)((hum_comps.cursor_2==-1)&&(hum_comps.dis_oper_mark._bit.cur2))?((hum_comps.dis_oper_mark._bit.dis2)?(MD_DIS_P9_DOT) :(MD_HIDE_P9_DOT)):(MD_DIS_P9_DOT);
    MD_14_PIN_LCD_SEG11&=0x0f;
    MD_14_PIN_LCD_SEG11|=hum_comps.SEG_TAB[a]<<4;
 }
 static void  MD_DISPLAY_WRITE_D2_2(unsigned char a) //  D11  H   L
 {
    MD_12_PIN_LCD_SEG09&=0x0f;
    MD_12_PIN_LCD_SEG09|=hum_comps.SEG_TAB[a]&0xf0;
   if(hum_comps.dot2_pos==2)((hum_comps.cursor_2==-1)&&(hum_comps.dis_oper_mark._bit.cur2))?((hum_comps.dis_oper_mark._bit.dis2)?(MD_DIS_P8_DOT) :(MD_HIDE_P8_DOT)):(MD_DIS_P8_DOT);
    MD_11_PIN_LCD_SEG08&=0x0f;
    MD_11_PIN_LCD_SEG08|=hum_comps.SEG_TAB[a]<<4;
 }
 static void  MD_DISPLAY_WRITE_D2_3(unsigned char a) //  D12  H   L 
 {
    MD_10_PIN_LCD_SEG05&=0x0f;
    MD_10_PIN_LCD_SEG05|=hum_comps.SEG_TAB[a]&0xf0;
    if(hum_comps.dot2_pos==3) ((hum_comps.cursor_2==-1)&&(hum_comps.dis_oper_mark._bit.cur2))?((hum_comps.dis_oper_mark._bit.dis2)?(MD_DIS_P7_DOT) :(MD_HIDE_P7_DOT)):(MD_DIS_P7_DOT);
    MD_09_PIN_LCD_SEG04&=0x0f;
    MD_09_PIN_LCD_SEG04|=hum_comps.SEG_TAB[a]<<4;
 }
 static void  MD_DISPLAY_WRITE_D2_4(unsigned char a) //  D12  H   L 
 {
    MD_20_PIN_LCD_SEG17&=0x0f;
    MD_20_PIN_LCD_SEG17|=hum_comps.SEG_TAB[a]&0xf0;
    if(hum_comps.dot2_pos==4) ((hum_comps.cursor_2==-1)&&(hum_comps.dis_oper_mark._bit.cur2))?((hum_comps.dis_oper_mark._bit.dis2)?(MD_DIS_P6_DOT) :(MD_HIDE_P6_DOT)):(MD_DIS_P6_DOT);
    MD_21_PIN_LCD_SEG18&=0x0f;
    MD_21_PIN_LCD_SEG18|=hum_comps.SEG_TAB[a]<<4;
 }
 static void  MD_DISPLAY_WRITE_D2_5(unsigned char a) //  D12  H   L 
  {
    MD_22_PIN_LCD_SEG19&=0x0f;
    MD_22_PIN_LCD_SEG19|=hum_comps.SEG_TAB[a]&0xf0;
    if(hum_comps.dot2_pos==5) ((hum_comps.cursor_2==-1)&&(hum_comps.dis_oper_mark._bit.cur2))?((hum_comps.dis_oper_mark._bit.dis2)?(MD_DIS_P5_DOT) :(MD_HIDE_P5_DOT)):(MD_DIS_P5_DOT);
    MD_23_PIN_LCD_SEG20&=0x0f;
    MD_23_PIN_LCD_SEG20|=hum_comps.SEG_TAB[a]<<4;
  }


static void MD_DISPLAY_WRITE_D3_0(unsigned char a)  //  D02  H   L 
 {
     MD_15_PIN_LCD_SEG12&=0xf8;
     MD_15_PIN_LCD_SEG12|=hum_comps.SEG_TAB[a]>>4;
     MD_16_PIN_LCD_SEG13&=0xf0;
     MD_16_PIN_LCD_SEG13|=hum_comps.SEG_TAB[a]&0x0f;
     
     
 }
 static void MD_DISPLAY_WRITE_D3_1(unsigned char a)  //  D03  H   L 
 {
     MD_17_PIN_LCD_SEG14&=0xf0;
     MD_17_PIN_LCD_SEG14|=hum_comps.SEG_TAB[a]>>4;
     if(hum_comps.dot3_pos==1)  MD_17_PIN_LCD_SEG14|=0x08;
     MD_18_PIN_LCD_SEG15&=0xf0;
     MD_18_PIN_LCD_SEG15|=hum_comps.SEG_TAB[a]&0x0f;
 }
 
 static void MD_DISPLAY_WRITE_D3_2(unsigned char a)//  D04  H   L  
 {
 
     MD_19_PIN_LCD_SEG16&=0xf0;
     MD_19_PIN_LCD_SEG16|=hum_comps.SEG_TAB[a]>>4;
     if(hum_comps.dot3_pos==2)  MD_19_PIN_LCD_SEG16|=0x08;
     MD_12_PIN_LCD_SEG09&=0xf0;
     MD_12_PIN_LCD_SEG09|=hum_comps.SEG_TAB[a]&0x0f;
     
 }
 
                  //  D10  H   L  
  static void  MD_DISPLAY_WRITE_D3_3(unsigned char a)   
  {
     MD_11_PIN_LCD_SEG08&=0xf0;
     MD_11_PIN_LCD_SEG08|=hum_comps.SEG_TAB[a]>>4;
     if(hum_comps.dot3_pos==3)  MD_11_PIN_LCD_SEG08|=0x08;
     MD_10_PIN_LCD_SEG05&=0xf0;
     MD_10_PIN_LCD_SEG05|=hum_comps.SEG_TAB[a]&0x0f;
  }

  static void MD_DISPLAY_WRITE_D3_4(unsigned char a)  //  D00  H   L 
  {
      MD_09_PIN_LCD_SEG04&=0xf8;
      MD_HIDE_P10_DOT;
      MD_09_PIN_LCD_SEG04|=hum_comps.SEG_TAB[a]>>4;
     if(hum_comps.dot3_pos==4)  MD_DIS_P10_DOT;
      MD_20_PIN_LCD_SEG17&=0xf0;
      MD_20_PIN_LCD_SEG17|=hum_comps.SEG_TAB[a]&0x0f;
   
  }
  static void MD_DISPLAY_WRITE_D3_5(unsigned char a)//  D01  H   L 
  {
  
      MD_21_PIN_LCD_SEG18&=0xf0;
      MD_21_PIN_LCD_SEG18|=hum_comps.SEG_TAB[a]>>4;
      if(hum_comps.dot3_pos==5)  MD_21_PIN_LCD_SEG18|=0x08;
      MD_22_PIN_LCD_SEG19&=0xf0;
      MD_22_PIN_LCD_SEG19|=hum_comps.SEG_TAB[a]&0x0f;
  }
  



static void display_line3_data()//display 3th line data
{
   #define MD_DIS_ALL_3_LINE_DATA (MD_DISPLAY_WRITE_D3_5(hum_comps.dig3_5),MD_DISPLAY_WRITE_D3_4(hum_comps.dig3_4),MD_DISPLAY_WRITE_D3_3(hum_comps.dig3_3),MD_DISPLAY_WRITE_D3_2(hum_comps.dig3_2),MD_DISPLAY_WRITE_D3_1(hum_comps.dig3_1),MD_DISPLAY_WRITE_D3_0(hum_comps.dig3_0))
   #define MD_HID_ALL_3_LINE_DATA (MD_DISPLAY_WRITE_D3_5(MD_HIDE_DISP)    ,MD_DISPLAY_WRITE_D3_4(MD_HIDE_DISP)    ,MD_DISPLAY_WRITE_D3_3(MD_HIDE_DISP)    ,MD_DISPLAY_WRITE_D3_2(MD_HIDE_DISP)    ,MD_DISPLAY_WRITE_D3_1(MD_HIDE_DISP)    ,MD_DISPLAY_WRITE_D3_0(MD_HIDE_DISP)    )
   if(hum_comps.cursor_3>-1)
   {
        MD_DISPLAY_WRITE_D3_5(hum_comps.dig3_5);
        MD_DISPLAY_WRITE_D3_4(hum_comps.dig3_4);
        MD_DISPLAY_WRITE_D3_3(hum_comps.dig3_3);
        MD_DISPLAY_WRITE_D3_2(hum_comps.dig3_2);
        MD_DISPLAY_WRITE_D3_1(hum_comps.dig3_1);
        MD_DISPLAY_WRITE_D3_0(hum_comps.dig3_0);
   }
   else
   {
   
 ((hum_comps.cursor_3==-1)&&(hum_comps.dis_oper_mark._bit.cur3))?((hum_comps.dis_oper_mark._bit.dis3)?MD_DIS_ALL_3_LINE_DATA                  :MD_HID_ALL_3_LINE_DATA             ):MD_DIS_ALL_3_LINE_DATA;
   }
   #undef MD_DIS_ALL_3_LINE_DATA
   #undef MD_HID_ALL_3_LINE_DATA

}
static void display_line2_data()//display 0th line data
{
   
       #define MD_DIS_ALL_2_LINE_DATA (MD_DISPLAY_WRITE_D2_5(hum_comps.dig2_5),MD_DISPLAY_WRITE_D2_4(hum_comps.dig2_4),MD_DISPLAY_WRITE_D2_3(hum_comps.dig2_3),MD_DISPLAY_WRITE_D2_2(hum_comps.dig2_2),MD_DISPLAY_WRITE_D2_1(hum_comps.dig2_1),MD_DISPLAY_WRITE_D2_0(hum_comps.dig2_0))
       #define MD_HID_ALL_2_LINE_DATA (MD_DISPLAY_WRITE_D2_5(MD_HIDE_DISP)    ,MD_DISPLAY_WRITE_D2_4(MD_HIDE_DISP)    ,MD_DISPLAY_WRITE_D2_3(MD_HIDE_DISP)    ,MD_DISPLAY_WRITE_D2_2(MD_HIDE_DISP)    ,MD_DISPLAY_WRITE_D2_1(MD_HIDE_DISP)    ,MD_DISPLAY_WRITE_D2_0(MD_HIDE_DISP)    )
       if(hum_comps.cursor_2>-1)
       {
        	((hum_comps.cursor_2==5)&&(hum_comps.dis_oper_mark._bit.cur2))?((hum_comps.dis_oper_mark._bit.dis2)?MD_DISPLAY_WRITE_D2_5(hum_comps.dig2_5):MD_DISPLAY_WRITE_D2_5(MD_HIDE_DISP)):MD_DISPLAY_WRITE_D2_5(hum_comps.dig2_5);
          	((hum_comps.cursor_2==4)&&(hum_comps.dis_oper_mark._bit.cur2))?((hum_comps.dis_oper_mark._bit.dis2)?MD_DISPLAY_WRITE_D2_4(hum_comps.dig2_4):MD_DISPLAY_WRITE_D2_4(MD_HIDE_DISP)):MD_DISPLAY_WRITE_D2_4(hum_comps.dig2_4);
        	((hum_comps.cursor_2==3)&&(hum_comps.dis_oper_mark._bit.cur2))?((hum_comps.dis_oper_mark._bit.dis2)?MD_DISPLAY_WRITE_D2_3(hum_comps.dig2_3):MD_DISPLAY_WRITE_D2_3(MD_HIDE_DISP)):MD_DISPLAY_WRITE_D2_3(hum_comps.dig2_3);
        	((hum_comps.cursor_2==2)&&(hum_comps.dis_oper_mark._bit.cur2))?((hum_comps.dis_oper_mark._bit.dis2)?MD_DISPLAY_WRITE_D2_2(hum_comps.dig2_2):MD_DISPLAY_WRITE_D2_2(MD_HIDE_DISP)):MD_DISPLAY_WRITE_D2_2(hum_comps.dig2_2);
        	((hum_comps.cursor_2==1)&&(hum_comps.dis_oper_mark._bit.cur2))?((hum_comps.dis_oper_mark._bit.dis2)?MD_DISPLAY_WRITE_D2_1(hum_comps.dig2_1):MD_DISPLAY_WRITE_D2_1(MD_HIDE_DISP)):MD_DISPLAY_WRITE_D2_1(hum_comps.dig2_1);
        	((hum_comps.cursor_2==0)&&(hum_comps.dis_oper_mark._bit.cur2))?((hum_comps.dis_oper_mark._bit.dis2)?MD_DISPLAY_WRITE_D2_0(hum_comps.dig2_0):MD_DISPLAY_WRITE_D2_0(MD_HIDE_DISP)):MD_DISPLAY_WRITE_D2_0(hum_comps.dig2_0);
        }
    	else
    	{
            ((hum_comps.cursor_2==-1)&&(hum_comps.dis_oper_mark._bit.cur2))?((hum_comps.dis_oper_mark._bit.dis2)?MD_DIS_ALL_2_LINE_DATA                  :MD_HID_ALL_2_LINE_DATA             ):MD_DIS_ALL_2_LINE_DATA;
        }
       #undef MD_DIS_ALL_2_LINE_DATA
       #undef MD_HID_ALL_2_LINE_DATA
}

static void display_line1_data()//display 1th line data
{
	 #define MD_DIS_ALL_1_LINE_DATA (MD_DISPLAY_WRITE_D1_3(hum_comps.dig1_3),MD_DISPLAY_WRITE_D1_2(hum_comps.dig1_2),MD_DISPLAY_WRITE_D1_1(hum_comps.dig1_1),MD_DISPLAY_WRITE_D1_0(hum_comps.dig1_0))
     #define MD_HID_ALL_1_LINE_DATA (MD_DISPLAY_WRITE_D1_3(MD_HIDE_DISP)    ,MD_DISPLAY_WRITE_D1_2(MD_HIDE_DISP)    ,MD_DISPLAY_WRITE_D1_1(MD_HIDE_DISP)    ,MD_DISPLAY_WRITE_D1_0(MD_HIDE_DISP)    )
     if(hum_comps.cursor_1>-1)
     {
    	((hum_comps.cursor_1==3)&&(hum_comps.dis_oper_mark._bit.cur1))?((hum_comps.dis_oper_mark._bit.dis1)?MD_DISPLAY_WRITE_D1_3(hum_comps.dig1_3):MD_DISPLAY_WRITE_D1_3(MD_HIDE_DISP)):MD_DISPLAY_WRITE_D1_3(hum_comps.dig1_3);
    	((hum_comps.cursor_1==2)&&(hum_comps.dis_oper_mark._bit.cur1))?((hum_comps.dis_oper_mark._bit.dis1)?MD_DISPLAY_WRITE_D1_2(hum_comps.dig1_2):MD_DISPLAY_WRITE_D1_2(MD_HIDE_DISP)):MD_DISPLAY_WRITE_D1_2(hum_comps.dig1_2);
    	((hum_comps.cursor_1==1)&&(hum_comps.dis_oper_mark._bit.cur1))?((hum_comps.dis_oper_mark._bit.dis1)?MD_DISPLAY_WRITE_D1_1(hum_comps.dig1_1):MD_DISPLAY_WRITE_D1_1(MD_HIDE_DISP)):MD_DISPLAY_WRITE_D1_1(hum_comps.dig1_1);
    	((hum_comps.cursor_1==0)&&(hum_comps.dis_oper_mark._bit.cur1))?((hum_comps.dis_oper_mark._bit.dis1)?MD_DISPLAY_WRITE_D1_0(hum_comps.dig1_0):MD_DISPLAY_WRITE_D1_0(MD_HIDE_DISP)):MD_DISPLAY_WRITE_D1_0(hum_comps.dig1_0);
     }
     else
     {
       ((hum_comps.cursor_1==-1)&&(hum_comps.dis_oper_mark._bit.cur1))?((hum_comps.dis_oper_mark._bit.dis1)?MD_DIS_ALL_1_LINE_DATA                  :MD_HID_ALL_1_LINE_DATA             ):MD_DIS_ALL_1_LINE_DATA;
     }
      #undef MD_DIS_ALL_1_LINE_DATA
      #undef MD_HID_ALL_1_LINE_DATA
}

static void display_line0_data()//display 1th line data
{
	 #define MD_DIS_ALL_0_LINE_DATA (MD_DISPLAY_WRITE_D0_1(hum_comps.dig0_1),MD_DISPLAY_WRITE_D0_0(hum_comps.dig0_0))
     #define MD_HID_ALL_0_LINE_DATA (MD_DISPLAY_WRITE_D0_1(MD_HIDE_DISP)    ,MD_DISPLAY_WRITE_D0_0(MD_HIDE_DISP)    )
     if(hum_comps.cursor_0>-1)
     {
    	((hum_comps.cursor_0==1)&&(hum_comps.dis_oper_mark._bit.cur0))?((hum_comps.dis_oper_mark._bit.dis0)?MD_DISPLAY_WRITE_D0_1(hum_comps.dig0_1):MD_DISPLAY_WRITE_D0_1(MD_HIDE_DISP)):MD_DISPLAY_WRITE_D0_1(hum_comps.dig0_1);
    	((hum_comps.cursor_0==0)&&(hum_comps.dis_oper_mark._bit.cur0))?((hum_comps.dis_oper_mark._bit.dis0)?MD_DISPLAY_WRITE_D0_0(hum_comps.dig0_0):MD_DISPLAY_WRITE_D0_0(MD_HIDE_DISP)):MD_DISPLAY_WRITE_D0_0(hum_comps.dig0_0);
     }
     else
     {
       ((hum_comps.cursor_0==-1)&&(hum_comps.dis_oper_mark._bit.cur0))?((hum_comps.dis_oper_mark._bit.dis0)?MD_DIS_ALL_0_LINE_DATA                  :MD_HID_ALL_0_LINE_DATA             ):MD_DIS_ALL_0_LINE_DATA;
     }
      #undef MD_DIS_ALL_1_LINE_DATA
      #undef MD_HID_ALL_1_LINE_DATA
}


//return low level key_valve
static int get_key(hum_comps_t *const this )
{
	static unsigned long key_delay_count=0;
        static int           current_key=MD_KEY_MASK;
	int                  key_valve;
	this->key_type=EM_NO_KEY;
	MD_KEY_VDD=1;
	delay_us(1);
	key_valve=(MD_KEY_DATA&MD_KEY_MASK);
	MD_KEY_VDD=0;
	if(key_valve!=current_key)
	{
		current_key=key_valve;
		key_delay_count=83;
		key_valve=MD_KEY_MASK;
	}
	else
	{
		if(key_delay_count>0)
		{
			key_delay_count--;
		}
		if(key_delay_count==81)
		{
			if((key_valve&MD_KEY_MASK)!=MD_KEY_MASK)
			{
				this->key_type=EM_SHORT_KEY;
				this->key=key_valve;
			}
		}
		else if(key_delay_count==23)
		{
			if((key_valve&MD_KEY_MASK)!=MD_KEY_MASK)
			{
				this->key_type=EM_LONG_KEY;
				this->key=key_valve;
			}
		}
		else
		{
			key_valve=MD_KEY_MASK;
		}
	}
	return key_valve;
}

static long isNum(int dig)
{
    if(dig<10)
    {
        return dig;
    }
    else 
    {
        return 0;
    }
}


/////////////////////////////////////mode enter and exit//////////////////
void enter_self_test_mode(void)
{
    hum_comps.current_mode=EM_SELF_TEST_MODE;
    clr_lcd();
    hum_comps.dis_oper_mark._bit.cur0=0;
    hum_comps.dis_oper_mark._bit.cur1=0;
    hum_comps.dis_oper_mark._bit.cur2=0;
}
void enter_normal_mode(void)
{
    hum_comps.current_mode=EM_NORMAL_MODE;
    mode_comps[hum_comps.current_mode].dis_option=0;
    clr_lcd();
    hum_comps.dis_oper_mark.All|=0x0000ff00;
    hum_comps.dis_oper_mark._bit.refresh_press_max=1;

    hum_comps.dis_oper_mark._bit.cur0=0;
    hum_comps.dis_oper_mark._bit.cur1=0;
    hum_comps.dis_oper_mark._bit.cur2=0;
}
void enter_lora_mode(void)
{
    hum_comps.current_mode=EM_LORA_MODE;
    mode_comps[hum_comps.current_mode].dis_option=0;
    clr_lcd();
    hum_comps.dis_oper_mark.All|=0x0000ff00;

    hum_comps.dis_oper_mark._bit.cur0=0;
    hum_comps.dis_oper_mark._bit.cur1=0;
    hum_comps.dis_oper_mark._bit.cur2=0;

}
void enter_cal_modify_mode(void)
{
    hum_comps.current_mode=EM_CAL_MODIFY_MODE;
    clr_lcd();
    mode_comps[hum_comps.current_mode].dis_option=mode_comps[EM_CAL_QUERY_MODE].dis_option;
    if(device_comps.cal_type==EM_CAL_PRESS)
    {
        hum_comps.cursor_2=5;
        hum_comps.cursor_2_count=2;
        hum_comps.dis_oper_mark._bit.cur2=1;
        hum_comps.dis_oper_mark._bit.refresh_option=1;
        hum_comps.dis_oper_mark._bit.refresh_press_adc=1;
        hum_comps.dis_oper_mark._bit.refresh_cal_param=1;
        
    }
   
    else if(device_comps.cal_type==EM_CAL_RES)
    {
        hum_comps.cursor_2=5;
        hum_comps.cursor_2_count=2;
        hum_comps.dis_oper_mark._bit.cur2=1;
        hum_comps.dis_oper_mark._bit.refresh_option=1;
        hum_comps.dis_oper_mark._bit.refresh_temp_adc=1;
        hum_comps.dis_oper_mark._bit.refresh_res_cal_param=1;
        
    }
    else if(device_comps.cal_type==EM_CAL_HIGH)
    {
        hum_comps.cursor_2=5;
        hum_comps.cursor_2_count=2;
        hum_comps.dis_oper_mark._bit.cur2=1;
        hum_comps.dis_oper_mark._bit.refresh_option=1;
        hum_comps.dis_oper_mark._bit.refresh_high_adc=1;
        hum_comps.dis_oper_mark._bit.refresh_high_cal_param=1;
        
    }
  
}

void enter_cal_query_mode(void)
{
    hum_comps.current_mode=EM_CAL_QUERY_MODE;
    clr_lcd();
    mode_comps[hum_comps.current_mode].dis_option=0;
    if(device_comps.cal_type==EM_CAL_PRESS)
    {
        hum_comps.cursor_2=5;
        hum_comps.cursor_2_count=2;
        hum_comps.dis_oper_mark._bit.cur2=0;
        hum_comps.dis_oper_mark._bit.refresh_option=1;
        hum_comps.dis_oper_mark._bit.refresh_press_adc=1;
        hum_comps.dis_oper_mark._bit.refresh_cal_param=1;
    }
   
    else if(device_comps.cal_type==EM_CAL_RES)
    {
        hum_comps.cursor_2=5;
        hum_comps.cursor_2_count=2;
        hum_comps.dis_oper_mark._bit.cur2=0;
        hum_comps.dis_oper_mark._bit.refresh_option=1;
        hum_comps.dis_oper_mark._bit.refresh_temp_adc=1;
        hum_comps.dis_oper_mark._bit.refresh_res_cal_param=1;
    }
    else if(device_comps.cal_type==EM_CAL_HIGH)
    {
        hum_comps.cursor_2=5;
        hum_comps.cursor_2_count=2;
        hum_comps.dis_oper_mark._bit.cur2=0;
        hum_comps.dis_oper_mark._bit.refresh_option=1;
        hum_comps.dis_oper_mark._bit.refresh_high_adc=1;
        hum_comps.dis_oper_mark._bit.refresh_high_cal_param=1;
    }
}

void enter_param_query_mode(void)
{
    hum_comps.current_mode=EM_PARAM_QUERY_MODE;
    clr_lcd();
    mode_comps[hum_comps.current_mode].dis_option=0;
    hum_comps.cursor_2=5;
    hum_comps.cursor_2_count=2;
    hum_comps.dis_oper_mark._bit.cur2=0;
    hum_comps.dis_oper_mark._bit.refresh_option=1;
    hum_comps.dis_oper_mark._bit.refresh_param_modifyed=1;
}

void enter_param_modify_mode(void)
{
    hum_comps.current_mode=EM_PARAM_MODIFY_MODE;
    clr_lcd();
    mode_comps[hum_comps.current_mode].dis_option=mode_comps[EM_PARAM_QUERY_MODE].dis_option;
    hum_comps.cursor_2_count=0;
    hum_comps.dis_oper_mark._bit.cur2=1;
    hum_comps.dis_oper_mark._bit.refresh_option=1;
    hum_comps.dis_oper_mark._bit.refresh_param_modifyed=1;
    hum_comps.cursor_2=5;
   
}

static void enter_debug_mode(void)
{
    hum_comps.current_mode=EM_DEBUG_MODE;
    clr_lcd();
    hum_comps.dis_oper_mark._bit.refresh_press_adc;
	hum_comps.dis_oper_mark._bit.refresh_temp_adc;

    hum_comps.dis_oper_mark._bit.cur0=0;
    hum_comps.dis_oper_mark._bit.cur1=0;
    hum_comps.dis_oper_mark._bit.cur2=0;

    mode_comps[hum_comps.current_mode].displayTimer=-1;
}

void  enter_report_mode(void)
{
    hum_comps.current_mode=EM_REPORT_MODE;
    clr_lcd();
    mode_comps[hum_comps.current_mode].dis_option=0;
    hum_comps.dis_oper_mark._bit.cur0=0;
    hum_comps.dis_oper_mark._bit.cur1=0;
    hum_comps.dis_oper_mark._bit.cur2=0;
    mode_comps[hum_comps.current_mode].displayTimer=0;
    
}

static void enter_pawd_mode(void)
{
    hum_comps.current_mode=EM_PWD_MODE;
    clr_lcd();
    hum_comps.dis_oper_mark._bit.cur0=0;
    hum_comps.dis_oper_mark._bit.cur1=0;
    hum_comps.dis_oper_mark._bit.cur2=1;
    hum_comps.dis_oper_mark._bit.dis2=0;
    hum_comps.dig2_0=0;
    hum_comps.dig2_1=0;
    hum_comps.dig2_2=0;
    hum_comps.dig2_3=0;
    hum_comps.dig2_4=0;
    hum_comps.dig2_5=0;
    hum_comps.cursor_2=5;
    hum_comps.dot2_pos=0;
    hum_comps.cursor_2_count=0;
      
}
void enter_4g_mode(void)
{
    hum_comps.current_mode=EM_4G_MODE;
    mode_comps[hum_comps.current_mode].dis_option=0;
    clr_lcd();
    hum_comps.dis_oper_mark.All|=0x0000ff00;
    hum_comps.dis_oper_mark._bit.cur0=0;
    hum_comps.dis_oper_mark._bit.cur1=0;
    hum_comps.dis_oper_mark._bit.cur2=0;
}
/*********************************start  key function************************************************************/

static void on_s_key()
{
	mode_comps[hum_comps.current_mode].on_s_key();
}
static void on_m_key()
{
	mode_comps[hum_comps.current_mode].on_m_key();
}
static void on_j_key()
{
	mode_comps[hum_comps.current_mode].on_j_key();
}
static void on_long_s_key()
{
	mode_comps[hum_comps.current_mode].on_long_s_key();
}
static void on_long_m_key()
{
	mode_comps[hum_comps.current_mode].on_long_m_key();
}
static void on_long_j_key()
{
	mode_comps[hum_comps.current_mode].on_long_j_key();
}
static void on_long_s_and_j_key()
{
	mode_comps[hum_comps.current_mode].on_long_s_and_j_key();
}


static void normal_mode_on_s_key()
{
     device_comps.max_press=0;
	 hum_comps.dis_oper_mark._bit.refresh_press_max=1;
}
static void normal_mode_on_m_key()
{
	mode_comps[hum_comps.current_mode].dis_option++;
	//mode_comps[hum_comps.current_mode].dis_option%=8;
	hum_comps.dis_oper_mark.All|=0x0000ff00;
	hum_comps.dis_oper_mark._bit.refresh_press_max=1;
	//if(mode_comps[hum_comps.current_mode].dis_option>2)
	//{
	 //   hum_comps.dis_oper_mark._bit.cur2=0;
	//}
	//mode_comps[hum_comps.current_mode].displayTimer=-1;
}

static void normal_mode_on_j_key()
{
 
}

static void _4g_mode_on_long_s_key()
{
   protocolComps.triggerIrq._bit.key=1;
}

static void normal_mode_on_long_m_key()
{
    device_comps.clr_press();
   // enter_pawd_mode();
}
static void normal_mode_on_long_s_and_j_key()
{
	enter_debug_mode();
}

static void normal_mode_on_long_j_key (void)
{
	if(!ircComps.sw._bit.runing)
   {
        MD_IR_VCM_ON;//o¨¬¨ªa1?¦Ì?//	 
        ircComps.op_window_time=30;
        *ircComps.recv_pos_pt = 0;
        #ifndef MD_MODBUS
        enable_irc_receive();
        #endif
        ircComps.sw._bit.runing=1;
    }
    else
   {
        ircComps.op_window_time=0;
        *ircComps.recv_pos_pt = 0;
       #ifndef MD_MODBUS
        disable_irc_receive();
        disable_irc_send();
       #endif
        MD_IR_VCM_OFF;//o¨¬¨ªa1?¦Ì?//	 
        ircComps.sw._bit.runing=0;
    }
    
	
}







static void debug_mode_on_s_key()
{
	
}
static void debug_mode_on_m_key()
{
	mode_comps[hum_comps.current_mode].dis_option++;
	mode_comps[hum_comps.current_mode].dis_option%=2;
}
static void debug_mode_on_j_key()
{
	
}
static void debug_mode_on_long_m_key()
{


}
static void debug_mode_on_long_s_and_j_key()
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

static void pwd_mode_on_s_key()
{
	hum_comps.cursor_2+=5;
	hum_comps.cursor_2%=6;//4->3->2...>0
}
static void pwd_mode_on_m_key()
{
    long pwd=isNum(hum_comps.dig2_2)*100+isNum(hum_comps.dig2_1)*10+isNum(hum_comps.dig2_0);
    if(pwd==235)
    {
        if(hum_comps.dig2_5>3||hum_comps.dig2_5==2) //dig0_5 cal_type 0:press 1:pt100  2:deltaP 3:high
    	{
            return;
        }
        if(hum_comps.dig2_5!=1&&hum_comps.dig2_5!=3)
        {
           if(hum_comps.dig2_4<3||hum_comps.dig2_4>5||hum_comps.dig2_3>1)//dig0_4 dot 3-5 dig0_3 unit 0Mpa,1Kpa
           {
              return;
           }
        }
        device_comps.cal_type=(cal_type_t)hum_comps.dig2_5;//0,press,1 pt100 2,deltaP 3,high
        if(device_comps.cal_type==EM_CAL_PRESS)
        {
            device_comps.calibration_param_bak.dot=hum_comps.dig2_4;
            device_comps.calibration_param_bak.unit=hum_comps.dig2_3;
        }
	    else if(device_comps.cal_type==EM_CAL_RES)
        {
            device_comps.res_calibration_param_bak.dot=hum_comps.dig2_4;
            device_comps.res_calibration_param_bak.unit=hum_comps.dig2_3;
        }
         else if(device_comps.cal_type==EM_CAL_HIGH)
        {
            device_comps.high_calibration_param_bak.dot=hum_comps.dig2_4;
            device_comps.high_calibration_param_bak.unit=hum_comps.dig2_3;
        }
        enter_cal_query_mode();
    }
    
    else if(pwd==237)
    {
        enter_param_query_mode();
    }
}
static void pwd_mode_on_j_key()
{
	*(&hum_comps.dig2_0+hum_comps.cursor_2)=(*(&hum_comps.dig2_0+hum_comps.cursor_2)+1)%10;
}
static void pwd_mode_on_long_m_key()
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
//	hum_comps.current_mode=EM_NORMAL_MODE;
//	hum_comps.dis_oper_mark|=MD_OPER_refresh_press_MARK;
//	hum_comps.dis_oper_mark|=MD_OPER_REFRESH_TEMP_MARK;
//	hum_comps.dis_oper_mark&=~MD_OPER_CUR_MARK;
}
static void pwd_mode_on_long_s_and_j_key(void)
{
	
}


static void cal_query_mode_on_s_key(void)
{
  
    enter_cal_modify_mode();
	
}
static void cal_query_mode_on_m_key(void)
{
	mode_comps[hum_comps.current_mode].dis_option++;
    if(device_comps.cal_type==EM_CAL_PRESS)
    {
        mode_comps[hum_comps.current_mode].dis_option%=4;
        hum_comps.dis_oper_mark._bit.refresh_option=1;
        hum_comps.dis_oper_mark._bit.refresh_press_adc=1;
        hum_comps.dis_oper_mark._bit.refresh_cal_param=1;
    }
   
    else if(device_comps.cal_type==EM_CAL_RES)
    {
        mode_comps[hum_comps.current_mode].dis_option%=2;
        hum_comps.dis_oper_mark._bit.refresh_option=1;
        hum_comps.dis_oper_mark._bit.refresh_temp_adc=1;
        hum_comps.dis_oper_mark._bit.refresh_res_cal_param=1;
    }
     else if(device_comps.cal_type==EM_CAL_HIGH)
    {
        mode_comps[hum_comps.current_mode].dis_option%=2;
        hum_comps.dis_oper_mark._bit.refresh_option=1;
        hum_comps.dis_oper_mark._bit.refresh_high_adc=1;
        hum_comps.dis_oper_mark._bit.refresh_high_cal_param=1;
    }
}
static void cal_query_mode_on_j_key(void)
{
   
}

static void cal_query_mode_on_long_m_key(void)
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

static void cal_modify_mode_on_s_key(void)
{
	
	hum_comps.cursor_2+=5;
	hum_comps.cursor_2%=6;//4->3->2...>0
}
static void cal_modify_mode_on_m_key(void)
{
     int save_data_en=0;
	 int opt=mode_comps[hum_comps.current_mode].dis_option;
	       
    if(device_comps.cal_type==EM_CAL_PRESS)
	{
		if(device_comps.sw._bit.adc_stb)
		{
            long num=isNum(hum_comps.dig2_5)*100000+isNum(hum_comps.dig2_4)*10000+isNum(hum_comps.dig2_3)*1000+
					 isNum(hum_comps.dig2_2)*100+isNum(hum_comps.dig2_1)*10+isNum(hum_comps.dig2_0);
			save_data_en=1;
			if(opt==3)
		    {
                device_comps.calibration_param.is_calibrated=1;
		    }
			device_comps.calibration_param.x[mode_comps[hum_comps.current_mode].dis_option]=device_comps.ad1_ad2_average_result;
			device_comps.calibration_param.y[mode_comps[hum_comps.current_mode].dis_option]=num;
			device_comps.calibration_param.t[mode_comps[hum_comps.current_mode].dis_option];
			device_comps.calibration_param.dot=device_comps.calibration_param_bak.dot;
			device_comps.calibration_param.unit=device_comps.calibration_param_bak.unit;
			device_comps.calibration_param.cs=Check_Sum_5A(&device_comps.calibration_param, & device_comps.calibration_param.cs-(unsigned char *)&device_comps.calibration_param);
			device_comps.save_calibration_param(&device_comps.calibration_param,sizeof(device_comps.calibration_param));
			hum_comps.dis_oper_mark._bit.refresh_cal_param=1;
		    hum_comps.dis_oper_mark._bit.cur2=0;
		}
	}
	else if(device_comps.cal_type==EM_CAL_RES)
	{
		if(device_comps.sw._bit.temp_adc_stb)
		{
            long num=isNum(hum_comps.dig2_5)*100000+isNum(hum_comps.dig2_4)*10000+isNum(hum_comps.dig2_3)*1000+
					 isNum(hum_comps.dig2_2)*100+isNum(hum_comps.dig2_1)*10+isNum(hum_comps.dig2_0);
			save_data_en=1;
			if(opt==1)
		    {
                device_comps.res_calibration_param.is_calibrated=1;
		    }
			device_comps.res_calibration_param.x[mode_comps[hum_comps.current_mode].dis_option]=device_comps.temp_p_temp_n_average_result;
			device_comps.res_calibration_param.y[mode_comps[hum_comps.current_mode].dis_option]=num;
			device_comps.res_calibration_param.t[mode_comps[hum_comps.current_mode].dis_option];
			device_comps.res_calibration_param.dot=device_comps.res_calibration_param_bak.dot;
			device_comps.res_calibration_param.unit=device_comps.res_calibration_param_bak.unit;
			device_comps.res_calibration_param.cs=Check_Sum_5A(&device_comps.res_calibration_param, & device_comps.res_calibration_param.cs-(unsigned char *)&device_comps.res_calibration_param);
			device_comps.save_res_calibration_param(&device_comps.res_calibration_param,sizeof(device_comps.res_calibration_param));
			hum_comps.dis_oper_mark._bit.refresh_res_cal_param=1;
		    hum_comps.dis_oper_mark._bit.cur2=0;
		}
	}
	else if(device_comps.cal_type==EM_CAL_HIGH)
	{
		if(device_comps.sw._bit.high_adc_stb)
		{
            long num=isNum(hum_comps.dig2_5)*100000+isNum(hum_comps.dig2_4)*10000+isNum(hum_comps.dig2_3)*1000+
					 isNum(hum_comps.dig2_2)*100+isNum(hum_comps.dig2_1)*10+isNum(hum_comps.dig2_0);
			save_data_en=1;
			if(opt==1)
		    {
                device_comps.high_calibration_param.is_calibrated=1;
		    }
			device_comps.high_calibration_param.x[mode_comps[hum_comps.current_mode].dis_option]= device_comps.high_p_high_n_average_result;
			device_comps.high_calibration_param.y[mode_comps[hum_comps.current_mode].dis_option]=num;
			device_comps.high_calibration_param.t[mode_comps[hum_comps.current_mode].dis_option];
			device_comps.high_calibration_param.dot=device_comps.high_calibration_param_bak.dot;
			device_comps.high_calibration_param.unit=device_comps.high_calibration_param_bak.unit;
			device_comps.high_calibration_param.cs=Check_Sum_5A(&device_comps.high_calibration_param, & device_comps.high_calibration_param.cs-(unsigned char *)&device_comps.high_calibration_param);
			device_comps.save_high_calibration_param(&device_comps.high_calibration_param,sizeof(device_comps.high_calibration_param));
			hum_comps.dis_oper_mark._bit.refresh_high_cal_param=1;
		    hum_comps.dis_oper_mark._bit.cur2=0;
		}
	}
	if(save_data_en==1)
	{
        enter_cal_query_mode();
		//hum_comps.current_mode=EM_CAL_QUERY_MODE;
		mode_comps[hum_comps.current_mode].dis_option=mode_comps[EM_CAL_MODIFY_MODE].dis_option;
	
	}
}
static void cal_modify_mode_on_j_key(void)
{
	*(&hum_comps.dig2_0+hum_comps.cursor_2)=(*(&hum_comps.dig2_0+hum_comps.cursor_2)+1)%10;
	
}
static void cal_modify_mode_on_long_s_key(void)
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
static void param_query_mode_on_s_key(void)
{
	enter_param_modify_mode();
}
static void param_query_mode_on_m_key(void)
{
    mode_comps[hum_comps.current_mode].dis_option++;
    mode_comps[hum_comps.current_mode].dis_option%=1;
    hum_comps.dis_oper_mark._bit.refresh_option=1;
    hum_comps.dis_oper_mark._bit.refresh_param_modifyed=1;
}

static void param_query_mode_on_long_m_key(void)
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


static void param_modify_mode_on_m_key(void)
{
	 int save_data_en=0;
	 int opt=mode_comps[hum_comps.current_mode].dis_option;
	 long num=isNum(hum_comps.dig2_5)*100000+isNum(hum_comps.dig2_4)*10000+isNum(hum_comps.dig2_3)*1000+
								isNum(hum_comps.dig2_2)*100+
									       isNum(hum_comps.dig2_1)*10+           
								               isNum(hum_comps.dig2_0);        
   
    *(&device_comps.float_level_param.bottom_s+opt)=num;
 	save_data_en=1;
    if(save_data_en==1)
	{
        device_comps.float_level_param.cs=Check_Sum_5A(&device_comps.float_level_param, & device_comps.float_level_param.cs-(unsigned char *)&device_comps.float_level_param);
	    device_comps.save_float_level_param(&device_comps.float_level_param,sizeof(device_comps.float_level_param));
		enter_param_query_mode();
		opt=mode_comps[hum_comps.current_mode].dis_option=mode_comps[EM_PARAM_MODIFY_MODE].dis_option;
	}
	
}
static void param_modify_mode_on_s_key(void)
{
	int opt=mode_comps[hum_comps.current_mode].dis_option;
    
    {
        hum_comps.cursor_2+=5;
        hum_comps.cursor_2%=6;//4->3->2...>0
    }
        
}
static void param_modify_mode_on_j_key(void)
{
	int opt=mode_comps[hum_comps.current_mode].dis_option;
    if(opt==0)
   {
 	*(&hum_comps.dig2_0+hum_comps.cursor_2)=(*(&hum_comps.dig2_0+hum_comps.cursor_2)+1)%10;
  }
}
static void param_modify_mode_on_long_s_key(void)
{
	
	
}
static void _4g_mode_on_j_key(void)
{
  
 
}
static void _4g_mode_on_long_m_key(void)
{
    enter_pawd_mode();
}

static void lora_mode_on_m_key(void)
{
    mode_comps[hum_comps.current_mode].dis_option++;
    mode_comps[hum_comps.current_mode].dis_option%=8;
    hum_comps.dis_oper_mark.All|=0x0000ff00;
    hum_comps.dis_oper_mark._bit.refresh_high=1;
     hum_comps.dis_oper_mark._bit.refresh_volume=1;
    if(mode_comps[hum_comps.current_mode].dis_option>2)
    {
        hum_comps.dis_oper_mark._bit.cur2=0;
    }
    mode_comps[hum_comps.current_mode].displayTimer=-1;
}

static void lora_mode_on_long_m_key(void)
{
    enter_pawd_mode();
}


///////////////////////END KEY FUNCTION////////////////////


//////////////START DISPLAY///////////////////////////////

static void hide_zero(unsigned char  *const hi_addr,unsigned char total_width,unsigned char reserved_num)//Number of digits reserved
{
	int i=0;
	unsigned char  *buf=hi_addr;
	for(i=0;i<total_width-reserved_num;i++)
	{
//		if(*(buf-i)>0 && *(buf-i)<10)
//		{
//			break;
//		}
//		else
//		{
//			*(buf-i)=MD_HIDE_DISP;
//				
//		}
        if(*(buf-i)==0)
		{
			
			*(buf-i)=MD_HIDE_DISP;
		}
		else
		{
			break;
				
		}
	}
}

static char lg(const long n)
{   
    if(n==1)
    {
        return 0;
    }
    if(n==10)
    {
        return 1;
    }
    if(n==100)
    {
        return 2;
    }
    if(n==1000)
    {
        return 3;
    }
    return 1;
}

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


void calc_seg_value(unsigned char *dig,const int len,long num)
{
    int i=0;
    if(len==0)
    {
        dig[0]=0x0e;
        return;
    }
    for(i=0;i<len;i++)
    {
        dig[i]=num/pwr(i)%10;
    }
}
void  display_opt(int opt)
{
    //hum_comps.dig0_0=MD_HIDE_DISP;
    hum_comps.dig0_1=opt%10;
    hum_comps.dot0_pos=0;
    MD_DISPLAY_WRITE_D0_1(hum_comps.dig0_1);
   // display_line0_data();
}

void display_temp_adc(void)
{
 
    if(hum_comps.dis_oper_mark._bit.refresh_temp_adc)
    {
         long num=device_comps.temp_p_temp_n_average_result; 
         int i=0;
         if(num<0)
    	 {
             num=-num;
             hum_comps.dig1_3=MD_HIDE_DISP-1;
             calc_seg_value(&hum_comps.dig1_0,3,num);
             
    	 }
    	 else
    	 {
    	     calc_seg_value(&hum_comps.dig1_0,4,num);
            
	     }
	     hum_comps.dot1_pos=0;
         hum_comps.dis_oper_mark._bit.refresh_temp_adc=0;
    }
  
   
    if(device_comps.sw._bit.temp_adc_stb)
    {
        hum_comps.dis_oper_mark._bit.cur1=0;
        hum_comps.dis_oper_mark._bit.dis1=1;
    }
    else
    {
        hum_comps.dis_oper_mark._bit.cur1=1;
        hum_comps.cursor_1=-1;
        hum_comps.cursor_1_count++;
        if(hum_comps.cursor_1_count>4)
        {
            hum_comps.cursor_1_count=0;
            if(hum_comps.dis_oper_mark._bit.dis1)
            {
                hum_comps.dis_oper_mark._bit.dis1=0;
            }
            else
            {
               hum_comps.dis_oper_mark._bit.dis1=1;
            }
        }
    }
    display_line1_data();

}

void display_press_adc(void)
{
   if(hum_comps.dis_oper_mark._bit.refresh_press_adc)
   {
        long num=device_comps.ad1_ad2_average_result; 
        int i=0;
        
         if(num<0)
    	 {
             num=-num;
             hum_comps.dig1_3=MD_HIDE_DISP-1;
             calc_seg_value(&hum_comps.dig1_0,3,num);
             
    	 }
    	 else
    	 {
    	     calc_seg_value(&hum_comps.dig1_0,4,num);
            
	     }
	     hum_comps.dot1_pos=0;
        hum_comps.dis_oper_mark._bit.refresh_press_adc=0;
  }
   
    if(device_comps.sw._bit.adc_stb)
    {
        hum_comps.dis_oper_mark._bit.cur1=0;
        hum_comps.dis_oper_mark._bit.dis1=1;
    }
    else
    {
        hum_comps.dis_oper_mark._bit.cur1=1;
        hum_comps.cursor_1=-1;
        hum_comps.cursor_1_count++;
        if(hum_comps.cursor_1_count>4)
        {
            hum_comps.cursor_1_count=0;
            if(hum_comps.dis_oper_mark._bit.dis1)
            {
                hum_comps.dis_oper_mark._bit.dis1=0;
            }
            else
            {
               hum_comps.dis_oper_mark._bit.dis1=1;
            }
        }
    }
      display_line1_data();
     
}

void display_high_adc(void)
{
   if(hum_comps.dis_oper_mark._bit.refresh_high_adc)
   {
        long num=device_comps.high_p_high_n_average_result; 
        int i=0;
        if(num<0)
    	 {
             num=-num;
             hum_comps.dig1_3=MD_HIDE_DISP-1;
             calc_seg_value(&hum_comps.dig1_0,3,num);
             
    	 }
    	 else
    	 {
    	     calc_seg_value(&hum_comps.dig1_0,4,num);
            
	     }
	     hum_comps.dot1_pos=0;

        hum_comps.dis_oper_mark._bit.refresh_high_adc=0;
    }
   
    if(device_comps.sw._bit.adc_stb)
    {
        hum_comps.dis_oper_mark._bit.cur1=0;
        hum_comps.dis_oper_mark._bit.dis1=1;
    }
    else
    {
        hum_comps.dis_oper_mark._bit.cur1=1;
        hum_comps.cursor_1=-1;
        hum_comps.cursor_1_count++;
        if(hum_comps.cursor_1_count>4)
        {
            hum_comps.cursor_1_count=0;
            if(hum_comps.dis_oper_mark._bit.dis1)
            {
                hum_comps.dis_oper_mark._bit.dis1=0;
            }
            else
            {
               hum_comps.dis_oper_mark._bit.dis1=1;
            }
        }
    }
    display_line1_data();
     
}

static void display_high(int index)
{
    if(!device_comps.high_calibration_param.is_calibrated)
    {
        hum_comps.dig2_0=MD_HIDE_DISP-1;
        hum_comps.dig2_1=MD_HIDE_DISP-1;
        hum_comps.dig2_2=MD_HIDE_DISP-1;
        hum_comps.dig2_3=MD_HIDE_DISP-1;
        hum_comps.dig2_4=MD_HIDE_DISP-1;
        hum_comps.dig2_5=MD_HIDE_DISP-1;
        hum_comps.dot2_pos=0;
        MD_HIDE_T6_CELSIUS;
        MD_HIDE_T5_MPA;MD_HIDE_T2_KPA;MD_HIDE_T7_M;
        display_line2_data();
        return;
    }
    if(hum_comps.dis_oper_mark._bit.refresh_high)
    {
        long num=device_comps.current_high/10;

        if(num<0)
		{
		    num=-num;
		    hum_comps.dig2_5=MD_HIDE_DISP-1;
		}
		else
		{
            hum_comps.dig2_5=MD_HIDE_DISP;
		}
	    calc_seg_value(&hum_comps.dig2_0,5,num);

        hum_comps.dot2_pos=device_comps.high_calibration_param.dot-1;// 0.0000
        hide_zero(&hum_comps.dig2_4,5,hum_comps.dot2_pos+1);
        
        MD_HIDE_T6_CELSIUS;
        MD_HIDE_T5_MPA;MD_HIDE_T2_KPA;MD_HIDE_T7_M;
        MD_DIS_T7_M;
       display_line2_data(); 
       hum_comps.dis_oper_mark._bit.refresh_high=0;
    }
}

static void display_volume(int index)
{
    if(!device_comps.high_calibration_param.is_calibrated)
    {
        hum_comps.dig2_0=MD_HIDE_DISP-1;
        hum_comps.dig2_1=MD_HIDE_DISP-1;
        hum_comps.dig2_2=MD_HIDE_DISP-1;
        hum_comps.dig2_3=MD_HIDE_DISP-1;
        hum_comps.dig2_4=MD_HIDE_DISP-1;
        hum_comps.dig2_5=MD_HIDE_DISP-1;
        hum_comps.dot2_pos=0;
        MD_HIDE_T6_CELSIUS;
        MD_HIDE_T5_MPA;MD_HIDE_T2_KPA;MD_HIDE_T7_M;
        display_line2_data();
        return;
        
    }
    if(hum_comps.dis_oper_mark._bit.refresh_volume)
    {
        long num=device_comps.current_volume/pwr(device_comps.high_calibration_param.dot-1);

	    calc_seg_value(&hum_comps.dig2_0,6,num);
        hum_comps.dot2_pos=3;// 0.000
        hide_zero(&hum_comps.dig2_5,6,hum_comps.dot2_pos+1);
        MD_HIDE_T6_CELSIUS;
        MD_HIDE_T5_MPA;MD_HIDE_T2_KPA;MD_HIDE_T7_M;
        display_line2_data(); 
	    hum_comps.dis_oper_mark._bit.refresh_volume=0;
    }
}
static void display_bottom_s(int index)
{
        long num=device_comps.float_level_param.bottom_s;
        calc_seg_value(&hum_comps.dig2_0,6,num);
       
        hum_comps.dot2_pos=3;// 0.0000
        hide_zero(&hum_comps.dig2_5,6,hum_comps.dot2_pos+1);
         MD_HIDE_T6_CELSIUS;
        MD_HIDE_T5_MPA;MD_HIDE_T2_KPA;MD_HIDE_T7_M;
        display_line2_data(); 
}


void display_temp()//refresh temperature
{
	unsigned char dot_pos=0;
	long num=device_comps.current_temp;
	hum_comps.dot1_pos=1;
     if(num<0)
	 {
         num=-num;
         hum_comps.dig1_3=MD_HIDE_DISP-1;
         calc_seg_value(&hum_comps.dig1_0,3,num);
         hide_zero(&hum_comps.dig1_2,3,hum_comps.dot1_pos+1);
	 }
	 else
	 {
	     calc_seg_value(&hum_comps.dig1_0,4,num);
         hide_zero(&hum_comps.dig1_3,4,hum_comps.dot1_pos+1);
	 }
	 
	 MD_DIS_T10_CELSIUS;
	 
	 hum_comps.dis_oper_mark._bit.cur1=0;
	 display_line1_data();//display 1 line data
}



static long normal_mode_Mpa_switch(long num,int n,int *dis_dot,int opt,int *dis_opt)
{
    long tmp;
    const int  dot=device_comps.calibration_param.dot-lg(n);    //main  variable dot
    num=num/n;  //main  variable num
    switch((opt>4)?(opt=0):(opt)) 
    {
         case 0:
                MD_DIS_T5_MPA;
                *dis_dot=dot; 
                *dis_opt=0;
                break;
         case 1:
                MD_DIS_T4_BAR;
                *dis_dot=dot-1; 
                *dis_opt=1;
                break;
         case 2:
                *dis_dot=dot-3; 
                if(*dis_dot>-1)
                {
                    tmp=num*145.0377439;
                    tmp/=1000;
                    num=tmp;
                    MD_DIS_T3_PSI;
                    *dis_opt=2;
                    break;
                }
                 
         case 3:
                    *dis_dot=dot-3; 
                    if(*dis_dot>-1)
                    {
                        MD_DIS_T2_KPA;
                        *dis_opt=3;
                        break;
                    }
                 
         case 4: 
                *dis_dot=dot-2; 
                if(*dis_dot>-1)
                {
                    tmp=num*10.19716;
                    tmp/=100;
                    num=tmp;
                    MD_DIS_T9_KGF_CM_2;
                    *dis_opt=4;
                    break;
                }
               
         default:
                  *dis_dot=dot;
                   MD_DIS_T5_MPA;
                  *dis_opt=0;
                   break;
    }
    return num;
 }
                
static long normal_mode_Kpa_switch(long num,int n,int *dis_dot,int opt,int *dis_opt)
{
    long tmp;
    const int  dot=device_comps.calibration_param.dot-lg(n);    //main  variable dot
    num=num/n;  //main  variable num
    switch((opt>5)?(opt=0):(opt)) 
    {
       case 0:
                MD_DIS_T2_KPA;
                *dis_dot=dot;
                *dis_opt=0;
                break;
       case 1:
                MD_DIS_T4_BAR;
                #if(MD_DIS_DIG==MD_DIS5)
                    num=num/pwr(dot-2);
                    *dis_dot=4;
                #elif(MD_DIS_DIG==MD_DIS4)
                    num=num/pwr(dot+1-2);
                    *dis_dot=3;
                #endif
                *dis_opt=1;
                break;
       case 2:
                MD_DIS_T3_PSI;
                num=num*145.0377439/1000;
                *dis_dot=dot;
                *dis_opt=2;
                break;
       case 3:
                MD_DIS_T5_MPA;
               #if(MD_DIS_DIG==MD_DIS5)
                  num=num/pwr(dot-1);
                  *dis_dot=4;
               #elif(MD_DIS_DIG==MD_DIS4)
                  num=num/pwr(dot+1-1);
                  *dis_dot=3;
               #endif
                 *dis_opt=3;
                break;
       case 4: 
              MD_DIS_T9_KGF_CM_2;
              num=num*10.19716/100;
              *dis_dot=dot+1;
              *dis_opt=4;
              break;
       case 5: 
              
              *dis_dot=dot-3;
              if(*dis_dot>-1)
              {
                  MD_DIS_T8_PA;
                   *dis_opt=5;
                  break; 
              }
                    
       default:
              
            MD_DIS_T2_KPA;
            *dis_dot=dot; 
            *dis_opt=0;
            break;

    }
    return num;
}
static void normal_mode_display(unsigned char opt)
{
    
    int dis_dot;    
    int dis_opt;
    long num;
    #if(MD_DIS_DIG==MD_DIS5)
    long n=10;
    #elif(MD_DIS_DIG==MD_DIS4)
    long n=100;
    #endif
     //   if(hum_comps.dis_oper_mark._bit.refresh_option)
    //    {
     //       display_opt(opt);
      //      hum_comps.dis_oper_mark._bit.refresh_option=0;
      //  }
        
     
    if(device_comps.calibration_param.is_calibrated)
    {
       
        if(hum_comps.dis_oper_mark._bit.refresh_press)
        {
           
           hide_all_unit();
           if(device_comps.calibration_param.unit==0)//Mpa
           {
                num=normal_mode_Mpa_switch(device_comps.current_press,n,&dis_dot,opt,&dis_opt);
                hum_comps.dot2_pos=dis_dot;
                mode_comps[hum_comps.current_mode].dis_option=dis_opt;
                switch(dis_opt) 
                {
                    case 0:
                            MD_DIS_T5_MPA;
                            break;
                     case 1:
                            MD_DIS_T4_BAR;
                            break;
                     case 2:
                            MD_DIS_T3_PSI;
                            break;
                     case 3:
                            MD_DIS_T2_KPA;
                            break;
                     case 4: 
                            MD_DIS_T9_KGF_CM_2;
                            break;
                     default:
                            mode_comps[hum_comps.current_mode].dis_option=0;
                            MD_DIS_T5_MPA;
                            break;
                }             

           }
           
           else if((device_comps.calibration_param.unit&0x0f)==1)//Kpa 
           {
                num=normal_mode_Kpa_switch(device_comps.current_press,n,&dis_dot,opt,&dis_opt);
                hum_comps.dot2_pos=dis_dot;
                mode_comps[hum_comps.current_mode].dis_option=dis_opt;
                 switch(dis_opt) 
                 {
                      case 0:
                                MD_DIS_T2_KPA;
                                break;
                       case 1:
                                MD_DIS_T4_BAR;
                                break;
                       case 2:
                                MD_DIS_T3_PSI;
                                break;
                       case 3:
                                MD_DIS_T5_MPA;
                                break;
                       case 4: 
                                MD_DIS_T9_KGF_CM_2;
                                break;
                        case 5: 
                                MD_DIS_T8_PA;
                                break;  
                        default:
                                  mode_comps[hum_comps.current_mode].dis_option=0;
                                  MD_DIS_T2_KPA;
                                  break;
                 }
          }

          if(num<0)
          {
            num=-num;
	        hum_comps.dig2_5=MD_HIDE_DISP-1;
          }
    	  else
    	  {
    		 hum_comps.dig2_5=MD_HIDE_DISP;  
    	  }
    	  calc_seg_value(&hum_comps.dig2_0,5,num);
          hide_zero(&hum_comps.dig2_4,5,hum_comps.dot2_pos+1);
          
          hum_comps.dis_oper_mark._bit.refresh_press=0;
       }
       
        if(!device_comps.sw._bit.over_range)
        {
            hum_comps.dis_oper_mark._bit.cur2=0;
            hum_comps.dis_oper_mark._bit.dis2=1;
        }
        else
        {
            hum_comps.dis_oper_mark._bit.cur2=1;
            hum_comps.cursor_2=-1;
            hum_comps.cursor_2_count++;
            if(hum_comps.cursor_2_count>9)
            {
                hum_comps.cursor_2_count=0;
                if(hum_comps.dis_oper_mark._bit.dis2)
                {
                    hum_comps.dis_oper_mark._bit.dis2=0;
                    //hum_comps.dot2_pos=0;//
                }
                else
                {
                   hum_comps.dis_oper_mark._bit.dis2=1;
                }
            }
        } 

    }    
    else
    {
        hum_comps.dis_oper_mark._bit.cur2=0;
        hum_comps.dig2_0=MD_HIDE_DISP-1;
        hum_comps.dig2_1=MD_HIDE_DISP-1;
        hum_comps.dig2_2=MD_HIDE_DISP-1;
        hum_comps.dig2_3=MD_HIDE_DISP-1;
        hum_comps.dig2_4=MD_HIDE_DISP-1;
        hum_comps.dig2_5=MD_HIDE_DISP-1;
        hum_comps.dot2_pos=0;
        
    }
    display_line2_data();
    
    if(hum_comps.dis_oper_mark._bit.refresh_press_max&&device_comps.calibration_param.is_calibrated)
    {
        if(device_comps.calibration_param.unit==0)//Mpa
        {
            num=normal_mode_Mpa_switch(device_comps.max_press,n,&dis_dot,opt,&dis_opt);
        }
        else if((device_comps.calibration_param.unit&0x0f)==1)//Kpa 
        {
             num=normal_mode_Kpa_switch(device_comps.max_press,n,&dis_dot,opt,&dis_opt);
        }
        if(num<0)
        {
            num=-num;
	        hum_comps.dig3_5=MD_HIDE_DISP-1;
        }
    	else 
    	{
    		hum_comps.dig3_5=MD_HIDE_DISP;
    	}
        hum_comps.dot3_pos=dis_dot;
        if(hum_comps.dot3_pos==4)
        {
            hum_comps.dot3_pos=3;
            num/=10;
        }
        calc_seg_value(&hum_comps.dig3_0,5,num);
        
         MD_DIS_T11_MAX;
        hide_zero(&hum_comps.dig3_4,5,hum_comps.dot3_pos+1);
        hum_comps.dis_oper_mark._bit.refresh_press_max=0;
        hum_comps.dis_oper_mark._bit.cur3=0;
        display_line3_data();
    }
     
	//if(hum_comps.dis_oper_mark._bit.refresh_temp)
	//{
     //   display_temp();//refresh temperature
		//hum_comps.dis_oper_mark._bit.refresh_temp=0;
	//}
	
	
}
static void debug_mode_display(unsigned char opt)
{
   long num=0;
   switch (opt)
   {
      case 0:
    	if(hum_comps.dis_oper_mark._bit.refresh_press_adc)
    	{
            num=device_comps.ad1_ad2_average_result; 
 			//	device_comps.output_debug_info(&device_comps);
 			if(num<0)
 			{
 			    num=-num;
 			    hum_comps.dig2_5=MD_HIDE_DISP-1;
 			}
 			else
 			{
                hum_comps.dig2_5=MD_HIDE_DISP;
 			}
 			hum_comps.dot2_pos=0;
 			calc_seg_value(&hum_comps.dig2_0,5,num);
            hum_comps.dis_oper_mark._bit.refresh_press_adc=0;
    	}


        if(device_comps.sw._bit.adc_stb)
        {
            hum_comps.dis_oper_mark._bit.cur2=0;
            hum_comps.dis_oper_mark._bit.dis2=1;
        }
        else
        {
            hum_comps.dis_oper_mark._bit.cur2=1;
            hum_comps.cursor_2=-1;
            hum_comps.cursor_2_count++;
            if(hum_comps.cursor_2_count>4)
            {
                hum_comps.cursor_2_count=0;
                if(hum_comps.dis_oper_mark._bit.dis2)
                {
                    hum_comps.dis_oper_mark._bit.dis2=0;
                }
                else
                {
                   hum_comps.dis_oper_mark._bit.dis2=1;
                }
            }
        }
         display_line2_data(); 
    	
    	
    	if(hum_comps.dis_oper_mark._bit.refresh_temp_adc)
    	{
    	    num=device_comps.temp_p_temp_n_average_result; 
       		if(num<0)
 			{
 			    num=-num;
 			    hum_comps.dig3_5=MD_HIDE_DISP-1;
 			}
 			else
 			{
                hum_comps.dig3_5=MD_HIDE_DISP;
 			}
 			hum_comps.dot3_pos=0;
 			calc_seg_value(&hum_comps.dig3_0,5,num);
           hum_comps.dis_oper_mark._bit.refresh_temp_adc=0;
    	}
      
       
        if(device_comps.sw._bit.temp_adc_stb)
        {
            hum_comps.dis_oper_mark._bit.cur3=0;
            hum_comps.dis_oper_mark._bit.dis3=1;
        }
        else
        {
            hum_comps.dis_oper_mark._bit.cur3=1;
            hum_comps.cursor_3=-1;
            hum_comps.cursor_3_count++;
            if(hum_comps.cursor_3_count>4)
            {
                hum_comps.cursor_3_count=0;
                if(hum_comps.dis_oper_mark._bit.dis3)
                {
                    hum_comps.dis_oper_mark._bit.dis3=0;
                }
                else
                {
                   hum_comps.dis_oper_mark._bit.dis3=1;
                }
            }
        }
        display_line3_data();
        
        break;
        
     case 1:
	    num=device_comps.report_param.triggerTimes;
        hum_comps.dot2_pos=0;//
        calc_seg_value(&hum_comps.dig2_0,6,num);
        hum_comps.dig3_0=MD_HIDE_DISP-1;
        hum_comps.dig3_1=MD_HIDE_DISP-1;
        hum_comps.dig3_2=MD_HIDE_DISP-1;
        hum_comps.dig3_3=MD_HIDE_DISP-1;
        hum_comps.dig3_4=MD_DIS_r;
        hum_comps.dig3_5=MD_DIS_t;
        hum_comps.dot3_pos=0;//
        hum_comps.dis_oper_mark._bit.cur2=0;
        hum_comps.dis_oper_mark._bit.cur3=0;
        display_line2_data();
        display_line3_data();
        break;
    }    
	
}
static void pwd_mode_display(unsigned char opt)
{
    if(hum_comps.dis_oper_mark._bit.cur2)
    {
        hum_comps.cursor_2_count++;
        if(hum_comps.cursor_2_count>2)
        {
           hum_comps.cursor_2_count=0;
            if(hum_comps.dis_oper_mark._bit.dis2)
            {
                hum_comps.dis_oper_mark._bit.dis2=0;
            }
            else
            {
               hum_comps.dis_oper_mark._bit.dis2=1;
            }
            display_line2_data();
        }
    }

}
static void param_query_mode_display(unsigned char opt)
{
    if(hum_comps.dis_oper_mark._bit.refresh_option)
    {
        display_opt(opt);
        hum_comps.dis_oper_mark._bit.refresh_option=0;
    }
 	if(hum_comps.dis_oper_mark._bit.refresh_param_modifyed)   
	{
        calc_seg_value(&hum_comps.dig2_0, 6, *(&device_comps.float_level_param.bottom_s+opt));
	    hum_comps.dot2_pos=3;
		display_line2_data();
		hum_comps.dis_oper_mark._bit.refresh_param_modifyed=0;
		
     }
}
static void param_modify_mode_display(unsigned char opt)
{
        if(hum_comps.dis_oper_mark._bit.refresh_option)
        {
            display_opt(opt);
            hum_comps.dis_oper_mark._bit.refresh_option=0;
        }
        
        if(hum_comps.dis_oper_mark._bit.refresh_param_modifyed)   
	    {
    	    calc_seg_value(&hum_comps.dig2_0, 6, *(&device_comps.float_level_param.bottom_s+opt));
    		hum_comps.dot2_pos=3;
    		display_line2_data();
    		hum_comps.dis_oper_mark._bit.refresh_param_modifyed=0;
		
     }
        if(hum_comps.dis_oper_mark._bit.cur2)
        {
            hum_comps.cursor_2_count++;
            if(hum_comps.cursor_2_count>2)
            {
                hum_comps.cursor_2_count=0;
                if(hum_comps.dis_oper_mark._bit.dis2)
                {
                    hum_comps.dis_oper_mark._bit.dis2=0;
                }
                else
                {
                    hum_comps.dis_oper_mark._bit.dis2=1;
                }
                
                display_line2_data();
           
            }
        }

	
}

static void self_test_mode_display(unsigned char opt)
{
	if(hum_comps.dis_oper_mark._bit.refresh_err_code)
	{
        hum_comps.dig2_0=0;
		hum_comps.dig2_1=0;
		hum_comps.dig2_2=0;
		hum_comps.dig2_3=0;
		hum_comps.dig2_4=0;
		hum_comps.dig2_5=0;
		if(device_comps.sw._bit.e2prom_driver_err)
		{
			hum_comps.dig2_0=1;
		}
	    if(device_comps.sw._bit.cs123x_driver_err)
		{
			hum_comps.dig2_1=1;
		}
		if(device_comps.sw._bit.lora_module_err)
		{
			hum_comps.dig2_2=1;
		}
	
        if(device_comps.sw._bit.rtc_module_err)
		{
			hum_comps.dig2_4=1;
		}
		
		hum_comps.dig2_5=0x0e;
		hum_comps.dot2_pos=0;// 
		display_line2_data();

	    hum_comps.dis_oper_mark._bit.refresh_err_code=0;
	}
}
/*********************************END DISPLAY FNCTION************************************************************/






static void lcd_test(hum_comps_t *const this)
{
	if(this->count==0*6)
	{
		MD_DISPLAY_WRITE_D0_0(this->dig0_0);// D00 H  L 
	}
    if(this->count==1*6)
	{
		MD_DISPLAY_WRITE_D0_1(this->dig0_1);
	}
	if(this->count==2*6)
	{
		MD_DISPLAY_WRITE_D1_0(this->dig1_0);
	}
	if(this->count==3*6)
	{
		MD_DISPLAY_WRITE_D1_1(this->dig1_1);
	}
	if(this->count==4*6)
	{
		MD_DISPLAY_WRITE_D1_2(this->dig1_2);// D04 H  L 
	}
	if(this->count==5*6)
	{
		MD_DISPLAY_WRITE_D1_3(this->dig1_3);// D10 H  L  
	}
	if(this->count==6*6)
	{
		MD_DISPLAY_WRITE_D2_0(this->dig2_0);// D11 H  L  
	}
	if(this->count==7*6)
	{
		MD_DISPLAY_WRITE_D2_1(this->dig2_1);// D12 H  L  
	}
	if(this->count==8*6)
	{
		MD_DISPLAY_WRITE_D2_2(this->dig2_2);// D12 H  L  
	}
	if(this->count==9*6)
	{
		MD_DISPLAY_WRITE_D2_3(this->dig2_3);// D12 H  L  
	}
    if(this->count==10*6)
    {
    	MD_DISPLAY_WRITE_D2_4(this->dig2_4);// D12 H  L  
    }
    if(this->count==11*6)
    {
    	MD_DISPLAY_WRITE_D2_5(this->dig2_5);// D12 H  L  
    }
    if(this->count==12*6)
    {
        dis_all_unit();
    }
    if(this->count==13*6)
    {
        
    	clr_lcd();
        MD_DISPLAY_WRITE_D2_4(MD_DIS_r);//R
        MD_DISPLAY_WRITE_D2_3(5);// S
        MD_DISPLAY_WRITE_D2_2(MD_DIS_t);// T
        MD_DISPLAY_WRITE_D2_1(MD_HIDE_DISP-1);//-
        MD_DISPLAY_WRITE_D2_0(systemComps.rst_code);//-
       
    }
   
}




static void handle_key(hum_comps_t *this)
{
	this->get_key(this);
	if(this->key_type==EM_NO_KEY)
	{
		return;
	}
	else if(this->key_type==EM_SHORT_KEY)
	{
		switch(this->key)
		{
			case MD_S_KEY:
					on_s_key();
			 		break;
			case MD_M_KEY:
					on_m_key();
	 				break;
			case MD_J_KEY:
					on_j_key();
	 				break;
			default:
					break;
		}
	}
	else if(this->key_type==EM_LONG_KEY)
	{
		switch(this->key)
		{
			case MD_S_KEY:
					on_long_s_key();
					break;
			case MD_M_KEY:
					on_long_m_key();
					break;
			case MD_J_KEY:
					on_long_j_key();
					break;
			case MD_S_KEY&MD_J_KEY:
					on_long_s_and_j_key();
					break;	
			
			default:
					break;
		}
	}
	
}


static void cal_query_mode_display(unsigned char opt)
{
	if(hum_comps.dis_oper_mark._bit.refresh_option)
    {
        display_opt(opt);
        hum_comps.dis_oper_mark._bit.refresh_option=0;
    }
    if(device_comps.cal_type==EM_CAL_PRESS)
    {
        display_press_adc();
        
        if(hum_comps.dis_oper_mark._bit.refresh_cal_param)
        {
            calc_seg_value(&hum_comps.dig2_0, 6,device_comps.calibration_param.y[opt]);
            hum_comps.dot2_pos=device_comps.calibration_param.dot;
            if((device_comps.calibration_param.unit&0x01)==0)
            {
                MD_DIS_T5_MPA;
            }
            else if((device_comps.calibration_param.unit&0x01)==1)
            {
                MD_DIS_T2_KPA;
            }
            display_line2_data();
            hum_comps.dis_oper_mark._bit.refresh_cal_param=0;
        }
        
    }
    else if(device_comps.cal_type==EM_CAL_RES)
    {
        display_temp_adc();
        if(hum_comps.dis_oper_mark._bit.refresh_res_cal_param)
        {
            
            calc_seg_value(&hum_comps.dig2_0, 6,device_comps.res_calibration_param.y[opt]);
            hum_comps.dot2_pos=2;
            display_line2_data();
            hum_comps.dis_oper_mark._bit.refresh_res_cal_param=0;
        }
    }
    else if(device_comps.cal_type==EM_CAL_HIGH)
    {
        display_high_adc();
        if(hum_comps.dis_oper_mark._bit.refresh_high_cal_param)
        {
            calc_seg_value(&hum_comps.dig2_0, 6, device_comps.high_calibration_param.y[opt]);
             hum_comps.dot2_pos=device_comps.high_calibration_param.dot;
             MD_DIS_T7_M;
            display_line2_data();
            hum_comps.dis_oper_mark._bit.refresh_high_cal_param=0;
        }
    }
 
    
}
static void cal_modify_mode_display(unsigned char opt)
{
	if(hum_comps.dis_oper_mark._bit.refresh_option)
    {
        display_opt(opt);
        hum_comps.dis_oper_mark._bit.refresh_option=0;
    }

    if(device_comps.cal_type==EM_CAL_PRESS)
    {
        display_press_adc();
       
        if(hum_comps.dis_oper_mark._bit.refresh_cal_param)
        {
            calc_seg_value(&hum_comps.dig2_0, 6,device_comps.calibration_param.y[opt]);
            hum_comps.dot2_pos=device_comps.calibration_param_bak.dot;
            if((device_comps.calibration_param_bak.unit&0x01)==0)
            {
                MD_DIS_T5_MPA;
            }
            else if((device_comps.calibration_param_bak.unit&0x01)==1)
            {
                MD_DIS_T2_KPA;
            }
            hum_comps.dis_oper_mark._bit.refresh_cal_param=0;
            
       }
    }
    else if(device_comps.cal_type==EM_CAL_RES)
    {
        display_temp_adc();
        if(hum_comps.dis_oper_mark._bit.refresh_res_cal_param)
        {
			calc_seg_value(&hum_comps.dig2_0, 6,device_comps.res_calibration_param.y[opt]);
            hum_comps.dot2_pos=2;
           // display_line2_data();
            hum_comps.dis_oper_mark._bit.refresh_res_cal_param=0;
        }
            
    }
    else if(device_comps.cal_type==EM_CAL_HIGH)
    {
        display_high_adc();
        if(hum_comps.dis_oper_mark._bit.refresh_high_cal_param)
        {
	       calc_seg_value(&hum_comps.dig2_0, 6, device_comps.high_calibration_param.y[opt]);
            hum_comps.dot2_pos=device_comps.high_calibration_param_bak.dot;;
           // display_line2_data();
           MD_DIS_T7_M;
            hum_comps.dis_oper_mark._bit.refresh_high_cal_param=0;
        }
            
    }
    if(hum_comps.dis_oper_mark._bit.cur2)
	{
		hum_comps.cursor_2_count++;
		if(hum_comps.cursor_2_count>2)
		{
		    hum_comps.cursor_2_count=0;
			if(hum_comps.dis_oper_mark._bit.dis2)
			{
				hum_comps.dis_oper_mark._bit.dis2=0;
			}
			else
			{
				hum_comps.dis_oper_mark._bit.dis2=1;
			}
			
			display_line2_data();
		/*	if(sensor_comps.sw&MD_DENSITY_STB)
			{
				 DIS_S5_STB; 
			}
			else
			{
				FLASH_S5_STB;
			}
		*/
		}
	}
	
}

static long iot_mode_Mpa_switch(long num,int n,int *dis_dot,int opt,int *dis_opt)
{
    long tmp;
    const int  dot=device_comps.calibration_param.dot-lg(n);    //main  variable dot
    num=num/n;  //main  variable num
    switch(opt) 
    {
         case 0:
                MD_DIS_T5_MPA;
                *dis_dot=dot; 
                *dis_opt=0;
                break;
         case 1:
                if(dot-3>-1)
                {
                     MD_DIS_T2_KPA;
                    *dis_dot=dot-3;
                    *dis_opt=1;
                     break;
                }
         case 2:
                if(dot-2>-1)
                {
                    MD_DIS_T7_M;
                    *dis_dot=dot-2; 
                    *dis_opt=2;
                    break;
                }
                 
        default:
                  *dis_opt=3;
                   break;
    }
    return num;
 }

static long iot_mode_Kpa_m_switch(long num,int n,int *dis_dot,int opt,int *dis_opt)
{
    long tmp;
    const int  dot=device_comps.calibration_param.dot-lg(n);    //main  variable dot
    num=num/n;  //main  variable num
    switch(opt) 
    {
       case 0:
                MD_DIS_T7_M; 
                *dis_dot=dot+1;
                *dis_opt=0;
                break;
               
       case 1:
                MD_DIS_T2_KPA;
                *dis_dot=dot;
                *dis_opt=1;
                break;
               
                
       case 2:
                 MD_DIS_T5_MPA; 
                *dis_dot=4;
                *dis_opt=2;
                 num=num/pwr(dot-1);
                 break;
               
       default:
                  *dis_opt=3;
                   break;

    }
    return num;
}

static long iot_mode_Kpa_switch(long num,int n,int *dis_dot,int opt,int *dis_opt)
{
    long tmp;
    const int  dot=device_comps.calibration_param.dot-lg(n);    //main  variable dot
    num=num/n;  //main  variable num
    switch(opt)
    {
       case 0:
                MD_DIS_T2_KPA;
                *dis_dot=dot;
                *dis_opt=0;
                break;
       case 1:
                MD_DIS_T5_MPA; 
                *dis_dot=4;
                *dis_opt=1;
                 num=num/pwr(dot-1);
                 break;
                
       case 2:
                MD_DIS_T7_M; 
                *dis_dot=dot+1;
                *dis_opt=2;
                break;
       default:
                  *dis_opt=3;
                   break;

    }
    return num;
}

static iot_mode_display_press(unsigned char opt)
{
    int dis_dot;    
    int dis_opt;
    long num;
    long n=10;
    if(device_comps.calibration_param.is_calibrated)
    {
       
        if(hum_comps.dis_oper_mark._bit.refresh_press)
        {
           
           hide_all_unit();
           if(device_comps.calibration_param.unit==0)//Mpa
           {
                num=iot_mode_Mpa_switch(device_comps.current_press,n,&dis_dot,opt,&dis_opt);
                hum_comps.dot2_pos=dis_dot;
                mode_comps[hum_comps.current_mode].dis_option=dis_opt;
                switch(dis_opt) 
                {
                     case 0:
                            MD_DIS_T5_MPA;
                            break;
                     case 1:
                            MD_DIS_T2_KPA;
                            break;
                     case 2:
                            MD_DIS_T7_M;
                            break;
                    default:
                            mode_comps[hum_comps.current_mode].dis_option=dis_opt;
                            break;
                }             

           }
           
           else if(device_comps.calibration_param.unit==1)//Kpa 
           {
                num=iot_mode_Kpa_switch(device_comps.current_press,n,&dis_dot,opt,&dis_opt);
                hum_comps.dot2_pos=dis_dot;
                mode_comps[hum_comps.current_mode].dis_option=dis_opt;
                 switch(dis_opt) 
                 {
                       case 0:
                                MD_DIS_T2_KPA;
                                break;
                       case 1:
                                MD_DIS_T5_MPA;
                                break;
                       case 2:
                                MD_DIS_T7_M;
                                break;
                       default:
                                mode_comps[hum_comps.current_mode].dis_option=dis_opt;
                                break;
                 }
           }
           else if(device_comps.calibration_param.unit==0x81)//Kpa -m
           {
                num=iot_mode_Kpa_m_switch(device_comps.current_press,n,&dis_dot,opt,&dis_opt);
                hum_comps.dot2_pos=dis_dot;
                mode_comps[hum_comps.current_mode].dis_option=dis_opt;
                 switch(dis_opt) 
                 {
                       case 0:
                                MD_DIS_T7_M;
                                break;
                       case 1:
                                MD_DIS_T2_KPA;
                                break;
                       case 2:
                                MD_DIS_T5_MPA;
                                break;
                       default:
                                mode_comps[hum_comps.current_mode].dis_option=dis_opt;
                                break;
                 }
           }
           calc_seg_value(&hum_comps.dig2_0, 6,num);
          
           hide_zero(&hum_comps.dig2_5,6,hum_comps.dot2_pos+1);
           hum_comps.dis_oper_mark._bit.refresh_press=0;
       }
       
        if(!device_comps.sw._bit.over_range)
        {
            hum_comps.dis_oper_mark._bit.cur2=0;
            hum_comps.dis_oper_mark._bit.dis2=1;
        }
        else
        {
            hum_comps.dis_oper_mark._bit.cur2=1;
            hum_comps.cursor_2=-1;
            hum_comps.cursor_2_count++;
            if(hum_comps.cursor_2_count>9)
            {
                hum_comps.cursor_2_count=0;
                if(hum_comps.dis_oper_mark._bit.dis2)
                {
                    hum_comps.dis_oper_mark._bit.dis2=0;
                    //hum_comps.dot2_pos=0;//
                }
                else
                {
                   hum_comps.dis_oper_mark._bit.dis2=1;
                }
            }
        } 

    }    
    else
    {
        hum_comps.dis_oper_mark._bit.cur2=0;
        hum_comps.dig2_0=MD_HIDE_DISP-1;
        hum_comps.dig2_1=MD_HIDE_DISP-1;
        hum_comps.dig2_2=MD_HIDE_DISP-1;
        hum_comps.dig2_3=MD_HIDE_DISP-1;
        hum_comps.dig2_4=MD_HIDE_DISP-1;
        hum_comps.dig2_5=MD_HIDE_DISP-1;
        hum_comps.dot2_pos=0;
        
    }
    display_line2_data();
}

static _4g_mode_display(unsigned char opt)
{
    if(hum_comps.dis_oper_mark._bit.refresh_option)
    {
        display_opt(opt);
        hum_comps.dis_oper_mark._bit.refresh_option=0;
    }
    
    switch(opt)
    {
       case 0: 
                    #ifdef MD_HIGH
                        display_high(opt);
                        break;
                    #endif
       case 1: 
		    #ifdef MD_HIGH
                        display_volume(opt);
                        break;
                    #endif
            
       case 2: 
                   #if defined (MD_TEMP)
                        if(hum_comps.dis_oper_mark._bit.refresh_temp)
                    	{
                            display_temp_to_line2();//refresh temperature
                    	    hum_comps.dis_oper_mark._bit.refresh_temp=0;
                    	}
                    	mode_comps[hum_comps.current_mode].dis_option=2;
			            break;
                    #elif defined (MD_HIGH)
                	   
                            display_bottom_s(opt);
                            break;
                	#else
                            iot_mode_display_press(opt);
                            break;
			        #endif
                    
        case 3:
               if(hum_comps.dis_oper_mark._bit.refresh_date)
               {
                    hide_all_unit();
                    hum_comps.dot2_pos=0;
                    hum_comps.dig2_0=rtc_valve.day&0x0f;
                    hum_comps.dig2_1=rtc_valve.day>>4;
                    hum_comps.dig2_2=rtc_valve.month&0x0f;
                    hum_comps.dig2_3=rtc_valve.month>>4;
                    hum_comps.dig2_4=rtc_valve.year&0x0f;
                    hum_comps.dig2_5=rtc_valve.year>>4;
                    display_line2_data();    // 
                    MD_DIS_P6_DOT;
                    MD_DIS_P8_DOT;
                    hum_comps.dis_oper_mark._bit.refresh_date=0;
                }
               
                break;
            //display date
        case 4: 
                if(hum_comps.dis_oper_mark._bit.refresh_time)
                {
                    hide_all_unit();
                    hum_comps.dot2_pos=0;
                    hum_comps.dig2_0=rtc_valve.sec&0x0f;
                    hum_comps.dig2_1=rtc_valve.sec>>4;
                    hum_comps.dig2_2=rtc_valve.min&0x0f;
                    hum_comps.dig2_3=rtc_valve.min>>4;
                    hum_comps.dig2_4=rtc_valve.hour&0x0f;
                    hum_comps.dig2_5=rtc_valve.hour>>4;
                    display_line2_data();    // 
                    MD_DIS_COL;
                    hum_comps.dis_oper_mark._bit.refresh_time=0;
                }
        //display time
            break;  

            case 5:     //display device id hi
                if(hum_comps.dis_oper_mark._bit.refresh_device_id_high)
                {
                    hide_all_unit();
                    hum_comps.dot2_pos=0;
                    hum_comps.dig2_0=device_comps.device_addr.addr[4]&0x0f;
                    hum_comps.dig2_1=device_comps.device_addr.addr[4]>>4;
                    hum_comps.dig2_2=device_comps.device_addr.addr[3]&0x0f;
                    hum_comps.dig2_3=device_comps.device_addr.addr[3]>>4;
                    hum_comps.dig2_4=device_comps.device_addr.addr[2]&0x0f;
                    hum_comps.dig2_5=device_comps.device_addr.addr[2]>>4;
                    display_line2_data();    // 
                     hum_comps.dis_oper_mark._bit.refresh_device_id_high=0;
                }
    
                break;
            case 6:     //display device id low
                if(hum_comps.dis_oper_mark._bit.refresh_device_id_low)
                {
                    hide_all_unit();
                    hum_comps.dot2_pos=0;
                    hum_comps.dig2_0=device_comps.device_addr.addr[6]&0x0f;
                    hum_comps.dig2_1=device_comps.device_addr.addr[6]>>4;
                    hum_comps.dig2_2=device_comps.device_addr.addr[5]&0x0f;
                    hum_comps.dig2_3=device_comps.device_addr.addr[5]>>4;
                    hum_comps.dig2_4=MD_HIDE_DISP;
                    hum_comps.dig2_5=MD_HIDE_DISP;
                    display_line2_data();    // 
                    hum_comps.dis_oper_mark._bit.refresh_device_id_low=0;
                }
    
                break;
            case 7:     //refresh ver
                if(hum_comps.dis_oper_mark._bit.refresh_device_id_low)
                {
                    hide_all_unit();
                    hum_comps.dot2_pos=1;
                    hum_comps.dig2_0=MD_FL_VER%10;
                    hum_comps.dig2_1=MD_FL_VER/10%10;
                    hum_comps.dig2_2=MD_HIDE_DISP-1;
                    hum_comps.dig2_3=0x0f;
                    hum_comps.dig2_4=MD_HIDE_DISP;
                    hum_comps.dig2_5=MD_HIDE_DISP;
                    display_line2_data();    // 
                    hum_comps.dis_oper_mark._bit.refresh_device_id_low=0;
                }
    
                break;
         default:
                mode_comps[hum_comps.current_mode].dis_option=0;
            break;
    
    
}
if(hum_comps.dis_oper_mark._bit.refresh_temp)
{
    display_temp();//refresh temperature
    hum_comps.dis_oper_mark._bit.refresh_temp=0;
}
        

}
static void display_temp_to_line2(void)
{
     
	long num=device_comps.current_temp;
	 //MD_DIS_T10_CELSIUS;
	 MD_DIS_T6_CELSIUS;
	switch(num<0)
	{
		case 1: num=-num;
			if(num<100)
			{
				hum_comps.dig2_0=num%10;
				hum_comps.dig2_1=num/10%10;
				hum_comps.dot2_pos=1;//                                                                
				hum_comps.dig2_2=MD_HIDE_DISP-1;//dis -  
				hum_comps.dig2_3=MD_HIDE_DISP;//
				hum_comps.dig2_4=MD_HIDE_DISP;// 
				hum_comps.dig2_5=MD_HIDE_DISP;// 
			}
			else if(num<1000)
			{
			    hum_comps.dig2_0=num%10;
				hum_comps.dig2_1=num/10%10;
				hum_comps.dig2_2=num/100%10;
				hum_comps.dot2_pos=1;
				hum_comps.dig2_3=MD_HIDE_DISP-1;//
				hum_comps.dig2_4=MD_HIDE_DISP;// 
				hum_comps.dig2_5=MD_HIDE_DISP;//
			}
			else
			{
				hum_comps.dig2_0=MD_HIDE_DISP;
				hum_comps.dig2_1=MD_HIDE_DISP;
				hum_comps.dig2_2=MD_HIDE_DISP;// 
				hum_comps.dot2_pos=0;
				hum_comps.dig2_3=MD_HIDE_DISP;//
				hum_comps.dig2_4=MD_HIDE_DISP;//
				hum_comps.dig2_5=MD_HIDE_DISP-1;//Overrange display - _ _
			}
			break;
		
		case 0:
			if(num<100)
			{
				hum_comps.dig2_0=num%10;
				hum_comps.dig2_1=num/10%10;
				hum_comps.dot2_pos=1;//P2    
				hum_comps.dig2_2=MD_HIDE_DISP;
				hum_comps.dig2_3=MD_HIDE_DISP;
				hum_comps.dig2_4=MD_HIDE_DISP;//dis - 
				hum_comps.dig2_5=MD_HIDE_DISP;//dis -
			}
			else if(num<1000)
			{
				hum_comps.dig2_0=num%10;
				hum_comps.dig2_1=num/10%10;
				hum_comps.dot2_pos=1;//P2     
				hum_comps.dig2_2=num/100%10;
				hum_comps.dig2_3=MD_HIDE_DISP;
				hum_comps.dig2_4=MD_HIDE_DISP;//dis - 
				hum_comps.dig2_5=MD_HIDE_DISP;//dis -
			}
			else if(num<10000)
			{
				hum_comps.dig2_0=num%10;
				hum_comps.dig2_1=num/10%10;
				hum_comps.dig2_2=num/100%10;
				hum_comps.dig2_3=num/1000%10;
				hum_comps.dot2_pos=1;//  
				hum_comps.dig2_4=MD_HIDE_DISP;//dis - 
				hum_comps.dig2_5=MD_HIDE_DISP;//dis -
			}
			else   // Overrange display 1_ _
			{
				hum_comps.dig2_0=MD_HIDE_DISP;
				hum_comps.dig2_1=MD_HIDE_DISP;
				hum_comps.dig2_2=MD_HIDE_DISP;
				hum_comps.dig2_3=MD_HIDE_DISP;
				hum_comps.dig2_4=MD_HIDE_DISP;//dis 
				hum_comps.dig2_5=1;
				hum_comps.dot2_pos=0;
		    }
			break;
		default:
			break;
	}
	display_line2_data();//display 1 line data

}

static void lora_mode_display(unsigned char opt) 
{

        if(hum_comps.dis_oper_mark._bit.refresh_option)
        {
            display_opt(opt);
            hum_comps.dis_oper_mark._bit.refresh_option=0;
        }
        
        switch(opt)
        {
            case 0: 
                    #ifdef MD_HIGH
                        display_high(opt);
                        break;
                    #endif
            case 1: 
		    #ifdef MD_HIGH
                        display_volume(opt);
                        break;
                    #endif
            
            case 2: 
                   #if defined (MD_TEMP)
                        if(hum_comps.dis_oper_mark._bit.refresh_temp)
                    	{
                            display_temp_to_line2();//refresh temperature
                    	    hum_comps.dis_oper_mark._bit.refresh_temp=0;
                    	}
                    	mode_comps[hum_comps.current_mode].dis_option=2;
			            break;
                    #elif defined (MD_HIGH)
                	   
                            display_bottom_s(opt);
                            break;
                	#else
                            
                            iot_mode_display_press(opt);
                            break;
			        #endif
                    
                   
            case 3:
                   if(hum_comps.dis_oper_mark._bit.refresh_date)
                   {
                        hum_comps.dot2_pos=0;
                        hum_comps.dig2_0=rtc_valve.day&0x0f;
                        hum_comps.dig2_1=rtc_valve.day>>4;
                        hum_comps.dig2_2=rtc_valve.month&0x0f;
                        hum_comps.dig2_3=rtc_valve.month>>4;
                        hum_comps.dig2_4=rtc_valve.year&0x0f;
                        hum_comps.dig2_5=rtc_valve.year>>4;
                        display_line2_data();    // 
                        //MD_HIDE_T10_CELSIUS;
                        MD_HIDE_T6_CELSIUS;
                        MD_HIDE_T5_MPA;MD_HIDE_T2_KPA;MD_HIDE_T7_M;
                        MD_DIS_P6_DOT;
                        MD_DIS_P8_DOT;
                        hum_comps.dis_oper_mark._bit.refresh_date=0;
                    }
                   
                    break;
                //display date
            case 4: 
                    if(hum_comps.dis_oper_mark._bit.refresh_time)
                    {
                        hum_comps.dot2_pos=0;
                        hum_comps.dig2_0=rtc_valve.sec&0x0f;
                        hum_comps.dig2_1=rtc_valve.sec>>4;
                        hum_comps.dig2_2=rtc_valve.min&0x0f;
                        hum_comps.dig2_3=rtc_valve.min>>4;
                        hum_comps.dig2_4=rtc_valve.hour&0x0f;
                        hum_comps.dig2_5=rtc_valve.hour>>4;
                         display_line2_data();    // 
                         MD_HIDE_T5_MPA;MD_HIDE_T2_KPA;MD_HIDE_T7_M;
                         MD_HIDE_P6_DOT;
                        MD_HIDE_P8_DOT;
                        MD_DIS_COL;
                        hum_comps.dis_oper_mark._bit.refresh_time=0;
                    }
            //display time
                break;  

                case 5:     //display device id hi
                    if(hum_comps.dis_oper_mark._bit.refresh_device_id_high)
                    {
                        if(loraComps.sw._bit.noParameter)
                        {
                            hum_comps.dot2_pos=0;
                            hum_comps.dig2_0=MD_HIDE_DISP;
                            hum_comps.dig2_1=MD_HIDE_DISP;
                            hum_comps.dig2_2=MD_HIDE_DISP;
                            hum_comps.dig2_3=MD_HIDE_DISP;
                            hum_comps.dig2_4=MD_HIDE_DISP;
                            hum_comps.dig2_5=MD_HIDE_DISP;
                        }
                        else
                        {
                            hum_comps.dot2_pos=0;
                            hum_comps.dig2_0=(loraComps.yl701_info_p->freq)&0x0000000f;
                            hum_comps.dig2_1=(loraComps.yl701_info_p->freq>>4)&0x0000000f;
                            hum_comps.dig2_2=(loraComps.yl701_info_p->freq>>8)&0x0000000f;
                            hum_comps.dig2_3=(loraComps.yl701_info_p->freq>>12)&0x0000000f;
                            hum_comps.dig2_4=(loraComps.yl701_info_p->freq>>16)&0x0000000f;
                            hum_comps.dig2_5=(loraComps.yl701_info_p->freq>>20)&0x0000000f;
                        }
                       
                        display_line2_data();    // 
                        MD_HIDE_COL;MD_HIDE_T5_MPA;MD_HIDE_T2_KPA;MD_HIDE_T7_M;
                        hum_comps.dis_oper_mark._bit.refresh_device_id_high=0;
                    }
        
                    break;
                case 6:     //display device id low
                    if(hum_comps.dis_oper_mark._bit.refresh_device_id_low)
                    {
                         if(loraComps.sw._bit.noParameter)
                        {
                            hum_comps.dot2_pos=0;
                            hum_comps.dig2_0=MD_HIDE_DISP;
                            hum_comps.dig2_1=MD_HIDE_DISP;
                            hum_comps.dig2_2=MD_HIDE_DISP;
                            hum_comps.dig2_3=MD_HIDE_DISP;
                            hum_comps.dig2_4=MD_HIDE_DISP;
                            hum_comps.dig2_5=MD_HIDE_DISP;
                        }
                        else
                        {
                            hum_comps.dot2_pos=0;
                            hum_comps.dig2_0=loraComps.yl701_info_p->NodeId%10;
                            hum_comps.dig2_1=loraComps.yl701_info_p->NodeId/10%10;
                            hum_comps.dig2_2=loraComps.yl701_info_p->NodeId/100%10;
                            hum_comps.dig2_3=loraComps.yl701_info_p->netId%10;
                            hum_comps.dig2_4=loraComps.yl701_info_p->breathTime%10;
                            hum_comps.dig2_5=loraComps.yl701_info_p->breathPeriod%10;
                        }
                        display_line2_data();    // 
                        MD_HIDE_COL;MD_HIDE_T5_MPA;MD_HIDE_T2_KPA;MD_HIDE_T7_M;
                        hum_comps.dis_oper_mark._bit.refresh_device_id_low=0;
                    }
        
                    break;
                case 7:     //refresh ver
                    if(hum_comps.dis_oper_mark._bit.refresh_ver)
                    {
                        hum_comps.dot2_pos=1;
                        hum_comps.dig2_0=MD_FL_VER%10;
                        hum_comps.dig2_1=MD_FL_VER/10%10;
                        hum_comps.dig2_2=MD_HIDE_DISP-1;
                        hum_comps.dig2_3=0x0f;
                        hum_comps.dig2_4=MD_HIDE_DISP;
                        hum_comps.dig2_5=MD_HIDE_DISP;
                        display_line2_data();    // 
                        MD_HIDE_COL;MD_HIDE_T5_MPA;MD_HIDE_T2_KPA;MD_HIDE_T7_M;
                        hum_comps.dis_oper_mark._bit.refresh_ver=0;
                    }
        
                    break;
             default:
                    mode_comps[hum_comps.current_mode].dis_option=0;
                break;
        
        
    }
    #ifndef MD_TEMP
    if(hum_comps.dis_oper_mark._bit.refresh_temp)
    {
        display_temp();//refresh temperature
        hum_comps.dis_oper_mark._bit.refresh_temp=0;
    }
    #endif
    
}

static void  display_ip(unsigned char opt)
{
   int flag;
   
   if(netComps.net_info.currentIP_No<2)
   {
        if(netComps.St._bit.ResolvedIP)
        {
            flag=1;//fill ip
        }
        else
        {
             flag=2;//fill __
        }
   }
   else //no set ip
   {
       flag=0;//fill 0
   }

    
   switch(opt)
   {
        case 0: 
          if(flag==1)    
          { 
                hum_comps.dig3_0=netComps.net_info.ResolvedIP[2]&0x0f;
                hum_comps.dig3_1=(netComps.net_info.ResolvedIP[2]>>4)&0x0f;
                hum_comps.dig3_2=netComps.net_info.ResolvedIP[1]&0x0f;
                hum_comps.dig3_3=(netComps.net_info.ResolvedIP[1]>>4)&0x0f;
                hum_comps.dig3_4=netComps.net_info.ResolvedIP[0]&0x0f;
                hum_comps.dig3_5=(netComps.net_info.ResolvedIP[0]>>4)&0x0f;
                hum_comps.dot3_pos=2;//
          }
          else if(flag==2)
          {
                hum_comps.dig3_0=hum_comps.dig3_1=hum_comps.dig3_2=MD_DIS__;
                hum_comps.dig3_3=hum_comps.dig3_4=hum_comps.dig3_5=MD_DIS__;
                hum_comps.dot3_pos=2;// 
          }
          else //no set ip
          {
                hum_comps.dig3_0=MD_DIS_p;
                hum_comps.dig3_1=0x01;
                hum_comps.dig3_2=MD_DIS_o;
                hum_comps.dig3_3=MD_DIS_n;
                hum_comps.dig3_4=hum_comps.dig3_5=MD_HIDE_DISP;
                hum_comps.dot3_pos=2;//  
          }
          break;
          
        case 1:
          if(flag==1)    
          {
                unsigned int port=0;
                if(netComps.net_info.currentIP_No==EM_IP0)
                {
                    port=device_comps.access_param.port;
                }
                else if(netComps.net_info.currentIP_No==EM_IP1)
                {
                    port=device_comps.access_param.port1;
                }
                
                hum_comps.dig3_0=port&0x0f;
                hum_comps.dig3_1=(port>>4)&0x0f;;
                hum_comps.dig3_2=(port>>8)&0x0f;
                hum_comps.dig3_3=(port>>12)&0x0f;
                hum_comps.dig3_4=netComps.net_info.ResolvedIP[3]&0x0f;
                hum_comps.dig3_5=(netComps.net_info.ResolvedIP[3]>>4)&0x0f;
                hum_comps.dot3_pos=0;
                
          }
          else if(flag==2)
          {
               hum_comps.dig3_0=hum_comps.dig3_1=hum_comps.dig3_2=MD_DIS__;
               hum_comps.dig3_3=hum_comps.dig3_4=hum_comps.dig3_5=MD_DIS__;
               hum_comps.dot3_pos=0;// 
          }
          else //no set ip
          {
                hum_comps.dig3_0=hum_comps.dig3_1=hum_comps.dig3_2=MD_HIDE_DISP-1;
                hum_comps.dig3_3=hum_comps.dig3_4=hum_comps.dig3_5=MD_HIDE_DISP-1;
                hum_comps.dot3_pos=0;// 
          }
          break;
                
        default:
             break;
   }
   hum_comps.dis_oper_mark._bit.cur3=0;
   display_line3_data();

}

static void report_mode_display(unsigned char opt)
{
   mode_comps[hum_comps.current_mode].displayTimer++;
   if(mode_comps[hum_comps.current_mode].displayTimer>=60)
   {
       mode_comps[hum_comps.current_mode].dis_option++;
       mode_comps[hum_comps.current_mode].dis_option%=2;
       mode_comps[hum_comps.current_mode].displayTimer=0;
   }
   display_ip(opt);
   
     switch(netComps.disCode)
     {
        case EM_DIS_ACT://---act
            hum_comps.dis_oper_mark._bit.cur2=0;
            hum_comps.dig2_0=0x0A;;
            hum_comps.dig2_1=*protocolComps.event_index_pt%10;
            hum_comps.dig2_2=*protocolComps.event_index_pt/10%10;
            hum_comps.dig2_3=MD_HIDE_DISP-1;
            hum_comps.dig2_4=*netComps.run_st%10;
            hum_comps.dig2_5=*netComps.run_st/10%10;
            hum_comps.dot2_pos=0;
            display_line2_data();
            break;
        
        case EM_DIS_OFF:// suc   or fal
            hum_comps.dis_oper_mark._bit.cur2=0;
            if(protocolComps.sw._bit.dataPushYet&&protocolComps.sw._bit.dataPushYet1)
            {
                hum_comps.dig2_0=0x0c;
                hum_comps.dig2_1=MD_DIS_U;
                hum_comps.dig2_2=5;//s
                hum_comps.dig2_3=0x0c;
                hum_comps.dig2_4=MD_DIS_U;
                hum_comps.dig2_5=5;
                hum_comps.dot2_pos=0;
                
            }
            else if(!protocolComps.sw._bit.dataPushYet && !protocolComps.sw._bit.dataPushYet1)
            {
                hum_comps.dig2_0=0x01;
                hum_comps.dig2_1=0x0a;
                hum_comps.dig2_2=0x0f;//
                hum_comps.dig2_3=0x01;
                hum_comps.dig2_4=0x0a;
                hum_comps.dig2_5=0x0f;
                hum_comps.dot2_pos=0;
            }
            else if(protocolComps.sw._bit.dataPushYet&& !protocolComps.sw._bit.dataPushYet1)
            {
               hum_comps.dig2_0=0x0c;
                hum_comps.dig2_1=MD_DIS_U;
                hum_comps.dig2_2=5;//s
                hum_comps.dig2_3=0x01;
                hum_comps.dig2_4=0x0a;
                hum_comps.dig2_5=0x0f;
                hum_comps.dot2_pos=0;
                
                

            }
            else if(!protocolComps.sw._bit.dataPushYet&& protocolComps.sw._bit.dataPushYet1)
            {
                hum_comps.dig2_0=0x01;
                hum_comps.dig2_1=0x0a;
                hum_comps.dig2_2=0x0f;//s
                hum_comps.dig2_3=0x0c;
                hum_comps.dig2_4=MD_DIS_U;
                hum_comps.dig2_5=5;
                hum_comps.dot2_pos=0;
            }
           display_line2_data();
            break;

       case EM_DIS_GPS_STATUS:
            hum_comps.dig2_0=5;;
            hum_comps.dig2_1=MD_DIS_p;
            hum_comps.dig2_2=MD_DIS_G;
            hum_comps.dig2_3=MD_HIDE_DISP-1;
            hum_comps.dig2_4=*netComps.run_st%10;
            hum_comps.dig2_5=*netComps.run_st/10%10;
            hum_comps.dot2_pos=0;
            if(!device_comps.gps.isLocSuc) 
            {
                hum_comps.dis_oper_mark._bit.cur2=1;
            }
            else 
            {
                hum_comps.dis_oper_mark._bit.cur2=0;
            }
            hum_comps.cursor_2=0;
            hum_comps.cursor_2_count++;
            if(hum_comps.cursor_2_count>5)
            {
                hum_comps.cursor_2_count=0;
                if(hum_comps.dis_oper_mark._bit.dis2)
                {
                    hum_comps.dis_oper_mark._bit.dis2=0;
                    
                }
                else
                {
                   hum_comps.dis_oper_mark._bit.dis2=1;
                }
            }
            
            display_line2_data();
            break;
        case EM_DIS_SEARCH_NET:
            hum_comps.dis_oper_mark._bit.cur2=0;
            hum_comps.dig2_0=netComps.net_info.Csq%10;
            hum_comps.dig2_1=netComps.net_info.Csq/10%10;
            hum_comps.dig2_2=MD_DIS_r;
            hum_comps.dig2_3=MD_HIDE_DISP-1;
            hum_comps.dig2_4=*netComps.run_st%10;
            hum_comps.dig2_5=*netComps.run_st/10%10;
            hum_comps.dot2_pos=0;
           
            display_line2_data();
            break;
        
        case EM_DIS_REGISTER_NET:
            hum_comps.dig2_0=MD_DIS__;;
            hum_comps.dig2_1=MD_DIS_G;
            if(strstr(netComps.net_info.band,"LTE"))
            {
                hum_comps.dig2_2=4;
            }
            else 
            {
                hum_comps.dig2_2=2;
            }
            hum_comps.dig2_3=MD_HIDE_DISP-1;
            hum_comps.dig2_4=*netComps.run_st%10;
            hum_comps.dig2_5=*netComps.run_st/10%10;
            hum_comps.dot2_pos=0;
             
            hum_comps.dis_oper_mark._bit.cur2=1;
            hum_comps.cursor_2=0;
            hum_comps.cursor_2_count++;
            if(hum_comps.cursor_2_count>9)
            {
                hum_comps.cursor_2_count=0;
                if(hum_comps.dis_oper_mark._bit.dis2)
                {
                    hum_comps.dis_oper_mark._bit.dis2=0;
                    
                }
                else
                {
                   hum_comps.dis_oper_mark._bit.dis2=1;
                }
            }
            
            display_line2_data();
        
			break;
        case EM_DIS_SEND://send
            hum_comps.dis_oper_mark._bit.cur2=0;
            hum_comps.dig2_0=5;//s
            hum_comps.dig2_1=MD_DIS_G;
            if(strstr(netComps.net_info.band,"LTE"))
            {
                hum_comps.dig2_2=4;
            }
            else 
            {
                hum_comps.dig2_2=2;
            }
            hum_comps.dig2_3=MD_HIDE_DISP-1;
            hum_comps.dig2_4=*netComps.run_st%10;
            hum_comps.dig2_5=*netComps.run_st/10%10;
            hum_comps.dot2_pos=0;
            display_line2_data();
            break;
        case EM_DIS_RECV://rece
            hum_comps.dis_oper_mark._bit.cur2=0;
            hum_comps.dig2_0=MD_DIS_r;//R
            hum_comps.dig2_1=MD_DIS_G;
            if(strstr(netComps.net_info.band,"LTE"))
            {
                hum_comps.dig2_2=4;
            }
            else 
            {
                hum_comps.dig2_2=2;
            }
            hum_comps.dig2_3=MD_HIDE_DISP-1;
            hum_comps.dig2_4=*netComps.run_st%10;
            hum_comps.dig2_5=*netComps.run_st/10%10;
            hum_comps.dot2_pos=0;
            display_line2_data();
            break;
    }

}


static void  display_special(void)
{
    if(!ircComps.sw._bit.runing)
	{
        MD_HIDE_T1_COMMUNICATION;
	}
	else
	{
        MD_DIS_T1_COMMUNICATION;
	}
	if(loraComps.sw._bit.refresh_rssi)
	{

	   int rssi=-164+(int)(loraComps.yl701_info_p->rssi);
        if(rssi<-110)
        {
            MD_DIS_N1_CSQ;
            MD_HIDE_N2_CSQ;
            MD_HIDE_N3_CSQ;
            MD_HIDE_N4_CSQ;
        }
        else if(rssi<-90)
        {
            MD_DIS_N1_CSQ;
            MD_DIS_N2_CSQ;
            MD_HIDE_N3_CSQ;
            MD_HIDE_N4_CSQ;
        }
        else if(rssi<-70)
        {
            MD_DIS_N1_CSQ;
            MD_DIS_N2_CSQ;
            MD_DIS_N3_CSQ;
            MD_HIDE_N4_CSQ;
        }
        else 
        {
            MD_DIS_N1_CSQ;
            MD_DIS_N2_CSQ;
            MD_DIS_N3_CSQ;
            MD_DIS_N4_CSQ;
        }
     
         loraComps.sw._bit.refresh_rssi=0;
    }

    if((netComps.St._bit.running)&&(netComps.disCode>=EM_DIS_SEARCH_NET))
	{
        if(netComps.net_info.Csq<7)
        {
            MD_DIS_N1_CSQ;
            MD_HIDE_N2_CSQ;
            MD_HIDE_N3_CSQ;
            MD_HIDE_N4_CSQ;
        }
        else if(netComps.net_info.Csq<15)
        {
            MD_DIS_N1_CSQ;
            MD_DIS_N2_CSQ;
            MD_HIDE_N3_CSQ;
            MD_HIDE_N4_CSQ;
        }
        else if(netComps.net_info.Csq<22)
        {
            MD_DIS_N1_CSQ;
            MD_DIS_N2_CSQ;
            MD_DIS_N3_CSQ;
            MD_HIDE_N4_CSQ;
        }
        else if(netComps.net_info.Csq<32)
        {
            MD_DIS_N1_CSQ;
            MD_DIS_N2_CSQ;
            MD_DIS_N3_CSQ;
            MD_DIS_N4_CSQ;
        }
        else
        {
            MD_HIDE_N1_CSQ;
            MD_HIDE_N2_CSQ;
            MD_HIDE_N3_CSQ;
            MD_HIDE_N4_CSQ;
        }
    }
  if(hum_comps.dis_oper_mark._bit.refresh_batt_symbol)
  {
       #if(MD_DEVICE_BATT ==MD_3_6V)
       {
            if(device_comps.batt<31)
            {
                MD_DIS_D1_BAT;
                MD_DIS_D4_BAT;
                MD_HIDE_D2_BAT;
                MD_HIDE_D3_BAT;
        		
            }
            else if(device_comps.batt<33)
            {
                MD_DIS_D1_BAT;
                MD_DIS_D4_BAT;
                MD_DIS_D3_BAT;
                MD_HIDE_D2_BAT;
        		
            }
            else 
            {
                MD_DIS_D1_BAT;
                MD_DIS_D4_BAT;
                MD_DIS_D3_BAT;
                MD_DIS_D2_BAT;
            }
       }
       #elif(MD_DEVICE_BATT ==MD_4_5V)
       {
            if(device_comps.batt<33)
            {
                MD_DIS_D1_BAT;
                MD_DIS_D4_BAT;
                MD_HIDE_D2_BAT;
                MD_HIDE_D3_BAT;
        		
            }
            else if(device_comps.batt<38)
            {
                MD_DIS_D1_BAT;
                MD_DIS_D4_BAT;
                MD_DIS_D3_BAT;
                MD_HIDE_D2_BAT;
        		
            }
            else 
            {
                MD_DIS_D1_BAT;
                MD_DIS_D4_BAT;
                MD_DIS_D3_BAT;
                MD_DIS_D2_BAT;
            }
        }
        #endif
    }
    hum_comps.dis_oper_mark._bit.refresh_batt_symbol=0;
}

static void handle_lcd_refresh()
{
    mode_comps[hum_comps.current_mode].display(mode_comps[hum_comps.current_mode].dis_option);
    if(hum_comps.current_mode!=EM_SELF_TEST_MODE)
    {
        display_special();
    }
}

static void hum_comps_init(hum_comps_t *const this)
{
    clr_lcd();   
}
static void nop()
{
	NOP();
}


mode_comps_t  mode_comps[]=//Handling of keys in different modes
{	//mode_comps_t  mode_comps[5];
	{"normal mode"       ,mode_comps+0  ,EM_NORMAL_MODE        ,normal_mode_on_s_key      ,normal_mode_on_m_key      ,normal_mode_on_j_key      ,nop                             ,normal_mode_on_long_m_key      ,normal_mode_on_long_j_key      ,normal_mode_on_long_s_and_j_key,normal_mode_display      ,0,0},
	{"debug mode"        ,mode_comps+1  ,EM_DEBUG_MODE         ,debug_mode_on_s_key       ,debug_mode_on_m_key       ,debug_mode_on_j_key       ,nop                             ,debug_mode_on_long_m_key       ,nop                            ,debug_mode_on_long_s_and_j_key ,debug_mode_display       ,0,0},
	{"password mode"     ,mode_comps+2  ,EM_PWD_MODE           ,pwd_mode_on_s_key         ,pwd_mode_on_m_key         ,pwd_mode_on_j_key         ,nop		                     ,pwd_mode_on_long_m_key         ,nop                            ,pwd_mode_on_long_s_and_j_key   ,pwd_mode_display         ,0,0},
	{"cal_query mode"    ,mode_comps+3  ,EM_CAL_QUERY_MODE     ,cal_query_mode_on_s_key   ,cal_query_mode_on_m_key   ,cal_query_mode_on_j_key   ,nop		                     ,cal_query_mode_on_long_m_key   ,nop                            ,nop                            ,cal_query_mode_display   ,0,0},
	{"cal_modify mode"   ,mode_comps+4  ,EM_CAL_MODIFY_MODE    ,cal_modify_mode_on_s_key  ,cal_modify_mode_on_m_key  ,cal_modify_mode_on_j_key  ,cal_modify_mode_on_long_s_key   ,nop                            ,nop                            ,nop                            ,cal_modify_mode_display  ,0,0},
	
    {"param_query mode"  ,mode_comps+5  ,EM_PARAM_QUERY_MODE   ,param_query_mode_on_s_key ,param_query_mode_on_m_key ,nop                       ,nop		                     ,param_query_mode_on_long_m_key ,nop                            ,nop                            ,param_query_mode_display ,0,0},
	{"param_modify mode" ,mode_comps+6  ,EM_PARAM_MODIFY_MODE  ,param_modify_mode_on_s_key,param_modify_mode_on_m_key,param_modify_mode_on_j_key,param_modify_mode_on_long_s_key ,nop                            ,nop                            ,nop                            ,param_modify_mode_display,0,0},
    {"self_test_mode"    ,mode_comps+7  ,EM_SELF_TEST_MODE     ,nop                       ,nop                       ,nop          	            ,nop			                 ,nop               	         ,nop                            ,nop                            ,self_test_mode_display   ,0,0},
    {"report_mode"       ,mode_comps+8  ,EM_REPORT_MODE        ,nop                       ,nop                       ,nop          	            ,nop			                 ,nop               	         ,nop                            ,nop                            ,report_mode_display      ,0,0},
    {"4g_mode"           ,mode_comps+9  ,EM_4G_MODE            ,nop                       ,lora_mode_on_m_key        ,nop                       ,_4g_mode_on_long_s_key			 ,normal_mode_on_long_m_key      ,normal_mode_on_long_j_key      ,normal_mode_on_long_s_and_j_key,_4g_mode_display         ,0,0},
    {"lora mode"         ,mode_comps+10 ,EM_LORA_MODE          ,nop                       ,lora_mode_on_m_key        ,nop                       ,nop			                 ,lora_mode_on_long_m_key        ,normal_mode_on_long_j_key      ,normal_mode_on_long_s_and_j_key,lora_mode_display        ,0,0},
};


static void hum_comps_task_handle()////Execution interval is 50 ms
{
	
	hum_comps_t *const this=hum_comps.this;
	if(this->do_init==1)
	{
		this->init(this);
		this->do_init=0;
	}
	if(!(this->count%6)&&(this->count<92))
	{
		 lcd_test(this);//interval time 300ms
		
	}
	if(this->count==92)
	{
		handle_key(this);
		handle_lcd_refresh();
	}
    else
    {
        this->count++;
        if(this->count==92)
        {
            hum_comps.dis_oper_mark._bit.test_ok=1;
	        enter_self_test_mode();
        }
    }
}

hum_comps_t hum_comps=
{

    "Human machine interface",     //char *desc;
	&hum_comps,                    //struct _HUM_COMPONENTS *this;
	1,                             //int   do_init;//Whether to initialize,1:init 0,no init
	hum_comps_init,                //void (*const init)(struct _HUM_COMPONENTS *);
	0,                             //unsigned long count;            //task_handle Called counter
	
	///******************************lcd seg define******************************/
    
    //0    1    2     3    4   5   6    7    8     9    A   b     C    d    E     F     H   G   h    c    J    L     n   N    o   P    q   r    t   U   _    -   NULL
    {0x5F,0x50, 0x3D, 0x79, 0x72,0x6B,0x6F,0x51, 0x7F, 0x7B,0x77,0x6E, 0x0F, 0x7C,0x2F, 0x27, 0x76,0x4F,0x66, 0x2C,0x58, 0x0E, 0x64,0x57,0x6C,0x37,0x73,0x24,0x2E,0x5E,0x08,0x20,0x00},
    8,//unsigned char dig0_0;
    8,//unsigned char dig0_1;
    0,//unsigned char dot0_pos;
	
	8,//unsigned char dig1_0;
	8,//unsigned char dig1_1;
	8,//unsigned char dig1_2;
	8,//unsigned char dig1_3;
    0,//unsigned char dot1_pos;
	
	8,//unsigned char dig2_0;
	8,//unsigned char dig2_1;
	8,//unsigned char dig2_2;
	8,//unsigned char dig2_3;
	8,//unsigned char dig2_4;
	8,//unsigned char dig2_5
	0,//unsigned char dot2_pos;

	8,//unsigned char dig3_0;
	8,//unsigned char dig3_1;
	8,//unsigned char dig3_2;
	8,//unsigned char dig3_3;
	8,//unsigned char dig3_4;
	8,//unsigned char dig3_5;
	8,//char          dot3_pos;

    {0},//union dis_oper_mark;
	0,//int cursor_0;//0 line cursor position
	0,//int cursor_1;
	0,//int cursor_2;
	0,//int cursor_3;
	0,//int cursor_0_count;
	0,//int cursor_1_count;
	0,//int cursor_2_count;
	0,//int cursor_3_count;
	/*******************end lcd seg define **************************/ 
	
	EM_SELF_TEST_MODE,//mode_type_t   current_mode;
	EM_NO_KEY,        //key_type_t key_type;//[EM_NO_KEY, EM_SHORT_KEY,EM_LONG_KEY]
	MD_KEY_MASK,      //int key;
	get_key,//int  (*const get_key)(struct _HUM_COMPONENTS *);
    hum_comps_task_handle,//void (*const task_handle)(void);
	
	
	//TODO
	
};













