#ifndef MAX86150_H
#define MAX86150_H

#include <stdint.h>
#include <stm32f1xx_hal.h>

/*Sensor I2C Address*/
#define Read_addr   0xBD    //sensor read address
#define Write_addr  0xBC    // sensor write address

typedef struct
{
    uint8_t i2c_addr;
    uint8_t active
}



#endif