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
* File Name    : r_cg_port.c
* Version      : Applilet4 for R7F0C003/004 V1.00.00.01 [01 Mar 2016]
* Device(s)    : R7F0C004M
* Tool-Chain   : CCRL
* Description  : This file implements device driver for Port module.
* Creation Date: 2021/6/18 星期五
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_port.h"
/* Start user code for include. Do not edit comment generated here */
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
* Function Name: R_PORT_Create
* Description  : This function initializes the Port I/O.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_PORT_Create(void)
{
    PFSEG0 = _00_PFSEG07_PORT | _00_PFSEG06_PORT | _20_PFSEG05_SEG | _10_PFSEG04_SEG;
    PFSEG1 = _80_PFSEG15_SEG | _40_PFSEG14_SEG | _20_PFSEG13_SEG | _10_PFSEG12_SEG | _08_PFSEG11_SEG | 
             _04_PFSEG10_SEG | _02_PFSEG09_SEG | _01_PFSEG08_SEG;
    PFSEG2 = _00_PFSEG23_PORT | _00_PFSEG22_PORT | _00_PFSEG21_PORT | _10_PFSEG20_SEG | _08_PFSEG19_SEG | 
             _04_PFSEG18_SEG | _02_PFSEG17_SEG | _01_PFSEG16_SEG;
    PFSEG3 = _80_PFSEG30_SEG | _40_PFSEG29_SEG | _00_PFSEG28_PORT | _00_PFSEG27_PORT | _00_PFSEG26_PORT | 
             _00_PFSEG25_PORT | _00_PFSEG24_PORT | _00_PFDEG_PORT;
    PFSEG4 = _00_PFSEG38_PORT | _00_PFSEG37_PORT | _00_PFSEG36_PORT | _00_PFSEG35_PORT | _00_PFSEG34_PORT | 
             _00_PFSEG33_PORT | _00_PFSEG32_PORT | _00_PFSEG31_PORT;
    PFSEG5 = _80_PFSEG46_SEG | _40_PFSEG45_SEG | _00_PFSEG44_PORT | _10_PFSEG43_SEG | _08_PFSEG42_SEG | 
             _00_PFSEG41_PORT | _00_PFSEG40_PORT | _01_PFSEG39_SEG;
    PFSEG6 = _00_PFSEG50_PORT | _00_PFSEG49_PORT | _00_PFSEG48_PORT | _01_PFSEG47_SEG;
    P0 = _00_Pn1_OUTPUT_0 | _00_Pn5_OUTPUT_0;
    P1 = _00_Pn0_OUTPUT_0 | _00_Pn1_OUTPUT_0 | _00_Pn2_OUTPUT_0 | _00_Pn3_OUTPUT_0 | _00_Pn5_OUTPUT_0 | 
         _00_Pn6_OUTPUT_0;
    P2 = _10_Pn4_OUTPUT_1 | _00_Pn5_OUTPUT_0 | _00_Pn6_OUTPUT_0 | _00_Pn7_OUTPUT_0;
    P3 = _00_Pn1_OUTPUT_0 | _00_Pn2_OUTPUT_0 | _00_Pn3_OUTPUT_0;
    P4 = _04_Pn2_OUTPUT_1 | _08_Pn3_OUTPUT_1 | _00_Pn4_OUTPUT_0 | _00_Pn6_OUTPUT_0 | _00_Pn7_OUTPUT_0;
    P6 = _00_Pn0_OUTPUT_0;
    P13 = _00_Pn0_OUTPUT_0;
    PU0 = _40_PUn6_PULLUP_ON | _80_PUn7_PULLUP_ON;
    PU3 = _10_PUn4_PULLUP_ON | _20_PUn5_PULLUP_ON;
    PU4 = _01_PUn0_PULLUP_ON | _02_PUn1_PULLUP_ON | _20_PUn5_PULLUP_ON;
    PU5 = _04_PUn2_PULLUP_ON | _08_PUn3_PULLUP_ON;
    PMC1 = _00_PMCn0_DI_ON | _00_PMCn1_DI_ON | _00_PMCn2_DI_ON | _00_PMCn3_DI_ON | _F0_PMC1_DEFAULT;
    PMC2 = _04_PMCn2_NOT_USE | _08_PMCn3_NOT_USE | _00_PMCn4_DI_ON | _00_PMCn5_DI_ON | _00_PMCn6_DI_ON | 
           _00_PMCn7_DI_ON | _03_PMC2_DEFAULT;
    PM0 = _01_PMn0_NOT_USE | _00_PMn1_MODE_OUTPUT | _04_PMn2_NOT_USE | _08_PMn3_NOT_USE | _10_PMn4_NOT_USE | 
          _00_PMn5_MODE_OUTPUT | _40_PMn6_NOT_USE | _80_PMn7_NOT_USE;
    PM1 = _00_PMn0_MODE_OUTPUT | _00_PMn1_MODE_OUTPUT | _00_PMn2_MODE_OUTPUT | _00_PMn3_MODE_OUTPUT | 
          _10_PMn4_NOT_USE | _00_PMn5_MODE_OUTPUT | _00_PMn6_MODE_OUTPUT | _80_PMn7_NOT_USE;
    PM2 = _01_PMn0_NOT_USE | _02_PMn1_NOT_USE | _04_PMn2_NOT_USE | _08_PMn3_NOT_USE | _00_PMn4_MODE_OUTPUT | 
          _00_PMn5_MODE_OUTPUT | _00_PMn6_MODE_OUTPUT | _00_PMn7_MODE_OUTPUT;
    PM3 = _01_PMn0_NOT_USE | _00_PMn1_MODE_OUTPUT | _00_PMn2_MODE_OUTPUT | _00_PMn3_MODE_OUTPUT | _10_PMn4_NOT_USE | 
          _20_PMn5_NOT_USE | _C0_PM3_DEFAULT;
    PM4 = _01_PMn0_NOT_USE | _02_PMn1_MODE_INPUT | _00_PMn2_MODE_OUTPUT | _00_PMn3_MODE_OUTPUT | 
          _00_PMn4_MODE_OUTPUT | _20_PMn5_MODE_INPUT | _00_PMn6_MODE_OUTPUT | _00_PMn7_MODE_OUTPUT;
    PM6 = _00_PMn0_MODE_OUTPUT | _02_PMn1_MODE_INPUT | _FC_PM6_DEFAULT;
    PM13 = _00_PMn0_MODE_OUTPUT | _FE_PM13_DEFAULT;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
