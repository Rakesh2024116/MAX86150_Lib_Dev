#include "MAX86150.h"


#define max_samples 32
/*Sensor Modes*/
#define PPG_Mode    0x21    // PPG Mode()
#define ECG_Mode    0x09    // ECG Mode

const uint16_t i2c_timeout = 100;

/**
 * @brief Struct to hold sample data from the MAX86150 sensor.
 * This struct contains three 8-bit fields to hold the LED1, LED2, and ECG data samples.
 */
typedef struct
{
    uint8_t led1;    ///< LED1 data sample
    uint8_t led2;    ///< LED2 data sample
    uint8_t ecg;     ///< ECG data sample
} MAX86150_samples;

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
            HAL_I2C_Mem_Write(hi2c, MAX_Write_addr, FIFO_Config, I2C_MEMADD_SIZE_8BIT, 0x7F, 1, i2c_timeout);
            // FIFO Config
            HAL_I2C_Mem_Write(hi2c, MAX_Write_addr, Overflow_cntr, I2C_MEMADD_SIZE_8BIT, 0x00, 1, i2c_timeout);

            
            HAL_I2C_Mem_Write(hi2c, MAX_Write_addr, FIFO_dataCtrl1, I2C_MEMADD_SIZE_8BIT, PPG_Mode, 1, i2c_timeout);
            HAL_I2C_Mem_Write(hi2c, MAX_Write_addr, FIFO_dataCtrl2, I2C_MEMADD_SIZE_8BIT, ECG_Mode, 1, i2c_timeout);
            
            //PPG and ECG Configuration
            HAL_I2C_Mem_Write(hi2c, MAX_Write_addr, PPG_Config1, I2C_MEMADD_SIZE_8BIT, 0x93, 1, i2c_timeout);
            HAL_I2C_Mem_Write(hi2c, MAX_Write_addr, PPG_Config2, I2C_MEMADD_SIZE_8BIT, 0x04, 1, i2c_timeout);

            HAL_I2C_Mem_Write(hi2c, MAX_Write_addr, LED_RGE, I2C_MEMADD_SIZE_8BIT, 0x00, 1, i2c_timeout);
            HAL_I2C_Mem_Write(hi2c, MAX_Write_addr, LED1_PA, I2C_MEMADD_SIZE_8BIT, 0xFF, 1, i2c_timeout);
            HAL_I2C_Mem_Write(hi2c, MAX_Write_addr, LED2_PA, I2C_MEMADD_SIZE_8BIT, 0xFF, 1, i2c_timeout);

            HAL_I2C_Mem_Write(hi2c, MAX_Write_addr, ECG_Config1, I2C_MEMADD_SIZE_8BIT, 0x00, 1, i2c_timeout);
            HAL_I2C_Mem_Write(hi2c, MAX_Write_addr, ECG_Config3, I2C_MEMADD_SIZE_8BIT, 0x00, 1, i2c_timeout);
            
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

uint8_t readData(I2CHandleTypeDef *hi2c, MAX86150_samples *samples)
{
    uint8_t fifo_wr_ptr = 0;
    uint8_t fifo_rd_ptr = 0;
    uint8_t ovf_counter = 0;
    uint8_t available_samples = 0;
    uint8_t num_samples_to_read = 0;
    uint8_t num_available_bytes = 0;

     // Step 1: Read FIFO_WR_PTR
    if (HAL_I2C_Mem_Read(hi2c, I2C_ADDR << 1, FIFO_Wr_ptr, I2C_MEMADD_SIZE_8BIT, &fifo_wr_ptr, 1, HAL_MAX_DELAY) != HAL_OK) {
        return 0;
    }

    // Step 2: Read OVF_COUNTER
    if (HAL_I2C_Mem_Read(hi2c, I2C_ADDR << 1, Overflow_cntr, I2C_MEMADD_SIZE_8BIT, &ovf_counter, 1, HAL_MAX_DELAY) != HAL_OK) {
        return 0;
    }

    // Step 3: Read FIFO_RD_PTR
    if (HAL_I2C_Mem_Read(hi2c, I2C_ADDR << 1, FIFO_Rd_ptr, I2C_MEMADD_SIZE_8BIT, &fifo_rd_ptr, 1, HAL_MAX_DELAY) != HAL_OK) {
        return 0;
    }
    // Step 4: Evaluate the number of available samples
    if (ovf_counter == 0) {
        if (fifo_wr_ptr >= fifo_rd_ptr) {
            available_samples = fifo_wr_ptr - fifo_rd_ptr;
        } else {
            available_samples = FIFO_CAPACITY - fifo_rd_ptr + fifo_wr_ptr;
        }
    } else {
        available_samples = FIFO_CAPACITY; // Overflow occurred
    }

    num_available_bytes = num_available_samples * 9;  // Total bytes in FIFO
    if (num_available_bytes % 9 != 0) {
        uint8_t leftover_bytes = num_available_bytes % 9;

        // Option 1: Skip leftover bytes (adjust FIFO_RD_PTR)
        uint8_t new_fifo_rd_ptr = (fifo_rd_ptr + leftover_bytes) % FIFO_CAPACITY;
        if (HAL_I2C_Mem_Write(hi2c, I2C_ADDR << 1, FIFO_Rd_ptr, I2C_MEMADD_SIZE_8BIT, &new_fifo_rd_ptr, 1, HAL_MAX_DELAY) != HAL_OK) {
            return 0;  // Failed to adjust read pointer
        }

        // Recalculate the number of available samples
        available_samples = (num_available_bytes - leftover_bytes) / 9;
    }

    // Limit the number of samples to read to the available samples or the maximum buffer size
    num_samples_to_read = (available_samples < max_samples) ? available_samples : max_samples;


}

