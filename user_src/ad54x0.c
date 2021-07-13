#include "r_cg_macrodriver.h"
#include "ad54x0.h"
#include "pcf857x.h"



/***************************************************************************

 Author        : Neil Zhao - CAST

 Date          : Feb 23th, 2009

 File          : ad5420driver_IO.c

 Hardware      : ADuC7026 and AD5420

 Description   : Use the GPIO to simulate the SPI communication of AD5420
	
***************************************************************************/


#define SET_CLEAR()		(pcf857x_comps.OUT._bit.no0=1,pcf857x_comps.write(&pcf857x_comps))//GP4DAT = (GP4DAT | 0x00040000)	//P4.2->CLEAR
#define CLR_CLEAR()		(pcf857x_comps.OUT._bit.no0=0,pcf857x_comps.write(&pcf857x_comps))//GP4DAT = (GP4DAT & 0xFFFBFFFF)

#define SET_LATCH()		(pcf857x_comps.OUT._bit.no1=1,pcf857x_comps.write(&pcf857x_comps))//GP4DAT = (GP4DAT | 0x00080000)	//P4.3->LATCH
#define CLR_LATCH()		(pcf857x_comps.OUT._bit.no1=0,pcf857x_comps.write(&pcf857x_comps))//GP4DAT = (GP4DAT & 0xFFF7FFFF)

#define	SET_SCL()		(pcf857x_comps.OUT._bit.no2=1,pcf857x_comps.write(&pcf857x_comps))//GP4DAT = (GP4DAT | 0x00100000)	//P4.4->SCLK
#define	CLR_SCL()		(pcf857x_comps.OUT._bit.no2=0,pcf857x_comps.write(&pcf857x_comps))//GP4DAT = (GP4DAT & 0xffefffff)

#define SET_SDO()		(pcf857x_comps.OUT._bit.no3=1,pcf857x_comps.write(&pcf857x_comps))//GP4DAT = (GP4DAT | 0x00200000)	//P4.5->SDIN
#define CLR_SDO()		(pcf857x_comps.OUT._bit.no3=0,pcf857x_comps.write(&pcf857x_comps))//GP4DAT = (GP4DAT & 0xffdfffff)

#define SDI             (pcf857x_comps.read(&pcf857x_comps),pcf857x_comps.IN._bit.no4)//

static void delay (int length)
{
	while (length >0)
    	length--;
}

//---------------------------------
//void WriteToAD5420(unsigned char count,unsigned char *Buf);
//---------------------------------
//Function that writes to the AD5420 via the SPI port. 
//--------------------------------------------------------------------------------
static void WriteToAD5420(unsigned char count,void const *const buf)
{

	unsigned	char	ValueToWrite = 0;
        unsigned	char	i = 0;
	unsigned	char	j = 0;
	unsigned        char const *const Buf=(unsigned char *)buf;;

	CLR_LATCH();

	for ( i=count;i>0;i-- )
 	{
	 	ValueToWrite =	*(Buf+i-1);
		for (j=0; j<8; j++)
		{
			CLR_SCL();
			if(0x80 == (ValueToWrite & 0x80))
			{
				SET_SDO();	    //Send one to SDIN pin of AD5420
			}
			else
			{
				CLR_SDO();	    //Send zero to SDIN pin of AD5420
			}
										 
			delay(1);
			SET_SCL();
			delay(1);
			ValueToWrite <<= 1;	//Rotate data
	
		}

	}
	CLR_SCL();
	delay(1);
	SET_LATCH();
	delay(20);
}


//---------------------------------
//ReadFromAD5420();
//---------------------------------
//Function that reads from the AD5420 via the SPI port. 
//--------------------------------------------------------------------------------
static void ReadFromAD5420(unsigned char count,void  * const buf)
{
	unsigned  char	i = 0;
	unsigned  char	j = 0;
	volatile unsigned  char  iTemp = 0;
	unsigned  char  RotateData = 0;
	unsigned  char  *const Buf=(unsigned char *)buf;

	CLR_LATCH();

	for(j=count; j>0; j--)
	{
		for(i=0; i<8; i++)
		{
		        CLR_SCL();
			RotateData <<= 1;		//Rotate data
			delay(1);
			CLR_SDO();				//write a nop condition when read the data. 
			iTemp = SDI;			//Read SDO of AD5420
			SET_SCL();	
			if(0x01 == (iTemp & 0x01))
			{
				RotateData |= 1;	
			}
			delay(1);
			
		}
		*(Buf+j-1)= RotateData;
	}
	CLR_SCL();
	delay(1);	 
	SET_LATCH();
	delay(20);
} 

static void write_control_reg(unsigned int const *const p)
{
	unsigned char buf[3];
	buf[2]=0x55;
	buf[1]=(*p)>>8;
	buf[0]=*p;
	WriteToAD5420(3,buf);
     
}

static void read_control_reg(unsigned int *const p)
{
	unsigned char buf[3];
	buf[2]=0x02;
	buf[1]=0x00;
	buf[0]=0x02;
	WriteToAD5420(3,buf);
	ReadFromAD5420(3,buf);
	*p=((unsigned int)buf[1]<<8)+buf[0];
}

static void write_data_reg(unsigned int const *const p)
{
	unsigned char buf[3];
	buf[2]=0x01;
	buf[1]=(*p)>>8;
	buf[0]=*p;
	WriteToAD5420(3,buf);
	CLR_CLEAR();
}

static void read_data_reg(unsigned int *const p)
{
	unsigned char buf[3];
	buf[2]=0x02;
	buf[1]=0x00;
	buf[0]=0x01;
	WriteToAD5420(3,buf);
	ReadFromAD5420(3,buf);
	*p=((unsigned int)buf[1]<<8)+buf[0];
}

static void read_status_reg(unsigned int *const p)
{
	unsigned char buf[3];
	buf[2]=0x02;
	buf[1]=0x00;
	buf[0]=0x00;
	WriteToAD5420(3,buf);
	ReadFromAD5420(3,buf);
	*p=((unsigned int)buf[1]<<8)+buf[0];
}

static void write_reset_reg(unsigned int const *const p)
{
	unsigned char buf[3];
	buf[2]=0x56;
	buf[1]=(*p)>>8;
	buf[0]=*p;
	WriteToAD5420(3,buf);
}
/***Input shift register format*****************
MSB                                     LSB   
DB23                       DB15~DB0   
Address byte               Data word

Address byte function          
00000000 No operation (NOP)
00000001 Data register
00000010 Read back the register value according to the read address (see Table 1)
01010101 Control register
01010110 Reset register
************************************************/
/*******************Table 1**************************
00 Read status register
01 Read data register
10 Read control register
****************************************************/
ad54x0_comps_t ad54x0_comps=
{
	"",//char desc[32];
	&ad54x0_comps,//struct_AD54X0_COMPONENTS *this;
	write_reset_reg,//void (*const write_reset_reg)(unsigned int const *const p);
	write_control_reg,//void (*const write_control_reg)(unsigned int const *const p);
	read_control_reg,//void (*const read_control_reg )(unsigned int *const p);      
	write_data_reg,//void (*const write_data_reg)(unsigned int const *const p);   
	read_data_reg,//void (*const read_data_reg )(unsigned int *const p);         
	read_status_reg//void (*const read_status_reg )(unsigned int *const p);      
};
