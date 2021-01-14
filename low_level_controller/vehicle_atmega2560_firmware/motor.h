// MIT License
// 
// Copyright (c) 2020 Lehrstuhl Informatik 11 - RWTH Aachen University
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// 
// This file is part of cpm_lab.
// 
// Author: i11 - Embedded Software, RWTH Aachen University

/*
 * motor.h
 *
 * Created: 20.09.2018 10:02:02
 *  Author: maczijewski
 */ 


#ifndef MOTOR_H_
#define MOTOR_H_


#include <stdint.h>


#define MOTOR_DIRECTION_BRAKE 0
#define MOTOR_DIRECTION_FORWARD 1
#define MOTOR_DIRECTION_REVERSE 2

/**
 * \brief TODO
 * \param direction
 * 
 * \author maczijewski
 * \ingroup low_level_controller
 */
void motor_set_direction(uint8_t direction);

/**
 * \brief TODO
 * \param duty
 * 
 * \author maczijewski
 * \ingroup low_level_controller
 */
void motor_set_duty(uint16_t duty); // values from 0 to 400

/**
 * \brief TODO
 * 
 * \author maczijewski
 * \ingroup low_level_controller
 */
void motor_setup();


#endif /* MOTOR_H_ */