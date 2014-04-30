/*******************************************************************************
* File Name: Codec.c
*
* Version 1.0
*
*  Description: This file contains the Codec initialization and volume update
*				routines for Cirrus Codec
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
#include <AudioControl.h>
#include <Application.h>
#include <Codec.h>
#include <Configuration.h>
#include <Project.h>
#include <VolumeControl.h>

CYDATA CodecRegister CodecWrite;    /* codec register read/write structure */
CYBIT codecInit = 0;                /* codec initialization status flag */

extern volatile uint8 USBFS_currentVolume[];
extern volatile uint8 USBFS_currentMute;

#ifndef ENABLE_DIGITAL_AUDIO_OUT_ONLY		
extern volatile uint8 USB_inputMute;
extern volatile uint8 USB_inputVolume[];  /* USB component audio IN path volume level */
#endif

/*******************************************************************************
* Function Name: InitCirrusCodec
********************************************************************************
* Summary: Initializes the CS42L51 codec as per the CS42L51 datasheet to enable
*          audio output DAC (with volume control support enabled) and audio input
*          ADC
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void InitCirrusCodec(void)
{
#ifdef CODEC_I2C_SUPPORTED
	if (!codecInit)
	{
        /* USER_CODE: [CODEC configuration] Placeholder for configuring a different codec instead of the CY8CKIT-033A onboard Cirrus
         * CS42L51 codec. Comment out below piece of I2C configuration before adding your custom codec configuration 
         * code */
		PSOC_CODEC_RST_Write(RELEASE_RESET);
	   
	    CodecWrite.address = CODEC_POWER_CTRL_REGISTER;
	    CodecWrite.value = CODEC_PDN_MASK;
	    WriteToSlave(CODEC_I2C_ADDRESS, &CodecWrite.address, sizeof(CodecWrite));
		
		    
	    CodecWrite.address = CODEC_INTERFACE_CTRL_REGISTER;
	    CodecWrite.value = CODEC_DAC_DIGITAL_INTERFACE_I2S_MASK | CODEC_ADC_DIGITAL_INTERFACE_I2S_MASK;
	    WriteToSlave(CODEC_I2C_ADDRESS, &CodecWrite.address,  sizeof(CodecWrite));
	            
	    /* Enable the Input PCM samples to pass through the Signal processing engine on the Codec for volume control */
	    CodecWrite.address = CODEC_DAC_CONTROL_REGISTER;
	    CodecWrite.value = SIGNAL_PROCESSING_ENGINE_TO_DAC | DAC_SZC_SOFT_RAMP;
	    WriteToSlave(CODEC_I2C_ADDRESS, &CodecWrite.address, sizeof(CodecWrite));
	    
	    /* Single volume control for DAC */
	    CodecWrite.address = CODEC_DAC_OUTPUT_CTRL_REGISTER;
	    CodecWrite.value = DAC_DEFAULT_HEADPHONE_GAIN | DAC_SINGLE_VOLUME_CONTROL;
	    WriteToSlave(CODEC_I2C_ADDRESS, &CodecWrite.address, sizeof(CodecWrite));
	    
	    /* Single volume for ADC */
	    CodecWrite.address = CODEC_MIC_CTRL_REGISTER;
	    #ifdef MIC_ENABLE
	    /* ADC 20dB boost, MIC bias on AIN3B pin ,  0.8 * VA bias and +16dB gain */
	    CodecWrite.value = ADC_SINGLE_VOLUME_CONTROL | ADCB_20dB_DIGITAL_BOOST | ADCA_20dB_DIGITAL_BOOST; 
	    #else
	    CodecWrite.value = ADC_SINGLE_VOLUME_CONTROL;
	    #endif
	    WriteToSlave(CODEC_I2C_ADDRESS, &CodecWrite.address,  sizeof(CodecWrite));
	    
	    #ifdef MIC_ENABLE
	    /* Route MIC as the ADC input */
	    CodecWrite.address = CODEC_ADC_SELECT_REGISTER;
	    CodecWrite.value = MIC_IN_TO_PRE_AMP_TO_PGA_INPUT; /* Route MIC input to PGA */
	    WriteToSlave(CODEC_I2C_ADDRESS, &CodecWrite.address,  sizeof(CodecWrite));
	    #endif

	    /* Enable below piece of code and comment out the next register write for DAC Output for enabling DAC limiter */
	    /* Limiter threshold configuration */
	    CodecWrite.address = CODEC_LIMITER_THRESHOLD_SZC_DISABLE_REGISTER;
	    CodecWrite.value = LIMITER_CUSHION_THRESHOLD | LIMITER_SOFT_RAMP_ENABLE;
	    WriteToSlave(CODEC_I2C_ADDRESS, &CodecWrite.address, sizeof(CodecWrite));
	    
	    /* limit DAC Output */
	    CodecWrite.address = CODEC_LIMITER_CONFIG_RELEASE_RATE_REGISTER;
	    CodecWrite.value = LIMITER_RELEASE_ENABLE | LIMITER_RELEASE_ALL | LIMITER_RELEASE_RATE_SLOWEST; 
	    WriteToSlave(CODEC_I2C_ADDRESS, &CodecWrite.address, sizeof(CodecWrite));
		
	    #ifdef ENABLE_VOLUME_CONTROL
	    UpdateCodecVolume((int8)USBFS_currentVolume[1] * VOLUME_STEP_SIZE_MAPPING);
		#else		
		UpdateCodecVolume(USBFS_currentVolume[1]);
		#endif
		
	    #ifndef ENABLE_DIGITAL_AUDIO_OUT_ONLY
		#ifdef ENABLE_VOLUME_CONTROL
		UpdateCodecAttenuation(USB_inputVolume[1]);		
		#endif
		#endif
			
	    CodecWrite.address = CODEC_POWER_CTRL_REGISTER;
	    CodecWrite.value = CODEC_POWER_ON_ALL;
	    WriteToSlave(CODEC_I2C_ADDRESS, &CodecWrite.address, sizeof(CodecWrite));
	    
	    #ifdef MIC_ENABLE
	    CodecWrite.address = CODEC_SPEED_POWER_CTRL_REGISTER;
	    /* Auto detect codec speed, power up MICA, enable BIAS */
	    CodecWrite.value = AUTO_DETECT_CODEC_SPEED | POWER_DOWN_MICB;
	    WriteToSlave(CODEC_I2C_ADDRESS, &CodecWrite.address, sizeof(CodecWrite));
	    #endif
	    
		codecInit = 1;
	}	
#endif
}

/*******************************************************************************
* Function Name: UpdateCodecVolume
********************************************************************************
* Summary: Updates the actual codec volume control with the volume parameter
*          passed
*
* Parameters:
*  volume - value of volume to be written to the codec register
*
* Return:
*  void
*
*******************************************************************************/
#ifdef CODEC_I2C_SUPPORTED
void UpdateCodecVolume(int8 volume)
{
    /* Cirrus codec volume control register has no sign bit, mask the sign bit and insert codec mute setting if mute
     * flag is set. Refer to CS42L51 datasheet PCMMIXA and PCMMIXB register description for details  */
    volume = (volume & SIGN_BIT_MASK) | (USBFS_currentMute ? CODEC_PCM_MUTE_MASK : 0) ;
    
    /* Update PCM channel 1 and 2 volume in the PCM Mixer register */
    CodecWrite.address = CODEC_VOLUME_CTRL_PCMMIXA_REGISTER;
    CodecWrite.value = volume;
    WriteToSlave(CODEC_I2C_ADDRESS, &CodecWrite.address, sizeof(CodecWrite));
    
    CodecWrite.address = CODEC_VOLUME_CTRL_PCMMIXB_REGISTER;
    WriteToSlave(CODEC_I2C_ADDRESS, &CodecWrite.address, sizeof(CodecWrite));
    
    /* USER_CODE: [CODEC output volume control] Placeholder for mapping the USB audio class volume to codec volume   
     * control settings for a custom codec */
}
#endif

/*******************************************************************************
* Function Name: UpdateCodecAttenuation
********************************************************************************
* Summary: Updates the actual CODEC ADC attenuator with the attenuation parameter
*          passed. This API is defined and not called anywhere as the line IN from
*		   the codec is not supported in this firmware version.
* Parameters:
*  attenuation - value of attenuation to be written to the CODEC register
*
* Return:
*  void
*
*******************************************************************************/
#ifdef CODEC_I2C_SUPPORTED
#ifndef ENABLE_DIGITAL_AUDIO_OUT_ONLY
#ifdef ENABLE_VOLUME_CONTROL
void UpdateCodecAttenuation(int8 attenuation)
{

	/* Notes:
     * 1. USB volume information high byte increments for every 1dB change in volume. The Cirrus codec also has a step 
     *    size of 1dB.
     * 2. This mapping is specific to Cirrus codec on CY8CKIT-033A. You should add appropriate code to map the 
     *    USB input volume level to the codec used in your design */
    
    if(attenuation == USB_MINIMUM_VOLUME_HIGH || USB_inputMute == TRUE)
    {
        /* For the least value of USB attenuation, set the codec attenuation to -96dB (the maximum attenuation value
         * supported by CS42L51 codec). Alternatively, you can also write to the codec input volume  mute control 
         * register when and unmute the codec when a non-minimal USB volume value is received from the USB host */
        attenuation = CODEC_MAXIMUM_ATTENUATION; 
    }
    
    /* When individual input volume channel control is required, the attenuator registers A and B must be written with
     * the corresponding values from the host. Currently, the same attenuation value is written to both the registers */
    CodecWrite.address = CODEC_ADCA_ATTENUATOR_REGISTER;
    CodecWrite.value = attenuation;
    WriteToSlave(CODEC_I2C_ADDRESS, &CodecWrite.address, sizeof(CodecWrite));
    
    CodecWrite.address = CODEC_ADCB_ATTENUATOR_REGISTER;
    WriteToSlave(CODEC_I2C_ADDRESS, &CodecWrite.address, sizeof(CodecWrite));

    /* USER_CODE: [CODEC input volume control] Placeholder for mapping the USB audio class volume to codec input gain   
     * control settings for a custom codec */
}
#endif
#endif
#endif

/*******************************************************************************
* Function Name: WriteToSlave
********************************************************************************
* Summary:
*        Writes data to the I2C slave device
*
* Parameters:
*  address - address of the I2C slave being addressed
*  writeBuffer - buffer which contains the data to be written
*  length - length of the data to be written
* 
* Return:
*  None
*
* Note:
*  Exits the function if the write is successful, otherwise keeps trying 
*  (blocking call).
*
*******************************************************************************/
#ifdef CODEC_I2C_SUPPORTED
void WriteToSlave(uint8 address, uint8* writeBuffer, uint8 length)
{
    CYDATA uint8 count;
    uint8 status;
    
    /* Wait for the current transfer to complete */
    while (I2C_Master_MasterStatus() & I2C_Master_MSTAT_XFER_INP);
    
    do
    {
        do
        {
            /* clear any previous status */
            I2C_Master_MasterClearStatus(); 
            
            /* write data to the slave */
            status = I2C_Master_MasterWriteBuf(address, writeBuffer, length, I2C_Master_MODE_COMPLETE_XFER);
            
        } while(status != I2C_Master_MSTR_NO_ERROR);
        
        count = 0;
        
        /* wait till write operation is complete or an error flag is set */
        while ((I2C_Master_MasterStatus() & (I2C_Master_MSTAT_WR_CMPLT | I2C_Master_MSTAT_ERR_MASK)) == 0)
        {
            if(I2C_DELAY_COUNT == count)
            {
                break;
            }
            
            CyDelayUs(length);
            count++;
        }
    
    }while(I2C_Master_MasterStatus()!= I2C_Master_MSTAT_WR_CMPLT);
    
    /* clear any previous status */
    I2C_Master_MasterClearStatus(); 
}
#endif
/* [] END OF FILE */
