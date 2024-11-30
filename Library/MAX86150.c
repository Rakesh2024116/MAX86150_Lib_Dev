#include "MAX86150.h"

/*Sensor Modes*/
#define PPG_Mode    0x21    // PPG & ECG Mode
#define ECG_Mode    0x09    // ECG Mode

const uint16_t i2c_timeout = 100;

uint8_t init_MAX86150(I2CHandleTypeDef *hi2c)
{
    const uint8_t Sensor_reset = 1;
    //Reset the Sensor
    HAL_I2C_Mem_Write(hi2c, MAX_Write_addr, System_Ctrl, I2C_MEMADD_SIZE_8BIT, &Sensor_reset, 1, i2c_timeout);
    HAL_Delay(100);

    //Read the Part ID
    uint8_t Part_ID = 0;
    if (HAL_I2C_Mem_Read(hi2c, MAX_Read_addr, MAX_Part_ID, I2C_MEMADD_SIZE_8BIT, &Part_ID, 1, i2c_timeout))
    {
        // Check the Part ID
        if(Part_ID == (0x1E))
        {
            //Reset the FIFO Register
            HAL_I2C_Mem_Write(hi2c, MAX_Write_addr, FIFO_Wr_ptr, I2C_MEMADD_SIZE_8BIT, 0x00, 1, i2c_timeout);
            HAL_I2C_Mem_Write(hi2c, MAX_Write_addr, FIFO_Rd_ptr, I2C_MEMADD_SIZE_8BIT, 0x00, 1, i2c_timeout);
            HAL_I2C_Mem_Write(hi2c, MAX_Write_addr, FIFO_Config, I2C_MEMADD_SIZE_8BIT, 0x5F, 1, i2c_timeout);
            // FIFO Config
            HAL_I2C_Mem_Write(hi2c, MAX_Write_addr, FIFO_dataCtrl1, I2C_MEMADD_SIZE_8BIT, PPG_Mode, 1, i2c_timeout);
            HAL_I2C_Mem_Write(hi2c, MAX_Write_addr, FIFO_dataCtrl2, I2C_MEMADD_SIZE_8BIT, ECG_Mode, 1, i2c_timeout);
            
            //PPG and ECG Configuration
            HAL_I2C_Mem_Write(hi2c, MAX_Write_addr, PPG_Config1, I2C_MEMADD_SIZE_8BIT, 0x00, 1, i2c_timeout);
            
        }
        else
        {
            return 1;
        }
    }
    else
    {
        return 1;
    }
}