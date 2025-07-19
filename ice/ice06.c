/**
 * @file ice07.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-08-25
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "../main.h"

#if defined(ICE06)

/*****************************************************************************/
/*  Custom Data Types                                                        */
/*****************************************************************************/

/*****************************************************************************/
/*  Global Variables                                                         */
/*****************************************************************************/
char APP_DESCRIPTION[] = "ECE353: ICE06 - PWM";

/**
 * @brief
 * This function will initialize all of the hardware resources for
 * the ICE
 */
void peripheral_init(void)
{
    /* This function needs to be written as part of the Ex07 PWM video*/
    leds_pwm_init();
}
/*****************************************************************************/
/* Application Code                                                          */
/*****************************************************************************/
static void ice06_update_duty_cycle(cyhal_pwm_t *pwm_obj, uint8_t duty_cycle)
{
    cy_rslt_t rslt = CY_RSLT_SUCCESS;

    /* Stop the PWM generation */
    /* ADD CODE */
     rslt = cyhal_pwm_stop(pwm_obj);

    CY_ASSERT(rslt == CY_RSLT_SUCCESS); // If the initialization, halt the MCU

    /* Change the Duty Cycle */
    /* ADD CODE */
     rslt = cyhal_pwm_set_duty_cycle(pwm_obj, duty_cycle, 2000);


    /* Start the PWM generation */
    /* ADD CODE */
    rslt = cyhal_pwm_start(pwm_obj);

    CY_ASSERT(rslt == CY_RSLT_SUCCESS); // If the initialization, halt the MCU
}

/**
 * @brief
 * This function implements the behavioral requirements for the ICE
 */
void main_app(void)
{
    uint8_t red_duty_cycle = 0;
    uint8_t green_duty_cycle = 0;
    uint8_t blue_duty_cycle = 0;

    while (1)
    {
        while(red_duty_cycle <= 100)
        {
            /* ADD CODE Set the duty cycle for the red LED */
            
            ice06_update_duty_cycle(&pwm_red_obj, red_duty_cycle);
  //          PORT_RGB_RED->OUT_SET = MASK_RGB_RED; 
            
            printf("Red %02d, Green %02d, Blue %02d\n\r", red_duty_cycle, green_duty_cycle, blue_duty_cycle);

            cyhal_system_delay_ms(200);

            /* ADD CODE increment the red duty cycle by 10 */
    //       ice06_update_duty_cycle(&pwm_red_obj, red_duty_cycle + 10);
            red_duty_cycle = red_duty_cycle + 10;
        }

        /*ADD CODE Turn off the red LED */
        PORT_RGB_RED->OUT_CLR = MASK_RGB_RED;

        while(green_duty_cycle <= 100)
        {
            /* ADD CODE Set the duty cycle for the green LED */
             ice06_update_duty_cycle(&pwm_green_obj, green_duty_cycle);
   //          PORT_RGB_GRN->OUT_SET = MASK_RGB_GRN;
            
            printf("Red %02d, Green %02d, Blue %02d\n\r", red_duty_cycle, green_duty_cycle, blue_duty_cycle);

            cyhal_system_delay_ms(200);

            /* ADD CODE increment the green duty cycle by 10 */
       //     ice06_update_duty_cycle(&pwm_green_obj, red_duty_cycle + 10); 
            green_duty_cycle = green_duty_cycle + 10;
        }

        /*ADD CODE Turn off the green LED */
        PORT_RGB_GRN->OUT_CLR = MASK_RGB_GRN;

        while(blue_duty_cycle <= 100)
        {
            /* ADD CODE Set the duty cycle for the blue LED */
            ice06_update_duty_cycle(&pwm_blue_obj, blue_duty_cycle);
   //         PORT_RGB_BLU->OUT_SET = MASK_RGB_BLU;
            
            printf("Red %02d, Green %02d, Blue %02d\n\r", red_duty_cycle, green_duty_cycle, blue_duty_cycle);

            cyhal_system_delay_ms(200);

            /* ADD CODE increment the blue duty cycle by 10 */
       //     ice06_update_duty_cycle(&pwm_blue_obj, blue_duty_cycle + 10);
       blue_duty_cycle = blue_duty_cycle + 10;
        }

        /*ADD CODE Turn off the blue LED */
         PORT_RGB_BLU->OUT_CLR = MASK_RGB_BLU;
    }
}

#endif