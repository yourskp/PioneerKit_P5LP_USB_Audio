/*******************************************************************************
* File Name: VolumeControl.c
*
* Version 1.0
*
*  Description: This file contains all the Volume control code
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
#include <USBInterface.h>

#ifdef ENABLE_VOLUME_CONTROL

#include <Codec.h>
#include <project.h>
#include <VolumeControl.h>

uint8 currentVolume = 0;                  /* Value of set volume level */
uint8 currentMute = 0;                    /* Mute status */

uint8 volumeUpdateSent = 0;               /* Volume update info sent over USB flag */
uint8 volumeUpdateReceived = 0;           /* USB host read the updated volume over USB status IN endpoint */
uint16 volumeChangeFlag = OUTPUT_VOLUME_CHANGED; /* Indicate a change in the Volume/Mute feature unit*/

#ifndef ENABLE_DIGITAL_AUDIO_OUT_ONLY
uint8 currentInputVolume;                 /* Audio IN path current volume level */
uint8 currentInputMute;                   /* Audio IN path mute status */
uint8 inEventPending = 0; 				  /* Flag to Check Audio IN volume after Audio OUT volume update is done */      
#endif

extern CYBIT codecInit;                      /* Check if codec is initialized before updating volume */
extern volatile uint8 USBFS_currentVolume[]; /* USB audio class volume setting buffer */
extern volatile uint8 USBFS_currentMute;     /* USB audio class mute buffer */

#ifndef ENABLE_DIGITAL_AUDIO_OUT_ONLY
extern volatile uint8 USB_inputMute;      /* USB audio IN path mute setting */
extern volatile uint8 USB_inputVolume[];  /* USB component audio IN path volume level */
#endif

extern volatile uint8 USBFS_minimumVolume[];
extern volatile uint8 USBFS_maximumVolume[];
extern volatile uint8 USBFS_resolutionVolume[];

extern volatile uint8 USBFS_currentCH1InputVolume[];
extern volatile uint8 USBFS_currentCH2InputVolume[];
extern volatile uint8 USBFS_currentTmpCH1InputVolume[];
extern volatile uint8 USBFS_currentTmpCH2InputVolume[];

/*******************************************************************************
* Function Name: HandleUSBVolumeUpdate
********************************************************************************
* Summary:  
*		Extract the volume change USB request and update the codec with the
*       received volume information.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void HandleUSBVolumeUpdate(void)
{
    int16 usbVolume;
    int8 codecVolume;
    
    /* USB volume information high byte increments for every 1dB change in volume. The Cirrus codec has a step size of
     * 0.5dB. To match the USB step size to Cirrus codec step size, multiply the volume gain in dB by 2 (1 USB unit = 2 
     * Cirrus codec unit).
     * Note : This mapping is specific to Cirrus codec on CY8CKIT-033A. You should add appropriate code to map the 
     * USB volume level to the codec used in your design */
    usbVolume =  ((int16)USBFS_currentVolume[1] << 8) | USBFS_currentVolume[0];
    usbVolume = usbVolume * VOLUME_STEP_SIZE_MAPPING;
    codecVolume = HI8(usbVolume);
    
    /* If the volume level has changed and the codec is initialized, then update the codec volume control register with
     * the updated volume level */
	if (codecInit == TRUE)
	{
	    if((currentVolume != codecVolume || currentMute != USBFS_currentMute))
	    {
	        currentVolume = codecVolume;
	        currentMute = USBFS_currentMute;
	        UpdateCodecVolume(codecVolume);
		}
		#ifndef ENABLE_DIGITAL_AUDIO_OUT_ONLY
		if((currentInputVolume != USB_inputVolume[1] || currentInputMute != USB_inputMute))
	    {
	        currentInputVolume = USB_inputVolume[1];
	        currentInputMute = USB_inputMute;
	        UpdateCodecAttenuation(USB_inputVolume[1]);
		}
        #endif
    }
    
	/* USER_CODE:[CODEC] If different codec is used than onboard Cirrus codec (CS42L51), then
	 * Comment out the functions InitCirrusCodec().
	 * Add your code for codec initialization in place of InitCirrusCodec(). 
	 * Update the UpdateCodecVolume() and UpdateCodecAttenuation() functions as required by new codec. 
	 * Update the codec.h file with the I2C address of codec and the register addresses of the new codec. */     
	
    /* If USB host has scanned for the updated volume information from the accessory, send a "No Volume Change"
     * status to the Host */
    if((USBFS_GetEPState(AUDIO_INT_STATUS_ENDPOINT) == USBFS_EVENT_PENDING) 
        && volumeUpdateSent && volumeUpdateReceived)
    {
        volumeChangeFlag = NO_VOLUME_CHANGE;
        USBFS_LoadInEP(AUDIO_INT_STATUS_ENDPOINT, (uint8 *)&volumeChangeFlag, sizeof(volumeChangeFlag));
        USBFS_LoadInEP(AUDIO_INT_STATUS_ENDPOINT, USBFS_NULL, sizeof(volumeChangeFlag));

        volumeUpdateSent = 0;
        volumeUpdateReceived = 0;
    }
    
    /* Use CapSense slider as volume control input - Place Holder Only. Separate example project for CapSense Slider 
	 * interface via PSoC 4 need to be developed for handling this volume control in PSoC 4 */
        HandleVolumeSlider();
 
}

/*******************************************************************************
* Function Name: HandleVolumeSlider
********************************************************************************
* Summary:  
*		Handles USB Host volume control using CapSense slider (mapped to PSoC 4A)
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void HandleVolumeSlider(void)
{
    /* Place Holder for CapSense Slider (using PSoC 4A) handling code for Volume update */
}

/*******************************************************************************
* Function Name: InitUSBVolumeLevel
********************************************************************************
* Summary: 
*		Initializes USB audio class volume range (minimum, maximum and resolution)
*       values based on the hardware volume control range supported by the codec
*
* Parameters:
*  None
*
* Return:
*  void
*
*******************************************************************************/
void InitUSBVolumeLevel(void)
{
    USBFS_minimumVolume[0] = USB_MINIMUM_VOLUME_LOW;
    USBFS_minimumVolume[1] = USB_MINIMUM_VOLUME_HIGH;
    USBFS_maximumVolume[0] = USB_MAXIMUM_VOLUME_LOW;
    USBFS_maximumVolume[1] = USB_MAXIMUM_VOLUME_HIGH;
    USBFS_resolutionVolume[0] = USB_VOLUME_RESOLUTION_LOW;
    USBFS_resolutionVolume[1] = USB_VOLUME_RESOLUTION_HIGH;
    USBFS_currentVolume[1] = SYSTEM_STARTUP_VOLUME_HIGH;
    
    /* save current values into tmp holding registers */
	USBFS_currentTmpCH1InputVolume[0] = USBFS_currentCH1InputVolume[0];
	USBFS_currentTmpCH2InputVolume[0] = USBFS_currentCH2InputVolume[0];
}

#endif  /* End of ENABLE_VOLUME_CONTROL */

/* [] END OF FILE */
