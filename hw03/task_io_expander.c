#include "task_lcd.h"

#if defined(HW03A)

QueueHandle_t q_IO_Exp;

/**
 * @brief
 * This file receives a score from one of the FSM tasks.  This score represents the number
 * of matches that the user has found.
 *
 * If the score is equal to 1, turn on the topmost RED LED.
 * If the score is equal to 2, turn on the topmost RED LED and the topmost Orange LED
 * If the score is equal to 3, turn on the topmost RED LED, the topmost Orange LED, and the yellow LED
 * .
 * .
 * .
 *
 * @param param
 * Not Used
 */
void task_io_expander(void *param)
{
    /* Suppress warning for unused parameter */
    (void)param;
    uint8_t score;
    i2c_init();
    
    io_expander_set_configuration(0x80); // Set bit 7 as input, bits 6-0 as outputs
    io_expander_set_output_port(0x00);   // Turn OFF all LEDs(initialize)
    /* Repeatedly running part of the task */
    for (;;)
    {
        /* wait to receive score through Queue */
        xQueueReceive(q_IO_Exp, &score, portMAX_DELAY);      
        // Set the LEDs based on the score
        if (score == 0) // For resetting
        {
            io_expander_set_output_port(0x00);
        }
        else if (score == 1)
        {
            io_expander_set_output_port(0x01); // Turn on topmost RED LED
        }
        else if (score == 2)
        {
            io_expander_set_output_port(0x03); // Turn on topmost RED and Orange LEDs
        }
        else if (score == 3)
        {
            io_expander_set_output_port(0x07); // Turn on topmost RED, Orange, and Yellow LEDs
        }
        else if (score == 4)
        {
            io_expander_set_output_port(0x0F); // Turn on next topmost LED
        }
        else if (score == 5)
        {
            io_expander_set_output_port(0x1F); // Turn on next topmost LED
        }
        else if (score == 6)
        {
            io_expander_set_output_port(0x3F); // Turn on next topmost LED
        }
        else if (score == 7)
        {
            io_expander_set_output_port(0x7F); // Turn on all LEDs
        }
    }
}

void task_io_expander_init(void)
{
    
    /* Create I)+expander Queues */
    q_IO_Exp = xQueueCreate(1, sizeof(u_int8_t));

    // Register the Task with FreeRTOS
    xTaskCreate(
        task_io_expander,
        "Task IO_Expander",
        configMINIMAL_STACK_SIZE,
        NULL,
        HW03_TASK_PRIORITY_UI_OUTPUT,
        NULL);
}

#endif
