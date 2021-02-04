#ifndef  SYSTEM_H
#define  SYSTEM_H

typedef void (*  pfun)(void);

typedef struct _SYSTEM_COMPONENTS
{
    union 
    {
    	int All;
    	struct
    	{
            int test_xt1_run_flag	:1;
            int is_xt1_running      :1;
        }_bit;
    }sw;
    char rst_code;
    pfun const  user_init;
    pfun const  call_back_task_remarks;
    pfun const  task_process;
    
}systemComps_t;


extern systemComps_t systemComps;

#endif
