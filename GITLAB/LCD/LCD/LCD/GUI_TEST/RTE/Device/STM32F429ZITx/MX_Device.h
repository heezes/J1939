/******************************************************************************
 * File Name   : MX_Device.h
 * Date        : 20/11/2017 19:41:59
 * Description : STM32Cube MX parameter definitions
 * Note        : This file is generated by STM32CubeMX (DO NOT EDIT!)
 ******************************************************************************/

#ifndef __MX_DEVICE_H
#define __MX_DEVICE_H

/*---------------------------- Clock Configuration ---------------------------*/

#define MX_LSI_VALUE                            32000
#define MX_LSE_VALUE                            32768
#define MX_HSI_VALUE                            16000000
#define MX_HSE_VALUE                            25000000
#define MX_EXTERNAL_CLOCK_VALUE                 12288000
#define MX_PLLCLKFreq_Value                     96000000
#define MX_SYSCLKFreq_VALUE                     16000000
#define MX_HCLKFreq_Value                       16000000
#define MX_FCLKCortexFreq_Value                 16000000
#define MX_CortexFreq_Value                     16000000
#define MX_AHBFreq_Value                        16000000
#define MX_APB1Freq_Value                       16000000
#define MX_APB2Freq_Value                       16000000
#define MX_APB1TimFreq_Value                    16000000
#define MX_APB2TimFreq_Value                    16000000
#define MX_48MHZClocksFreq_Value                48000000
#define MX_EthernetFreq_Value                   16000000
#define MX_LCDTFTFreq_Value                     12250000
#define MX_I2SClocksFreq_Value                  96000000
#define MX_SAI_AClocksFreq_Value                12250000
#define MX_SAI_BClocksFreq_Value                12250000
#define MX_RTCFreq_Value                        32000
#define MX_WatchDogFreq_Value                   32000
#define MX_MCO1PinFreq_Value                    16000000
#define MX_MCO2PinFreq_Value                    16000000

/*-------------------------------- SPI5       --------------------------------*/

#define MX_SPI5                                 1

/* GPIO Configuration */

/* Pin PF7 */
#define MX_SPI5_SCK_GPIO_Speed                  GPIO_SPEED_FREQ_VERY_HIGH
#define MX_SPI5_SCK_Pin                         PF7
#define MX_SPI5_SCK_GPIOx                       GPIOF
#define MX_SPI5_SCK_GPIO_PuPd                   GPIO_NOPULL
#define MX_SPI5_SCK_GPIO_Pin                    GPIO_PIN_7
#define MX_SPI5_SCK_GPIO_AF                     GPIO_AF5_SPI5
#define MX_SPI5_SCK_GPIO_Mode                   GPIO_MODE_AF_PP

/* Pin PF8 */
#define MX_SPI5_MISO_GPIO_Speed                 GPIO_SPEED_FREQ_VERY_HIGH
#define MX_SPI5_MISO_Pin                        PF8
#define MX_SPI5_MISO_GPIOx                      GPIOF
#define MX_SPI5_MISO_GPIO_PuPd                  GPIO_NOPULL
#define MX_SPI5_MISO_GPIO_Pin                   GPIO_PIN_8
#define MX_SPI5_MISO_GPIO_AF                    GPIO_AF5_SPI5
#define MX_SPI5_MISO_GPIO_Mode                  GPIO_MODE_AF_PP

/* Pin PF9 */
#define MX_SPI5_MOSI_GPIO_Speed                 GPIO_SPEED_FREQ_VERY_HIGH
#define MX_SPI5_MOSI_Pin                        PF9
#define MX_SPI5_MOSI_GPIOx                      GPIOF
#define MX_SPI5_MOSI_GPIO_PuPd                  GPIO_NOPULL
#define MX_SPI5_MOSI_GPIO_Pin                   GPIO_PIN_9
#define MX_SPI5_MOSI_GPIO_AF                    GPIO_AF5_SPI5
#define MX_SPI5_MOSI_GPIO_Mode                  GPIO_MODE_AF_PP

/*-------------------------------- SYS        --------------------------------*/

#define MX_SYS                                  1

/* GPIO Configuration */

/*-------------------------------- NVIC       --------------------------------*/

#define MX_NVIC                                 1

/*-------------------------------- GPIO       --------------------------------*/

#define MX_GPIO                                 1

/* GPIO Configuration */

/* Pin PG13 */
#define MX_PG13_GPIO_Speed                      GPIO_SPEED_FREQ_LOW
#define MX_PG13_Pin                             PG13
#define MX_PG13_GPIOx                           GPIOG
#define MX_PG13_PinState                        GPIO_PIN_RESET
#define MX_PG13_GPIO_PuPd                       GPIO_NOPULL
#define MX_PG13_GPIO_Pin                        GPIO_PIN_13
#define MX_PG13_GPIO_ModeDefaultOutputPP        GPIO_MODE_OUTPUT_PP

/* Pin PG14 */
#define MX_PG14_GPIO_Speed                      GPIO_SPEED_FREQ_LOW
#define MX_PG14_Pin                             PG14
#define MX_PG14_GPIOx                           GPIOG
#define MX_PG14_PinState                        GPIO_PIN_RESET
#define MX_PG14_GPIO_PuPd                       GPIO_NOPULL
#define MX_PG14_GPIO_Pin                        GPIO_PIN_14
#define MX_PG14_GPIO_ModeDefaultOutputPP        GPIO_MODE_OUTPUT_PP

#endif  /* __MX_DEVICE_H */

