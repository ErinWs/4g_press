#ifndef  DEVICE_H
#define  DEVICE_H

#define  MD_3_6V   0
#define  MD_4_5V   1
#define  MD_DEVICE_BATT          MD_3_6V
 
#define  MD_4G                   0     //4.5V 
#define  MD_LORA                 1     //3.6V
#define  MD_NORMAL               2     //3.6V  switch unit

#define  MD_PRODUCT_NAME         MD_4G //

#define  MD_DIS5                5
#define  MD_DIS4                4
#define  MD_DIS_DIG              MD_DIS4

//#define   MD_MODBUS            //modbus rs485 use irc interface,no sleep
//#define   MD_NO_LCD
//#define   MD_TEMP		        //temp display line 2
//#define   MD_HIGH
//#define   MD_IGNORE_ALL_ERR
#define   MD_USE_RTC_MODULE


//#define   MD_EXT_MEASUREMENT_MODULE
#define   MD_RAD         0
#define   MD_PRESS       1
#define   MD_EXT_MEASUREMENT_MODULE_TYPE   MD_PRESS


#define  APP_PROTOCOL_ZHIAN            0
#define  APP_PROTOCOL_SELF             1
#define  APP_PROTOCOL_SHENGHUO         2
#define  APP_PROTOCOL_SIDI_INFO        3

#define  APP_PROTOCOL_TYPE             APP_PROTOCOL_SIDI_INFO

#if (APP_PROTOCOL_TYPE==APP_PROTOCOL_ZHIAN)
#define  MD_PUSH_DATA_TO_IP1
#else
#define  MD_ALT_IP
#endif



#define   MD_CH0_GAIN    (3<<2)
#define   MD_CH1_GAIN    (3<<2)


#define   MD_E2PROM_DRIVER_ERR           (1<<0)

#ifndef   MD_EXT_MEASUREMENT_MODULE
#define   MD_CS123X_DRIVER_ERR           (1<<1)
#else
#define   MD_CS123X_DRIVER_ERR           (0<<1)
#endif

#if(MD_PRODUCT_NAME==MD_LORA)
#define   MD_LORA_MODULE_ERR             (1<<2)
#else
#define   MD_LORA_MODULE_ERR             (0<<2)
#endif


#if defined (MD_USE_RTC_MODULE)
#define   MD_RTC_MODULE_ERR              (1<<4)
#else
#define   MD_RTC_MODULE_ERR              (0<<4)
#endif


#define   MD_ADC_MAX_POS               6
#define   MD_SET_REF_3030C_ON       P1_bit.no6=1
#define   MD_SET_REF_3030C_OFF      P1_bit.no6=0

#define   MD_SET_BAT_CTL_ON         P4_bit.no4=1
#define   MD_SET_BAT_CTL_OFF        P4_bit.no4=0



#define MD_MEMORY_TEST_START_ADDR                      0x0000   //
 

#define MD_CALIBRATION_PARAM_START_ADDR                0x0030   //64bytes
#define MD_RES_CALIBRATION_PARAM_START_ADDR            0x0070   //64bytes
#define MD_ALARM_PARAM_START_ADDR                      0x00b0   //32
#define MD_REPORT_PARAM_START_ADDR                     0x00d0   //16
#define MD_DEVICE_SN_START_ADDR                        0x00e0   //16
#define MD_DEVICE_ADDR_START_ADDR                      0x00f0   //16

#define MD_TIME_SEG_DATA_PARAM_START_ADDR              0x0100
#define MD_MODBUS_PARAM_START_ADDR                     0x0110
#define MD_DEVICE_COE_START_ADDR                       0x0120

#define MD_HIGH_CALIBRATION_PARAM_START_ADDR           0x0140


#define MD_ACCESS_PARAM_START_ADDR                     0x0180

#define MD_SYSTEM_TIME_START_ADDR                      0x01c0
#define MD_DEVICE_SENSOR_START_ADDR                    0x01d0

#define MD_LORA_PARAM_START_ADDR                       0x01e0

#define MD_TIME_SEG_DATA_START_ADDR                    0x0200
#define MD_TIME_SEG_DATA_END_ADDR                      0x0400 

#define MD_GPS_INFO_START_ADDR                         0x0600 
#define MD_MANUFACTURER_INFO_START_ADDR                0x0620
#define MD_DEVICE_INFO_START_ADDR                      0x0640
#define MD_SENSOR_INFO_START_ADDR                      0x0680
#define MD_LEVEL_PARAM_START_ADDR                      0x06c0





  
  typedef struct
  {
      long x[4];
      long y[4];
      long t[4];//Temperature value for temperature compensation
      char dot;
      unsigned char unit;//0Mpa ,1Kpa,0x81 Kpa m
      unsigned char is_calibrated; 
      unsigned char cs;
  }
  calibration_param_t;

  typedef struct
  {
      long x[4];
      long y[4];
      long t[4];//Temperature value for temperature compensation
      char dot;
      unsigned char unit;
      unsigned char is_calibrated; 
      unsigned char cs;
  }
  high_calibration_param_t;

  typedef struct
    {
        long x[4];
        long y[4];
        long t[4];//Temperature value for temperature compensation
        char dot;
        unsigned char unit;
        unsigned char is_calibrated; 
        unsigned char cs;
    }
    res_calibration_param_t;

typedef enum  
{
	EM_CAL_PRESS=0,
	EM_CAL_RES,
	EM_CAL_DELTAP,
	EM_CAL_HIGH
	//....TODO.....
	
}cal_type_t;
  
  typedef struct
  {
      unsigned char min;
      unsigned char hour;
      unsigned int  min_Interval;
      unsigned char hour_Interval;
      int disFactor;
      unsigned long triggerTimes;
      unsigned char cs;
  }
  report_param_t;
 
typedef struct
{
    long bottom_s;
    long sample_interval_value;
    long total_high_mm;
    long offset_mm;
    long coe;
    long _4ma_lower_range;
    long _4ma_high_range;
    long _4ma_dir;
    unsigned char cs;
    
}
level_param_t;

  typedef struct
  {
      unsigned char ip[25];
      unsigned int  port;
      unsigned char ip1[25];
      unsigned int  port1;
      char          flag;//0 null ,1 ip, 2,domain
      unsigned char cs;
  }
  access_param_t;
 

  typedef struct 
  {
      union 
      {
     	unsigned int All;
     	struct
     	{
              unsigned char high_temp_alarm_en     :1;
              unsigned char low_temp_alarm_en		:1;
              unsigned char high_press_alarm_en	:1;
              unsigned char low_press_alarm_en	    :1;
         }_bit;
      }sw;
      long press_high_upper;
      long press_high_lower;
      long press_low_upper;
      long press_low_lower;
      int  unit;

      int temp_high;
      int temp_low;
      
      unsigned char cs;
  }
  alarm_param_t;
  
  typedef struct
  {
        unsigned char year;
        unsigned char month;
        unsigned char day;
        unsigned char hour;
        unsigned char min;
        unsigned char sec;
        unsigned char cs;
  }system_time_t;
        
   typedef struct 
   {
       unsigned char sn[15];
       unsigned char cs;
   }
   device_sn_t;

                        
//  typedef struct
//  {
//         union 
//        {
//        	unsigned char All;
//        	struct
//        	{
//                 unsigned char test_finished      :1;
//                 unsigned char count_en		    :1;
//                 unsigned char isWarn		        :1;
//            }_bit;
//        }sw;
//        long  ref_press;
//        long  press_set_format_valve;
//        int   delay_timer;
//        int   timer;
//   }_4g_t;



 typedef struct 
 {
     unsigned char addr[7];
     unsigned char cs;
 }
 device_addr_t;

 typedef struct 
 {
     unsigned char ser_num[8];
     unsigned char cs;
 }
 device_sensor_t;

typedef struct
{
    int  store_addr;
    unsigned char lastSampleTime[7];
    unsigned char cs;
}
TimeSegData_t;



typedef struct _DEVICE_COMPONENTS
{
	char *desc;
	struct _DEVICE_COMPONENTS  *const this;
	int   do_init;//Whether to initialize,1:init 0,no init
	unsigned char (*const init)(struct _DEVICE_COMPONENTS *);
	unsigned long count;            //Called counter
	
    union 
    {
    	unsigned long All;
    	struct{
                unsigned char e2prom_driver_err		            :1;
                unsigned char cs123x_driver_err		            :1;
                unsigned char lora_module_err                   :1;
                unsigned char res1		                        :1;
                unsigned char rtc_module_err		            :1;
                unsigned char pcf857x_driver_err		        :1;
                unsigned char ad54x0_driver_err		            :1;
				unsigned char res		            			:1;
                
                unsigned char temp_adc_stb	                    :1;
                unsigned char isPressNoConnect	                :1;
                unsigned char isPLowRealseTriggered             :1;
                unsigned char isPLowLessTriggered               :1;
                unsigned char isPHighRealseTriggered            :1;
                unsigned char isPHighOverTriggered              :1;
                unsigned char over_range	                    :1;
                unsigned char high_over_range	                :1;
                unsigned char adc_busy	                        :1;
                unsigned char adc_stb	                        :1;
                unsigned char high_adc_stb	                    :1;
                unsigned char batt_status                       :1;
                unsigned char data_mode                         :1;
                unsigned char isOnline                          :1;

                unsigned char isTempNoConnect	                :1;
                unsigned char isTHighOverTriggered	            :1;
                unsigned char isTHighRealseTriggered	         :1;
                unsigned char isTLowLessTriggered                :1;
                unsigned char isTLowRealseTriggered              :1;
                


                

    	}_bit;
    }sw;
    int PHihgOverTimer;
    int PHihgRealseTimer;
    int PLowLessTimer;
    int PLowRealseTimer;
    int THihgOverTimer;
    int THihgRealseTimer;
    int TLowLessTimer;
    int TLowRealseTimer;
	
	long  ad1_convert_result[MD_ADC_MAX_POS];
	unsigned int  ad1_pos;
	long  ad2_convert_result[MD_ADC_MAX_POS];
	unsigned int  ad2_pos;
	long          ad1_ad2_average_result;//(ad1-ad2)/ad1_pos
	
	long  temp_p_convert_result[MD_ADC_MAX_POS];
	unsigned int  temp_p_pos;
	long  temp_n_convert_result[MD_ADC_MAX_POS];
	unsigned int  temp_n_pos;
	long          temp_p_temp_n_average_result;//(temp_p_convert_result-temp_n_convert_result)/temp_p_pos

    long  high_p_convert_result[MD_ADC_MAX_POS];
	unsigned int  high_p_pos;
	long  high_n_convert_result[MD_ADC_MAX_POS];
	unsigned int  high_n_pos;
	long          high_p_high_n_average_result;//(high_p_convert_result-high_n_convert_result)/high_p_pos

	
	long  ad3_convert_result[MD_ADC_MAX_POS];
	unsigned int  ad3_pos;
	long          ad3_average_result;
	
	long  current_4_20ma;
    
	long ntc_valve;
	long ( * const calc_ad3_average)(struct _DEVICE_COMPONENTS  *const);
	long ( * const calc_ntc_valve)(struct _DEVICE_COMPONENTS  *const);//point to function calc_ntc( device_comps_t *const this) whith param adc_result[][] 
	int  ( * const calc_temp)  (struct _DEVICE_COMPONENTS     *const);//point to function calc_temp( sensor_comp_t *const this)with param ntc_valve
    long pt_valve;
    long ( * const calc_temp_p_temp_n_average)(struct _DEVICE_COMPONENTS  *const);
	long ( * const calc_pt_valve)(struct _DEVICE_COMPONENTS  *const);//
	int  ( * const calc_pt_temp)  (struct _DEVICE_COMPONENTS     *const);//

	int  current_temp;
	int  current_temp_n_1;
	int  current_temp_n_2;

    long ( *const calc_high_p_high_n_average)(struct _DEVICE_COMPONENTS *const ); //point to calc_signal_period(sensor_comp_t *const this)
    long ( *const calc_current_high)(struct _DEVICE_COMPONENTS *const ); //point to calc_signal_period(sensor_comp_t *const this)
    long  current_volume;
    long  current_high;  //Yn
    long  current_high_n_1;//Yn-1
    long  current_high_n_2;//Yn-2

	
	long ( *const calc_ad1_ad2_average)(struct _DEVICE_COMPONENTS *const ); //point to calc_signal_period(sensor_comp_t *const this)
	long ( *const calc_current_press)(struct _DEVICE_COMPONENTS *const ); //point to calc_signal_period(sensor_comp_t *const this)
    long  current_press;  //Yn
    long  max_press;
	long  current_press_n_1;//Yn-1
	long  current_press_n_2;//Yn-2

    struct 
    {
        int press;
        int temp;
        int high;
        long press_clr_value;
        int  current;
        unsigned char cs;
    }coe;
    int (*read_coe)(void *,int );
    int (*save_coe)(void const *,int);
    int (*clr_press)(void);

    struct 
    {
        union 
        {
        	unsigned char All;
        	struct
        	{
                unsigned char isLocSuc      :1;
                unsigned char isActive      :1;
            }_bit;
        }sw;
        long glng;
        long glat;
        unsigned char loc_times;
        unsigned char cs;
    }gps;
    int (*read_gps_loc)(void *,int );
    int (*save_gps_loc)(void const *,int);
    int (*get_gps_info_from_net)(char const *);
    struct 
    {
        char name[25];
        unsigned char cs;
    }manufacturer_info;
    int (*read_manufacturer_info)(void *,int );
    int (*save_manufacturer_info)(void const *,int);

    struct 
    {
        char type[25];
        char id[25];
        unsigned char cs;
    }device_info;
    int (*read_device_info)(void *,int );
    int (*save_device_info)(void const *,int);

    struct 
    {
        char id[25];
        unsigned char cs;
    }sensor_info;
    int (*read_sensor_info)(void *,int );
    int (*save_sensor_info)(void const *,int);

    struct
    {
        union 
        {
        	unsigned char All;
        	struct
        	{
                unsigned char on      :1;
                unsigned char running :1;
            }_bit;
        }sw;
        int timer;
        void (*start)(int timer);
        void (*stop)(void);
    }buzzer;
  
    unsigned char batt;//batt voltage
	calibration_param_t calibration_param;
	calibration_param_t calibration_param_bak;
	cal_type_t cal_type;//0 cal press, 1,cal temp
	int (*read_calibration_param)(void *,int );
    int (*save_calibration_param)(void const *,int);

    res_calibration_param_t  res_calibration_param;
    res_calibration_param_t  res_calibration_param_bak;
    int (*read_res_calibration_param)(void *,int );
    int (*save_res_calibration_param)(void const *,int);
    
    high_calibration_param_t  high_calibration_param;
    high_calibration_param_t  high_calibration_param_bak;
    int (*read_high_calibration_param)(void *,int );
    int (*save_high_calibration_param)(void const *,int);

	
    report_param_t report_param;
    int (*read_report_param)(void *,int);
	int (*save_report_param)(void const *,int);
    
    access_param_t access_param;
    int (*read_access_param)(void *,int);
	int (*save_access_param)(void const *,int);
	
    alarm_param_t alarm_param;
	int (*read_alarm_param)(void *,int);
	int (*save_alarm_param)(void const *,int);

    device_addr_t device_addr;
    int (*read_device_addr)(void *,int);
	int (*save_device_addr)(void const *,int);

	device_sensor_t device_sensor;
    int (*read_device_sensor)(void *,int);
	int (*save_device_sensor)(void const *,int);
	

	TimeSegData_t TimeSegData;
	int (*read_time_seg_data_param)(void * , int);
	int (*save_time_seg_data_param)(void const * , int);

	system_time_t system_time;
	int (*read_system_time)(void * , int);
	int (*save_system_time)(void const * , int);

	device_sn_t device_sn;
	int (*read_device_sn)(void * , int);
	int (*save_device_sn)(void const * , int);

	level_param_t  level_param;
	int (*read_level_param)(void * , int);
	int (*save_level_param)(void const * , int);

	
	
    unsigned int report_interval_timer;
	int _0_5s_timr_acc;
	

	void (*read_all_param)(struct _DEVICE_COMPONENTS  *const);
    
	void  ( *const output_debug_info)(struct _DEVICE_COMPONENTS const  *const);//point to sensor_comps_output_debug_info(device_comps_t const *const this)
	char debug_info[32];
	void  ( *const task_handle)(void);//point to device_comps_task_handle
}device_comps_t; 

extern device_comps_t device_comps;
void _0_5s_task_handle(void);
void _50ms_task_handle(void);


#endif
