#include "MAX86150.h"


#define max_samples 32

/*Sensor Modes*/


const uint16_t i2c_timeout = 100;


uint8_t init_MAX86150(I2C_HandleTypeDef *hi2c)
{
    uint8_t Sensor_reset = 1;
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
            uint8_t FIFO_MASK = 	0x7F;
			uint8_t PPG_Mode = 0x21;   // PPG Mode()
			uint8_t ECG_Mode = 0x09;   // ECG Mode
            uint8_t PPG_100SPS = 0x93;
            uint8_t PPG_16SAVG = 0x04;
            uint8_t LEDs_PA_50ma = 0xFF;
            //Reset the FIFO Register
            HAL_I2C_Mem_Write(hi2c, MAX_Write_addr, FIFO_Wr_ptr, I2C_MEMADD_SIZE_8BIT, 0x00, 1, i2c_timeout);
            HAL_I2C_Mem_Write(hi2c, MAX_Write_addr, FIFO_Rd_ptr, I2C_MEMADD_SIZE_8BIT, 0x00, 1, i2c_timeout);
            HAL_I2C_Mem_Write(hi2c, MAX_Write_addr, FIFO_Config, I2C_MEMADD_SIZE_8BIT, &FIFO_MASK, 1, i2c_timeout);
            // FIFO Config
            HAL_I2C_Mem_Write(hi2c, MAX_Write_addr, Overflow_cntr, I2C_MEMADD_SIZE_8BIT, 0x00, 1, i2c_timeout);

            HAL_I2C_Mem_Write(hi2c, MAX_Write_addr, FIFO_dataCtrl1, I2C_MEMADD_SIZE_8BIT, &PPG_Mode, 1, i2c_timeout);
            HAL_I2C_Mem_Write(hi2c, MAX_Write_addr, FIFO_dataCtrl2, I2C_MEMADD_SIZE_8BIT, &ECG_Mode, 1, i2c_timeout);
            
            //PPG and ECG Configuration
            HAL_I2C_Mem_Write(hi2c, MAX_Write_addr, PPG_Config1, I2C_MEMADD_SIZE_8BIT, &PPG_100SPS, 1, i2c_timeout);
            HAL_I2C_Mem_Write(hi2c, MAX_Write_addr, PPG_Config2, I2C_MEMADD_SIZE_8BIT, &PPG_16SAVG, 1, i2c_timeout);

            HAL_I2C_Mem_Write(hi2c, MAX_Write_addr, LED_RGE, I2C_MEMADD_SIZE_8BIT, 0x00, 1, i2c_timeout);
            HAL_I2C_Mem_Write(hi2c, MAX_Write_addr, LED1_PA, I2C_MEMADD_SIZE_8BIT, &LEDs_PA_50ma, 1, i2c_timeout);
            HAL_I2C_Mem_Write(hi2c, MAX_Write_addr, LED2_PA, I2C_MEMADD_SIZE_8BIT, &LEDs_PA_50ma, 1, i2c_timeout);

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
    return 0;
}

uint8_t readData(I2C_HandleTypeDef *hi2c, MAX86150_samples *samples)
{
    uint8_t fifo_wr_ptr = 0;
    uint8_t fifo_rd_ptr = 0;
    uint8_t ovf_counter = 0;
    uint8_t num_samples_to_read = 0;
    uint8_t num_available_bytes = 0;

     // Step 1: Read FIFO_WR_PTR
    if (HAL_I2C_Mem_Read(hi2c, MAX_Read_addr, FIFO_Wr_ptr, I2C_MEMADD_SIZE_8BIT, &fifo_wr_ptr, 1, HAL_MAX_DELAY) != HAL_OK) {
        return 1;
    }
    // Step 2: Read FIFO_RD_PTR
    if (HAL_I2C_Mem_Read(hi2c, MAX_Read_addr, FIFO_Rd_ptr, I2C_MEMADD_SIZE_8BIT, &fifo_rd_ptr, 1, HAL_MAX_DELAY) != HAL_OK) {
        return 1;
    }
    // Step 3: Read OVF_COUNTER
    if (HAL_I2C_Mem_Read(hi2c, MAX_Read_addr, Overflow_cntr, I2C_MEMADD_SIZE_8BIT, &ovf_counter, 1, HAL_MAX_DELAY) != HAL_OK) {
        return 1;
    }
    // Step 4: Evaluate the number of available samples
    uint8_t available_samples = 0;
    if (ovf_counter == 0) {
        if (fifo_wr_ptr >= fifo_rd_ptr) {
            available_samples = fifo_wr_ptr - fifo_rd_ptr;
        } else {
            available_samples = FIFO_CAPACITY - fifo_rd_ptr + fifo_wr_ptr;
        }
    } else {
        available_samples = FIFO_CAPACITY; // Overflow occurred
    }

    num_available_bytes = available_samples * 9;  // Total bytes in FIFO
    if (num_available_bytes % 9 != 0) {
        uint8_t leftover_bytes = num_available_bytes % 9;

        // Option 1: Skip leftover bytes (adjust FIFO_RD_PTR)
        uint8_t new_fifo_rd_ptr = (fifo_rd_ptr + leftover_bytes) % FIFO_CAPACITY;
        if (HAL_I2C_Mem_Write(hi2c, MAX_Write_addr, FIFO_Rd_ptr, I2C_MEMADD_SIZE_8BIT, &new_fifo_rd_ptr, 1, HAL_MAX_DELAY) != HAL_OK) {
            return 1;  // Failed to adjust read pointer
        }

        // Recalculate the number of available samples
        available_samples = (num_available_bytes - leftover_bytes) / 9;
    }

    // Limit the number of samples to read to the available samples or the maximum buffer size
    num_samples_to_read = (available_samples < max_samples) ? available_samples : max_samples;

    // Step 5: Read the samples from the FIFO
    uint8_t samples_buffer[9];
    for (uint8_t i = 0; i < num_samples_to_read; i++)
    {
        if (HAL_I2C_Mem_Read(hi2c, MAX_Read_addr, FIFO_DATA, I2C_MEMADD_SIZE_8BIT, samples_buffer, 9, HAL_MAX_DELAY) != HAL_OK) {
            return i; // Return the number of samples successfully read
        }

        samples[i].led1 = (uint32_t)((samples_buffer[0] << 16) | (samples_buffer[1] << 8) | samples_buffer[2]);
        samples[i].led2 = (uint32_t)((samples_buffer[3] << 16) | (samples_buffer[4] << 8) | samples_buffer[5]);
        samples[i].ecg = (uint32_t)((samples_buffer[6] << 16) | (samples_buffer[7] << 8) | samples_buffer[8]);  
    }
    return 0;
}

