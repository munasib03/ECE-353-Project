/**
 * @file ice01.c
 * @author M Sadman Sakib (msakib@wisc.edu)
 * @brief
 * @version 0.1
 * @date 2023-08-25
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "../main.h"

#if defined(EX15)


char APP_DESCRIPTION[] = "ECE353: Example 15 - FreeRTOS";

/* ADD CODE */
/* Add Task Handles (TaskHandle_t) for Each Task */
TaskHandle_t Task_Handle_Led_Green = NULL;
TaskHandle_t Task_Handle_Led_Red = NULL;

/*****************************************************************************/
/* Function Declarations                                                     */
/*****************************************************************************/
void task_led_green(void *pvParameters);
void task_led_red(void *pvParameters);

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
    /* Create the tasks that will toggle the Red and Green LEDs*/
    xTaskCreate
    (
     task_led_green,
     "Green LED Task",
     configMINIMAL_STACK_SIZE,
     NULL,
     2,
     NULL
    );

    xTaskCreate
    (
     task_led_red,
     "RED LED Task",
     configMINIMAL_STACK_SIZE,
     NULL,
     2,
     NULL
    );
    /* Start the Scheduler */
    vTaskStartScheduler();

    while(1)
    {
     CY_ASSERT(0);

    };
}

/**
 * @brief
 *  This task will blink the Green LED on/off at a rate of 0.5Hz
 * @param pvParameters
 */
void task_led_green(void *pvParameters)
{
    /* ADD CODE */
    /* Define behavior of the green LED Task */
    while(1)
    {
     PORT_RGB_GRN->OUT_SET = MASK_RGB_GRN; // Turn LED on
     vTaskDelay(250);                      // Sleep for 250 ms
     PORT_RGB_GRN->OUT_CLR = MASK_RGB_GRN; // Turn LED off
     vTaskDelay(250);                      // Sleep for 250 ms
    }
}

/**
 * @brief
 *  This task will blink the Red LED on/off at a rate of 0.75Hz
 * @param pvParameters
 */

void task_led_red(void *pvParameters)
{
    /* ADD CODE */
    /* Define behavior of the red LED Task */
    while(1)
    {
     PORT_RGB_GRN->OUT_SET = MASK_RGB_RED; // Turn LED on
     vTaskDelay(375);                      // Sleep for 375 ms
     PORT_RGB_GRN->OUT_CLR = MASK_RGB_RED; // Turn LED off
     vTaskDelay(375);                      // Sleep for 375 ms
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
