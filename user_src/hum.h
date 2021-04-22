#ifndef   HUM_H
#define   HUM_H
#define   MD_09_PIN_LCD_SEG04   SEG4
#define   MD_10_PIN_LCD_SEG05   SEG5
#define   MD_11_PIN_LCD_SEG08   SEG8
#define   MD_12_PIN_LCD_SEG09   SEG9
#define   MD_13_PIN_LCD_SEG10   SEG10
#define   MD_14_PIN_LCD_SEG11   SEG11
#define   MD_15_PIN_LCD_SEG12   SEG12
#define   MD_16_PIN_LCD_SEG13   SEG13
#define   MD_17_PIN_LCD_SEG14   SEG14
#define   MD_18_PIN_LCD_SEG15   SEG15
#define   MD_19_PIN_LCD_SEG16   SEG16
#define   MD_20_PIN_LCD_SEG17   SEG17
#define   MD_21_PIN_LCD_SEG18   SEG18
#define   MD_22_PIN_LCD_SEG19   SEG19
#define   MD_23_PIN_LCD_SEG20   SEG20

#define   MD_DIS_T1_COMMUNICATION       MD_23_PIN_LCD_SEG20|=0x01
#define   MD_DIS_T2_KPA                 MD_23_PIN_LCD_SEG20|=0x02
#define   MD_DIS_T3_PSI                 MD_23_PIN_LCD_SEG20|=0x04
#define   MD_DIS_T4_BAR                 MD_23_PIN_LCD_SEG20|=0x08
#define   MD_DIS_T5_MPA                 MD_09_PIN_LCD_SEG04|=0x08
#define   MD_DIS_T6_CELSIUS             MD_19_PIN_LCD_SEG16|=0x10
#define   MD_DIS_T7_M                   MD_19_PIN_LCD_SEG16|=0x20
#define   MD_DIS_T8_PA                  MD_19_PIN_LCD_SEG16|=0x80
#define   MD_DIS_T9_KGF_CM_2            MD_18_PIN_LCD_SEG15|=0x80
#define   MD_DIS_T10_CELSIUS            MD_15_PIN_LCD_SEG12|=0x80
#define   MD_DIS_T11_MAX                MD_15_PIN_LCD_SEG12|=0x40
#define   MD_DIS_T12_MMH2O              MD_15_PIN_LCD_SEG12|=0x20
#define   MD_DIS_T13_MMHG               MD_15_PIN_LCD_SEG12|=0x10
#define   MD_DIS_T14_INH2O              MD_13_PIN_LCD_SEG10|=0x40
#define   MD_DIS_T15_INHG               MD_13_PIN_LCD_SEG10|=0x20
#define   MD_DIS_T16_V                  MD_13_PIN_LCD_SEG10|=0x10
#define   MD_DIS_T17_MA                 MD_15_PIN_LCD_SEG12|=0x08



#define   MD_DIS_P1_DOT                 MD_21_PIN_LCD_SEG18|=0x08
#define   MD_DIS_P2_DOT                 MD_11_PIN_LCD_SEG08|=0x08
#define   MD_DIS_P3_DOT                 MD_19_PIN_LCD_SEG16|=0x08
#define   MD_DIS_P4_DOT                 MD_17_PIN_LCD_SEG14|=0x08
#define   MD_DIS_P5_DOT                 MD_22_PIN_LCD_SEG19|=0x80
#define   MD_DIS_P6_DOT                 MD_20_PIN_LCD_SEG17|=0x80
#define   MD_DIS_P7_DOT                 MD_10_PIN_LCD_SEG05|=0x80
#define   MD_DIS_P8_DOT                 MD_12_PIN_LCD_SEG09|=0x80
#define   MD_DIS_P9_DOT                 MD_16_PIN_LCD_SEG13|=0x80
#define   MD_DIS_P10_DOT                MD_19_PIN_LCD_SEG16|=0x40

#define   MD_DIS_COL                    MD_13_PIN_LCD_SEG10|=0x80

#define   MD_DIS_D1_BAT                 MD_13_PIN_LCD_SEG10|=0x08
#define   MD_DIS_D2_BAT                 MD_13_PIN_LCD_SEG10|=0x01
#define   MD_DIS_D3_BAT                 MD_13_PIN_LCD_SEG10|=0x04
#define   MD_DIS_D4_BAT                 MD_13_PIN_LCD_SEG10|=0x02

#define   MD_DIS_N1_CSQ                 MD_14_PIN_LCD_SEG11|=0x01
#define   MD_DIS_N2_CSQ                 MD_14_PIN_LCD_SEG11|=0x02
#define   MD_DIS_N3_CSQ                 MD_14_PIN_LCD_SEG11|=0x04
#define   MD_DIS_N4_CSQ                 MD_14_PIN_LCD_SEG11|=0x08

#define   MD_HIDE_T1_COMMUNICATION       MD_23_PIN_LCD_SEG20&=~0x01
#define   MD_HIDE_T2_KPA                 MD_23_PIN_LCD_SEG20&=~0x02
#define   MD_HIDE_T3_PSI                 MD_23_PIN_LCD_SEG20&=~0x04
#define   MD_HIDE_T4_BAR                 MD_23_PIN_LCD_SEG20&=~0x08
#define   MD_HIDE_T5_MPA                 MD_09_PIN_LCD_SEG04&=~0x08
#define   MD_HIDE_T6_CELSIUS             MD_19_PIN_LCD_SEG16&=~0x10
#define   MD_HIDE_T7_M                   MD_19_PIN_LCD_SEG16&=~0x20
#define   MD_HIDE_T8_PA                  MD_19_PIN_LCD_SEG16&=~0x80
#define   MD_HIDE_T9_KGF_CM_2            MD_18_PIN_LCD_SEG15&=~0x80
#define   MD_HIDE_T10_CELSIUS            MD_15_PIN_LCD_SEG12&=~0x80
#define   MD_HIDE_T11_MAX                MD_15_PIN_LCD_SEG12&=~0x40
#define   MD_HIDE_T12_MMH2O              MD_15_PIN_LCD_SEG12&=~0x20
#define   MD_HIDE_T13_MMHG               MD_15_PIN_LCD_SEG12&=~0x10
#define   MD_HIDE_T14_INH2O              MD_13_PIN_LCD_SEG10&=~0x40
#define   MD_HIDE_T15_INHG               MD_13_PIN_LCD_SEG10&=~0x20
#define   MD_HIDE_T16_V                  MD_13_PIN_LCD_SEG10&=~0x10
#define   MD_HIDE_T17_MA                 MD_15_PIN_LCD_SEG12&=~0x08

#define   MD_HIDE_P1_DOT                 MD_21_PIN_LCD_SEG18&=~0x08
#define   MD_HIDE_P2_DOT                 MD_11_PIN_LCD_SEG08&=~0x08
#define   MD_HIDE_P3_DOT                 MD_19_PIN_LCD_SEG16&=~0x08
#define   MD_HIDE_P4_DOT                 MD_17_PIN_LCD_SEG14&=~0x08
#define   MD_HIDE_P5_DOT                 MD_22_PIN_LCD_SEG19&=~0x80
#define   MD_HIDE_P6_DOT                 MD_20_PIN_LCD_SEG17&=~0x80
#define   MD_HIDE_P7_DOT                 MD_10_PIN_LCD_SEG05&=~0x80
#define   MD_HIDE_P8_DOT                 MD_12_PIN_LCD_SEG09&=~0x80
#define   MD_HIDE_P9_DOT                 MD_16_PIN_LCD_SEG13&=~0x80
#define   MD_HIDE_P10_DOT                MD_19_PIN_LCD_SEG16&=~0x40

#define   MD_HIDE_COL                    MD_13_PIN_LCD_SEG10&=~0x80

#define   MD_HIDE_D1_BAT                 MD_13_PIN_LCD_SEG10&=~0x08
#define   MD_HIDE_D2_BAT                 MD_13_PIN_LCD_SEG10&=~0x01
#define   MD_HIDE_D3_BAT                 MD_13_PIN_LCD_SEG10&=~0x04
#define   MD_HIDE_D4_BAT                 MD_13_PIN_LCD_SEG10&=~0x02

#define   MD_HIDE_N1_CSQ                 MD_14_PIN_LCD_SEG11&=~0x01
#define   MD_HIDE_N2_CSQ                 MD_14_PIN_LCD_SEG11&=~0x02
#define   MD_HIDE_N3_CSQ                 MD_14_PIN_LCD_SEG11&=~0x04
#define   MD_HIDE_N4_CSQ                 MD_14_PIN_LCD_SEG11&=~0x08



#define   MD_HIDE_DISP                  (sizeof(hum_comps.SEG_TAB)-1)
#define   MD_DIS_H                      16
#define   MD_DIS_G                      17
#define   MD_DIS_h                      18
#define   MD_DIS_c                      19
#define   MD_DIS_J                      20
#define   MD_DIS_L                      21
#define   MD_DIS_n                      22
#define   MD_DIS_N                      23
#define   MD_DIS_o                      24
#define   MD_DIS_p                      25
#define   MD_DIS_q                      26
#define   MD_DIS_r                      27
#define   MD_DIS_t                      28
#define   MD_DIS_U                      29
#define   MD_DIS__                      30


#define   MD_FL_VER                      0


typedef enum  
{
	EM_NORMAL_MODE=0,
	EM_DEBUG_MODE,
	EM_PWD_MODE,
	EM_CAL_QUERY_MODE,   // Calibration  query mode
	EM_CAL_MODIFY_MODE,  //Calibration modify mode
	EM_PARAM_QUERY_MODE, //param query mode
	EM_PARAM_MODIFY_MODE,//param modify mode
	EM_SELF_TEST_MODE,    //test mode used to check ad54x0,e2prom,ds18b20
	EM_REPORT_MODE,
	EM_4G_MODE,
	EM_LORA_MODE
	//....TODO.....
	
}mode_type_t;

typedef struct //Handling of keys in different modes
{
	char *desc;
	struct _MODE_COMPONENTS *this;
	mode_type_t type;
	void (*on_s_key)(void);
	void (*on_m_key)(void);
	void (*on_j_key)(void);
	void (*on_long_s_key)(void);
	void (*on_long_m_key)(void);
	void (*on_long_j_key)(void);
	void (*on_long_s_and_j_key)(void);
	void (*display)(unsigned char opt);
	unsigned char dis_option;
	unsigned char displayTimer;
}mode_comps_t;

typedef enum
{
	EM_NO_KEY,               //em:enum 
	EM_SHORT_KEY,
	EM_LONG_KEY
} key_type_t;

/*	  display erea
      |******0 erea******|	   |********1 erea**********|
                 |**************2 erea****************|
*/

typedef struct _HUM_COMPONENTS
{
	char *desc;
	struct _HUM_COMPONENTS *this;
	int   do_init;//Whether to initialize,1:init 0,no init
	void (*const init)(struct _HUM_COMPONENTS *);
	unsigned long count;            //task_handle Called counter
	
	/******************************lcd seg define******************************/
    const unsigned char  SEG_TAB[33];
    unsigned char dig0_0;
    unsigned char dig0_1;
    unsigned char dot0_pos;
	
	unsigned char dig1_0;
	unsigned char dig1_1;
	unsigned char dig1_2;
	unsigned char dig1_3;
    unsigned char dot1_pos;
	
	unsigned char dig2_0;
	unsigned char dig2_1;
	unsigned char dig2_2;
	unsigned char dig2_3;
	unsigned char dig2_4;
	unsigned char dig2_5;
	char          dot2_pos;

	unsigned char dig3_0;
	unsigned char dig3_1;
	unsigned char dig3_2;
	unsigned char dig3_3;
	unsigned char dig3_4;
	unsigned char dig3_5;
	char          dot3_pos;

    union 
    {
    	unsigned long All;
    	struct{
                unsigned char cur0		            :1;
                unsigned char dis0		            :1;
                unsigned char cur1		            :1;
                unsigned char dis1		            :1;
                unsigned char cur2		            :1;
                unsigned char dis2                  :1;
                unsigned char cur3                  :1;
                unsigned char dis3                   :1;
               
                
                unsigned char refresh_date		    :1;
                unsigned char refresh_time		    :1;
                unsigned char refresh_device_id_high:1;
                unsigned char refresh_device_id_low :1;
                unsigned char refresh_temp		    :1;
                unsigned char refresh_press		    :1;//switch press press unit,data ,time
                unsigned char refresh_option      	:1;
                unsigned char refresh_ver      		:1;
                
                unsigned char res1			 :1;	
                unsigned char refresh_cal_param     :1;
                unsigned char refresh_batt_symbol   :1;
                unsigned char refresh_press_adc	    :1;//debug mode use
                unsigned char refresh_temp_adc	    :1;//debug mode use
                unsigned char test_ok               :1;
                unsigned char refresh_press_max     :1;
                unsigned char refresh_err_code      :1;
                
                unsigned char refresh_res_cal_param :1;
                unsigned char refresh_high_cal_param :1;
                unsigned char refresh_high          :1;
                unsigned char refresh_volume         :1;
                unsigned char refresh_high_adc	    :1;
                unsigned char refresh_param_modifyed	    :1;
           

    	}_bit;
    }dis_oper_mark;
	int cursor_0;//0 line cursor position
	int cursor_1;
	int cursor_2;
	int cursor_0_count;
	int cursor_1_count;
	int cursor_2_count;
	/*******************end lcd seg define **************************/ 
	
	mode_type_t   current_mode;
	key_type_t key_type;//[EM_NO_KEY, EM_SHORT_KEY,EM_LONG_KEY]
	int key;
	int  (*const get_key)(struct _HUM_COMPONENTS *);
    void (*const task_handle)(void);
	
	
	//TODO
	
}hum_comps_t;
extern mode_comps_t  mode_comps[];
extern hum_comps_t hum_comps;
extern void clr_lcd(void);
extern void enter_normal_mode(void);
extern void enter_4g_mode(void);
extern void enter_cal_modify_mode(void);
extern void enter_cal_query_mode(void);
extern void enter_report_mode(void);
extern void enter_lora_mode(void);
#endif