// main.h
// Josh Brake
// jbrake@hmc.edu
// 10/31/22 

#ifndef MAIN_H
#define MAIN_H
#include <stdbool.h>

#include "STM32L432KC.h"
#include <stm32l432xx.h>

///////////////////////////////////////////////////////////////////////////////
// Custom defines
///////////////////////////////////////////////////////////////////////////////

#define ENCODER_A PA2
#define ENCODER_B PA3
#define UPDATE_TIM TIM2
#define REPORT_TIM TIM15

#endif // MAIN_H