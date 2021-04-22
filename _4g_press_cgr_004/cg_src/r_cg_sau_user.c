/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIESREGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2016 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_cg_sau_user.c
* Version      : Applilet4 for R7F0C003/004 V1.00.00.01 [01 Mar 2016]
* Device(s)    : R7F0C004M
* Tool-Chain   : CCRL
* Description  : This file implements device driver for SAU module.
* Creation Date: 2020/12/30 星期�?
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_sau.h"
#include "r_cg_tau.h"
/* Start user code for include. Do not edit comment generated here */
#include "irc.h"
#include "net.h"
#include "lora.h"
#include "modbus.h"
#include "device.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
#pragma interrupt r_uart0_interrupt_send(vect=INTST0)
#pragma interrupt r_uart0_interrupt_receive(vect=INTSR0)
#pragma interrupt r_uart2_interrupt_send(vect=INTST2)
#pragma interrupt r_uart2_interrupt_receive(vect=INTSR2)
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
extern volatile uint8_t * gp_uart0_tx_address;         /* uart0 send buffer address */
extern volatile uint16_t  g_uart0_tx_count;            /* uart0 send data number */
extern volatile uint8_t * gp_uart0_rx_address;         /* uart0 receive buffer address */
extern volatile uint16_t  g_uart0_rx_count;            /* uart0 receive data number */
extern volatile uint16_t  g_uart0_rx_length;           /* uart0 receive data length */
extern volatile uint8_t * gp_uart2_tx_address;         /* uart2 send buffer address */
extern volatile uint16_t  g_uart2_tx_count;            /* uart2 send data number */
extern volatile uint8_t * gp_uart2_rx_address;         /* uart2 receive buffer address */
extern volatile uint16_t  g_uart2_rx_count;            /* uart2 receive data number */
extern volatile uint16_t  g_uart2_rx_length;           /* uart2 receive data length */
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: r_uart0_interrupt_receive
* Description  : None
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_uart0_interrupt_receive(void)
{
    uint8_t rx_data;
    uint8_t err_type;
    
    err_type = (uint8_t)(SSR01 & 0x0007U);
    SIR01 = (uint16_t)err_type;

    if (err_type != 0U)
    {
        r_uart0_callback_error(err_type);
    }
    
    rx_data = RXD0;

   
	  ircComps.recv_base_pt[*ircComps.recv_pos_pt]=rx_data;
	  *ircComps.recv_pos_pt+=1;
	  *ircComps.recv_pos_pt&=0x7f;
}
/***********************************************************************************************************************
* Function Name: r_uart0_interrupt_send
* Description  : None
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_uart0_interrupt_send(void)
{
    if (g_uart0_tx_count > 0U)
    {
        TXD0 = *gp_uart0_tx_address;
        gp_uart0_tx_address++;
        g_uart0_tx_count--;
    }
    else
    {
        r_uart0_callback_sendend();
    }
}
/***********************************************************************************************************************
* Function Name: r_uart0_callback_receiveend
* Description  : This function is a callback function when UART0 finishes reception.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void r_uart0_callback_receiveend(void)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_uart0_callback_softwareoverrun
* Description  : This function is a callback function when UART0 receives an overflow data.
* Arguments    : rx_data -
*                    receive data
* Return Value : None
***********************************************************************************************************************/
static void r_uart0_callback_softwareoverrun(uint16_t rx_data)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_uart0_callback_sendend
* Description  : This function is a callback function when UART0 finishes transmission.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void r_uart0_callback_sendend(void)
{
    /* Start user code. Do not edit comment generated here */
    #ifndef MD_MODBUS
    //R_TAU0_Channel0_Stop();
    disable_irc_send() ;   
    enable_irc_receive(); 
    #else
    if(modbusComps.sw._bit.baud_modified)
    {
       modbus_modify_baud(modbusComps.param_pt->baud);
       modbusComps.sw._bit.baud_modified=0;
    }
    disable_modbus_send();
    enable_modbus_receive();
    #endif
    
    if(loraComps.sw._bit.param_modified)
    {
        WDTE='R';
    }
 /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_uart0_callback_error
* Description  : This function is a callback function when UART0 reception error occurs.
* Arguments    : err_type -
*                    error type value
* Return Value : None
***********************************************************************************************************************/
static void r_uart0_callback_error(uint8_t err_type)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_uart2_interrupt_receive
* Description  : None
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_uart2_interrupt_receive(void)
{
    uint8_t rx_data;
    uint8_t err_type;
    
    err_type = (uint8_t)(SSR11 & 0x0007U);
    SIR11 = (uint16_t)err_type;

    if (err_type != 0U)
    {
        r_uart2_callback_error(err_type);
    }
    
    rx_data = RXD2;
    if(rx_data!='\0')
  {
      netComps.recv_base_pt[*netComps.recv_Idx_pt] = rx_data;
      *netComps.recv_Idx_pt+=1;
      *netComps.recv_Idx_pt&=0x3ff;
      netComps.recv_RxSt=&netComps.recv_base_pt[*netComps.recv_Idx_pt-1];
  }
  else 
  {
      netComps.recv_base_pt[*netComps.recv_Idx_pt] = '\0';
      *netComps.recv_Idx_pt = 0;
  }

//    if (g_uart2_rx_length > g_uart2_rx_count)
//    {
//        *gp_uart2_rx_address = rx_data;
//        gp_uart2_rx_address++;
//        g_uart2_rx_count++;

//        if (g_uart2_rx_length == g_uart2_rx_count)
//        {
//            r_uart2_callback_receiveend();
//        }
//    }
//    else
//    {
//        r_uart2_callback_softwareoverrun(rx_data);
//    }
}
/***********************************************************************************************************************
* Function Name: r_uart2_interrupt_send
* Description  : None
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_uart2_interrupt_send(void)
{
    if (g_uart2_tx_count > 0U)
    {
        TXD2 = *gp_uart2_tx_address;
        gp_uart2_tx_address++;
        g_uart2_tx_count--;
    }
    else
    {
        r_uart2_callback_sendend();
    }
}
/***********************************************************************************************************************
* Function Name: r_uart2_callback_receiveend
* Description  : This function is a callback function when UART2 finishes reception.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void r_uart2_callback_receiveend(void)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_uart2_callback_softwareoverrun
* Description  : This function is a callback function when UART2 receives an overflow data.
* Arguments    : rx_data -
*                    receive data
* Return Value : None
***********************************************************************************************************************/
static void r_uart2_callback_softwareoverrun(uint16_t rx_data)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_uart2_callback_sendend
* Description  : This function is a callback function when UART2 finishes transmission.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void r_uart2_callback_sendend(void)
{
    /* Start user code. Do not edit comment generated here */
//    loraComps.sw._bit.busy=0;
//    if(loraComps.work_st.mode==EM_RUN)
//    {
//	    loraComps.op_window_time=0;
// loraComps.sw._bit.reading_rssi=0;
//   MD_LORA_INTP_ENABLE();
//          
//    }
    /* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_uart2_callback_error
* Description  : This function is a callback function when UART2 reception error occurs.
* Arguments    : err_type -
*                    error type value
* Return Value : None
***********************************************************************************************************************/
static void r_uart2_callback_error(uint8_t err_type)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
void enable_lora(void)
{
    
    R_UART2_Start();
}

void disable_lora(void)
{
    R_UART2_Stop();
}


void disable_uart0_send(void)
{
    STMK0 = 1U;    /* disable INTST0 interrupt */
    ST0 |= _0001_SAU_CH0_STOP_TRG_ON;
    SOE0 &= ~_0001_SAU_CH0_OUTPUT_ENABLE;    /* disable UART0 output */
    STIF0 = 0U;
    SO0 |= _0001_SAU_CH0_DATA_OUTPUT_1;    /* output level normal */
}

void disable_irc_send(void)
{
    disable_uart0_send();
    
    R_TAU0_Channel0_Stop();
   TO0 &= ~_0010_TAU_CH4_OUTPUT_VALUE_1;

}

void disable_modbus_send(void)
{
    NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
    MD_RESET_RS_485_T_R;
    disable_uart0_send();
    modbusComps.sw._bit.busy=0;
}




void enable_uart0_send(void)
{
    SO0 |= _0001_SAU_CH0_DATA_OUTPUT_1;    /* output level normal */
    SOE0 |= _0001_SAU_CH0_OUTPUT_ENABLE;    /* enable UART0 output */
    SS0 |= _0001_SAU_CH0_START_TRG_ON;    /* enable UART0 receive and transmit */
    STIF0 = 0U;    /* clear INTST0 interrupt flag */
    STMK0 = 0U;    /* enable INTST0 interrupt */
}
void enable_irc_send(void)
{
    enable_uart0_send();
}

 void enable_modbus_send(void)
 {
     MD_SET_RS_485_T_R;
     NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();
     enable_uart0_send();
 }




 void disable_uart0_receive(void)
 {
    SRMK0 = 1U;    /* disable INTSR0 interrupt */
    ST0 |= _0002_SAU_CH1_STOP_TRG_ON ;    /* disable UART0 receive and transmit */
    SRIF0 = 0U;    /* clear INTSR0 interrupt flag */
 }
void disable_irc_receive(void)
{
    disable_uart0_receive();
}

void disable_modbus_receive(void)
{
    MD_SET_RS_485_T_R;
    disable_uart0_receive();
}



void enable_uart0_receive(void)
{
    SS0 |= _0002_SAU_CH1_START_TRG_ON ;    /* enable UART0 receive and transmit */
    SRIF0 = 0U;    /* clear INTSR0 interrupt flag */
    SRMK0 = 0U;    /* enable INTSR0 interrupt */
}
void enable_irc_receive(void)
{
    enable_uart0_receive();
}
void enable_modbus_receive(void)
{
    MD_RESET_RS_485_T_R;
    enable_uart0_receive();
}

int modbus_modify_baud(int baud)
{
    if(baud==0||baud>4)
    {
        return 0;
    }
    disable_modbus_receive();
    disable_modbus_send();
   // NOP();
    if(baud==1)//2400
   {
         SPS0 = _0004_SAU_CK00_FCLK_4 | _0040_SAU_CK01_FCLK_4;
   }
   else if(baud==2)//4800
   {
         SPS0 = _0003_SAU_CK00_FCLK_3 | _0030_SAU_CK01_FCLK_3;
   }
   else if(baud==3)//9600
   {
         SPS0 = _0002_SAU_CK00_FCLK_2 | _0020_SAU_CK01_FCLK_2;
   }
   else if(baud==4)//19200
   {
         SPS0 = _0001_SAU_CK00_FCLK_1 | _0010_SAU_CK01_FCLK_1;
   }
    SCR00 = _8000_SAU_TRANSMISSION | _0000_SAU_INTSRE_MASK | _0000_SAU_PARITY_NONE | _0080_SAU_LSB | 
            _0010_SAU_STOP_1 | _0007_SAU_LENGTH_8;
    SCR01 = _4000_SAU_RECEPTION | _0000_SAU_INTSRE_MASK | _0000_SAU_PARITY_NONE | _0080_SAU_LSB | _0010_SAU_STOP_1 | 
            _0007_SAU_LENGTH_8;
}


/*
 uint8_t rx_data;
    uint8_t err_type;
    
    err_type = (uint8_t)(SSR11 & 0x0007U);
    SIR11 = (uint16_t)err_type;

    if (err_type != 0U)
    {
        r_uart2_callback_error(err_type);
    }
    
    rx_data = RXD2;

	loraComps.recv_base_pt[*loraComps.recv_pos_pt]=rx_data;
	*loraComps.recv_pos_pt+=1;
	*loraComps.recv_pos_pt&=0x3f;


	
	
  uint8_t rx_data;
    uint8_t err_type;
    
    err_type = (uint8_t)(SSR01 & 0x0007U);
    SIR01 = (uint16_t)err_type;

    if (err_type != 0U)
    {
        r_uart0_callback_error(err_type);
    }
    
    rx_data = RXD0;

   
	  ircComps.recv_base_pt[*ircComps.recv_pos_pt]=rx_data;
	  *ircComps.recv_pos_pt+=1;
	  *ircComps.recv_pos_pt&=0x7f;
  
	  
	  
	  
uint8_t rx_data;
    uint8_t err_type;
    
    err_type = (uint8_t)(SSR11 & 0x0007U);
    SIR11 = (uint16_t)err_type;

    if (err_type != 0U)
    {
        r_uart2_callback_error(err_type);
    }
    
    rx_data = RXD2;
    if(rx_data!='\0')
  {
      netComps.recv_base_pt[*netComps.recv_Idx_pt] = rx_data;
      *netComps.recv_Idx_pt+=1;
      *netComps.recv_Idx_pt&=0x3ff;
      netComps.recv_RxSt=&netComps.recv_base_pt[*netComps.recv_Idx_pt-1];
  }
  else 
  {
      netComps.recv_base_pt[*netComps.recv_Idx_pt] = '\0';
      *netComps.recv_Idx_pt = 0;
  }


*/

/* End user code. Do not edit comment generated here */
