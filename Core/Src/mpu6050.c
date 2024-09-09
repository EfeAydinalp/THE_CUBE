#include <mpu6050.h>
#include <stdio.h>
#include "main.h"
extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart2;
#include <string.h>
#include <math.h>
//accel_x/y/z_filtered and gyro_x/y/z_filtered values needed for filtered data part.
float accel_x_filtered = 0.0, accel_y_filtered = 0.0, accel_z_filtered = 0.0;
float gyro_x_filtered = 0.0, gyro_y_filtered = 0.0, gyro_z_filtered = 0.0;
//previous_accel_x/y/z adn previous_gyro_x/y/z values needed for filtered data part.
float previous_accel_x = 0.0, previous_accel_y = 0.0, previous_accel_z = 0.0;
float previous_gyro_x = 0.0, previous_gyro_y = 0.0, previous_gyro_z = 0.0;

//Initializes the MPU6050 sensor by configuring its registers and taking it out of sleep mode.
void mpu6050_init()
{
	//HAL_StatusTypeDef HAL_I2C_IsDeviceReady(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint32_t Trials, uint32_t Timeout);
	HAL_StatusTypeDef ret = HAL_I2C_IsDeviceReady(&hi2c1, DEVICE_ADDRESS <<1, 1, 100);
	if(ret == HAL_OK)
	{
		printf("  MPU6050 is ON!\n");
	}
	else
	{
		printf(" Something went wrong. Check the cables or device please.\n");
	}
	uint8_t temp_data = FS_GYRO_500;
	ret = HAL_I2C_Mem_Write(&hi2c1, DEVICE_ADDRESS<<1, REF_CONFIG_GYRO, 1, &temp_data, 1, 100);
	//HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
	//from stm32f4xx_hal_i2c.h file
	if(ret == HAL_OK)
	{
		printf(" 27th register is written correctly and device is configurated.\n");
	}
	else
	{
		printf(" Register Problem\n");
	}
	//Acceleratior part
	temp_data = FS_ACC_4G;
	ret = HAL_I2C_Mem_Write(&hi2c1, DEVICE_ADDRESS<<1, REF_CONFIG_ACC, 1, &temp_data, 1, 100);
	//HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
	//from stm32f4xx_hal_i2c.h file
	if(ret == HAL_OK)
	{
		printf(" 28th register is written correctly and device is configurated.\n");
	}
	else
	{
		printf(" Register Problem\n");
	}
	//Sleep Mode
	temp_data = 0;
	ret = HAL_I2C_Mem_Write(&hi2c1, DEVICE_ADDRESS<<1, REF_CONFIG_CTRL, 1, &temp_data, 1, 100);
	//HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
	//from stm32f4xx_hal_i2c.h file
	if(ret == HAL_OK)
	{
		printf(" Woke up. Ready to humble\n");
	}
	else
	{
		printf(" ZzZzZzZzZzZ...\n");
	}
}
//Reads raw data from the MPU6050 sensor.
void mpu6050_read_raw()
{
	uint8_t datax[2];
	int16_t raw_accel_x;
	HAL_I2C_Mem_Read(&hi2c1, (DEVICE_ADDRESS<<1)+1, REF_DATA, 1, datax, 2, 100);
	raw_accel_x = ((int16_t)datax[0]<<8)+datax[1];

	uint8_t datay[2];
	int16_t raw_accel_y;
	HAL_I2C_Mem_Read(&hi2c1, (DEVICE_ADDRESS<<1)+1, REF_DATA+2, 1, datay, 2, 100);
	raw_accel_y = ((int16_t)datay[0]<<8)+datay[1];

	uint8_t dataz[2];
	int16_t raw_accel_z;
	HAL_I2C_Mem_Read(&hi2c1, (DEVICE_ADDRESS<<1)+1, REF_DATA+4, 1, dataz, 2, 100);
	raw_accel_z = ((int16_t)dataz[0]<<8)+dataz[1];

	int16_t raw_gyro_x;
	HAL_I2C_Mem_Read(&hi2c1, (DEVICE_ADDRESS<<1)+1, REF_DATA_GYR, 1, datax, 2, 100);
	raw_gyro_x = ((int16_t)datax[0]<<8)+datax[1];

	int16_t raw_gyro_y;
	HAL_I2C_Mem_Read(&hi2c1, (DEVICE_ADDRESS<<1)+1, REF_DATA_GYR+2, 1, datay, 2, 100);
	raw_gyro_y = ((int16_t)datay[0]<<8)+datay[1];

	int16_t raw_gyro_z;
	HAL_I2C_Mem_Read(&hi2c1, (DEVICE_ADDRESS<<1)+1, REF_DATA_GYR+4, 1, dataz, 2, 100);
	raw_gyro_z = ((int16_t)dataz[0]<<8)+dataz[1];

	printf("raw_accel_x =%d  ",raw_accel_x);
	printf("raw_accel_y =%d  ",raw_accel_y);
	printf("raw_accel_z =%d  |  ",raw_accel_z);

	printf("raw_x_gyr =%d  ",raw_gyro_x);
	printf("raw_gyro_y =%d  ",raw_gyro_y);
	printf("raw_gyro_z =%d  \n",raw_gyro_z);
}
//Reads and converts raw data from the MPU6050 sensor into usable units.
void mpu6050_read_converted()
{
	uint8_t datax[2];
	int16_t raw_accel_x;
	HAL_I2C_Mem_Read(&hi2c1, (DEVICE_ADDRESS<<1)+1, REF_DATA, 1, datax, 2, 100);
	raw_accel_x = ((int16_t)datax[0]<<8)+datax[1];

	uint8_t datay[2];
	int16_t raw_accel_y;
	HAL_I2C_Mem_Read(&hi2c1, (DEVICE_ADDRESS<<1)+1, REF_DATA+2, 1, datay, 2, 100);
	raw_accel_y = ((int16_t)datay[0]<<8)+datay[1];

	uint8_t dataz[2];
	int16_t raw_accel_z;
	HAL_I2C_Mem_Read(&hi2c1, (DEVICE_ADDRESS<<1)+1, REF_DATA+4, 1, dataz, 2, 100);
	raw_accel_z = ((int16_t)dataz[0]<<8)+dataz[1];

	int16_t raw_gyro_x;
	HAL_I2C_Mem_Read(&hi2c1, (DEVICE_ADDRESS<<1)+1, REF_DATA_GYR, 1, datax, 2, 100);
	raw_gyro_x = ((int16_t)datax[0]<<8)+datax[1];


	int16_t raw_gyro_y;
	HAL_I2C_Mem_Read(&hi2c1, (DEVICE_ADDRESS<<1)+1, REF_DATA_GYR+2, 1, datay, 2, 100);
	raw_gyro_y = ((int16_t)datay[0]<<8)+datay[1];


	int16_t raw_gyro_z;
	HAL_I2C_Mem_Read(&hi2c1, (DEVICE_ADDRESS<<1)+1, REF_DATA_GYR+4, 1, dataz, 2, 100);
	raw_gyro_z = ((int16_t)dataz[0]<<8)+dataz[1];

	float converted_accel_x = (float)raw_accel_x / 16384.0;
	float converted_accel_y = (float)raw_accel_y / 16384.0;
	float converted_accel_z = (float)raw_accel_z / 16384.0;

	float converted_gyro_x = (float)raw_gyro_x / 131.0;
	float converted_gyro_y = (float)raw_gyro_y / 131.0;
	float converted_gyro_z = (float)raw_gyro_z / 131.0;

	printf("converted_accel_x =%.2f  ",converted_accel_x);
	printf("converted_accel_y =%.2f  ",converted_accel_y);
	printf("converted_accel_z =%.2f  |  ",converted_accel_z);

	printf("converted_gyro_x =%.2f  ",converted_gyro_x);
	printf("converted_gyro_y =%.2f  ",converted_gyro_y);
	printf("converted_gyro_z =%.2f  \n",converted_gyro_z);
}
//Reads, converts, and applies a low-pass filter to the MPU6050 sensor data
void mpu6050_read_filtered()
{
	uint8_t datax[2];
	int16_t raw_accel_x;
	HAL_I2C_Mem_Read(&hi2c1, (DEVICE_ADDRESS<<1)+1, REF_DATA, 1, datax, 2, 100);
	raw_accel_x = ((int16_t)datax[0]<<8)+datax[1];

	uint8_t datay[2];
	int16_t raw_accel_y;
	HAL_I2C_Mem_Read(&hi2c1, (DEVICE_ADDRESS<<1)+1, REF_DATA+2, 1, datay, 2, 100);
	raw_accel_y = ((int16_t)datay[0]<<8)+datay[1];

	uint8_t dataz[2];
	int16_t raw_accel_z;
	HAL_I2C_Mem_Read(&hi2c1, (DEVICE_ADDRESS<<1)+1, REF_DATA+4, 1, dataz, 2, 100);
	raw_accel_z = ((int16_t)dataz[0]<<8)+dataz[1];

	int16_t raw_gyro_x;
	HAL_I2C_Mem_Read(&hi2c1, (DEVICE_ADDRESS<<1)+1, REF_DATA_GYR, 1, datax, 2, 100);
	raw_gyro_x = ((int16_t)datax[0]<<8)+datax[1];

	int16_t raw_gyro_y;
	HAL_I2C_Mem_Read(&hi2c1, (DEVICE_ADDRESS<<1)+1, REF_DATA_GYR+2, 1, datay, 2, 100);
	raw_gyro_y = ((int16_t)datay[0]<<8)+datay[1];

	int16_t raw_gyro_z;
	HAL_I2C_Mem_Read(&hi2c1, (DEVICE_ADDRESS<<1)+1, REF_DATA_GYR+4, 1, dataz, 2, 100);
	raw_gyro_z = ((int16_t)dataz[0]<<8)+dataz[1];

	float converted_accel_x = (float)raw_accel_x / 16384.0;
	float converted_accel_y = (float)raw_accel_y / 16384.0;
	float converted_accel_z = (float)raw_accel_z / 16384.0;

	float converted_gyro_x = (float)raw_gyro_x / 131.0;
	float converted_gyro_y = (float)raw_gyro_y / 131.0;
	float converted_gyro_z = (float)raw_gyro_z / 131.0;

	accel_x_filtered = ALPHA * converted_accel_x + (1.0 - ALPHA) * previous_accel_x;
	accel_y_filtered = ALPHA * converted_accel_y + (1.0 - ALPHA) * previous_accel_y;
	accel_z_filtered = ALPHA * converted_accel_z + (1.0 - ALPHA) * previous_accel_z;

	gyro_x_filtered = ALPHA * converted_gyro_x + (1.0 - ALPHA) * previous_gyro_x;
	gyro_y_filtered = ALPHA * converted_gyro_y + (1.0 - ALPHA) * previous_gyro_y;
	gyro_z_filtered = ALPHA * converted_gyro_z + (1.0 - ALPHA) * previous_gyro_z;

    // Update for the previous values with the current ones for the next iteration
    previous_accel_x = accel_x_filtered;
    previous_accel_y = accel_y_filtered;
    previous_accel_z = accel_z_filtered;

    previous_gyro_x = gyro_x_filtered;
    previous_gyro_y = gyro_y_filtered;
    previous_gyro_z = gyro_z_filtered;

	printf("accel_x_filtered =%.2f  ",accel_x_filtered);
	printf("accel_y_filtered =%.2f  ",accel_y_filtered);
	printf("accel_z_filtered =%.2f  |  ",accel_z_filtered);

	printf("gyro_x_filtered =%.2f  ",gyro_x_filtered);
	printf("gyro_y_filtered =%.2f  ",gyro_y_filtered);
	printf("gyro_z_filtered =%.2f  \n",gyro_z_filtered);
}
//Initializes the UART peripheral for communication.
void MX_USART2_UART_Init(void)
{
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        Error_Handler();
    }
}
//Reads filtered data from the MPU6050 and sends it via UART.
void mpu6050_read_and_send_filtered()
{
    uint8_t data[14];
    int16_t raw_accel_x, raw_accel_y, raw_accel_z;
    int16_t raw_gyro_x, raw_gyro_y, raw_gyro_z;
    // Read accelerometer and gyroscope data
    HAL_I2C_Mem_Read(&hi2c1, (DEVICE_ADDRESS << 1) + 1, REF_DATA, 1, data, 14, 100);
    // Combine the data bytes into integers
    raw_accel_x = (int16_t)(data[0] << 8 | data[1]);
    raw_accel_y = (int16_t)(data[2] << 8 | data[3]);
    raw_accel_z = (int16_t)(data[4] << 8 | data[5]);

    raw_gyro_x = (int16_t)(data[8] << 8 | data[9]);
    raw_gyro_y = (int16_t)(data[10] << 8 | data[11]);
    raw_gyro_z = (int16_t)(data[12] << 8 | data[13]);
    // Convert the raw data to meaningful values
    float converted_accel_x = (float)raw_accel_x / 16384.0;
    float converted_accel_y = (float)raw_accel_y / 16384.0;
    float converted_accel_z = (float)raw_accel_z / 16384.0;

    float converted_gyro_x = (float)raw_gyro_x / 131.0;
    float converted_gyro_y = (float)raw_gyro_y / 131.0;
    float converted_gyro_z = (float)raw_gyro_z / 131.0;
    // Apply filtering (Simple Low-Pass Filter)
    accel_x_filtered = ALPHA * converted_accel_x + (1.0 - ALPHA) * previous_accel_x;
    accel_y_filtered = ALPHA * converted_accel_y + (1.0 - ALPHA) * previous_accel_y;
    accel_z_filtered = ALPHA * converted_accel_z + (1.0 - ALPHA) * previous_accel_z;

    gyro_x_filtered = ALPHA * converted_gyro_x + (1.0 - ALPHA) * previous_gyro_x;
    gyro_y_filtered = ALPHA * converted_gyro_y + (1.0 - ALPHA) * previous_gyro_y;
    gyro_z_filtered = ALPHA * converted_gyro_z + (1.0 - ALPHA) * previous_gyro_z;
    // Update previous values for the next iteration
    previous_accel_x = accel_x_filtered;
    previous_accel_y = accel_y_filtered;
    previous_accel_z = accel_z_filtered;

    previous_gyro_x = gyro_x_filtered;
    previous_gyro_y = gyro_y_filtered;
    previous_gyro_z = gyro_z_filtered;
    // Create a string with the filtered data
    char uart_buffer[100];
    snprintf(uart_buffer, sizeof(uart_buffer), "AX=%.2f AY=%.2f AZ=%.2f GX=%.2f GY=%.2f GZ=%.2f\r\n",
             accel_x_filtered, accel_y_filtered, accel_z_filtered,
             gyro_x_filtered, gyro_y_filtered, gyro_z_filtered);
    // Send the data via UART
    HAL_UART_Transmit(&huart2, (uint8_t*)uart_buffer, strlen(uart_buffer), HAL_MAX_DELAY);
}
