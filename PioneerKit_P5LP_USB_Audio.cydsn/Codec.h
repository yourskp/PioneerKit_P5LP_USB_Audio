/*******************************************************************************
* File Name: Codec.h
*
* Version 1.0
*
*  Description: This file contains the Codec register definitions and constants
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

#if !defined(CODEC_H)
#define CODEC_H

#include <cytypes.h>

/* Define MIC_ENABLE macro to enable passive microphone connection to Cirrus codec */
//#define MIC_ENABLE 

#define CODEC_I2C_ADDRESS                               0x4A

#define CODEC_ID_REGISTER                               0x01
#define CODEC_POWER_CTRL_REGISTER                       0x02
#define CODEC_SPEED_POWER_CTRL_REGISTER                 0x03
#define CODEC_INTERFACE_CTRL_REGISTER                   0x04
#define CODEC_MIC_CTRL_REGISTER                         0x05
#define CODEC_ADC_CTRL_REGISTER                         0x06
#define CODEC_ADC_SELECT_REGISTER                       0x07
#define CODEC_DAC_OUTPUT_CTRL_REGISTER                  0x08
#define CODEC_DAC_CONTROL_REGISTER                      0x09
#define CODEC_ALCA_PGAA_VOLUME_REGISTER                 0x0A
#define CODEC_ALCB_PGAB_VOLUME_REGISTER                 0x0B
#define CODEC_ADCA_ATTENUATOR_REGISTER                  0x0C
#define CODEC_ADCB_ATTENUATOR_REGISTER                  0x0D
#define CODEC_VOLUME_CTRL_ADCMIXA_REGISTER              0x0E
#define CODEC_VOLUME_CTRL_ADCMIXB_REGISTER              0x0F
#define CODEC_VOLUME_CTRL_PCMMIXA_REGISTER              0x10
#define CODEC_VOLUME_CTRL_PCMMIXB_REGISTER              0x11
#define CODEC_BEEP_FREQ_ON_TIME_REGISTER                0x12
#define CODEC_BEEP_OFF_TIME_VOL_REGISTER                0x13
#define CODEC_BEEP_CTRL_TONE_CONFIG_REGISTER            0x14
#define CODEC_TONE_CONTROL_REGISTER                     0x15
#define CODEC_VOLUME_CTRL_AOUTA_REGISTER                0x16
#define CODEC_VOLUME_CTRL_AOUTB_REGISTER                0x17
#define CODEC_PCN_ADC_CHANNEL_MIXER_REGISTER            0x18
#define CODEC_LIMITER_THRESHOLD_SZC_DISABLE_REGISTER    0x19
#define CODEC_LIMITER_CONFIG_RELEASE_RATE_REGISTER      0x1A
#define CODEC_LIMITER_ATTACK_RATE_REGISTER              0x1B
#define CODEC_ALC_ENABLE_ATTACK_RATE_REGISTER           0x1C
#define CODEC_ALC_RELEASE_RATE_REGISTER                 0x1D
#define CODEC_ALC_THRESHOLD_REGISTER                    0x1E
#define CODEC_NOISE_GATE_CONFIG_REGISTER                0x1F
#define CODEC_STATUS_REGISTER                           0x20
#define CODEC_CHARGE_PUMP_FREQUENCY_REGISTER            0x21

#define CODEC_PDN_MASK                                  0x01
#define CODEC_AUTO_DETECT_SPEED_MODE_MASK               0x80
#define CODEC_DAC_DIGITAL_INTERFACE_I2S_MASK            0x08
#define CODEC_ADC_DIGITAL_INTERFACE_I2S_MASK            0x04
#define CODEC_LOOPBACK_ENABLE                           0x80
#define DAC_SINGLE_VOLUME_CONTROL_MASK                  0x01
#define CODEC_STARTUP_VOLUME_VALUE                      0x00

#define ADC_SINGLE_VOLUME_CONTROL                       0x80
#define ADCB_20dB_DIGITAL_BOOST                         0x40
#define ADCA_20dB_DIGITAL_BOOST                         0x20
#define MIC_BIAS_ON_AIN3B_MICIN2_PIN                    0x00
#define MIC_BIAS_LEVEL_0P8_VA                           0x00
#define MIC_PRE_AMP_16dB_BOOST                          0x00

#define ADCB_HIGH_PASS_FILTER_ENABLE                    0x80
#define ADCA_HIGH_PASS_FILTER_ENABLE                    0x20
#define ADCB_SOFTRAMP_ENABLE                            0x08
#define ADCA_SOFTRAMP_ENABLE                            0x02

#define RELEASE_RESET                                   0x01
#define HOLD_RESET                                      0x00

#define CODEC_PCM_MUTE_MASK                             0x80
#define UNMUTE_PCMA_OUTPUT_TO_MIXER                     0x00
#define UNMUTE_PCMB_OUTPUT_TO_MIXER                     0x00

#define LIMITER_MAX_THRESHOLD_0DB                       0x00
#define LIMITER_CUSHION_THRESHOLD                       0x04
#define LIMITER_SOFT_RAMP_ENABLE                        0x02

#define CODEC_POSITIVE_MAX_VOLUME_OFFSET                39
#define CODEC_VOLUME_FORMAT_ADJUSTMENT                  89

#define PCM_SERIAL_PORT_TO_DAC                          0x00
#define SIGNAL_PROCESSING_ENGINE_TO_DAC                 0x40
#define ADC_SERIAL_PORT_TO_DAC                          0x80
#define DAC_FREEZE_CONTROL                              0x20
#define DAC_DE_EMPHASIS_CONTROL                         0x08
#define DAC_ANALOG_OUTPUT_AUTO_MUTE                     0x04
#define DAC_SZC_IMMEDIATE_CHANGE                        0x00
#define DAC_SZC_ZERO_CROSS                              0x01
#define DAC_SZC_SOFT_RAMP                               0x02
#define DAC_SZC_SOFT_RAMP_ON_ZERO_CROSSINGS             0x03

#define DAC_DEFAULT_HEADPHONE_GAIN                      0x60
#define DAC_SINGLE_VOLUME_CONTROL                       0x10
#define DAC_PCMB_INV_SIG_POLARITY                       0x08
#define DAC_PCMA_INV_SIG_POLARITY                       0x04
#define DAC_MUTE_CHANNELB                               0x02
#define DAC_MUTE_CHANNELA                               0x01

#define MIC_IN_TO_PRE_AMP_TO_PGA_INPUT                  0x30

#define LIMITER_RELEASE_ENABLE                          0x80
#define LIMITER_RELEASE_ALL                             0x40
#define LIMITER_RELEASE_RATE_SLOWEST                    0x3F

#define CODEC_POWER_ON_ALL                              0x00

#define AUTO_DETECT_CODEC_SPEED                         0x80
#define POWER_DOWN_MICB                                 0x08

#define ATTENUATION_DIVIDER                             5
#define ATTENUATION_OFFSET                              25
#define MIN_ATTENUATION_VALUE                           206
#define MIN_CODEC_ATTENUATION_VALUE                     0x80
#define SIGN_BIT_MASK                                   0x7F

void InitCirrusCodec(void);
void UpdateCodecVolume(int8 volume);
void UpdateCodecAttenuation(int8 attenuation);
void SetMaxI2CBusSpeed();		
void SetMinI2CBusSpeed();		
void WriteToSlave(uint8 address, uint8* writeBuffer, uint8 length);
typedef struct CodecConfig
{
    uint8 address;
    uint8 value;
} CodecRegister;

extern CYBIT codecInit;

#endif

/* [] END OF FILE */
