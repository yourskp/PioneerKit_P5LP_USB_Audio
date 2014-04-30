/*******************************************************************************
* File Name: `$INSTANCE_NAME`.h
* Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`
*
* Description:
*  This file provides constants and parameter values and API definition for the 
*  I2S Component
*
* Note:
*
********************************************************************************
* Copyright 2008-2011, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
********************************************************************************/

#if !defined(`$INSTANCE_NAME`_H)
#define `$INSTANCE_NAME`_H

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h"

void  `$INSTANCE_NAME`_Start(void);
void  `$INSTANCE_NAME`_Stop(void)                       `=ReentrantKeil($INSTANCE_NAME . "_Stop")`;


/***************************************
*             Registers                 
***************************************/

#define `$INSTANCE_NAME`_CONTROL_REG   			        (* (reg8 *) `$INSTANCE_NAME`_ControlReg__CONTROL_REG)
#define `$INSTANCE_NAME`_CONTROL_PTR   			        (  (reg8 *) `$INSTANCE_NAME`_ControlReg__CONTROL_REG)

#define `$INSTANCE_NAME`_AUX_CONTROL_REG                (* (reg8 *) `$INSTANCE_NAME`_dp_ByteSwap_u0__DP_AUX_CTL_REG)
#define `$INSTANCE_NAME`_AUX_CONTROL_PTR                (  (reg8 *) `$INSTANCE_NAME`_dp_ByteSwap_u0__DP_AUX_CTL_REG)

#define `$INSTANCE_NAME`_INPUT_FIFO_0_CLR               (0x01u)
#define `$INSTANCE_NAME`_OUTPUT_FIFO_1_CLR              (0x02u) 
#define `$INSTANCE_NAME`_INPUT_FIFO_LEVEL_HALF_EMPTY    (0x0Cu)

#define `$INSTANCE_NAME`_EN                             (0x01u)
#define `$INSTANCE_NAME`_RES_CTRL_16                    (0x02u)


#endif

//[] END OF FILE
