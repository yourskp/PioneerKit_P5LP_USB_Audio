/*******************************************************************************
* File Name: `$INSTANCE_NAME`.c
* Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`
*
* Description:
*  This file contains the function definitions associated with the PDM CIC
*  filter component
*
* Note:
*
*******************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "`$INSTANCE_NAME`.h"
#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h"

#include "`$INSTANCE_NAME`_Int_Ctrl_Reg.h"

#if (`$INSTANCE_NAME`_COMB_CTRL_AVAIL)
	#include "`$INSTANCE_NAME`_Comb_Ctrl_Reg.h"
#endif

#if (`$INSTANCE_NAME`_RIGHT_CHANNEL_ENABLED)
	#include "`$INSTANCE_NAME`_DMA_CombD0Update_R_dma.h"
	#include "`$INSTANCE_NAME`_DMA_IntOut_R_dma.h"
#endif

#if (`$INSTANCE_NAME`_LEFT_CHANNEL_ENABLED)
	#include "`$INSTANCE_NAME`_DMA_CombD0Update_L_dma.h"
	#include "`$INSTANCE_NAME`_DMA_IntOut_L_dma.h"
#endif


#if (`$INSTANCE_NAME`_INTERRUPT_ENABLED)
	#include "`$INSTANCE_NAME`_Status_Reg.h"
#endif

#if(`$INSTANCE_NAME`_LEFT_CHANNEL_ENABLED)
	/* Address of registers holding the Integrator output. Note that these registers
	   hold 16-bit values */
	#define `$INSTANCE_NAME`_INT_L_LO16_OUTPUT_REG_PTR  `$INSTANCE_NAME`_Integrator_genblk2_dp32_l_u0__16BIT_F0_REG
	#define `$INSTANCE_NAME`_INT_L_HI16_OUTPUT_REG_PTR  `$INSTANCE_NAME`_Integrator_genblk2_dp32_l_u2__16BIT_F0_REG

	/* Address of registers holding the comb input for first stage. Note that these registers
	   hold 16-bit values */
	#define `$INSTANCE_NAME`_COMB_L_LO16_INPUT_REG_PTR  `$INSTANCE_NAME`_Comb_L_genblk2_dp32_u0__16BIT_A0_REG
	#define `$INSTANCE_NAME`_COMB_L_HI16_INPUT_REG_PTR  `$INSTANCE_NAME`_Comb_L_genblk2_dp32_u2__16BIT_A0_REG
#endif

#if(`$INSTANCE_NAME`_RIGHT_CHANNEL_ENABLED)
	/* Address of registers holding the Integrator output. Note that these registers
	   hold 16-bit values */
	#define `$INSTANCE_NAME`_INT_R_LO16_OUTPUT_REG_PTR  `$INSTANCE_NAME`_Integrator_genblk3_dp32_r_u0__16BIT_F0_REG
	#define `$INSTANCE_NAME`_INT_R_HI16_OUTPUT_REG_PTR  `$INSTANCE_NAME`_Integrator_genblk3_dp32_r_u2__16BIT_F0_REG

	/* Address of registers holding the comb input for first stage. Note that these registers
	   hold 16-bit values */
	#define `$INSTANCE_NAME`_COMB_R_LO16_INPUT_REG_PTR  `$INSTANCE_NAME`_Comb_R_genblk2_dp32_u0__16BIT_A0_REG
	#define `$INSTANCE_NAME`_COMB_R_HI16_INPUT_REG_PTR  `$INSTANCE_NAME`_Comb_R_genblk2_dp32_u2__16BIT_A0_REG
#endif

uint8 `$INSTANCE_NAME`_initVar = 0u;

static `$INSTANCE_NAME`_BACKUP_STRUCT  `$INSTANCE_NAME`_backup =
{
    /* Component disabled by deafult */
    `$INSTANCE_NAME`_DISABLED,

    /* Decimation counter period */
    `$INSTANCE_NAME`_CNT7_PERIOD,

};

/***************************************
*        Function Prototypes
***************************************/

static void `$INSTANCE_NAME`_Integrator_Init(void);
static void `$INSTANCE_NAME`_Comb_Init(void);

#if(`$INSTANCE_NAME`_RIGHT_CHANNEL_ENABLED)
	static void `$INSTANCE_NAME`_IntOutDma_R_Init(void);
	static void `$INSTANCE_NAME`_IntOutDma_R_Start(void);
	static void `$INSTANCE_NAME`_IntOutDma_R_Stop(void);
	static void `$INSTANCE_NAME`_CombD0UpdateDma_R_Init(void);
	static void `$INSTANCE_NAME`_CombD0UpdateDma_R_Start(void);
	static void `$INSTANCE_NAME`_CombD0UpdateDma_R_Stop(void);
#endif

#if(`$INSTANCE_NAME`_LEFT_CHANNEL_ENABLED)
	static void `$INSTANCE_NAME`_IntOutDma_L_Init(void);
	static void `$INSTANCE_NAME`_IntOutDma_L_Start(void);
	static void `$INSTANCE_NAME`_IntOutDma_L_Stop(void);
	static void `$INSTANCE_NAME`_CombD0UpdateDma_L_Init(void);
	static void `$INSTANCE_NAME`_CombD0UpdateDma_L_Start(void);
	static void `$INSTANCE_NAME`_CombD0UpdateDma_L_Stop(void);
#endif

/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_Start
********************************************************************************
*
* Summary:
*  Starts the PDM CIC Filter component
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  `$INSTANCE_NAME`_initVar - is used to indicate initial configuration
*  of this component.  The variable is initialized to zero and set to 1
*  the first time `$INSTANCE_NAME`_Start() is called. This allows for component
*  initialization without re-initialization in all subsequent calls
*  to the `$INSTANCE_NAME`_Start() routine.
*
*
*******************************************************************************/
void `$INSTANCE_NAME`_Start(void)
{
    if(0u == `$INSTANCE_NAME`_initVar)
    {
        /* Initialize DMA's only the first time */
		
		#if(`$INSTANCE_NAME`_RIGHT_CHANNEL_ENABLED)
	        `$INSTANCE_NAME`_IntOutDma_R_Init();
			`$INSTANCE_NAME`_CombD0UpdateDma_R_Init();	
		#endif
		
		#if(`$INSTANCE_NAME`_LEFT_CHANNEL_ENABLED)
	        `$INSTANCE_NAME`_IntOutDma_L_Init();
			`$INSTANCE_NAME`_CombD0UpdateDma_L_Init();
		#endif		

        /* Set init flag */
        `$INSTANCE_NAME`_initVar = 1u;
    }
	
	`$INSTANCE_NAME`_Int_Ctrl_Reg_Control = `$INSTANCE_NAME`_Int_Ctrl_Reg_Control & (~`$INSTANCE_NAME`_INT_ENABLE_MASK);
	
	#if(`$INSTANCE_NAME`_COMB_CTRL_AVAIL)
		`$INSTANCE_NAME`_Comb_Ctrl_Reg_Control = `$INSTANCE_NAME`_Comb_Ctrl_Reg_Control & (~`$INSTANCE_NAME`_COMB_ENABLE_MASK);
	#endif /* (`$INSTANCE_NAME`_COMB_CTRL_AVAIL) */

	/* Initialize the comb and integrator components */
	`$INSTANCE_NAME`_Comb_Init();
	`$INSTANCE_NAME`_Integrator_Init();	
	
	#if(`$INSTANCE_NAME`_LEFT_CHANNEL_ENABLED)
		/*  Start the DMA channels */
		`$INSTANCE_NAME`_IntOutDma_L_Start();
		`$INSTANCE_NAME`_CombD0UpdateDma_L_Start();
	#endif
	
	#if(`$INSTANCE_NAME`_RIGHT_CHANNEL_ENABLED)
		/*  Start the DMA channels */
		`$INSTANCE_NAME`_IntOutDma_R_Start();
		`$INSTANCE_NAME`_CombD0UpdateDma_R_Start();	
	#endif
	
	/* Enable interrupt status generation if required */
	#if (`$INSTANCE_NAME`_INTERRUPT_ENABLED)
		`$INSTANCE_NAME`_ReadInterruptStatus(); /* Clear the status register before enabling interrupt */
		`$INSTANCE_NAME`_EnableInterruptGeneration();
	#endif
	
	#if(`$INSTANCE_NAME`_COMB_CTRL_AVAIL)
		`$INSTANCE_NAME`_Comb_Ctrl_Reg_Control = `$INSTANCE_NAME`_Comb_Ctrl_Reg_Control | `$INSTANCE_NAME`_COMB_ENABLE_MASK;
	#endif /* (`$INSTANCE_NAME`_COMB_CTRL_AVAIL) */
	
	`$INSTANCE_NAME`_Int_Ctrl_Reg_Control = `$INSTANCE_NAME`_Int_Ctrl_Reg_Control | `$INSTANCE_NAME`_INT_ENABLE_MASK;

}

/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_Stop
********************************************************************************
*
* Summary:
*  Stops the PDM CIC Filter component
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  None.
*
*
*******************************************************************************/
void `$INSTANCE_NAME`_Stop(void)
{	
	#if(`$INSTANCE_NAME`_LEFT_CHANNEL_ENABLED)
		/*  Stop the DMA channels */
		`$INSTANCE_NAME`_IntOutDma_L_Stop();
		`$INSTANCE_NAME`_CombD0UpdateDma_L_Stop();
	#endif
	
	#if(`$INSTANCE_NAME`_RIGHT_CHANNEL_ENABLED)
		/*  Stop the DMA channels */
		`$INSTANCE_NAME`_IntOutDma_R_Stop();
		`$INSTANCE_NAME`_CombD0UpdateDma_R_Stop();	
	#endif	
	
	#if(`$INSTANCE_NAME`_COMB_CTRL_AVAIL)
		`$INSTANCE_NAME`_Comb_Ctrl_Reg_Control = `$INSTANCE_NAME`_Comb_Ctrl_Reg_Control & (~`$INSTANCE_NAME`_COMB_ENABLE_MASK);
	#endif /* (`$INSTANCE_NAME`_COMB_CTRL_AVAIL) */	
	
	`$INSTANCE_NAME`_Int_Ctrl_Reg_Control = `$INSTANCE_NAME`_Int_Ctrl_Reg_Control & (~`$INSTANCE_NAME`_INT_ENABLE_MASK);
	
	/* Disable interrupt status generation if required */
	#if (`$INSTANCE_NAME`_INTERRUPT_ENABLED)
		`$INSTANCE_NAME`_DisableInterruptGeneration();
		`$INSTANCE_NAME`_ReadInterruptStatus(); /* Clear the status register after disabling interrupt */
	#endif
}

/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_SaveConfig
********************************************************************************
*
* Summary:
*  Saves PDM CIC filter configuration.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  `$INSTANCE_NAME`_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void `$INSTANCE_NAME`_SaveConfig(void)
{
	`$INSTANCE_NAME`_backup.Cnt7Period = `$INSTANCE_NAME`_CNT7_PERIOD_REG;
}

/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores PDM CIC filter configuration.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  `$INSTANCE_NAME`_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void `$INSTANCE_NAME`_RestoreConfig(void)
{
	`$INSTANCE_NAME`_CNT7_PERIOD_REG = `$INSTANCE_NAME`_backup.Cnt7Period;
}

/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_Sleep
********************************************************************************
*
* Summary:
*  Prepares PDM CIC component to sleep.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  `$INSTANCE_NAME`_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void `$INSTANCE_NAME`_Sleep(void)
{
    /* Get component state */
    `$INSTANCE_NAME`_backup.enableState = ((uint8) `$INSTANCE_NAME`_IS_ENABLE);

    /* Save registers configuration */
    `$INSTANCE_NAME`_SaveConfig();

    /* Stop component */
    `$INSTANCE_NAME`_Stop();
}

/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_Wakeup
********************************************************************************
*
* Summary:
*  Prepares PDM CIC component for wakeup.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  `$INSTANCE_NAME`_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
/* Restores the component after low power mode exit */
void `$INSTANCE_NAME`_Wakeup(void)
{
    /* Restore registers values */
    `$INSTANCE_NAME`_RestoreConfig();

    if(0u != `$INSTANCE_NAME`_backup.enableState)
    {
    	/* Start component */
    	`$INSTANCE_NAME`_Start();
    }
}

/* Conditional compilation of Interrupt related API's */
#if (`$INSTANCE_NAME`_INTERRUPT_ENABLED)

	/*******************************************************************************
	* Function Name: `$INSTANCE_NAME`_ReadInterruptStatus
	********************************************************************************
	*
	* Summary:
	*  Returns the current value of the PDM CIC status register
	*
	* Parameters:
	*  None.
	*
	* Return:
	*  uint8 - Status register value
	*
	* Global Variables:
	*  None.
	*
	*
	*******************************************************************************/
	uint8 `$INSTANCE_NAME`_ReadInterruptStatus()
	{
		return(`$INSTANCE_NAME`_Status_Reg_Read());
	}

	/*******************************************************************************
	* Function Name: `$INSTANCE_NAME`_EnableInterruptGeneration
	********************************************************************************
	*
	* Summary:
	*  Enables interrupt signal generation by the status register
	*
	* Parameters:
	*  None.
	*
	* Return:
	*  None.
	*
	* Global Variables:
	*  None.
	*
	*
	*******************************************************************************/
	void  `$INSTANCE_NAME`_EnableInterruptGeneration()
	{
		`$INSTANCE_NAME`_Status_Reg_InterruptEnable();
	}

	/*******************************************************************************
	* Function Name: `$INSTANCE_NAME`_DisableInterruptGeneration
	********************************************************************************
	*
	* Summary:
	*  Disables interrupt signal generation by the status register
	*
	* Parameters:
	*  None.
	*
	* Return:
	*  None.
	*
	* Global Variables:
	*  None.
	*
	*
	*******************************************************************************/
	void  `$INSTANCE_NAME`_DisableInterruptGeneration()
	{
		`$INSTANCE_NAME`_Status_Reg_InterruptDisable();
	}

#endif /* (`$INSTANCE_NAME`_INTERRUPT_ENABLED) */

/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_Integrator_Init
********************************************************************************
*
* Summary:
*  Initializes the integrator
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  None.
*
*
*******************************************************************************/
static void `$INSTANCE_NAME`_Integrator_Init(void)
{
	uint8 enableInterrupts;	
	
	enableInterrupts = CyEnterCriticalSection();
	
	#if(`$INSTANCE_NAME`_LEFT_CHANNEL_ENABLED)	
		/* Reset the F0, F1 FIFO buffer pointers and status signals */
		CY_SET_REG8(`$INSTANCE_NAME`_Integrator_genblk2_dp32_l_u0__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_Integrator_genblk2_dp32_l_u0__DP_AUX_CTL_REG) | 0x03 );
		CY_SET_REG8(`$INSTANCE_NAME`_Integrator_genblk2_dp32_l_u1__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_Integrator_genblk2_dp32_l_u1__DP_AUX_CTL_REG) | 0x03 );
		CY_SET_REG8(`$INSTANCE_NAME`_Integrator_genblk2_dp32_l_u2__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_Integrator_genblk2_dp32_l_u2__DP_AUX_CTL_REG) | 0x03 );	
		CY_SET_REG8(`$INSTANCE_NAME`_Integrator_genblk2_dp32_l_u3__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_Integrator_genblk2_dp32_l_u3__DP_AUX_CTL_REG) | 0x03 );	
		
		CY_SET_REG8(`$INSTANCE_NAME`_Integrator_genblk2_dp32_l_u0__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_Integrator_genblk2_dp32_l_u0__DP_AUX_CTL_REG) & 0xFC );
		CY_SET_REG8(`$INSTANCE_NAME`_Integrator_genblk2_dp32_l_u1__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_Integrator_genblk2_dp32_l_u1__DP_AUX_CTL_REG) & 0xFC );
		CY_SET_REG8(`$INSTANCE_NAME`_Integrator_genblk2_dp32_l_u2__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_Integrator_genblk2_dp32_l_u2__DP_AUX_CTL_REG) & 0xFC );
		CY_SET_REG8(`$INSTANCE_NAME`_Integrator_genblk2_dp32_l_u3__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_Integrator_genblk2_dp32_l_u3__DP_AUX_CTL_REG) & 0xFC );	
	#endif /* (`$INSTANCE_NAME`_LEFT_CHANNEL_ENABLED) */
	
	#if(`$INSTANCE_NAME`_RIGHT_CHANNEL_ENABLED)
		/* Reset the F0, F1 FIFO buffer pointers and status signals */
		CY_SET_REG8(`$INSTANCE_NAME`_Integrator_genblk3_dp32_r_u0__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_Integrator_genblk3_dp32_r_u0__DP_AUX_CTL_REG) | 0x03 );
		CY_SET_REG8(`$INSTANCE_NAME`_Integrator_genblk3_dp32_r_u1__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_Integrator_genblk3_dp32_r_u1__DP_AUX_CTL_REG) | 0x03 );
		CY_SET_REG8(`$INSTANCE_NAME`_Integrator_genblk3_dp32_r_u2__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_Integrator_genblk3_dp32_r_u2__DP_AUX_CTL_REG) | 0x03 );	
		CY_SET_REG8(`$INSTANCE_NAME`_Integrator_genblk3_dp32_r_u3__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_Integrator_genblk3_dp32_r_u3__DP_AUX_CTL_REG) | 0x03 );	
		
		CY_SET_REG8(`$INSTANCE_NAME`_Integrator_genblk3_dp32_r_u0__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_Integrator_genblk3_dp32_r_u0__DP_AUX_CTL_REG) & 0xFC );
		CY_SET_REG8(`$INSTANCE_NAME`_Integrator_genblk3_dp32_r_u1__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_Integrator_genblk3_dp32_r_u1__DP_AUX_CTL_REG) & 0xFC );
		CY_SET_REG8(`$INSTANCE_NAME`_Integrator_genblk3_dp32_r_u2__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_Integrator_genblk3_dp32_r_u2__DP_AUX_CTL_REG) & 0xFC );
		CY_SET_REG8(`$INSTANCE_NAME`_Integrator_genblk3_dp32_r_u3__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_Integrator_genblk3_dp32_r_u3__DP_AUX_CTL_REG) & 0xFC );	
	#endif /* (`$INSTANCE_NAME`_RIGHT_CHANNEL_ENABLED) */
	
	/* Enable the decimator counter module */
	CY_SET_REG8(`$INSTANCE_NAME`_Integrator_Counter7__CONTROL_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_Integrator_Counter7__CONTROL_AUX_CTL_REG) | 0x20);
	
	CyExitCriticalSection(enableInterrupts);	
	
}

/*******************************************************************************
* Function Name: `$INSTANCE_NAME`_Comb_Init
********************************************************************************
*
* Summary:
*  Initializes the comb
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  None.
*
*
*******************************************************************************/
static void `$INSTANCE_NAME`_Comb_Init(void)
{
	uint8 enableInterrupts;	
	
	enableInterrupts = CyEnterCriticalSection();
	
	#if(`$INSTANCE_NAME`_LEFT_CHANNEL_ENABLED)	
		/* Reset the F0, F1 FIFO buffer pointers and status signals */
		CY_SET_REG8(`$INSTANCE_NAME`_Comb_L_genblk2_dp32_u0__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_Comb_L_genblk2_dp32_u0__DP_AUX_CTL_REG) | 0x03 );
		CY_SET_REG8(`$INSTANCE_NAME`_Comb_L_genblk2_dp32_u1__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_Comb_L_genblk2_dp32_u1__DP_AUX_CTL_REG) | 0x03 );
		CY_SET_REG8(`$INSTANCE_NAME`_Comb_L_genblk2_dp32_u2__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_Comb_L_genblk2_dp32_u2__DP_AUX_CTL_REG) | 0x03 );	
		CY_SET_REG8(`$INSTANCE_NAME`_Comb_L_genblk2_dp32_u3__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_Comb_L_genblk2_dp32_u3__DP_AUX_CTL_REG) | 0x03 );	
		
		CY_SET_REG8(`$INSTANCE_NAME`_Comb_L_genblk2_dp32_u0__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_Comb_L_genblk2_dp32_u0__DP_AUX_CTL_REG) & 0xFC );
		CY_SET_REG8(`$INSTANCE_NAME`_Comb_L_genblk2_dp32_u1__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_Comb_L_genblk2_dp32_u1__DP_AUX_CTL_REG) & 0xFC );
		CY_SET_REG8(`$INSTANCE_NAME`_Comb_L_genblk2_dp32_u2__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_Comb_L_genblk2_dp32_u2__DP_AUX_CTL_REG) & 0xFC );
		CY_SET_REG8(`$INSTANCE_NAME`_Comb_L_genblk2_dp32_u3__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_Comb_L_genblk2_dp32_u3__DP_AUX_CTL_REG) & 0xFC );
	#endif /* (`$INSTANCE_NAME`_LEFT_CHANNEL_ENABLED) */
	
	#if(`$INSTANCE_NAME`_RIGHT_CHANNEL_ENABLED)	
		/* Reset the F0, F1 FIFO buffer pointers and status signals */
		CY_SET_REG8(`$INSTANCE_NAME`_Comb_R_genblk2_dp32_u0__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_Comb_R_genblk2_dp32_u0__DP_AUX_CTL_REG) | 0x03 );
		CY_SET_REG8(`$INSTANCE_NAME`_Comb_R_genblk2_dp32_u1__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_Comb_R_genblk2_dp32_u1__DP_AUX_CTL_REG) | 0x03 );
		CY_SET_REG8(`$INSTANCE_NAME`_Comb_R_genblk2_dp32_u2__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_Comb_R_genblk2_dp32_u2__DP_AUX_CTL_REG) | 0x03 );	
		CY_SET_REG8(`$INSTANCE_NAME`_Comb_R_genblk2_dp32_u3__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_Comb_R_genblk2_dp32_u3__DP_AUX_CTL_REG) | 0x03 );	
		
		CY_SET_REG8(`$INSTANCE_NAME`_Comb_R_genblk2_dp32_u0__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_Comb_R_genblk2_dp32_u0__DP_AUX_CTL_REG) & 0xFC );
		CY_SET_REG8(`$INSTANCE_NAME`_Comb_R_genblk2_dp32_u1__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_Comb_R_genblk2_dp32_u1__DP_AUX_CTL_REG) & 0xFC );
		CY_SET_REG8(`$INSTANCE_NAME`_Comb_R_genblk2_dp32_u2__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_Comb_R_genblk2_dp32_u2__DP_AUX_CTL_REG) & 0xFC );
		CY_SET_REG8(`$INSTANCE_NAME`_Comb_R_genblk2_dp32_u3__DP_AUX_CTL_REG, CY_GET_REG8(`$INSTANCE_NAME`_Comb_R_genblk2_dp32_u3__DP_AUX_CTL_REG) & 0xFC );		
	#endif /* (`$INSTANCE_NAME`_RIGHT_CHANNEL_ENABLED) */
	
	CyExitCriticalSection(enableInterrupts);
	
	#if(`$INSTANCE_NAME`_LEFT_CHANNEL_ENABLED)	
		/* Write 0 to the 32-bit D0 register */
		CY_SET_REG8(`$INSTANCE_NAME`_Comb_L_genblk2_dp32_u0__D0_REG, 0x00);
		CY_SET_REG8(`$INSTANCE_NAME`_Comb_L_genblk2_dp32_u1__D0_REG, 0x00);
		CY_SET_REG8(`$INSTANCE_NAME`_Comb_L_genblk2_dp32_u2__D0_REG, 0x00);
		CY_SET_REG8(`$INSTANCE_NAME`_Comb_L_genblk2_dp32_u3__D0_REG, 0x00);	
	#endif /* (`$INSTANCE_NAME`_LEFT_CHANNEL_ENABLED) */
	
	#if(`$INSTANCE_NAME`_RIGHT_CHANNEL_ENABLED)
		/* Write 0 to the 32-bit D0 register */
		CY_SET_REG8(`$INSTANCE_NAME`_Comb_R_genblk2_dp32_u0__D0_REG, 0x00);
		CY_SET_REG8(`$INSTANCE_NAME`_Comb_R_genblk2_dp32_u1__D0_REG, 0x00);
		CY_SET_REG8(`$INSTANCE_NAME`_Comb_R_genblk2_dp32_u2__D0_REG, 0x00);
		CY_SET_REG8(`$INSTANCE_NAME`_Comb_R_genblk2_dp32_u3__D0_REG, 0x00);	
	#endif /* (`$INSTANCE_NAME`_RIGHT_CHANNEL_ENABLED) */

}

#if (`$INSTANCE_NAME`_RIGHT_CHANNEL_ENABLED)

	/*******************************************************************************
	* Function Name: `$INSTANCE_NAME`_IntOutDma_R_Init
	********************************************************************************
	*
	* Summary:
	*  Initializes the DMA channel associated with the right channel integrator output
	*
	* Parameters:
	*  None.
	*
	* Return:
	*  None.
	*
	* Global Variables:
	*  None.
	*
	*
	*******************************************************************************/
	static void `$INSTANCE_NAME`_IntOutDma_R_Init(void)
	{
		uint8 DMA_IntegratorOut_Channel, DMA_IntegratorOut_TD[2];

		/* Transfer data in 2-byte bursts since UDB spoke is 2 bytes wide.
		   requestPerBurst = 1.
		   Peripheral (UDB) to Peripheral (UDB) transfer */
		DMA_IntegratorOut_Channel = `$INSTANCE_NAME`_DMA_IntOut_R_DmaInitialize(2, 1, HI16(CYDEV_PERIPH_BASE), HI16(CYDEV_PERIPH_BASE));	
		
		DMA_IntegratorOut_TD[0] = CyDmaTdAllocate();
		DMA_IntegratorOut_TD[1] = CyDmaTdAllocate();
		
		/* Circular TD's. Each TD transfer 2 bytes. Auto execute second TD. */
		CyDmaTdSetConfiguration(DMA_IntegratorOut_TD[0], 2, DMA_IntegratorOut_TD[1], TD_AUTO_EXEC_NEXT); 
		CyDmaTdSetConfiguration(DMA_IntegratorOut_TD[1], 2, DMA_IntegratorOut_TD[0], `$INSTANCE_NAME`_DMA_IntOut_R__TD_TERMOUT_EN);
		
		/* Each TD transfers 16-bit FIFO value to 16-bit A0. Order TD's such that MSB datapath FIFO is read last whose status triggers the DMA */
		CyDmaTdSetAddress(DMA_IntegratorOut_TD[0], (uint16)(`$INSTANCE_NAME`_INT_R_LO16_OUTPUT_REG_PTR), (uint16)(`$INSTANCE_NAME`_COMB_R_LO16_INPUT_REG_PTR));
		CyDmaTdSetAddress(DMA_IntegratorOut_TD[1], (uint16)(`$INSTANCE_NAME`_INT_R_HI16_OUTPUT_REG_PTR), (uint16)(`$INSTANCE_NAME`_COMB_R_HI16_INPUT_REG_PTR));
		
		/* Set the initial TD for the channel */
		CyDmaChSetInitialTd(DMA_IntegratorOut_Channel, DMA_IntegratorOut_TD[0]);
	}

	/*******************************************************************************
	* Function Name: `$INSTANCE_NAME`_IntOutDma_R_Start
	********************************************************************************
	*
	* Summary:
	*  Starts the DMA channel associated with the right channel integrator output
	*
	* Parameters:
	*  None.
	*
	* Return:
	*  None.
	*
	* Global Variables:
	*  None.
	*
	*
	*******************************************************************************/
	static void `$INSTANCE_NAME`_IntOutDma_R_Start(void)
	{	
		CyDmaClearPendingDrq(`$INSTANCE_NAME`_DMA_IntOut_R_DmaHandle);
		
		CyDmaChEnable(`$INSTANCE_NAME`_DMA_IntOut_R_DmaHandle, 1);
	}

	/*******************************************************************************
	* Function Name: `$INSTANCE_NAME`_IntOutDma_R_Stop
	********************************************************************************
	*
	* Summary:
	*  Stops (and disables) the DMA channel associated with the right channel integrator output
	*
	* Parameters:
	*  None.
	*
	* Return:
	*  None.
	*
	* Global Variables:
	*  None.
	*
	*
	*******************************************************************************/
	static void `$INSTANCE_NAME`_IntOutDma_R_Stop(void)
	{	
		/* Terminate the TD chain which also disables the channel */
		CyDmaChSetRequest(`$INSTANCE_NAME`_DMA_IntOut_R_DmaHandle , CPU_TERM_CHAIN);
	}

	/*******************************************************************************
	* Function Name: `$INSTANCE_NAME`_CombD0UpdateDma_R_Init
	********************************************************************************
	*
	* Summary:
	*  Initializes the DMA channel associated with the Comb D0 register update
	*  for right channel
	*
	* Parameters:
	*  None.
	*
	* Return:
	*  None.
	*
	* Global Variables:
	*  None.
	*
	*
	*******************************************************************************/
	static void `$INSTANCE_NAME`_CombD0UpdateDma_R_Init(void)
	{
		uint8 DMA_CombD0Update_Channel, DMA_CombD0Update_TD[2];

		/* Transfer data in 2-byte bursts since UDB spoke is 2 bytes wide.
			requestPerBurst = 1.
		   Peripheral (UDB) to Peripheral (UDB) transfer */
		DMA_CombD0Update_Channel = `$INSTANCE_NAME`_DMA_CombD0Update_R_DmaInitialize(2, 1, HI16(CYDEV_PERIPH_BASE), HI16(CYDEV_PERIPH_BASE));
		DMA_CombD0Update_TD[0] = CyDmaTdAllocate();
		DMA_CombD0Update_TD[1] = CyDmaTdAllocate();
		
		/* Circular TD's. Each TD transfer 2 bytes. Auto execute second TD. */
		CyDmaTdSetConfiguration(DMA_CombD0Update_TD[0], 2, DMA_CombD0Update_TD[1], TD_AUTO_EXEC_NEXT);
		CyDmaTdSetConfiguration(DMA_CombD0Update_TD[1], 2, DMA_CombD0Update_TD[0], `$INSTANCE_NAME`_DMA_CombD0Update_R__TD_TERMOUT_EN);
		
		/* Each TD transfers 16-bit A1 value to 16-bit D0. */
		CyDmaTdSetAddress(DMA_CombD0Update_TD[0], (uint16)`$INSTANCE_NAME`_Comb_R_genblk2_dp32_u0__16BIT_A1_REG, (uint16)`$INSTANCE_NAME`_Comb_R_genblk2_dp32_u0__16BIT_D0_REG);
		CyDmaTdSetAddress(DMA_CombD0Update_TD[1], (uint16)`$INSTANCE_NAME`_Comb_R_genblk2_dp32_u2__16BIT_A1_REG, (uint16)`$INSTANCE_NAME`_Comb_R_genblk2_dp32_u2__16BIT_D0_REG);
		
		/* Set the initial TD for the channel */
		CyDmaChSetInitialTd(DMA_CombD0Update_Channel, DMA_CombD0Update_TD[0]);
	}

	/*******************************************************************************
	* Function Name: `$INSTANCE_NAME`_CombD0UpdateDma_R_Start
	********************************************************************************
	*
	* Summary:
	*  Starts the DMA channel associated with the Comb D0 register update for right
	*  channel
	*
	* Parameters:
	*  None.
	*
	* Return:
	*  None.
	*
	* Global Variables:
	*  None.
	*
	*
	*******************************************************************************/
	static void `$INSTANCE_NAME`_CombD0UpdateDma_R_Start(void)
	{	
		CyDmaClearPendingDrq(`$INSTANCE_NAME`_DMA_CombD0Update_R_DmaHandle);	
		CyDmaChEnable(`$INSTANCE_NAME`_DMA_CombD0Update_R_DmaHandle, 1);
	}

	/*******************************************************************************
	* Function Name: `$INSTANCE_NAME`_CombD0UpdateDma_R_Stop
	********************************************************************************
	*
	* Summary:
	*  Stops (and disables) the DMA channel associated with the Comb D0 register update
	*  for the right channel
	*
	* Parameters:
	*  None.
	*
	* Return:
	*  None.
	*
	* Global Variables:
	*  None.
	*
	*
	*******************************************************************************/
	static void `$INSTANCE_NAME`_CombD0UpdateDma_R_Stop(void)
	{	
		/* Terminate the TD chain which also disables the channel */
		CyDmaChSetRequest(`$INSTANCE_NAME`_DMA_CombD0Update_R_DmaHandle , CPU_TERM_CHAIN);
	}
#endif /* (`$INSTANCE_NAME`_RIGHT_CHANNEL_ENABLED) */

#if (`$INSTANCE_NAME`_LEFT_CHANNEL_ENABLED)
	/*******************************************************************************
	* Function Name: `$INSTANCE_NAME`_IntOutDma_L_Init
	********************************************************************************
	*
	* Summary:
	*  Initializes the DMA channel associated with the left channel integrator output
	*
	* Parameters:
	*  None.
	*
	* Return:
	*  None.
	*
	* Global Variables:
	*  None.
	*
	*
	*******************************************************************************/
	static void `$INSTANCE_NAME`_IntOutDma_L_Init(void)
	{
		uint8 DMA_IntegratorOut_Channel, DMA_IntegratorOut_TD[2];

		/* Transfer data in 2-byte bursts since UDB spoke is 2 bytes wide.
		   requestPerBurst = 1.
		   Peripheral (UDB) to Peripheral (UDB) transfer */
		DMA_IntegratorOut_Channel = `$INSTANCE_NAME`_DMA_IntOut_L_DmaInitialize(2, 1, HI16(CYDEV_PERIPH_BASE), HI16(CYDEV_PERIPH_BASE));	
		
		DMA_IntegratorOut_TD[0] = CyDmaTdAllocate();
		DMA_IntegratorOut_TD[1] = CyDmaTdAllocate();
		
		/* Circular TD's. Each TD transfer 2 bytes. Auto execute second TD. */
		CyDmaTdSetConfiguration(DMA_IntegratorOut_TD[0], 2, DMA_IntegratorOut_TD[1], TD_AUTO_EXEC_NEXT); 
		CyDmaTdSetConfiguration(DMA_IntegratorOut_TD[1], 2, DMA_IntegratorOut_TD[0], `$INSTANCE_NAME`_DMA_IntOut_L__TD_TERMOUT_EN);
		
		/* Each TD transfers 16-bit FIFO value to 16-bit A0. Order TD's such that MSB datapath FIFO is read last whose status triggers the DMA */
		CyDmaTdSetAddress(DMA_IntegratorOut_TD[0], (uint16)(`$INSTANCE_NAME`_INT_L_LO16_OUTPUT_REG_PTR), (uint16)(`$INSTANCE_NAME`_COMB_L_LO16_INPUT_REG_PTR));
		CyDmaTdSetAddress(DMA_IntegratorOut_TD[1], (uint16)(`$INSTANCE_NAME`_INT_L_HI16_OUTPUT_REG_PTR), (uint16)(`$INSTANCE_NAME`_COMB_L_HI16_INPUT_REG_PTR));
		
		/* Set the initial TD for the channel */
		CyDmaChSetInitialTd(DMA_IntegratorOut_Channel, DMA_IntegratorOut_TD[0]);
	}

	/*******************************************************************************
	* Function Name: `$INSTANCE_NAME`_IntOutDma_L_Start
	********************************************************************************
	*
	* Summary:
	*  Starts the DMA channel associated with the left channel integrator output
	*
	* Parameters:
	*  None.
	*
	* Return:
	*  None.
	*
	* Global Variables:
	*  None.
	*
	*
	*******************************************************************************/
	static void `$INSTANCE_NAME`_IntOutDma_L_Start(void)
	{	
		CyDmaClearPendingDrq(`$INSTANCE_NAME`_DMA_IntOut_L_DmaHandle);
		
		CyDmaChEnable(`$INSTANCE_NAME`_DMA_IntOut_L_DmaHandle, 1);
	}

	/*******************************************************************************
	* Function Name: `$INSTANCE_NAME`_IntOutDma_L_Stop
	********************************************************************************
	*
	* Summary:
	*  Stops (and disables) the DMA channel associated with the left channel integrator output
	*
	* Parameters:
	*  None.
	*
	* Return:
	*  None.
	*
	* Global Variables:
	*  None.
	*
	*
	*******************************************************************************/
	static void `$INSTANCE_NAME`_IntOutDma_L_Stop(void)
	{	
		/* Terminate the TD chain which also disables the channel */
		CyDmaChSetRequest(`$INSTANCE_NAME`_DMA_IntOut_L_DmaHandle , CPU_TERM_CHAIN);
	}

	/*******************************************************************************
	* Function Name: `$INSTANCE_NAME`_CombD0UpdateDma_L_Init
	********************************************************************************
	*
	* Summary:
	*  Initializes the DMA channel associated with the Comb D0 register update
	*  for left channel
	*
	* Parameters:
	*  None.
	*
	* Return:
	*  None.
	*
	* Global Variables:
	*  None.
	*
	*
	*******************************************************************************/
	static void `$INSTANCE_NAME`_CombD0UpdateDma_L_Init(void)
	{
		uint8 DMA_CombD0Update_Channel, DMA_CombD0Update_TD[2];

		/* Transfer data in 2-byte bursts since UDB spoke is 2 bytes wide.
			requestPerBurst = 1.
		   Peripheral (UDB) to Peripheral (UDB) transfer */
		DMA_CombD0Update_Channel = `$INSTANCE_NAME`_DMA_CombD0Update_L_DmaInitialize(2, 1, HI16(CYDEV_PERIPH_BASE), HI16(CYDEV_PERIPH_BASE));
		DMA_CombD0Update_TD[0] = CyDmaTdAllocate();
		DMA_CombD0Update_TD[1] = CyDmaTdAllocate();
		
		/* Circular TD's. Each TD transfer 2 bytes. Auto execute second TD. */
		CyDmaTdSetConfiguration(DMA_CombD0Update_TD[0], 2, DMA_CombD0Update_TD[1], TD_AUTO_EXEC_NEXT);
		CyDmaTdSetConfiguration(DMA_CombD0Update_TD[1], 2, DMA_CombD0Update_TD[0], `$INSTANCE_NAME`_DMA_CombD0Update_L__TD_TERMOUT_EN);
		
		/* Each TD transfers 16-bit A1 value to 16-bit D0. */
		CyDmaTdSetAddress(DMA_CombD0Update_TD[0], (uint16)`$INSTANCE_NAME`_Comb_L_genblk2_dp32_u0__16BIT_A1_REG, (uint16)`$INSTANCE_NAME`_Comb_L_genblk2_dp32_u0__16BIT_D0_REG);
		CyDmaTdSetAddress(DMA_CombD0Update_TD[1], (uint16)`$INSTANCE_NAME`_Comb_L_genblk2_dp32_u2__16BIT_A1_REG, (uint16)`$INSTANCE_NAME`_Comb_L_genblk2_dp32_u2__16BIT_D0_REG);
		
		/* Set the initial TD for the channel */
		CyDmaChSetInitialTd(DMA_CombD0Update_Channel, DMA_CombD0Update_TD[0]);
	}

	/*******************************************************************************
	* Function Name: `$INSTANCE_NAME`_CombD0UpdateDma_L_Start
	********************************************************************************
	*
	* Summary:
	*  Starts the DMA channel associated with the Comb D0 register update for left
	*  channel
	*
	* Parameters:
	*  None.
	*
	* Return:
	*  None.
	*
	* Global Variables:
	*  None.
	*
	*
	*******************************************************************************/
	static void `$INSTANCE_NAME`_CombD0UpdateDma_L_Start(void)
	{	
		CyDmaClearPendingDrq(`$INSTANCE_NAME`_DMA_CombD0Update_L_DmaHandle);	
		CyDmaChEnable(`$INSTANCE_NAME`_DMA_CombD0Update_L_DmaHandle, 1);
	}

	/*******************************************************************************
	* Function Name: `$INSTANCE_NAME`_CombD0UpdateDma_L_Stop
	********************************************************************************
	*
	* Summary:
	*  Stops (and disables) the DMA channel associated with the Comb D0 register update
	*  for the left channel
	*
	* Parameters:
	*  None.
	*
	* Return:
	*  None.
	*
	* Global Variables:
	*  None.
	*
	*
	*******************************************************************************/
	static void `$INSTANCE_NAME`_CombD0UpdateDma_L_Stop(void)
	{	
		/* Terminate the TD chain which also disables the channel */
		CyDmaChSetRequest(`$INSTANCE_NAME`_DMA_CombD0Update_L_DmaHandle , CPU_TERM_CHAIN);
	}
#endif /* (`$INSTANCE_NAME`_LEFT_CHANNEL_ENABLED) */


/* [] END OF FILE */
