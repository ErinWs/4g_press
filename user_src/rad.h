#ifndef RAD_H
#define RAD_H



typedef struct _RAD_COMPONENTS
{
	char *desc;
	struct  _RAD_COMPONENTS *this;
	unsigned char *const recv_base_pt;//Passed to uart recv  point
	unsigned char *const recv_pos_pt;
    unsigned short       op_window_time; 
    
    union 
    {
    	unsigned char All;
    	struct{
                unsigned char running		    :1;
                unsigned char busy		        :1;
               }_bit;
    }sw;
	void (*write)(unsigned char * const , unsigned int );
	void (*const start)(void);
	void (*const stop)(void);
	unsigned int high_cm[30];
	int          high_pos;
	unsigned int (*get_high)(unsigned char *,int len);

    long  average_high_mm;
	long  average_high_cm;
	long  position_height_mm;
	long  position_height_cm;
	void (*const task_handle)(void);
	
}radComps_t;
extern radComps_t radComps;
#endif


