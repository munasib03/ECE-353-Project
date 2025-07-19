/**
 * @file ice05.c
 * @author
 * @brief
 * @version 0.1
 * @date 2023-08-25
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "../main.h"

#if defined(ICE05)

/*****************************************************************************/
/* Macros                                                                    */
/*****************************************************************************/

/*****************************************************************************/
/* Global Variables                                                          */
/*****************************************************************************/
char APP_DESCRIPTION[] = "ECE353: ICE 05 - SysTick";

/*****************************************************************************/
/* Function Declarations                                                     */
/*****************************************************************************/

/*****************************************************************************/
/* Function Definitions                                                      */
/*****************************************************************************/

/**
 * @brief
 * This function will initialize all of the hardware resources for
 * the ICE
 */
void peripheral_init(void)
{
    /* Initialize the LEDs */
    leds_init();

    /* Set the SysTick Timer to that it  expires every 100uS*/
    /* ADD CODE */
    systick_init(TICKS_US_100);
}

/**
 * @brief
 * This function implements the behavioral requirements for the ICE
 */
void main_app(void)
{
    uint8_t timer_count = 0;
    uint8_t on_threshold = 50;

    for (;;)
    {
        /* Check to see if the systick timer has expired */
        if (systick_expired())
        {
            /* ADD CODE */
            timer_count = (timer_count + 1) % 100;
        }
        
        /* Determine if the Red LED should be on or off */
        /* ADD CODE */
        
 
            if (timer_count < on_threshold){
                PORT_RGB_RED->OUT_SET = MASK_RGB_RED;
            
            }else if (timer_count > on_threshold){
                PORT_RGB_RED->OUT_CLR = MASK_RGB_RED;
                
            }
            
        
    }
}
#endif
