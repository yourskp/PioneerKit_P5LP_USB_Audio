/*******************************************************************************
* File Name: VolumeControl.h 
*
* Version 1.0
*
*  Description: This file provides routines for volume updates
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
#include <Configuration.h>

#ifdef ENABLE_VOLUME_CONTROL

#if !defined(VolumeControl_H)
#define VolumeControl_H

void InitUSBVolumeLevel(void);
void HandleUSBVolumeUpdate(void);
void HandleVolumeSlider(void);

#define MINIMUM_VOLUME_LEVEL                   0
#define MAXIMUM_VOLUME_LEVEL                   255

#define CODEC_MAX_VOLUME_VALUE                 0xFF
#define CODEC_MIN_VOLUME_VALUE                 0x00

#define AUDIO_CLASS_INTERFACE_INFO             0x00
#define CUR_VOLUME_ATTRIBUTE                   0x01
#define MASTER_CHANNEL                         0x00
#define AUDIO_CONTROL_INTERFACE                0x00
#define VOLUME_CONTROL_FEATURE_UNIT            0x02

#define MAX_POSITIVE_SIGNED_BYTE_VALUE         128

#define AUDIO_INT_STATUS_ENDPOINT              0x06

#define BLOCK_DISPLAY_VALUE                    0xC0
#define NON_ZERO_NUMBER                        1

#endif

#define USB_MINIMUM_VOLUME_HIGH                0xCE
#define USB_MINIMUM_VOLUME_LOW                 0x00
#define USB_MINIMUM_VOLUME                     (int16)(((int16)USB_MINIMUM_VOLUME_HIGH << 8) | USB_MINIMUM_VOLUME_LOW)
#define CODEC_MAXIMUM_ATTENUATION              0x80
#define USB_MAXIMUM_VOLUME_HIGH                0x00
#define USB_MAXIMUM_VOLUME_LOW                 0x00
#define USB_MAXIMUM_VOLUME                     (int16)(((int16)USB_MAXIMUM_VOLUME_HIGH << 8) | USB_MAXIMUM_VOLUME_LOW)
#define SYSTEM_STARTUP_VOLUME_HIGH             0xE7 /* This corresponds to -25dB */
#define USB_VOLUME_RESOLUTION_HIGH             0x00
#define USB_VOLUME_RESOLUTION_LOW              0x80

#define OUTPUT_VOLUME_CHANGED                  0x8002
#define INPUT_VOLUME_CHANGED                   0x8006
#define NO_VOLUME_CHANGE                       0x0000

#define VOLUME_STEP_SIZE_MAPPING               2
#define USB_MINIMUM_VOLUME_DB                  -50


#endif /* End of #ifdef ENABLE_VOLUME_CONTROL */

/* [] END OF FILE */
