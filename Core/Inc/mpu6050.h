#ifndef INC_MPU6050_H_
#define INC_MPU6050_H_

#define DEVICE_ADDRESS 0x68

#define FS_GYRO_250  0
#define FS_GYRO_500  8
#define FS_GYRO_1000 9
#define FS_GYRO_2000 10

#define FS_ACC_2G  0
#define FS_ACC_4G  8
#define FS_ACC_8G  9
#define FS_ACC_16G  10

#define REF_CONFIG_GYRO 27
#define REF_CONFIG_ACC  28 
#define REF_CONFIG_CTRL 107
#define REF_DATA        59 

#define REF_DATA_GYR        67

#define ALPHA 0.98 // Adjust this value to tune the filter

void mpu6050_init();
void mpu6050_read_raw();
void mpu6050_read_converted();
void mpu6050_read_filtered();
void mpu6050_read_and_send_filtered();

#endif /* INC_MPU6050_H_ */
