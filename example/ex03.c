/**
 * @file ice01.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-08-25
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "../main.h"

#if defined(EX03)

/*****************************************************************************/
/* Macros                                                                    */
/*****************************************************************************/

/*****************************************************************************/
/* Global Variables                                                          */
/*****************************************************************************/
char APP_DESCRIPTION[] = "ECE353: Ex03 PSoC6 IO Ports";

/*****************************************************************************/
/* Function Declarations                                                     */
/*****************************************************************************/

/*****************************************************************************/
/* Function Definitions                                                      */
/*****************************************************************************/

/**
 * @brief
 * This function will initialize hardware resources 
 */
void peripheral_init(void)
{
    cy_rslt_t rslt;

    /* Initialize SW1 P6_3 as an input */
     rslt = cyhal_gpio_init(PIN_SW1,CYHAL_GPIO_DIR_INPUT,CYHAL_GPIO_DRIVE_NONE, 0);
     CY_ASSERT (rslt == CY_RSLT_SUCCESS);
    /* Initialize Red LED P9_0 as an output*/
      rslt = cyhal_gpio_init(PIN_RGB_RED,CYHAL_GPIO_DIR_OUTPUT,CYHAL_GPIO_DRIVE_STRONG, 0);
     CY_ASSERT (rslt == CY_RSLT_SUCCESS);
}

/**
 * @brief
 * This function implements the behavioral requirements for the application
 */
void main_app(void)
{
/* Local vars that store the current and previous state of SW1 */
   bool sw1_curr = false;
   bool sw1_prev = false;

    while(1){
        /* Check the current value of SW1 */
           // if((GPIO_PRT6->IN & ( 1 << 3)) == 0)
           if( (PORT_BUTTONS->IN & SW1_MASK) == 0)
           {
                sw1_curr = true;
            }else{
                sw1_curr = false;
            }
        /* If the button was pressed (Falling Edge), turn red LED ON */
            if(sw1_curr && !sw1_prev){
                printf("SW1 Pressed\n\r");
                PORT_RGB_RED->OUT |= MASK_RGB_RED;
            }
        /* If the button was released, turn OFF LED */
             if(!sw1_curr && sw1_prev){
                printf("SW1 Released\n\r");
                PORT_RGB_RED->OUT &= ~MASK_RGB_RED;
            }
        /* Save the current state to the previous state */
             sw1_prev = sw1_curr;
        /* Delay for 100mS */
        cyhal_system_delay_ms(100);

    };
}
#endif