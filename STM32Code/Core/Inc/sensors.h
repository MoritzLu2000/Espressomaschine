#ifndef SENSORS_H
#define SENSORS_H

#include "main.h"

void    MAX31865_Init(SPI_HandleTypeDef *hspi);
float   MAX31865_ReadTemperature(SPI_HandleTypeDef *hspi);
uint8_t MAX31865_ReadFault(SPI_HandleTypeDef *hspi);

#endif /* SENSORS_H */
