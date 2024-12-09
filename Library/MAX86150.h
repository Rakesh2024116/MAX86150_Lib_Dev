#ifndef MAX86150_H
#define MAX86150_H

#include <stdint.h>
#include <string.h>
#include <stm32f1xx_hal.h>

/*Sensor I2C Address*/
#define MAX_Read_addr   0xBD    //sensor read address
#define MAX_Write_addr  0xBC    // sensor write address

/*Sensor Register Addresses*/

#define MAX_Part_ID     0xFF    // Part ID register
#define System_Ctrl     0x0D    // System Control Register

/*PPG Configuration  register address*/
#define PPG_Config1     0x0E    //Led Pulse width, Sample rate, ADC range control
#define PPG_Config2     0x0F    // Sample Averaging Options

/*LED Register Addresses*/
#define LED1_PA         0x11    // IR Pulse Ampplitude
#define LED2_PA         0x12    // Red Pulse Amplitude

#define LED_RGE         0x14    // LED Range control address

#define LED_PILOT_PA    0x15    // LED Proximity mode Pulse Amplitude

/*ECG configuration register address*/
#define ECG_Config1     0x3C    // ECG Configuration Register
#define ECG_Config3     0x3E    // ECG Gain settings

/*Interrupt Status Register Address
    INT_Enable1
        ->Proximity Interrupt Enable
        ->Ambient Light Cancellation(ALC) Overflow Interrupt enable
        ->New PPG FIFO Data Ready Interrrupt enable
        ->FIFO almost full flag enable
    INT_Enable2
        ->New ECG FIFO data ready Interrupt enable
        ->VDD Out-of-Range Indicator enable    
    INT_Status1
        ->Power Ready Flag 
        ->Proximity Interrupt
        ->Ambient Light Cancellation Overflow
        ->New PPG FIFO Data Ready
        ->FIFO almost full FLag
    INT_Status2
        ->New ECG FIFO Data Ready
        ->VDD Out-of-Range Flag
      */
#define INT_Enable1         0x02    // Interrupt Enable Register 1
#define INT_Enable2         0x03    // Interrupt Enable Register 2
#define INT_Status1         0x00    // Interrupt Status Register 1
#define INT_Status2         0x01    // Interrupt Status Register 2

/*FIFO Register Address*/

#define Overflow_cntr       0x05    // FIFO Overflow Counter
#define FIFO_Config         0x08    // FIFO configuration
#define FIFO_Rd_ptr         0x06    // FIFO read Pointer
#define FIFO_Wr_ptr         0x04    // FIFO write Pointer
#define FIFO_DATA           0x07    // FIFO Data Register
#define FIFO_dataCtrl1      0x09    // FIFO Data Control Register 1
#define FIFO_dataCtrl2      0x0A    // FIFO Data Control Register 2

#define FIFO_CAPACITY       32      // Maximum samples in FIFO

/**
 * @brief Struct to hold sample data from the MAX86150 sensor.
 * This struct contains three 8-bit fields to hold the LED1, LED2, and ECG data samples.
 */
typedef struct
{
    uint32_t led1;    ///< LED1 data sample
    uint32_t led2;    ///< LED2 data sample
    uint32_t ecg;     ///< ECG data sample
} MAX86150_samples;


/*Function Prototypes*/
/**
  * @brief  Sensor initialization function
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @note   This function is used to initialize the MAX86150 sensor.
  * @retval 0 -> Success
  *         1 -> Fail
  */


uint8_t init_MAX86150(I2C_HandleTypeDef *hi2c); // Initialize MAX86150 sensor
uint8_t readData(I2C_HandleTypeDef *hi2c, MAX86150_samples *samples); // Read data from MAX86150 sensor

#endif