#ifndef PROTOCOL_H
#define PROTOCOL_H

typedef struct 
{
	union 
    {
    	uint8_t All;
    	struct
    	{
    		
    		uint8_t key         	:1;	//Button trigger report
    		uint8_t PHighOver	    :1;	
    		uint8_t PHighRealse 	:1; 
    		uint8_t PLowLess		:1; 
    		uint8_t PLowRealse	    :1;
    		uint8_t timeAuto 		:1;
    		uint8_t intervalTime    :1;
    		uint8_t batteryBlunt    :1;
    	}_bit;
    }triggerIrq;
    union
    {
        uint8_t All;
        struct
        {
                uint8_t AckTmrOut     :1;
                uint8_t DataRdy         :1;	
                uint8_t cmd_shutDown   	:1;	
                uint8_t isAckTmrOut 	        :1; 
                uint8_t res4     		:1; 
                uint8_t res5    	    :1;
                uint8_t dataPushYet1 		    :1;
                uint8_t dataPushYet     :1;
                
        }_bit;
    }sw;
	unsigned char * const msg;
	unsigned int   msgLen;
	unsigned int   AckTmr;
	unsigned char  *step;
    int * const event_index_pt;
	void (*const task_handle)(void);
	
	
}protocolComps_t;
extern protocolComps_t protocolComps;

#endif


