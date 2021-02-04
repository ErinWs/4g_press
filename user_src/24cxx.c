/*****************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_userdefine.h"
#include "r_cg_adc.h"
#include "r_cg_sau.h"
#include "device.h"
#include "string.h"
#include "24cxx.h"

#define  MD_I2C_VCC             P3_bit.no3 // 
int      MD_I2C_WP;		       
#define  MD_I2C_SCL             P6_bit.no0     //
#define  MD_I2C_SDA             P6_bit.no1    //
#define  MD_I2C_SDA_MODE        PM6_bit.no1   //


#define  MD_I2C_STATUS_TRUE     0
#define  MD_I2C_STATUS_FALSE    1
#define  MD_I2C_RETURN_FALSE    {MD_I2C_VCC=0;MD_I2C_WP=0;MD_I2C_SCL = 0;return MD_I2C_STATUS_FALSE;}
#define  MD_I2C_RETURN_TRUE     {return MD_I2C_STATUS_TRUE;}

static void DelayMs(unsigned short TimeMs)
{
	volatile unsigned short j;
	unsigned short  i;
	for(j=0;j<TimeMs*10;j++)
	{		
		i=97;	//100us at 8Mhz
		while(i>0) 
		{	
			i--;
		}
	}
}

static void I2CDelay(void)	
{	unsigned char i;
	i=7;	    //10us at 8Mhz
	while(i>0)
	{
		i--;
	}
}

static void I2CStart(void)
{
	
	MD_I2C_SCL = 1;
	MD_I2C_SDA_MODE = 1; 
	I2CDelay(); 
	MD_I2C_SDA = 0;
	MD_I2C_SDA_MODE = 0;  
	I2CDelay();
	MD_I2C_SCL = 0;
}

static void I2CStop(void)
{
	MD_I2C_SDA_MODE = 0;
	MD_I2C_SDA = 0;
	I2CDelay(); 
	MD_I2C_SCL = 1; 
	I2CDelay(); 
	MD_I2C_SDA_MODE = 1;  
	I2CDelay();
	MD_I2C_SDA_MODE = 1;
}
 
static void I2CACK(void)
{
	MD_I2C_SDA = 0;
	MD_I2C_SDA_MODE = 0;
	I2CDelay(); 
	MD_I2C_SCL   = 1;
	I2CDelay(); 
	MD_I2C_SCL   = 0;
	I2CDelay(); 
}

static void I2CNOACK(void)
{
	MD_I2C_SDA_MODE = 1;
	I2CDelay(); 
	MD_I2C_SCL = 1;
	I2CDelay(); 
	MD_I2C_SCL = 0;
	I2CDelay(); 
}

static unsigned char  I2CByteTX(unsigned char byte)
{
	unsigned char loop = 8;
    while(loop--)
    {
		if(byte & 0x80)  
		{
		    MD_I2C_SDA = 1;
		}
		else
		{
			MD_I2C_SDA = 0;
			MD_I2C_SDA_MODE = 0;
		}
		I2CDelay();
		MD_I2C_SCL  = 1;
		I2CDelay();
		MD_I2C_SCL  = 0;
		byte  <<= 1;
	} 
	MD_I2C_SDA_MODE = 1; 
	I2CDelay();
	MD_I2C_SCL  = 1;
	I2CDelay();
	if(MD_I2C_SDA == 1)
	{
		MD_I2C_SCL = 1;
		return MD_I2C_STATUS_FALSE;
	}
	MD_I2C_SCL = 0;
	I2CDelay();
	return MD_I2C_STATUS_TRUE;
}

static unsigned char  I2CByteRX(void)
{
	unsigned char byte;
	unsigned char loop = 8;
	MD_I2C_SDA_MODE = 1;
	do
	{
		byte <<= 1;
		MD_I2C_SCL = 1;
		I2CDelay();
		if(MD_I2C_SDA == 1) 
		{
		    byte |= 0x01;
		}
		MD_I2C_SCL = 0;
		I2CDelay();
		loop--;
	}while(loop > 0);
	return(byte);
}

static unsigned char write_eeprom(unsigned int addr,  void const *buf, unsigned int n)
{
	unsigned char w_wait,e2page,SlaveAddr;	
	unsigned char const *src=(unsigned char *)buf;
	e2page = 16;
	MD_I2C_VCC = 1;
	I2CDelay();
	MD_I2C_WP=0; 
reStart:
	SlaveAddr = 0xA0 + (unsigned char)((addr & 0x0700)>>7);
	for(w_wait=0; w_wait<4; w_wait++)
	{
		I2CStart();
		if(MD_I2C_STATUS_TRUE == I2CByteTX(SlaveAddr))
		{
		    break;
		}
		I2CStop();
		I2CStop();
		DelayMs(1);
	}	
	if(w_wait >= 4) 
	{
	    MD_I2C_RETURN_FALSE;
	}
	if(MD_I2C_STATUS_FALSE == I2CByteTX((unsigned char)(addr%0x100))) 
	{
	    MD_I2C_RETURN_FALSE; 
	}
	while(n--)
	{
		if(MD_I2C_STATUS_FALSE == I2CByteTX(*src++)) 
		{
		    MD_I2C_RETURN_FALSE;
		}
		addr++;
		if((addr % e2page) == 0)
		{
		    break;
		}
	}	
	I2CStop();
	DelayMs(5);
	if(n != 0xffff)	
	{
	    goto reStart;
	}
	
	I2CDelay();
	MD_I2C_VCC=0;
	MD_I2C_WP=0;
	MD_I2C_SCL = 0;	
	MD_I2C_RETURN_TRUE;
}

static unsigned char read_eeprom(unsigned int addr, void *buf, unsigned int n)
{
	unsigned char w_wait,SlaveAddr;
	unsigned char *dest=(unsigned char *)buf;
	MD_I2C_VCC = 1;
	I2CDelay(); 	
	SlaveAddr = 0xA0 + (unsigned char)((addr & 0x0700)>>7);	
	for(w_wait=0; w_wait<4; w_wait++)
	{
		I2CStart();
		if(MD_I2C_STATUS_TRUE == I2CByteTX(SlaveAddr))
	    {
		    break;
		}
		I2CStop();
		I2CStop();
		DelayMs(1);
	}
	
	if(w_wait >= 4)
	{
	    MD_I2C_RETURN_FALSE;
	}
	if(MD_I2C_STATUS_FALSE == I2CByteTX((unsigned char)(addr%0x100)))
	{
	    MD_I2C_RETURN_FALSE;
	}
	I2CStart();
	if(MD_I2C_STATUS_FALSE == I2CByteTX((SlaveAddr+1))) 
	{
	    MD_I2C_RETURN_FALSE;
	}
	while(n--)
	{
		*dest++	= I2CByteRX();
		if(n != 0)
		{
			I2CACK();
		}
		addr++;
	}
	I2CNOACK();
	I2CStop();	
	DelayMs(1);
	I2CDelay();MD_I2C_VCC=0;MD_I2C_SCL = 0;	
	MD_I2C_RETURN_TRUE;
}

static unsigned char init_eeprom()
{
    unsigned char status=MD_I2C_STATUS_FALSE;
    MD_I2C_VCC=0;           
    MD_I2C_WP=0;            
    MD_I2C_SCL=0;            
    MD_I2C_SDA=0;            
    MD_I2C_SDA_MODE=0;
    MD_I2C_RETURN_TRUE;
    status=MD_I2C_STATUS_TRUE;
    return status;

}
_24cxx_comps_t _24cxx_comps=
{
	"e2pom r/w driver", //char *desc;
	&_24cxx_comps,      //struct _24CXX_COMPONENTS *this;
	init_eeprom,        // unsigned char init_eeprom(void);
	write_eeprom,       //unsigned char write_eeprom(unsigned int addr,  void const *buf, unsigned int n);
	read_eeprom         //unsigned char read_eeprom (unsigned int addr,  void *buf, unsigned int n)
};