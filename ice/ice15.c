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

#if defined(ICE15)

char APP_DESCRIPTION[] = "ECE353: ICE 15 - FreeRTOS";

TaskHandle_t Task_Handle_Button_SW1 = NULL;
TaskHandle_t Task_Handle_Button_SW2 = NULL;

/*****************************************************************************/
/* Function Declarations                                                     */
/*****************************************************************************/
void task_button_sw1(void *pvParameters);
void task_button_sw2(void *pvParameters);

/*****************************************************************************/
/* Peripheral Initialization                                                  */
/*****************************************************************************/

/**
 * @brief
 * This function will initialize all of the hardware resources for
 * the ICE
 */
void peripheral_init(void)
{
    /* Initialize the LEDs that will be controlled by the FreeRTOS Tasks*/
    leds_init();

    /* Initialize the buttons */
    push_buttons_init(false);

    /* Initialize the Buzzer */
    pwm_buzzer_init();
}

/*****************************************************************************/
/* Application Code                                                          */
/*****************************************************************************/
/**
 * @brief
 * This function implements the behavioral requirements for the ICE
 */
void main_app(void)
{
    /* ADD CODE */
    /* Create SW1 Task */
    xTaskCreate
    (
     task_button_sw1,
     "Switch 1 Task",
     configMINIMAL_STACK_SIZE,
     NULL,
     2,
     NULL
    );

    /* ADD CODE */
    /* Create SW2 Task */

     xTaskCreate
    (
     task_button_sw2,
     "Switch 2 Task",
     configMINIMAL_STACK_SIZE,
     NULL,
     2,
     NULL
    );

    /* Start the Scheduler */
    vTaskStartScheduler();

    while (1)
    {
    };
}

/**
 * @brief
 * This task will turn the buzzer on/off by detecting when SW1 has been pressed. This task
 * should sleep for 50mS.  When it is active, check the current state of SW1 and compare it with
 * the previous value of SW1.  If you detect that the button has been pressed, then toggle the
 * buzzer On/Off
 * @param pvParameters
 */
void task_button_sw1(void *pvParameters)
{
    /* ADD CODE */
    /* Allocate any local variables used in this task */
    bool buzzer_status = false;
    button_state_t sw1_curr_state = BUTTON_INACTIVE;
    button_state_t sw1_prev_state = BUTTON_INACTIVE;
    while (1)
    {
        /* ADD CODE */
        /* Sleep for 50mS -- DO NOT use any cyhal_ functions to delay */
        vTaskDelay(50);  
        /* ADD CODE */
        /* Check the current state of the button */
        if ((PORT_BUTTONS->IN & SW1_MASK) == 0x00)
        {
            sw1_curr_state = BUTTON_PRESSED;
        }
        else
        {
            sw1_curr_state = BUTTON_INACTIVE;
        }

        /* ADD CODE */
        /* Detect if the button has been pressed */
        if ((sw1_curr_state == BUTTON_PRESSED) && (sw1_prev_state == BUTTON_INACTIVE) /*Check the current and previous states of the button*/)
        {
            /* ADD CODE */
            /* Determine if the buzzer should be started or stopped*/
            if(buzzer_status == false){
                pwm_buzzer_start();
                buzzer_status = true;
            }else if(buzzer_status == true){
                pwm_buzzer_stop();
                buzzer_status = false;
            }
             
        }

        /* ADD CODE */
        /* Update the previous button state */
        sw1_prev_state = sw1_curr_state;
    }
}

/**
 * @brief
 * This task will turn the green on/off by detecting when SW2 has been pressed. This task
 * should sleep for 50mS.  When it is active, check the current state of SW1 and compare it with
 * the previous value of SW2.  If you detect that the button has been pressed, then toggle the
 * Green LED On/Off
 * @param pvParameters
 */

void task_button_sw2(void *pvParameters)
{
    /* ADD CODE */
    /* Allocate any local variables used in this task */
    button_state_t sw2_curr_state = BUTTON_INACTIVE;
    button_state_t sw2_prev_state = BUTTON_INACTIVE;
    bool Green_LED_status = false;
    while (1)
    {
        /* ADD CODE */
        /* Sleep for 50mS -- DO NOT use any cyhal_ functions to delay */
        vTaskDelay(50);  
        /* ADD CODE */
        /* Check the current state of the button */
        if ((PORT_BUTTONS->IN & SW2_MASK) == 0x00)
        {
            sw2_curr_state = BUTTON_PRESSED;
        }
        else
        {
            sw2_curr_state = BUTTON_INACTIVE;
        }

        /* ADD CODE */
        /* Detect if the button has been pressed */
        if ((sw2_curr_state == BUTTON_PRESSED) && (sw2_prev_state == BUTTON_INACTIVE) /*Check the current and previous states of the button*/)
        {
            /* ADD CODE */
            /* Toggle the Green LED */
            if(Green_LED_status == false){
                PORT_RGB_GRN->OUT_SET = MASK_RGB_GRN;
                Green_LED_status = true;
            }else if(Green_LED_status == true){
                PORT_RGB_GRN->OUT_CLR = MASK_RGB_GRN;
                Green_LED_status = false;
            }
        }

        /* ADD CODE */
        /* Update the previous button state */
        sw2_prev_state = sw2_curr_state;
    }
}

//*****************************************************************************
//
//! \brief Application defined malloc failed hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void vApplicationMallocFailedHook()
{
    /* Handle Memory Allocation Errors */
    while (1)
    {
    }
}

//*****************************************************************************
//
//! \brief Application defined stack overflow hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
    // Handle FreeRTOS Stack Overflow
    while (1)
    {
    }
}

#endif