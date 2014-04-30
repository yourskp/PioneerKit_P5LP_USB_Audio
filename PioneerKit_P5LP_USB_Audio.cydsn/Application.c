/*******************************************************************************
* File Name: Application.c
*
* Version 1.0
*
*  Description: This file contains all the application layer code for
*               user interface handling routines
*
********************************************************************************
* Copyright (2008-2013), Cypress Semiconductor Corporation.
********************************************************************************
* This software is owned by Cypress Semiconductor Corporation (Cypress) and is
* protected by and subject to worldwide patent protection (United States and
* foreign), United States copyright laws and international treaty provisions.
* Cypress hereby grants to licensee a personal, non-exclusive, non-transferable
* license to copy, use, modify, create derivative works of, and compile the
* Cypress Source Code and derivative works for the sole purpose of creating
* custom software in support of licensee product to be used only in conjunction
* with a Cypress integrated circuit as specified in the applicable agreement.
* Any reproduction, modification, translation, compilation, or representation of
* this software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH
* REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* Cypress reserves the right to make changes without further notice to the
* materials described herein. Cypress does not assume any liability arising out
* of the application or use of any product or circuit described herein. Cypress
* does not authorize its products for use as critical components in life-support
* systems where a malfunction or failure may reasonably be expected to result in
* significant injury to the user. The inclusion of Cypress' product in a life-
* support systems application implies that the manufacturer assumes all risk of
* such use and in doing so indemnifies Cypress against all charges. Use may be
* limited by and subject to the applicable Cypress software license agreement.
*******************************************************************************/
#include <Application.h>
#include <AudioControl.h>
#include <Configuration.h>
#include <codec.h>		
#include <device.h>
#include <Interrupts.h>
#include <stdio.h>
#include <USBInterface.h>
#include <VolumeControl.h>

/* function declaration */
static void HandleUserInputs(void);
static void CheckPlayPauseButton(void);

/* Extern declarations */
extern uint8 USBFS_initVar;
extern volatile uint8 USBFS_currentVolume[];
extern volatile uint8 USBFS_currentMute;

extern CYBIT codecInit;
extern CYBIT outPlaying;

/* Global variables used for playlist-control buttons */
uint8 buttonStatus = 0;
uint32 buttonMask;
CYBIT applicationInitiate = TRUE;
uint8 playlistControlReport = 0;
uint8 prevReport = 0;

/* Global variables used for the switch "PSOC_SW", which is used for Play/Pause */
uint8 switchDebounceTimer = 0;

/*******************************************************************************
* Function Name: ConfigureApplication
********************************************************************************
* Summary:
*        This function configures the application layer hardware modules and
*        clocks to be in sync with the audio clock updates done in 
*        ConfigureAudioPath API
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void ConfigureApplication(void)
{
    CYGlobalIntEnable;
	
    /* Configure the I2C clock speed for 400kHz operation */
	#ifdef CODEC_I2C_SUPPORTED
	I2C_Master_CLKDIV1_REG = I2C_DIVIDER_400KHZ_OPERATION;
	#endif
            
    /* Enable 8ms tick sleep ISR */
    SystemTickTimer_Start();
    isr_Tick_StartEx(Tick_Interrupt);
    
    #ifdef ENABLE_WATCHDOG
    /* Watchdog timer is clocked from internal 1KHz ILO, for USB Operation 100KHz ILO is selected in the clock
     * configuration wizard, explicitly enable 1KHz ILO for WDT */
    CyILO_Start1K();
    
    /* Enable Watchdog for recovering from unresponsive system, The time for WDT is set to two 1sec tick events */
    CyWdtStart(CYWDT_1024_TICKS, CYWDT_LPMODE_DISABLED);
    #endif
    
    /* USER_CODE: [Accessory Initialization] Placeholder for initializing additional user added components and 
     * external peripherals. This block of code is called once after accessory reset event and the system clocks 
     * will be in idle mode (See TopDesign.cysch for idle mode clock details) */
}

/*******************************************************************************
* Function Name: HandleApplicationInterface
********************************************************************************
* Summary:
*        This function handles all the application layer related firmware code
*        related to user inputs
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void HandleApplicationInterface(void)
{
   
	HandleUserInputs(); /* Handle play/pause button user inputs */
    
	/* Turning ON the STATUS LED consumes ~4 mA. Comment out the below line if LED indication while audio streaming 
	 * is not needed */
	UpdateAudioStatusUI(); /* Update the Status LED depending on the audio streaming mode */
		
	#ifdef ENABLE_VOLUME_CONTROL
    HandleUSBVolumeUpdate(); /* Update the codec volume as per the volume update from the host */
    #endif 
	
}

/*******************************************************************************
* Function Name: HandleUserInputs
********************************************************************************
* Summary:  Handles play/pause button user inputs for play track 
*           functions.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
static void HandleUserInputs(void)
{
     CheckPlayPauseButton();
	 
	if(IS_MAC_PC_PLAYLIST_CONTROL_NEEDED())
    {
        prevReport = playlistControlReport;
        USBFS_LoadInEP(MAC_PC_HID_CONTROL_ENDPOINT, USBFS_NULL, sizeof(playlistControlReport));
    }
	
}

/*******************************************************************************
* Function Name: CheckPlayPauseButton
********************************************************************************
* Summary: Check whether Play/Pause button is pressed and take necessary action
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
static void CheckPlayPauseButton(void)
{   
    /* Check if PSOC_SW is pressed */
   
	if(PSOC_SW_Read() == FALSE)
    {
     	/* Register the button press for the first time and check if button debouncing is completed */
		if(!(buttonStatus & PLAY_PAUSE_MASK)) 
        {
            buttonStatus = buttonStatus | PLAY_PAUSE_MASK; /* register button press event */
            switchDebounceTimer = SWITCH_DEBOUNCE_DELAY;
		}
		/* If the button status is pressed and switch debounce is complete update the PlaylistControlReport*/
        else if ((buttonStatus & PLAY_PAUSE_MASK) && (switchDebounceTimer == 0))
		{
			playlistControlReport = playlistControlReport | MAC_PC_PLAY_PAUSE_MASK;
		}
    	
    }
	/* If the button is released, clear the button status and update the button release status in PlaylistcontrolReport */
    else 
	{
		if((buttonStatus & PLAY_PAUSE_MASK))
	    {
	        buttonStatus = buttonStatus & (~PLAY_PAUSE_MASK);
	        playlistControlReport = playlistControlReport & (~MAC_PC_PLAY_PAUSE_MASK);
	    }
	}
}

/* [] END OF FILE */
