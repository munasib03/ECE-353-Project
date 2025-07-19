#include "task_state_game_over.h"

#if defined(HW03A)

TaskHandle_t Task_Handle_State_Game_Over;

/**
 * @brief
 * 
 * The GAME OVER task will sleep (blocked state) until a Task Notification is received.
 * 
 * The GAME OVER Task will display the game over image to the user by sending a Square_t structure
 * to the LCD Task where the image_type field type is set to  MEM_IMAGE_GAME_OVER
 *
 * The GAME OVER task also monitors eg_UI for EVENT_UI_SW2 and EVENT_IPC_RX_RESTART.
 *
 * If EVENT_UI_SW2 is detected
 *    1. The GAME OVER task will clear the LCD 
 *    2. The GAME OVER task will transmit REMOTE_CMD_RESTART to the remote board
 *    3. The GAME OVER task will wait for REMOTE_CMD_RESTART to be ACKED
 *    4. The GAME OVER task will send a Task Notification to the INIT SEND Task
 *    5. The GAME OVER task will move into a blocked state waiting for a Task Notification
 * 
 * If EVENT_IPC_RX_RESTART is detected, the GAME OVER task will clear the LCD and wake up the INIT RECEIVE task.
 *    1. The GAME OVER task will clear the LCD 
 *    2. The GAME OVER task will transmit REMOTE_CMD_ACK to the remote board
 *    3. The GAME OVER task will send a Task Notification to the INIT RECEIVE Task
 *    4. The GAME OVER task will move into a blocked state waiting for a Task Notification
 *
 * @param param
 */
void task_state_game_over(void *param)
{
  /* Suppress warning for unused parameter */
  (void)param;

  /* Repeatedly running part of the task */
  for (;;)
  {
    /* Wait for the task notification to wake up */
    ulTaskNotifyTake(true, portMAX_DELAY);

    task_print_info("Entering State Game Over ");

    /* Clear Any Past Events from the Event Group. This helps to prevent the situation where
   the user was pressing SW1 or moving the joystick in one of the other states*/
    xEventGroupClearBits(eg_UI, 0xFFFFFF);

    /* Draw game over image through LCD */
    Square_t game_over_square;
    game_over_square.image_type = MEM_IMAGE_GAME_OVER;
    xQueueSend(q_LCD, &game_over_square, portMAX_DELAY);

    EventBits_t uxBits = xEventGroupWaitBits(eg_UI,EVENT_UI_SW2 |  EVENT_IPC_RX_RESTART,pdTRUE,pdFALSE, portMAX_DELAY);

   /* If EVENT_UI_SW2 is detected */
    if (( uxBits & EVENT_UI_SW2 ) != 0)
    {
      /* Clear the LCD */
      Square_t clear_LCD_square;
      clear_LCD_square.image_type = MEM_IMAGE_LCD_RESET;
      xQueueSend(q_LCD, &clear_LCD_square, portMAX_DELAY);

    /* transmit REMOTE_CMD_RESTART to the remote board */
      Remote_Uart_Data_t tx_data_restart;
      tx_data_restart.num_bytes = 1;
      tx_data_restart.payload[0] = REMOTE_CMD_RESTART;
      xQueueSend(q_Remote_Tx, &tx_data_restart, portMAX_DELAY);
     // wait for REMOTE_CMD_RESTART to be ACKED
      xEventGroupWaitBits(eg_UI,EVENT_IPC_RX_ACK,pdTRUE,pdTRUE, portMAX_DELAY);
     // send a Task Notification to the INIT SEND Task
      xTaskNotifyGive(Task_Handle_State_Init_Send);  

    /* enter blocked state and Wait for the task notification to wake up */
    ulTaskNotifyTake(true, portMAX_DELAY);

    }
    /* If EVENT_IPC_RX_RESTART is detected */
    else if ((uxBits & EVENT_IPC_RX_RESTART ) != 0)
    {
       /* Clear the LCD */
      Square_t clear_LCD_square;
      clear_LCD_square.image_type = MEM_IMAGE_LCD_RESET;
      xQueueSend(q_LCD, &clear_LCD_square, portMAX_DELAY);

      /* transmit REMOTE_CMD_RESTART to the remote board */
      Remote_Uart_Data_t tx_data_restart;
      tx_data_restart.num_bytes = 1;
      tx_data_restart.payload[0] = REMOTE_CMD_ACK;
      xQueueSend(q_Remote_Tx, &tx_data_restart, portMAX_DELAY);
      // send a Task Notification to the INIT RECEIVE Task
      xTaskNotifyGive(Task_Handle_State_Init_Receive);

    }

  }
}

void task_state_game_over_init(void)
{
  xTaskCreate(
      task_state_game_over,
      "Task State Game Over",
      5 * configMINIMAL_STACK_SIZE,
      NULL,
      HW03_TASK_PRIORITY_FSM,
      &Task_Handle_State_Game_Over);
}
#endif
