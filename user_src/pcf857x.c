#include "r_cg_macrodriver.h"
#include "pcf857x.h"

	       
#define  MD_PCF857X_I2C_SCL             P4_bit.no4     //
#define  MD_PCF857X_I2C_SDA             P4_bit.no5    //
#define  MD_PCF857X_I2C_SDA_MODE        PM4_bit.no5   //


#define  MD_PCF857X_I2C_TRUE     0
#define  MD_PCF857X_I2C_FALSE    1
#define  MD_PCF857X_I2C_RETURN_FALSE    {MD_PCF857X_I2C_SCL=0;return MD_PCF857X_I2C_FALSE;}
#define  MD_PCF857X_I2C_RETURN_TRUE     {return MD_PCF857X_I2C_TRUE;}

//static void DelayMs(unsigned short TimeMs)
//{
//	unsigned short j;
//	unsigned short  i;
//	for(j=0;j<TimeMs*10;j++)
//	{		
//		i=97;	//100us at 8Mhz
//		while(i>0) 
//		{	
//			i--;
//		}
//	}
//}
static void PCF857X_I2CDelay(void)	
{	unsigned char i;
	i=7;	    //10us at 8Mhz
	while(i>0)
	{
		i--;
	}
}

static void PCF857X_I2CStart(void)
{
	
	MD_PCF857X_I2C_SCL = 1;
	MD_PCF857X_I2C_SDA_MODE = 1; 
	PCF857X_I2CDelay(); 
	MD_PCF857X_I2C_SDA = 0;
	MD_PCF857X_I2C_SDA_MODE = 0;  
	PCF857X_I2CDelay();
	MD_PCF857X_I2C_SCL = 0;
}

static void PCF857X_I2CStop(void)
{
	MD_PCF857X_I2C_SDA_MODE = 0;
	MD_PCF857X_I2C_SDA = 0;
	PCF857X_I2CDelay(); 
	MD_PCF857X_I2C_SCL = 1; 
	PCF857X_I2CDelay(); 
	MD_PCF857X_I2C_SDA_MODE = 1;  
	PCF857X_I2CDelay();
	MD_PCF857X_I2C_SDA_MODE = 1;
}
 
static void PCF857X_I2CACK(void)
{
	MD_PCF857X_I2C_SDA = 0;
	MD_PCF857X_I2C_SDA_MODE = 0;
	PCF857X_I2CDelay(); 
	MD_PCF857X_I2C_SCL   = 1;
	PCF857X_I2CDelay(); 
	MD_PCF857X_I2C_SCL   = 0;
	PCF857X_I2CDelay(); 
}

static void PCF857X_I2CNOACK(void)
{
	MD_PCF857X_I2C_SDA_MODE = 1;
	PCF857X_I2CDelay(); 
	MD_PCF857X_I2C_SCL = 1;
	PCF857X_I2CDelay(); 
	MD_PCF857X_I2C_SCL = 0;
	PCF857X_I2CDelay(); 
}

static unsigned char  PCF857X_I2CByteTX(unsigned char byte)
{
	unsigned char loop = 8;
    while(loop--)
    {
		if(byte & 0x80)  
		{
		    MD_PCF857X_I2C_SDA = 1;
		}
		else
		{
			MD_PCF857X_I2C_SDA = 0;
			MD_PCF857X_I2C_SDA_MODE = 0;
		}
		PCF857X_I2CDelay();
		MD_PCF857X_I2C_SCL  = 1;
		PCF857X_I2CDelay();
		MD_PCF857X_I2C_SCL  = 0;
		byte  <<= 1;
	} 
	MD_PCF857X_I2C_SDA_MODE = 1; 
	PCF857X_I2CDelay();
	MD_PCF857X_I2C_SCL  = 1;
	PCF857X_I2CDelay();
	if(MD_PCF857X_I2C_SDA == 1)
	{
		MD_PCF857X_I2C_SCL = 1;
		return MD_PCF857X_I2C_FALSE;
	}
	MD_PCF857X_I2C_SCL = 0;
	PCF857X_I2CDelay();
	return MD_PCF857X_I2C_TRUE;
}

static unsigned char  PCF857X_I2CByteRX(void)
{
	unsigned char byte;
	unsigned char loop = 8;
	MD_PCF857X_I2C_SDA_MODE = 1;
	do
	{
		byte <<= 1;
		MD_PCF857X_I2C_SCL = 1;
		PCF857X_I2CDelay();
		if(MD_PCF857X_I2C_SDA == 1) 
		{
		    byte |= 0x01;
		}
		MD_PCF857X_I2C_SCL = 0;
		PCF857X_I2CDelay();
		loop--;
	}while(loop > 0);
	return(byte);
}



static int pcf857x_read(pcf857x_comps_t *this)
{
    unsigned char w_wait,SlaveAddr;	
	SlaveAddr = 0x41;
	for(w_wait=0; w_wait<1; w_wait++)
	{
		PCF857X_I2CStart();
		if(MD_PCF857X_I2C_TRUE == PCF857X_I2CByteTX(SlaveAddr))
		{
		    break;
		}
		PCF857X_I2CStop();
		PCF857X_I2CStop();
		//DelayMs(1);
	}	
	if(w_wait >= 1) 
	{
	    MD_PCF857X_I2C_RETURN_FALSE;
	}
	this->IN.PX= PCF857X_I2CByteRX();
	PCF857X_I2CNOACK();
	PCF857X_I2CStop();
	MD_PCF857X_I2C_RETURN_TRUE;
}


static int pcf857x_write(pcf857x_comps_t *this)
{
    unsigned char w_wait,SlaveAddr;	
	SlaveAddr = 0x40;
	for(w_wait=0; w_wait<1; w_wait++)
	{
		PCF857X_I2CStart();
		if(MD_PCF857X_I2C_TRUE == PCF857X_I2CByteTX(SlaveAddr))
		{
		    break;
		}
		PCF857X_I2CStop();
		PCF857X_I2CStop();
		//DelayMs(1);
	}	
	if(w_wait >= 1) 
	{
	    MD_PCF857X_I2C_RETURN_FALSE;
	}
	if(MD_PCF857X_I2C_FALSE == PCF857X_I2CByteTX(this->OUT.PX)) 
	{
	    MD_PCF857X_I2C_RETURN_FALSE; 
	}
	PCF857X_I2CStop();
	MD_PCF857X_I2C_RETURN_TRUE;
}

static int pcf857x_init(void)
{
    pcf857x_comps_t *this=&pcf857x_comps;
    pcf857x_comps.OUT._bit.no0=1;
    pcf857x_comps.OUT._bit.no1=1;
    pcf857x_comps.OUT._bit.no2=0;
    pcf857x_comps.OUT._bit.no3=0;
    pcf857x_comps.OUT._bit.no4=1;
    if(MD_PCF857X_I2C_TRUE==this->write(this))
    {
        return 0;
    }
    else
    {
        return 1;
    }
}


pcf857x_comps_t pcf857x_comps=
{
    "",
    &pcf857x_comps,
    {0},
    {0},
    pcf857x_init,
    pcf857x_read,
    pcf857x_write
  
};


