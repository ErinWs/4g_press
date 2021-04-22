#include "r_cg_macrodriver.h"
#include "r_cg_sau.h"
#include "r_cg_rtc.h"
#include "device.h"
#include "net.h"
#include "hum.h"
#include "protocol.h"
#include "string.h"
#include "math.h"
#include "stdlib.h"
#include "stdio.h"



#define GSMPWRCTL_pin	P1_bit.no2
#define GSMON_pin	P1_bit.no1
#define GSMRST_pin      P1_bit.no3


#define MD_NET_DEVICE_SEND_STEP      31
#define MD_NET_DEVICE_DELAY_STEP     50
#define MD_NET_DEVICE_NULL_STEP      -1

#define   MD_MODULE_OP_MAX_TIME      180


#define  enable_net_receive()       R_UART2_Start()
#define  disable_net_receive()      R_UART2_Stop()

typedef unsigned char BYTE;


static struct 
{ 
    union 
    {
    	unsigned char All;
    	struct
    	{
    		uint8_t RegestCSNet     :1;
    		uint8_t RegestPSGNet    :1;
    		uint8_t RegestPSENet  	:1;
    		uint8_t res1	            :1;
    		uint8_t res2		        :1;
    		uint8_t res3	            :1;
    	   
    		
    	}_bit;
    } St;
    
	unsigned short NoAckTimes;
	unsigned char  AT_Waiting;	//AT?¨¹¨¢?¡¤¡é?¨ª¡À¨º??//
	unsigned char  Flag_St;
	unsigned char _Flag_St;
	unsigned int   reStartTimes;
	unsigned char  NoRegestPSGNetCount;
	unsigned char  Reset_St ;
	unsigned char  TurnOFF_St;
	
	unsigned char ErrCode;
	unsigned char err_count;
    unsigned char TX_ATCombuffer[120]; 
	unsigned char send_buf[612];
	unsigned char recv_buf[1024];
	unsigned int  recv_Idx;
	unsigned char recv_msg_buf[256];//send to protocol layer
	
	
    char          pdp_id;
    unsigned char NoRegestPSENetCount;;
    unsigned int  NoRegestCSNetCount;
	
	
	MD_STATUS  (*write)(uint8_t * const tx_buf, uint16_t tx_num);
	//unsigned char * (*check_net_device_ack)(unsigned char *FindStr);  
	
}
netMisc=
{
    {0},

	0,
	0,
	0,
	0,
	2,
	0,
	0,
	0,
	
	3,
	0,
	{0},
	{0},
	{0},
	 0,
	{0},
	
	1,
    0,
	0,
		
	R_UART2_Send
};


static void init_ENV_net_device(void)
{
    netMisc.AT_Waiting=0;

    netMisc.NoRegestCSNetCount=0;
    netMisc.NoRegestPSGNetCount=0;
    netMisc.NoRegestPSENetCount=0;
    netMisc.err_count=0;

    netComps.St._bit.socket_connected=0;
    netComps.St._bit.ResolvedIP=0;
    netComps.St._bit.noIP=0;

    netComps.disCode=EM_DIS_ACT;
}
    
static void cold_start_net_device()	
{
    netMisc.Flag_St = 0;
    netMisc.Reset_St=0;//turn on moudle //
    init_ENV_net_device();
}  
   
static void hot_start_net_device()	
{
    netMisc.Flag_St = 0;
    netMisc.Reset_St=2;//
    init_ENV_net_device();
}    

static void stop_net_device()
{
    netMisc.Flag_St = 1;
    netMisc.TurnOFF_St=0;//turn on moudle //
    netMisc.AT_Waiting=0;
}

static void on_net_device_ack_ok()
{
       netMisc.AT_Waiting=0;
       netMisc.NoAckTimes=0;
       netMisc._Flag_St=netMisc.Flag_St;
       netMisc.Flag_St++;

       netMisc.err_count=0;
}

static void net_status_sjmp_any_step(unsigned char Flag_St)
{
        netMisc.AT_Waiting=0;
        netMisc.NoAckTimes=0;
        netMisc._Flag_St=netMisc.Flag_St;
        netMisc.Flag_St=Flag_St; 
}
 
static void reset_net_deive_rcvbuf(void)
{
    netMisc.recv_Idx = 0;
    netComps.recv_RxSt=&netComps.recv_base_pt[0];
    memset(&netMisc.recv_buf[0],0,sizeof(netMisc.recv_buf));
}


static void DealAnalyticCode(int err)//err=0;err£¬err=1,recv ok £¬err=3,no data
{
    if(err==0)//
    {
        net_status_sjmp_any_step(MD_NET_DEVICE_SEND_STEP);//send data
    }

    else if(err==1)//err=1,recv data ok
    {
    	net_status_sjmp_any_step(MD_NET_DEVICE_SEND_STEP);//send data
    }
    else if(err==3)//,err=3,no data
    {
    	net_status_sjmp_any_step(MD_NET_DEVICE_SEND_STEP);//send data
    }
    else
    {
    	net_status_sjmp_any_step(MD_NET_DEVICE_SEND_STEP);//send data
    }

}
	
//void HexSrtToHex(unsigned char *des,unsigned char const *src,unsigned int Length)
//{
//	unsigned int  Count=0;
//	unsigned int  i=0;
//	unsigned char Temp_H=0;
//	unsigned char Temp_L=0;
//
//	while(Count<Length)
//	{
//		if( src[i]<=0x39)
//		{
//			//src[i]-=0x30;
//			Temp_H=src[i]-0x30;
//		}
//		else 
//		{
//			//src[i]-=0x37;
//			Temp_H=src[i]-0x37;
//		}
//
//		if( src[i+1]<=0x39)	
//		{	
//			//src[i+1]-=0x30;
//			Temp_L=src[i+1]-0x30;
//			
//		}
//		else 
//		{
//			//src[i+1]-=0x37;
//			Temp_L=src[i+1]-0x37;
//		}
//		//des[Count++]=((src[i]<<4) |src[i+1]);
//		des[Count++]=((Temp_H<<4) |Temp_L);
//		i+=2;
//	}
//
//}

static void HandleNoAck()
{
	if(netMisc.NoAckTimes<1)
	{
		netMisc.AT_Waiting =0;
		netMisc.NoAckTimes++;
    }
	else 
	{
        netComps.St._bit.noResponse=1;
        net_status_sjmp_any_step(MD_NET_DEVICE_NULL_STEP);
    }
}


static void   HandleErr()
{
    
    if(netMisc.err_count<1)
    {
        netMisc.err_count++;
        net_status_sjmp_any_step(MD_NET_DEVICE_DELAY_STEP);//delay function
    }
    else 
    {
        if(netMisc.reStartTimes>0)
        {
            hot_start_net_device();
            netMisc.reStartTimes--;
        }
        else 
        {
            netComps.St._bit.err=1;
            net_status_sjmp_any_step(MD_NET_DEVICE_NULL_STEP);
        }
    }
}

static void   HandleRegTimeOut()
{
    if(netMisc.reStartTimes>0)
    {
        hot_start_net_device();
        netMisc.reStartTimes--;
    }
    else 
    {
        netComps.St._bit.err=1;
        net_status_sjmp_any_step(MD_NET_DEVICE_NULL_STEP);
    }
}


static void TX_ATData(unsigned char  *ptr,unsigned int WaitingTime)
{
    netComps.AckTmr = WaitingTime;
	netMisc.AT_Waiting = 1;
	netMisc.write(ptr,strlen(ptr));
}

static void TX_ATCommand(unsigned char  *ptr,unsigned short WaitingTime)
{
	unsigned char TxLen = 0;
    while(*ptr!='\0')
    {
		netMisc.TX_ATCombuffer[TxLen] = *ptr++;
		TxLen++;
	}
    netComps.AckTmr = WaitingTime;
	netMisc.AT_Waiting = 1;
	netMisc.write(&netMisc.TX_ATCombuffer[0], TxLen);
}

static unsigned char * check_net_device_ack(unsigned char *FindStr)  
{
    char FindstrLen;
    char *StartPt,*EndPt;
    void *rslt=(void *)0;
    FindstrLen =(char) strlen((char *)FindStr);
    StartPt = &netComps.recv_base_pt[0];
    EndPt = netComps.recv_RxSt;
    while(EndPt+1-StartPt>=FindstrLen)
    {
    	if(!memcmp(FindStr,StartPt,FindstrLen))
    	 {
    		 rslt = StartPt+FindstrLen;
    		 break;
    	 } 
    	 StartPt++;
    }
    return(rslt);
}





//static void GetNuestats()
//{
//	unsigned char i=0;
//	unsigned char *pt_rssi;
//	unsigned char  buf[12]="";
//	if(!netMisc.AT_Waiting)
//	{
//		reset_net_deive_rcvbuf();
//		//TX_ATCommand("\r\nAT+COPS?\r\n",60); 
//		TX_ATCommand("\r\nAT+NUESTATS\r\n",3); 
//	}
//	else if(!netComps.AckTmr)
//	{
//		HandleNoAck();   	
//	} 
//	else if(check_net_device_ack("OK\r\n"))
//	{
//		 if(pt_rssi=check_net_device_ack("ECL:"))
//		 {
//			netComps.net_info.ecl=*pt_rssi -0x30;
//		 }
//		
//		 if(pt_rssi=check_net_device_ack("SNR:"))
//		 {
//			memset(buf,0,sizeof(buf));
//		 	for(i=0;i<sizeof(buf);i++)
//			{
//				if(*pt_rssi!='\r')
//				{
//				   buf[i]=*pt_rssi++;
//				}
//				else
//				{
//					break;
//				}
//			}
//			netComps.net_info.sina=atol(buf)/10<-128?-128:atol(buf)/10;
//		 }
//		 
//		 if(pt_rssi=check_net_device_ack("RSRQ:"))
//		 {
//			memset(buf,0,sizeof(buf));
//	        for(i=0;i<sizeof(buf);i++)
//		    {
//				if(*pt_rssi!='\r')
//				{
//				    buf[i]=*pt_rssi++;
//				}
//				else
//				{
//					break;
//				}
//	        } 
//	        netComps.net_info.rsrq=atol(buf)/10<-128?-128:atol(buf)/10;
//			
//		 }
//		 if(pt_rssi=check_net_device_ack("Signal power:"))
//		 {
//			memset(buf,0,sizeof(buf));
//		 	for(i=0;i<sizeof(buf);i++)
//			{
//				if(*pt_rssi!='\r')
//				{
//				   buf[i]=*pt_rssi++;
//				}
//				else
//				{
//					break;
//				}
//			} 
//			netComps.net_info.rsrp=atol(buf)/10<atol(buf)/10<-128?-128:atol(buf)/10;
//		 }
//		 
//		if(pt_rssi=check_net_device_ack("PCI:"))
//		{
//            memset(buf,0,sizeof(buf));
//            for(i=0;i<sizeof(buf);i++)
//            {
//                if(*pt_rssi!='\r')
//                {
//                  buf[i]=*pt_rssi++;
//                }
//                else
//                {
//                   break;
//                }
//            } 
//            netComps.net_info.pci=atol(buf);
//        }
//		 if(pt_rssi=check_net_device_ack("Cell ID:"))
//		 {
//			memset(buf,0,sizeof(buf));
//			//int k=5;
//		 	//memset(netComps.net_info.cellId,0,sizeof(netComps.net_info.cellId));
//			for(i=0;i<sizeof(buf);i++)
//			{
//				if(*pt_rssi!='\r')
//				{
//                    buf[i]=*pt_rssi++;
//                }
//				else
//				{
//					break;
//				}
//			} 
//			netComps.net_info.cellId=atol(buf);
//			/*
//			while(i!=0)
//			{
//				if(i>1)
//				{
//					netComps.net_info.cellId[k]= buf[i-1]+(buf[i-2]<<4);
//					k--;
//					i-=2;
//				}
//				else if(i>0)
//				{
//					netComps.net_info.cellId[k--]= buf[i-1];
//					i--;
//				}
//			}
//			NOP();
//			*/
//			
//		 }
//		 if(pt_rssi=check_net_device_ack("EARFCN:"))
//		 {
//			memset(buf,0,sizeof(buf));
//		 	for(i=0;i<sizeof(buf);i++)
//			{
//				if(*pt_rssi!='\r')
//				{
//				   buf[i]=*pt_rssi++;
//				}
//				else
//				{
//					break;
//				}
//			} 
//			netComps.net_info.earFcn=atol(buf);
//		 }
//		 on_net_device_ack_ok();
//	}
//}

static void DelayMs(unsigned short TimeMs){
	unsigned short j;
	unsigned char i;
	for(j=0;j<TimeMs*10;j++)
	{		
		//i=7;	//10us at 8Mhz
		i=97;	//100us at 8Mhz
		while(i>0) i--;
	}
}





 void DataAdd30H(unsigned char *i, unsigned char const *j,unsigned int Len)
{
	while(Len--)
	{
		if(((*j&0xF0)>>4)<10)
		{
			*i++=((*j&0xF0)>>4) +0x30;
		}
		else 
		{
			*i++=((*j&0xF0)>>4) +0x37;
		}
		if((*j&0x0F)<10)
		{
			*i++=(*j&0x0F)+0x30;
		}
		else 
		{
			*i++=(*j&0x0F) +0x37;
		}
		j++;
	}
}



unsigned char PacketCoapMsg(unsigned char const *buf,int len)
{
    unsigned char ret=0;
//    unsigned int  i;
//    unsigned char Var[8]="";
      memset(netMisc.send_buf,0,sizeof(netMisc.send_buf));
//    sprintf(netMisc.send_buf,"%s","\r\nAT+NMGS=");
//    i=strlen(netMisc.send_buf);
//    sprintf(Var,"%d",len);
//    memcpy(&netMisc.send_buf[i],Var,strlen(Var));
//    i+=strlen(Var);
//    netMisc.send_buf[i++]=',';
   // DataAdd30H(&netMisc.send_buf[i],buf,len);
//    i+=len*2;
//    memcpy(&netMisc.send_buf[i++],"\r\n",2);//ADD \r\n 
//    ret=1;
      memcpy(&netMisc.send_buf,buf,len); 

    return ret;
}              

static void GetHexMsg(unsigned char *des,unsigned char const*src,unsigned int Length)
{
	HexSrtToHex(des,src,Length);
}

static unsigned char GetMsg(unsigned char const *buf,int len)
{
	unsigned char err=0;
	unsigned int Len= 0;
	memset(netMisc.recv_msg_buf,0,sizeof(netMisc.recv_msg_buf));	
	//GetHexMsg(netMisc.recv_msg_buf,buf,len);
	memcpy(netMisc.recv_msg_buf,buf,len);
	netComps.msg;//=netMisc.recv_msg_buf;
	netComps.msgLen=len;
	netComps.St._bit.recvData=1;
	err=1;
    return err;////err=0;´íÎó£¬err=1,recv data ok;
}              

static void power_on_net_device(void)
{
   GSMPWRCTL_pin = 1;
   GSMRST_pin=1;
   DelayMs(30);
   GSMON_pin = 1;   
   DelayMs(25);
   GSMRST_pin=0;
}


static void reset_net_device(void)
{
	switch(netMisc.Reset_St)
	{
		case 0:
			power_on_net_device();	
		    reset_net_deive_rcvbuf();
			netMisc.Reset_St++;
			netComps.AckTmr=30;
			break;
			
		case 1:
		    if(check_net_device_ack("RDY\r\n"))
		    {
                GSMON_pin = 0;
                netMisc.Reset_St=0;
                net_status_sjmp_any_step(10);
            }
            else if(!netComps.AckTmr)
			{
                GSMON_pin = 0;
                netMisc.Reset_St=0;
			    net_status_sjmp_any_step(10);
			}
			break;

			
		case 2:
                //TX_ATCommand("\r\nAT+CFUN=1,1\r\n",30);
		        GSMRST_pin=1;
		        netComps.AckTmr = 3;
                netMisc.Reset_St++;
                break;
        case 3:
            if(!netComps.AckTmr)
			{
                GSMRST_pin=0;
                netMisc.Reset_St=1;
                reset_net_deive_rcvbuf();
                netComps.AckTmr=30;
            }
   }
}


void power_down_net_deivice(void)
{   
     char msg[32]="";
     switch(netMisc.TurnOFF_St)
     { 
          case 0: 
        	   	if(!netMisc.AT_Waiting)
    			{
    				reset_net_deive_rcvbuf();
    		        TX_ATCommand("\r\nAT+QICLOSE=0\r\n",10);
                } 
    			else if(!netComps.AckTmr)
    			{
    		         netMisc.TurnOFF_St++;
    				 netMisc.AT_Waiting=0;
    			}
    			else if(check_net_device_ack("OK\r\n"))
    			{
    				 netMisc.TurnOFF_St++;
    				 netMisc.AT_Waiting=0;
    			}	
    			break;
    			
           case 1: 
        	   	if(!netMisc.AT_Waiting)
    			{
    				reset_net_deive_rcvbuf();
    		        TX_ATCommand("\r\nAT+QIDEACT=1\r\n",10);
                } 
    			else if(!netComps.AckTmr)
    			{
    		         netMisc.TurnOFF_St++;
    				 netMisc.AT_Waiting=0;
    			}
    			else if(check_net_device_ack("OK\r\n"))
    			{
    				 netMisc.TurnOFF_St++;
    				 netMisc.AT_Waiting=0;
    			}	
    			break;
    			
            case 2: 
        	   	if(!netMisc.AT_Waiting)
    			{
    				reset_net_deive_rcvbuf();
    		        //TX_ATCommand("\r\nAT+QPOWD\r\n",3);
			netMisc.AT_Waiting=1;
                    GSMON_pin = 1;
                    netComps.AckTmr = 3;
                } 
    			else if(!netComps.AckTmr)
    			{
                     GSMON_pin = 0;
                     netMisc.TurnOFF_St++;
    				 netMisc.AT_Waiting=0;
    			}
//    			else if(check_net_device_ack("OK\r\n"))
//    			{
//                     netMisc.TurnOFF_St++;
//    				 netMisc.AT_Waiting=0;
//    			}	
    			break; 
    			
            case 3: 
                if(!netMisc.AT_Waiting) 
                {
                    netComps.AckTmr =5;	
                    netMisc.AT_Waiting = 1;
                }
                else if(!netComps.AckTmr)
                {
                    netMisc.AT_Waiting = 0;
                    //memset(&netComps.net_info,0,sizeof(netComps.net_info));
                    netComps.net_info.Csq=99;
            		netComps.net_info.CsQ=99;
                    GSMPWRCTL_pin =0;
                    netComps.St._bit.running=0;
            		netComps.St._bit.offing=0;
            		*protocolComps.event_index_pt=-1;
            		disable_net_receive();
            		enter_4g_mode();
              }	
               break;
            default: break;
     }
}


static void GetRssi(void)
{
	
	unsigned char *pt_rssi;
	if(!netMisc.AT_Waiting)
	{
		reset_net_deive_rcvbuf();
		TX_ATCommand("\r\nAT+CSQ\r\n",10);
	} 
	else if(!netComps.AckTmr)
	{
		HandleNoAck(); 
	}
	else if(check_net_device_ack("OK\r\n"))
	{
	      if(pt_rssi=check_net_device_ack("+CSQ: "))
	      {
                char *ptsz;
                netComps.net_info.Csq=strtol(pt_rssi,&ptsz,10);
                netComps.net_info.CsQ=strtol(ptsz+1,&ptsz,10);
    	  }
	      on_net_device_ack_ok(); 
	}  
}

//static unsigned char GMTtoLocalTime(unsigned char *tbuf)//yy mm dd hh mm ss '+' zz
//{
//	unsigned int buf[8];
//	unsigned int i=0;
//	unsigned char  monthlengths[]={31,28,31,30,31,30,31,31,30,31,30,31};
//   #define  ADJFACTOR  1
//   	for(i=0;i<8;i++)
//	{
//		if(i==0)
//		{
//			buf[i]=2000+tbuf[i];
//			if(((buf[i]%4==0) && (buf[i]%100!=0)) || (buf[i]%400==0))
//			{
//				 monthlengths[2-ADJFACTOR]=29;
//			}
//		}
//		buf[i]=tbuf[i];
//	}
//	buf[7]=(unsigned int)buf[7]*15/60;//15min
//	if(buf[6]=='+')
//	{
//		buf[3]+=buf[7];//timeZone
//		if(buf[3]<24)
//		{
//			
//		}
//		else
//		{
//			buf[3]%=24;//h
//			buf[2]++;//dd
//			if(buf[2]>monthlengths[buf[1]-ADJFACTOR])
//			{
//				buf[2]%=monthlengths[buf[1]-ADJFACTOR];
//			        buf[1]++;//mm
//				if(buf[1]>12)
//				{
//					buf[1]%=12;
//					buf[0]++;//yy++
//				}
//			}
//		}
//	}
//	else if(tbuf[6]=='-')
//	{
//		if(buf[3]>=buf[7])
//		{
//			buf[3]-=buf[7];
//		}
//		else
//		{
//			buf[3]=buf[3]+24-buf[7];//h
//		        buf[2]--;//dd
//			if(!buf[2])
//			{
//				buf[1]--;//mm
//			        buf[2]=monthlengths[buf[1]-ADJFACTOR];
//			        
//				if(!buf[1])
//				{
//				    buf[0]--;//yy--
//				    buf[1]=1;
//				}
//			}
//		}
//	}
//	for(i=0;i<6;i++)
//	{
//		tbuf[i]=((buf[i]%100/10)<<4)+(buf[i]%10);
//	}
//	return 1;
//}


static void Srv_GSM(void)
{     
    unsigned char *pt_rssi;
    unsigned char i;
    unsigned char msg[64]="";
	
	switch(netMisc.Flag_St)
	{
        case 0:
			reset_net_device();		
			break;


         case 1:
			power_down_net_deivice();		
			break;
			
			
		case 10: 
			if(!netMisc.AT_Waiting) 
			{
				netComps.AckTmr =5;	
				netMisc.AT_Waiting = 1;
			}
			else if(!netComps.AckTmr)
			{
 				 on_net_device_ack_ok();
			}
			break;
			
		case 11:	
			if(!netMisc.AT_Waiting)
			{
				reset_net_deive_rcvbuf();
		        TX_ATCommand("\r\nATE0\r\n",10);
            } 
			else if(!netComps.AckTmr)
			{
				  HandleNoAck(); 
			}
			else if(check_net_device_ack("OK\r\n"))
			{
				 on_net_device_ack_ok();
			}	
			break;
			
		case 12:	
			if(!netMisc.AT_Waiting)
			{
                reset_net_deive_rcvbuf();
                TX_ATCommand("\r\nATI\r\n",10);//Ver
			}  
			else if(!netComps.AckTmr)
			{
				HandleNoAck(); 
			}
			else if(check_net_device_ack("OK\r\n"))
			{
				 if(pt_rssi=check_net_device_ack("Revision: "))
				 {
                    memset(netComps.net_info.firmVer,0,sizeof(netComps.net_info.firmVer));
                    for(i=0;i<sizeof(netComps.net_info.firmVer);i++)
                    {
                        if(*pt_rssi!='\r')
                        {
                    	  netComps.net_info.firmVer[i]=*pt_rssi;
                    	  netComps.net_info.firmhdVer[i]=*pt_rssi;
                          pt_rssi++;
                        }
                        else
                        {
                           break; 	
                        }
                    }
                 }
                 on_net_device_ack_ok();
			}	
			break;
			
        case 13: 
			if(!netMisc.AT_Waiting)
			{
				reset_net_deive_rcvbuf();
			    TX_ATCommand("\r\nAT+CGSN\r\n",10);//IMEI
			} 
			else if(!netComps.AckTmr)
			{
				HandleNoAck();  
			}
			else if(pt_rssi=check_net_device_ack("OK\r\n"))
			{
				if(pt_rssi=check_net_device_ack("\r\n"))
				 {
					memset(netComps.net_info.imei,0,sizeof(netComps.net_info.imei));
			        for(i=0;i<sizeof(netComps.net_info.imei);i++)
				    {
						if(*pt_rssi!='\r')
						{
						    netComps.net_info.imei[i]=*pt_rssi++;
						}
						else
						{
							break;
						}
				    } 
				 }
				 on_net_device_ack_ok();
			}	
            break;
            
		case 14:  	
			if(!netMisc.AT_Waiting)
			{
				reset_net_deive_rcvbuf();
				TX_ATCommand("\r\nAT+ICCID\r\n",10); 
			}
			else if(!netComps.AckTmr)
			{
                HandleNoAck(); 
            } 
			else if(check_net_device_ack("ERROR"))
			{
                HandleErr();
            }
			else if(check_net_device_ack("OK\r\n"))
			{
				if(pt_rssi=check_net_device_ack("+ICCID: "))
				 {
					 memset(netComps.net_info.iccid,0,sizeof(netComps.net_info.iccid));
				     for(i=0;i<sizeof(netComps.net_info.iccid);i++)
					 {
    						if(*pt_rssi!='\r')
    						{
    					 	    netComps.net_info.iccid[i]=*pt_rssi++;
                            }
    						else
    						{
    						   break; 	
    						}
					  }
					  netComps.disCode=EM_DIS_SEARCH_NET;
				 }
				 on_net_device_ack_ok();
			}
			break;
			
        case 15: 	
			if(!netMisc.AT_Waiting)
			{
				reset_net_deive_rcvbuf();
			    TX_ATCommand("\r\nAT+CIMI\r\n",10);//
			} 
			else if(!netComps.AckTmr)
			{
				HandleNoAck();  
			}
			else if(check_net_device_ack("OK\r\n"))
			{
				 if(pt_rssi=check_net_device_ack("\r\n"))
				 {
					 memset(netComps.net_info.imsi,0,sizeof(netComps.net_info.imsi));
				     for(i=0;i<sizeof(netComps.net_info.imsi);i++)
					 {
						if(*pt_rssi!='\r')
						{
					 	  netComps.net_info.imsi[i]=*pt_rssi++;
						}
						else
						{
						   break; 	
						}
					  }
				 }
				 on_net_device_ack_ok();
			}	
			break;	



			
/********************** Start to check registration**********************/

        case 16: 
		if(!netMisc.AT_Waiting)                                          
    		{
    			netComps.AckTmr =2;	
    			netMisc.AT_Waiting = 1;
    		}
    		else if(!netComps.AckTmr)
    		{
    			on_net_device_ack_ok();
    		}
    		break;
    		
        case 17:
            GetRssi();
            break;
            
        case 18:  
			if(!netMisc.AT_Waiting)
			{
                reset_net_deive_rcvbuf();
				TX_ATCommand("\r\nAT+CREG?\r\n",10); 
			}
			else if(!netComps.AckTmr)
			{
				HandleNoAck();
            } 
			else if(check_net_device_ack("OK\r\n"))
			{
				if(check_net_device_ack("+CREG: 0,1"))//home 
				{
                   netMisc.St._bit.RegestCSNet=1;
                   on_net_device_ack_ok();
			    }
				else if(check_net_device_ack("+CREG: 0,5"))//Roaming
				{
				    netMisc.St._bit.RegestCSNet=1;
				    on_net_device_ack_ok();
			    }
				else//if(check_net_device_ack("+CREG: 0,2"))//search......
				{
                    netMisc.St._bit.RegestCSNet=0;
				    on_net_device_ack_ok();
                }
				
			}
			break;

         case 19:  
			if(!netMisc.AT_Waiting)
			{
                reset_net_deive_rcvbuf();
				TX_ATCommand("\r\nAT+CGREG?\r\n",10); 
			}
			else if(!netComps.AckTmr)
			{
				HandleNoAck(0);
            } 
			else if(check_net_device_ack("OK\r\n"))
			{
				if(check_net_device_ack("+CGREG: 0,1"))//home 
				{
				   netMisc.St._bit.RegestPSGNet=1;
                   on_net_device_ack_ok();
				}
				else if(check_net_device_ack("+CGREG: 0,5"))//Roaming
				{
					netMisc.St._bit.RegestPSGNet=1;
                    on_net_device_ack_ok();
				}
				else //if(check_net_device_ack("+CGREG: 0,2"))//search......
				{
                    netMisc.St._bit.RegestPSGNet=0;
                    on_net_device_ack_ok();

                }
            }
			break;

         case 20:  
			if(!netMisc.AT_Waiting)
			{
                reset_net_deive_rcvbuf();
				TX_ATCommand("\r\nAT+CEREG?\r\n",10); 
			}
			else if(!netComps.AckTmr)
			{
				HandleNoAck(0);
            } 
			else if(check_net_device_ack("OK\r\n"))
			{
				if(check_net_device_ack("+CEREG: 0,1"))//home 
				{
                    netMisc.St._bit.RegestPSENet=1;
                    on_net_device_ack_ok();
				}
				else if(check_net_device_ack("+CEREG: 0,5"))//Roaming
				{
					netMisc.St._bit.RegestPSENet=1;
                    on_net_device_ack_ok();
				}
				else //if(check_net_device_ack("+CEREG: 0,2"))//search......
				{
                    netMisc.St._bit.RegestPSENet=0;
                    on_net_device_ack_ok();
                }
            }
			break;

        case 21:
              if(!netMisc.St._bit.RegestCSNet)
              {
                    netMisc.NoRegestCSNetCount++;
                    if(netMisc.NoRegestCSNetCount>100)
                    {
                        HandleRegTimeOut();
                    	netMisc.NoRegestCSNetCount=0;
                    	break;
                    }
              }
              else if(!netMisc.St._bit.RegestPSGNet)
              {
                    netMisc.NoRegestPSGNetCount++;
                    if(netMisc.NoRegestPSGNetCount>20)
                    {
                    	netMisc.NoRegestPSGNetCount=0;
                    	netMisc.St._bit.RegestPSGNet=1;
                    }
              } 
              else if(!netMisc.St._bit.RegestPSENet)
              {
                    netMisc.NoRegestPSENetCount++;
                    if(netMisc.NoRegestPSENetCount>20)
                    {
                        netMisc.NoRegestPSENetCount=0;
                        netMisc.St._bit.RegestPSENet=1;
                    }
              }
               if(netMisc.St._bit.RegestCSNet&&netMisc.St._bit.RegestPSGNet&&netMisc.St._bit.RegestPSENet)
               {
                    //netComps.op_window_tmr=150;
                    netMisc.NoRegestPSGNetCount=0;
                    netMisc.NoRegestCSNetCount=0;
                    netMisc.NoRegestPSENetCount=0;
                    on_net_device_ack_ok();
               }
               else
               {
                    net_status_sjmp_any_step(16);//delay & read csq
               }
		        break;
                
/**************************end to check registration************************/

			
      case 22:  
                    if(!netMisc.AT_Waiting) 
    				{
    					reset_net_deive_rcvbuf();
    					TX_ATCommand("\r\nAT+QLTS=2\r\n",10);
    				}
    				else if(!netComps.AckTmr)
    				{
    					HandleNoAck(); 
    				}
    				else if(check_net_device_ack("OK\r\n"))
    				{
    					 
    					 if(pt_rssi=check_net_device_ack("+QLTS: \"20"))
    					 {
    						unsigned char Tbuf[8]=""; 
    					    unsigned char i=0;
    						unsigned char TimeZoneEn=0;
    						for(i=0;i<6;i++)
    						{
    							Tbuf[i]=((*(pt_rssi+3*i)-0x30)*10)+(*(pt_rssi+3*i+1)-0x30);
    						}
    						Tbuf[6]=*(pt_rssi+3*i-1);//+-
    						Tbuf[7]=((*(pt_rssi+3*i)-0x30)*10)+(*(pt_rssi+3*i+1)-0x30);
                            //if(GMTtoLocalTime(Tbuf))
                            for(i=0;i<6;i++)
                        	{
                        		Tbuf[i]=((Tbuf[i]%100/10)<<4)+(Tbuf[i]%10);
                        	}
    						{
    							if((Tbuf[0]>0x16)&&(Tbuf[0]<0x50)&&(Tbuf[1]<0x13)&&(Tbuf[1]>0x00)&&(Tbuf[2]<0x32)&&
    							 (Tbuf[2]>0x00)&&(Tbuf[3]<0x24)&&(Tbuf[4]<0x60)&&(Tbuf[5]<0x60))
    							{
    								rtc_valve.year=Tbuf[0];
    								rtc_valve.month=Tbuf[1];
    								rtc_valve.day=Tbuf[2];
    								rtc_valve.hour=Tbuf[3];
    								rtc_valve.min=Tbuf[4];
    								rtc_valve.sec=Tbuf[5];
							        R_RTC_Set_CounterValue(rtc_valve);	
    							}
    						}
    					}
    					on_net_device_ack_ok();
    			    }
			    break;
         case 23:  
            if(!netMisc.AT_Waiting)
			{
				reset_net_deive_rcvbuf();
			    TX_ATCommand("\r\nAT+QNWINFO\r\n",10);//
			} 
			else if(!netComps.AckTmr)
			{
				HandleNoAck();  
			}
			else if(check_net_device_ack("OK\r\n"))
			{
				 if(pt_rssi=check_net_device_ack("+QNWINFO: "))
				 {
					 memset(netComps.net_info.act,0,sizeof(netComps.net_info.act));
					 memset(netComps.net_info.oper,0,sizeof(netComps.net_info.oper));
					 memset(netComps.net_info.band,0,sizeof(netComps.net_info.band));
				     for(i=0;i<sizeof(netComps.net_info.act)-1;i++)
					 {
						if(*pt_rssi!=',')
						{
					 	  netComps.net_info.act[i]=*pt_rssi++;
						}
						else
						{
						   break; 	
						}
					 }
					 pt_rssi++;//sjmp ,
					 for(i=0;i<sizeof(netComps.net_info.oper)-1;i++)
					 {
						if(*pt_rssi!=',')
						{
					 	  netComps.net_info.oper[i]=*pt_rssi++;
						}
						else
						{
						   break; 	
						}
					 }
					 pt_rssi++;//sjmp ,
					 for(i=0;i<sizeof(netComps.net_info.band)-1;i++)
					 {
						if(*pt_rssi!=',')
						{
					 	  netComps.net_info.band[i]=*pt_rssi++;
						}
						else
						{
						   break; 	
						}
					 }
				 }
                 on_net_device_ack_ok();
				 netComps.disCode=EM_DIS_REGISTER_NET;
			}	
			break;
			
        case 24: 
			if(!netMisc.AT_Waiting)
			{
				reset_net_deive_rcvbuf();
			    TX_ATCommand("\r\nAT+QICSGP=1\r\n",10);
			} 
			else if(!netComps.AckTmr)
			{
				HandleNoAck();  
			}
			else if(pt_rssi=check_net_device_ack("OK\r\n"))
			{
				if(pt_rssi=check_net_device_ack("+QICSGP: "))
				 {
//					memset(netComps.net_info.imei,0,sizeof(netComps.net_info.imei));
//			        for(i=0;i<sizeof(netComps.net_info.imei);i++)
//				    {
//						if(*pt_rssi!='\r')
//						{
//						    netComps.net_info.imei[i]=*pt_rssi++;
//						}
//						else
//						{
//							break;
//						}
//				    } 
				 }
				 on_net_device_ack_ok();
			}	
            break;
            
        case 25: 
			if(!netMisc.AT_Waiting)
			{
				reset_net_deive_rcvbuf();
			    TX_ATCommand("\r\nAT+QIDEACT=1\r\n",150);
			} 
			else if(!netComps.AckTmr)
			{
				HandleErr(); 
			}
			else if(pt_rssi=check_net_device_ack("OK\r\n"))
			{
	            on_net_device_ack_ok();
			}
			else if(pt_rssi=check_net_device_ack("ERROR"))
			{
                HandleErr();
			}
            break; 
            
       case 26: 
			if(!netMisc.AT_Waiting)
			{
				reset_net_deive_rcvbuf();
			    TX_ATCommand("\r\nAT+QIACT=1\r\n",150);
			} 
			else if(!netComps.AckTmr)
			{
				HandleErr(); 
			}
			else if(pt_rssi=check_net_device_ack("OK\r\n"))
			{
	            on_net_device_ack_ok();
			}
			else if(pt_rssi=check_net_device_ack("ERROR"))
			{
                HandleErr();
			}
            break; 

         case 27:
            if(device_comps.access_param.flag==2)//domain acces
            {
                if(!netMisc.AT_Waiting)
    			{
                    char msg[120]="\r\nAT+QIDNSGIP=1,";
                    sprintf(msg+strlen(msg),"%s",device_comps.access_param.domain_name);
                    sprintf(msg+strlen(msg),"%s","\r\n");
    				reset_net_deive_rcvbuf();
                    TX_ATCommand(msg,120);
    			} 
    			else if(!netComps.AckTmr)
    			{
    				on_net_device_ack_ok();
    			}
    			else if(pt_rssi=check_net_device_ack("+QIURC: \"dnsgip\",\""))
    			{
                    if(strstr(pt_rssi,"\r\n"))
                    {
                         char *ptsz;
                         device_comps.access_param.ip[0]=strtol(pt_rssi,&ptsz,10);
                         device_comps.access_param.ip[1]=strtol(ptsz+1,&ptsz,10);
                         device_comps.access_param.ip[2]=strtol(ptsz+1,&ptsz,10);
                         device_comps.access_param.ip[3]=strtol(ptsz+1,&ptsz,10);
        				 netComps.St._bit.ResolvedIP=1;
        				 reset_net_deive_rcvbuf();
        				 netComps.AckTmr=2;
                    }
                    
    			}
                else if(pt_rssi=check_net_device_ack("ERROR"))
    			{
                    HandleErr();
    			}
            }
            else if(device_comps.access_param.flag==1)//ip access
            {
                 on_net_device_ack_ok();
            }
			else 
			{
                net_status_sjmp_any_step(MD_NET_DEVICE_NULL_STEP);
                netComps.St._bit.noIP=1;
			}
            break; 
             
           case 28: 
              if(!netMisc.AT_Waiting)
              {
                  reset_net_deive_rcvbuf();
                  TX_ATCommand("\r\nAT+QISDE=0\r\n",10);
              } 
              else if(!netComps.AckTmr)
              {
                  HandleNoAck();  //
              }
              else if(pt_rssi=check_net_device_ack("OK\r\n"))
              {
                  on_net_device_ack_ok();
              }
              break; 
			  
			  
        case 29: 
			if(!netMisc.AT_Waiting)
			{
				reset_net_deive_rcvbuf();
			    TX_ATCommand("\r\nAT+QICLOSE=0\r\n",150);
			} 
			else if(!netComps.AckTmr)
			{
				HandleNoAck();  
			}
			else if(pt_rssi=check_net_device_ack("OK\r\n"))
			{
	            on_net_device_ack_ok();
	            netComps.St._bit.socket_connected=0;
			}
			else if(pt_rssi=check_net_device_ack("ERROR"))
			{
                HandleErr();
			}
            break;
            
        case 30: 
			if(!netMisc.AT_Waiting)
			{
                char msg[120]="\r\nAT+QIOPEN=1,0,\"TCP\",";
                if(device_comps.access_param.flag==2)//domain acces
                {
                    sprintf(msg+strlen(msg),"\"%s\",",device_comps.access_param.domain_name);
                }
                else
                {
                    sprintf(msg+strlen(msg),"\"%d.",device_comps.access_param.ip[0]);
                    sprintf(msg+strlen(msg),"%d.",device_comps.access_param.ip[1]);
                    sprintf(msg+strlen(msg),"%d.",device_comps.access_param.ip[2]);
                    sprintf(msg+strlen(msg),"%d\",",device_comps.access_param.ip[3]);
                }
                sprintf(msg+strlen(msg),"%u,",device_comps.access_param.port);
                sprintf(msg+strlen(msg),"%s" ,"0,0\r\n");
				reset_net_deive_rcvbuf();
                TX_ATCommand(msg,150);
			} 
			else if(!netComps.AckTmr)
			{
				HandleNoAck();  
			}
			else if(pt_rssi=check_net_device_ack("OK\r\n"))
			{
			    if(check_net_device_ack("+QIOPEN: 0,0"))
				{
					on_net_device_ack_ok();
					netComps.St._bit.socket_connected=1;
	            }
	            else if(check_net_device_ack("+QIOPEN: 0,566"))
				{
					HandleErr();
	            }
	        }
			else if(pt_rssi=check_net_device_ack("ERROR"))
			{
                HandleErr();
			}
            break;
        
     


        

/*****************************START MSG LOOP*****************/

			
		case 31:
			if(!netMisc.AT_Waiting)
			{
				reset_net_deive_rcvbuf();
				netMisc.AT_Waiting=1;
				netComps.AckTmr=2; 
			}
			else if(!netComps.AckTmr)
			{
                if(protocolComps.sw._bit.DataRdy)
				{
	                PacketCoapMsg(protocolComps.msg,protocolComps.msgLen);
                    on_net_device_ack_ok();
				}
				else
				{
					net_status_sjmp_any_step(37);
				}
            }
			break;
			
		case 32:
             GetRssi();
    	     break;	

		case 33:
            if(!netMisc.AT_Waiting)
            {
                char msg[32]="";
                sprintf(msg,"\r\nAT+QISEND=0,%d\r\n",protocolComps.msgLen);
                reset_net_deive_rcvbuf();
                TX_ATCommand(msg,10);
            } 
            else if(!netComps.AckTmr)
            {
                HandleNoAck();  //
            }
            else if(pt_rssi=check_net_device_ack("> "))
            {
                on_net_device_ack_ok();
            }
            else if(pt_rssi=check_net_device_ack("ERROR"))
            {
                HandleErr();
            }
            break;

        case 34://SendData
		    if(!netMisc.AT_Waiting)
			{
				reset_net_deive_rcvbuf();
				TX_ATData(&netMisc.send_buf[0],10);
				netComps.disCode=EM_DIS_SEND;
			} 
			else if(!netComps.AckTmr)
			{
				HandleNoAck(); 
			}
			else if(check_net_device_ack("SEND OK\r\n"))
			{
                
                 on_net_device_ack_ok();
				
			}
            else if(pt_rssi=check_net_device_ack("ERROR"))
            {
                HandleErr();
            }

			break;
			
        case 35:
            if(!netMisc.AT_Waiting)
			{ 
                netMisc.AT_Waiting=1;
			 	netComps.AckTmr = 2;//////////////////1S
			}
			else if(!netComps.AckTmr)
			{
              on_net_device_ack_ok();
			}
            break;
                
		case 36://que SendData
    	    if(!netMisc.AT_Waiting)
    		{
    			reset_net_deive_rcvbuf();
                TX_ATCommand("\r\nAT+QISEND=0,0\r\n",10);
    		} 
    		else if(!netComps.AckTmr)
    		{
                HandleNoAck(); 
    		}
    		else if(pt_rssi=check_net_device_ack("OK\r\n"))
    		{
    		    if(pt_rssi=check_net_device_ack("+QISEND: "))
        		{
    		        char *ptsz;
                    long noAckBytes;
                    static int query_times=0;
                    strtol(pt_rssi,&ptsz,10);
                    strtol(ptsz+1, &ptsz,10);
                    noAckBytes=strtol(ptsz+1, &ptsz,10);
                    if(!noAckBytes)
                    {
                        protocolComps.sw._bit.DataRdy=0;
                        protocolComps.sw._bit.dataPushYet=1;
                        query_times=0;
                        on_net_device_ack_ok();
                    }
                    else
                    {
                        if(query_times<3)
                        {
                             
                             net_status_sjmp_any_step(MD_NET_DEVICE_DELAY_STEP);//
                             query_times++;
                        }
                        else
                        {
                            net_status_sjmp_any_step(29);//close socket
                            query_times=0;
                        }
                        
                    }
    			}
    		}
            else if(pt_rssi=check_net_device_ack("ERROR"))
            {
                HandleErr();
            }

    		break; 
			
		case 37://///////////////////Wait Rec
		  
		        if(!netMisc.AT_Waiting)
    			{ 
                    netMisc.AT_Waiting=1;
    			 	netComps.AckTmr = 2;
    			}
    			else if(!netComps.AckTmr)
    			{
                   
                    if(!netComps.St._bit.recvData)
                    {
                        on_net_device_ack_ok();
                    }
    				else
    				{
                        net_status_sjmp_any_step(MD_NET_DEVICE_SEND_STEP);//send data
    				}
    			}
                break;
         	      
		case 38:
	    	if(!netMisc.AT_Waiting)
			{
               reset_net_deive_rcvbuf();
			   TX_ATCommand("\r\nAT+QIRD=0,512\r\n",10);
            }
			else if(!netComps.AckTmr)
			{
				HandleNoAck(); 
			}
			else if(check_net_device_ack("OK\r\n"))
			{
				int Length=0;
				
	            if(pt_rssi=check_net_device_ack("+QIRD: "))
				{
                    
                    Length=atoi(pt_rssi);
                    if((Length<1)||(Length>256))
                    {
                    	netMisc.ErrCode=0;
                    }
					else
					{
                         while((*pt_rssi!='\n') &&(*pt_rssi!='\0'))
            			 {
            				 pt_rssi++;
            			 }
                         netMisc.ErrCode=GetMsg(pt_rssi+1,Length);
						 netComps.disCode=EM_DIS_RECV; 
					}
                }
				else
				{
					netMisc.ErrCode=3;//err=0;no data or err err=1,recv data,err=3,other
				}
				DealAnalyticCode(netMisc.ErrCode);
            }
            break;    

       /*****************************END MSG LOOP*****************/
       

        case MD_NET_DEVICE_DELAY_STEP:
			if(!netMisc.AT_Waiting) //delay fuc
			{
				netComps.AckTmr =3;	//
				netMisc.AT_Waiting = 1;
			}
			else if(!netComps.AckTmr)
			{
 				net_status_sjmp_any_step(netMisc._Flag_St);//back to called delay function 
			}
			break;

        
        case MD_NET_DEVICE_NULL_STEP:

        default: 
                break;

		  
	}
}




void net_task_handle(void)
{
	if(netComps.St._bit.on)
	{
        enable_net_receive();
		if(!GSMPWRCTL_pin)
		{
			cold_start_net_device();
		}
		else
		{
			hot_start_net_device();
		}
		
		netMisc.reStartTimes=1;
        netComps.St._bit.running=1;
        netComps.op_window_tmr = MD_MODULE_OP_MAX_TIME;	//5¡¤??¨®//
        netComps.St._bit.on=0;
	}
	
	if(netComps.St._bit.off)
	{
		netComps.St._bit.off=0;
		netComps.St._bit.offing=1;
		stop_net_device();
	}
	
	if(netComps.St._bit.running)
	{
		Srv_GSM();
	}
	
}


netComps_t netComps=
{
	{0},//st
	EM_DIS_ACT,
	&netMisc.recv_msg_buf,//unsigned char *const msg;//Passed to the protocol layer point
	0,//unsigned int   msglen;
	netMisc.recv_buf,//igned char *const recv_base_pt;//Passed to uart recv  point
   &netMisc.recv_Idx,//
	
	0,//unsigned char *recv_RxSt;
	30,//unsigned short AckTmr;//netComps.AckTmr
	0,//unsigned short op_window_tmr; 
	&netMisc.Flag_St,//unsigned char const *run_st;
	
	{0},//netinfo
	net_task_handle,//void (*const task_handle)(void);
};

