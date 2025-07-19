/**
 * @file systick.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-09-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "systick.h"

/**
 * @brief 
 *  Initilaizes the SysTick Timer for a configurable interval of time. 
 * @param ticks 
 *  The Number clock cycles 
 */
bool systick_init(uint32_t ticks)
{
    if(ticks > 0xFFFFFF){
    // Systick tiemr is only 24-bits, so return an error
    return false;
    }
    //Turn off the Systick Timer
    SysTick->CTRL = 0;

    //set the current value to 0
     SysTick->VAL = 0;

    //set the period
     SysTick->LOAD = ticks -1;

    //set the clock source to the main CPU clock and turn the timer on
    SysTick->CTRL = SYSTICK_CTRL_CLKSOURCE | SYSTICK_CTRL_ENABLE;

    return true;
}


bool systick_expired(void)
{
    if(SysTick->CTRL & SYSTICK_CTRL_COUNTFLAG){
        return true;
    }else{
        return false;
    }
}