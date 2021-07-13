#ifndef PRESS_H
#define PRESS_H



typedef struct _PRESS_COMPONENTS
{
	char *desc;
	struct  _PRESS_COMPONENTS *this;
	unsigned char *const recv_base_pt;//Passed to uart recv  point
	unsigned char *const recv_pos_pt;
    unsigned short       op_window_time; 
    
    union 
    {
    	unsigned char All;
    	struct{
                unsigned char running		    :1;
                unsigned char busy		        :1;
                unsigned char on		        :1;
                unsigned char window_running	:1;
                
               }_bit;
    }sw;
	void (*write)(unsigned int cmd);
	void (*const start)(void);
	void (*const stop)(void);
	
	unsigned int (*pro_press_com)(unsigned char Cmd,unsigned char *buf,int len);

 
	void (*const task_handle)(void);
	
}pressComps_t;
extern pressComps_t pressComps;
#endif



