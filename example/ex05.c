/**
 * @file ice01.c
 * @author M Sadman Sakib(msakib@wisc.edu)
 * @brief
 * @version 0.1
 * @date 2023-08-25
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "../main.h"

#if defined(EX05)

/*****************************************************************************/
/* Macros                                                                    */
/*****************************************************************************/

/*****************************************************************************/
/* Global Variables                                                          */
/*****************************************************************************/
char APP_DESCRIPTION[] = "ECE353: Ex05 SysTick Timer";

/*****************************************************************************/
/* Function Declarations                                                     */
/*****************************************************************************/

/*****************************************************************************/
/* Function Definitions                                                      */
/*****************************************************************************/

/**
 * @brief
 * This function will initialize hardware resources
 *
 * - Enable the LEDs
 * - Enable the SysTick so that it expires every 5mS
 */
void peripheral_init(void)
{
    // Initialize the LEDs
    leds_init();

    systick_init(TICKS_MS_005);
}

/**
 * @brief
 * Using the SysTick timer, blink the red LED on/off every second.
 */
void main_app(void)
{
    uint8_t expire_count = 0;
    bool red_led_on = false;

    while(1){
        while(!systick_expired()){
            /*We wait...*/
        }
        expire_count = expire_count + 1;
    

    if(expire_count == 200){
 
      if (red_led_on){
        PORT_RGB_RED->OUT_CLR = MASK_RGB_RED;
        red_led_on = false;
      }else{
        PORT_RGB_RED->OUT_SET = MASK_RGB_RED;
        red_led_on = true;
      }
      expire_count = 0;
    }
  }
}
#endif