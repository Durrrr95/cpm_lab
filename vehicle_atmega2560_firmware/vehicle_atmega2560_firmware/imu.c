/*
 * imu.c
 *
 * Created: 27.09.2018 10:54:21
 *  Author: maczijewski
 */ 

#define BNO055_ADDRESS (0x28)

#define BNO055_CHIP_ID      (0xA0)
#define BNO055_EULER_H_LSB_ADDR (0x1A)
#define BNO055_EULER_H_MSB_ADDR (0x1B)
#define BNO055_OPR_MODE_ADDR     0X3D
#define BNO055_PWR_MODE_ADDR     0X3E
#define BNO055_POWER_MODE_NORMAL        0X00
#define BNO055_OPERATION_MODE_NDOF      0X0C
#define BNO055_LINEAR_ACCEL_DATA_X_LSB_ADDR  0X28
#define BNO055_ACCEL_DATA_X_LSB_ADDR         0X08


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include "twi.h"
#include "util.h"

bool imu_setup() {
	
	uint8_t buffer[10];
	uint8_t status = 0;
	
	
	// TODO reset the IMU, because it may already have been setup, if this ATmega was reset independently.
	
	_delay_ms(10);
	
	// check chip ID
	uint8_t imu_chip_id = 0;
	uint8_t bytes_read = 0;
	bytes_read = twi_readFrom(BNO055_ADDRESS, &imu_chip_id, 1, true);	
	if(bytes_read != 1 || imu_chip_id != BNO055_CHIP_ID) {
		return false;
	}
	
	_delay_ms(10);
	
	// set power mode
	buffer[0] = BNO055_PWR_MODE_ADDR;
	buffer[1] = BNO055_POWER_MODE_NORMAL;
	status = twi_writeTo(BNO055_ADDRESS, buffer, 2, true, true);
	if(status != 0) {
		return false;
	}
	
	_delay_ms(10);
	
	// set operation mode
	buffer[0] = BNO055_OPR_MODE_ADDR;
	buffer[1] = BNO055_OPERATION_MODE_NDOF;
	status = twi_writeTo(BNO055_ADDRESS, buffer, 2, true, true);
	if(status != 0) {
		return false;
	}
	
	
	return true;
	
}

bool imu_read(uint16_t* imu_yaw, int16_t* imu_acceleration_forward, int16_t* imu_acceleration_left) {
	
	bool success_flag = true;
	uint8_t buffer[10];
	
	// read yaw
	buffer[0] = BNO055_EULER_H_LSB_ADDR;
	if(twi_writeTo(BNO055_ADDRESS, buffer, 1, true, false) != 0) success_flag = false;
	if(twi_readFrom(BNO055_ADDRESS, buffer, 2, true) != 2) success_flag = false;
	*imu_yaw = *((uint16_t*)(buffer));
	 
	_delay_us(50);
	
	// read acceleration
	buffer[0] = BNO055_ACCEL_DATA_X_LSB_ADDR;
	if(twi_writeTo(BNO055_ADDRESS, buffer, 1, true, false) != 0) success_flag = false;
	if(twi_readFrom(BNO055_ADDRESS, buffer, 4, true) != 4) success_flag = false;
	
	*imu_acceleration_left = -*((int16_t*)(buffer)); // TODO update axis mapping when the PCB arrives
	*imu_acceleration_forward = *((int16_t*)(buffer+2));
	
	return success_flag;
}