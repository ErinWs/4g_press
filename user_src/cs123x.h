#ifndef CS123X_H
#define CS123X_H

//#define  MD_CS2137_CLK               P4_bit.no3
//#define  MD_CS1237_DATA              P5_bit.no2
//#define  MD_CS1237_DATA_DIR          PM5_bit.no2

//
//typedef struct _CS1237_COMPONENTS
//{
//	char *desc;
//	struct _CS123X_COMPONENTS *this;
//    union 
//    {
//    	unsigned char All;
//    	struct
//    	{
//                unsigned char adc_updated		    :1;
//                unsigned char running		        :1;       
//    	 }_bit;
//    }sw;
//	void (*const power_down)(void); //
//	void (*const restart)(void);// 
//	int (*const Init )(unsigned char dat);     
//	int (*const read_adc)(long *const ad);   // 
//	unsigned char (*const read_reg )(long * ad);         
//	void (*const write_reg )(long *ad,unsigned char dat); 
//	void (*const disale_interrup)(void);
//	void (*const enable_interrupt)(void);
//	void  ( *const task_handle)(void);
//	
//}cs1237_comps_t;
//extern cs1237_comps_t cs1237_comps;


#define  MD_CS213X_CLK               P4_bit.no2
#define  MD_CS123X_DATA              P5_bit.no3
#define  MD_CS123X_DATA_DIR          PM5_bit.no3
typedef struct _CS123X_COMPONENTS
{
	char  *desc;
	struct _CS123X_COMPONENTS *this;
	const int init_channel;
	int current_channel;
	union 
    {
    	unsigned char All;
    	struct
    	{
                unsigned char adc_updated		    :1;
                unsigned char running 	            :1;       
    	 }_bit;
    }sw;
	void (*const power_down)(void); //
	void (*const restart)(void);// 
	int (*const Init )(unsigned char dat);     
	int (*const read_adc)(long *const ad);   // 
	unsigned char (*const read_reg )(long * ad);         
	void (*const write_reg )(long *ad,unsigned char dat); 
	void (*const disale_interrup)(void);
	void (*const enable_interrupt)(void);
	void (*const stop)(void);
	void  ( *const task_handle)(void);//point to device_comps_task_handle
	
}cs123x_comps_t;
extern cs123x_comps_t cs123x_comps;
#endif