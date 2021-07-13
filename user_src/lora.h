#ifndef LORA_H
#define LORA_H
#define  MD_LORA_AUX_PIN          1//P13.7
#define  MD_LORA_EN_PIN           1//P4.5
#define  MD_LORA_EN_PIN_RESET     NOP()// P4.5=0
#define  MD_LORA_EN_PIN_SET       NOP()//P4.5=1
#define  MD_LORA_SET_PIN_RESET    NOP()//P4.4=0
#define  MD_LORA_SET_PIN_SET      NOP()//P4.4=1

#define  MD_LORA_INTP_ENABLE()   R_INTC0_Start()
#define  MD_LORA_INTP_DISABLE()  R_INTC0_Stop() 

typedef struct _YL701_INFO
{
     unsigned char rssi;
     unsigned char rate;
     unsigned char verify;
     unsigned long freq;
     unsigned char sf;
     unsigned char workMode;
     unsigned char bw;
     unsigned int  NodeId;
     unsigned char netId;
     unsigned char power;
     unsigned char breathPeriod;
     unsigned char breathTime;
     unsigned char cs;
}yl701_info_t;

	
typedef struct _LORA_COMPONENTS
{

    char *desc;  
	struct _LORA_COMPONENTS  *const this;
	int   do_init;//Whether to initialize,1:init 0,no init
	unsigned char (*const init)(struct _LORA_COMPONENTS *);
	unsigned long count;            //Called counter
 
	unsigned char *const recv_base_pt;//Passed to uart recv  point
	unsigned char *const send_base_pt;//Passed to uart send  point
	unsigned char *const recv_pos_pt;
	unsigned short       op_window_time; 

	unsigned int   re_try_times;
	
    yl701_info_t *yl701_info_p;
    int (*save_yl701_info)(void const *,int);
    int (*read_yl701_info)(void  *,int);
    
    
	struct
	{
	    enum
	    {  EM_RUN,
	       EM_CFG,
	    } mode;
	    enum
	    {
	        EM_SEND,
	        EM_RECV
	    }dir;
	}work_st;

	
    union 
    {
    	int All;
    	struct{
                unsigned char runing		        :1;
                unsigned char initing		        :1;
                unsigned char sendReq               :1;
                unsigned char init_ok               :1;
                unsigned char reading_rssi          :1;
                unsigned char read_rssi_ok          :1;
                unsigned char refresh_rssi          :1;
                unsigned char busy                  :1;
                unsigned char noParameter           :1;
                unsigned char param_modified        :1;
              
               }_bit;
    }sw;
    void (*write)(unsigned char * const , unsigned int );
    void (*const task_handle)(void);
    void (*const task_50ms)(void);
}loraComps_t;

extern loraComps_t loraComps;

 void enter_cad_mode(void);



#endif