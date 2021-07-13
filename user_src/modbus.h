#ifndef  MODBUS_H
#define  MODBUS_H
#define MD_SET_RS_485_T_R     1//P1.5=1
#define MD_RESET_RS_485_T_R   1//P1.5=0

typedef	struct
{   
    unsigned char addr;
    unsigned char baud;
    unsigned char cs;
}modebs_param_t;

typedef struct 
{
 
	unsigned char *const recv_base_pt;//Passed to uart recv  point
	unsigned char *const recv_pos_pt;
	
	unsigned short       op_window_time; 

    unsigned char *const recv_cfg_base_pt;
    unsigned char *const send_cfg_base_pt;
	unsigned char *const recv_cfg_pos_pt;
	
	modebs_param_t *param_pt;
    union 
    {
    	unsigned char All;
    	struct{
                unsigned char runing		        :1;
                unsigned char busy		            :1;
                unsigned char baud_modified 		:1;
               }_bit;
    }sw;

    int (*read_param)(void *,int);
    int (*save_param)(void const *,int);

    int (*modify_baud)(int,int);
    void (*write)(unsigned char * const , unsigned int );
    void (*const task_handle)(void);
}modbusComps_t;

extern modbusComps_t modbusComps;


#endif