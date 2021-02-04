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
* File Name    : r_cg_cgc_user.c
* Version      : Applilet4 for R7F0C003/004 V1.00.00.01 [01 Mar 2016]
* Device(s)    : R7F0C004M
* Tool-Chain   : CCRL
* Description  : This file implements device driver for CGC module.
* Creation Date: 2020/12/30 星期�?
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
/* Start user code for include. Do not edit comment generated here */
#include "system.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_CGC_Get_ResetSource
* Description  : This function process of Reset.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_CGC_Get_ResetSource(void)
{
    uint8_t reset_flag = RESF;
    uint8_t porsr_flag = PORSR;
    /* Start user code. Do not edit comment generated here */
    if(reset_flag&0x01)
	{
	   systemComps.rst_code=1;
	}
	else if(reset_flag&0x02)
	{
        systemComps.rst_code=2;
	}
	else if(reset_flag&0x04)
	{
        systemComps.rst_code=3;
	}
	else if(reset_flag&0x10)
	{
        systemComps.rst_code=4;
	}
	else if(reset_flag&0x80)
	{
        systemComps.rst_code=5;
	}
	else if(!(porsr_flag&0x01))
	{
		systemComps.rst_code=0;	
	}
	else
	{
        systemComps.rst_code=0x0e;
	}
    	
  
    
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
