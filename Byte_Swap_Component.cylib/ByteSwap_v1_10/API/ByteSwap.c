/*******************************************************************************
* File Name: `$INSTANCE_NAME`.c  
* Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`
*
* Description:
*  This file contains the setup, control and status commands for the ByteSwap
*  component.  
*
* Note: 
*
*******************************************************************************
* Copyright 2008-2011, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "`$INSTANCE_NAME`.h"  

/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_Start
********************************************************************************
*
* Summary:
*  Starts the Byte swap component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void `$INSTANCE_NAME`_Start(void)
{
	/* Configure Aux control register for FIFO status */
	`$INSTANCE_NAME`_AUX_CONTROL_REG = `$INSTANCE_NAME`_AUX_CONTROL_REG & (~(`$INSTANCE_NAME`_INPUT_FIFO_0_CLR | 
	                                                                        `$INSTANCE_NAME`_OUTPUT_FIFO_1_CLR) );
																			
	`$INSTANCE_NAME`_AUX_CONTROL_REG = `$INSTANCE_NAME`_AUX_CONTROL_REG | `$INSTANCE_NAME`_INPUT_FIFO_LEVEL_HALF_EMPTY;
	
    /* Set Control register enable flag  */
	`$INSTANCE_NAME`_CONTROL_REG = `$INSTANCE_NAME`_CONTROL_REG | `$INSTANCE_NAME`_EN;
        
}

/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_Stop
********************************************************************************
*
* Summary:
*  Stops the Byte swap component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void `$INSTANCE_NAME`_Stop(void)
{
	/* Clear Aux control FIFO status */
	`$INSTANCE_NAME`_AUX_CONTROL_REG = `$INSTANCE_NAME`_AUX_CONTROL_REG | (`$INSTANCE_NAME`_INPUT_FIFO_0_CLR | 
	                                                                        `$INSTANCE_NAME`_OUTPUT_FIFO_1_CLR);
	
    /* Clears the Control register enable flag  */
	`$INSTANCE_NAME`_CONTROL_REG = `$INSTANCE_NAME`_CONTROL_REG & (~ `$INSTANCE_NAME`_EN);
}


/* [] END OF FILE */
