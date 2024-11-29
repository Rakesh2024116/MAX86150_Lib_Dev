#include "MAX86150.h"


const uint16_t i2c_timeout = 100;

uint8_t init_MAX86150(I2CHandleTypeDef *hi2c)
{
    const uint8_t Sensor_reset = 1;
    //Reset the Sensor
    HAL_I2C_Mem_Write(hi2c, MAX_Write_addr, System_Ctrl, I2C_MEMADD_SIZE_8BIT, &Sensor_reset, 1, i2c_timeout);
    HAL_Delay(100);

    //Read the Part ID
    uint8_t Part_ID;
    HAL_I2C_Mem_Read(hi2c, MAX_Read_addr, MAX_Part_ID, I2C_MEMADD_SIZE_8BIT, &Part_ID, 1, i2c_timeout);
    // Check the Part ID
    if(Part_ID == 30)
    {
        
    }
    
}