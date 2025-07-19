#include "task_state_init_receive.h"

#if defined(HW03A)

TaskHandle_t Task_Handle_State_Waiting;

/**
 * @brief
 * When the WAITING task is woken up using the a Task Notification, it will will draw the LCD grid with
 * the inactive color. The WAITING task will update the game board so that none of the squares are
 * displayed as the active square.
 *
 * The WAITING task will then wait to receive squares from q_Squares.
 * When a square is received, send an ACK to the remote board and then display the image on the LCD.
 *
 * When two squares have been received, check to see if the squares match.
 *
 * If the two squares match, determine if the game is over. If the game is over, send a Task Notification to
 * the Game Over Task.  If the two squares match but the game is not over, then remain in the WAITING
 * task.
 *
 * If the two squares do not match, display the images for 1 second and then send a Task Notification
 * to ACTIVE task, and place the WAITING task into a blocked state waiting on a Task Notification.
 *
 * @param param
 */
void task_state_waiting(void *param)
{
  /* Suppress warning for unused parameter */
  (void)param;
  bool state_active = false;
  bool game_over = false;
  /* Repeatedly running part of the task */
  for (;;)
  {
    /* Wait for the task notification to wake up */
    ulTaskNotifyTake(true, portMAX_DELAY);

    state_active = true;
    task_print_info("Entering State WAITING ");
    Square_t first_square;
    Square_t second_square;
    Square_t send_squares;
    int counter = 0;
    /* Clear Any Past Events from the Event Group. This helps to prevent the situation where
       the user was pressing SW1 or moving the joystick in one of the other states*/
    xEventGroupClearBits(eg_UI, 0xFFFFFF);

    while (state_active)
    {
      
      // draw the LCD grid with the inactive color through LCD
      if (counter == 0)
      {
        send_squares.active = false;
        send_squares.image_type = MEM_IMAGE_GRID_INACTIVE;
        xQueueSend(q_LCD, &send_squares, portMAX_DELAY);
        counter++;
      }
      // Receive first square
      xQueueReceive(q_Squares, &first_square, portMAX_DELAY);
      // Send ACK
      Remote_Uart_Data_t tx_data_SendAck;
      tx_data_SendAck.num_bytes = 1;
      tx_data_SendAck.payload[0] = REMOTE_CMD_ACK;
      xQueueSend(q_Remote_Tx, &tx_data_SendAck, portMAX_DELAY);
      // Draw first square to LCD
      first_square.status = SQUARE_STATUS_SELECTED;
      xQueueSend(q_LCD, &first_square, portMAX_DELAY);
      // Receive Second square
      xQueueReceive(q_Squares, &second_square, portMAX_DELAY);
      // Send ACK
      xQueueSend(q_Remote_Tx, &tx_data_SendAck, portMAX_DELAY);
      // Draw Second square to LCD
      second_square.status = SQUARE_STATUS_SELECTED;
      xQueueSend(q_LCD, &second_square, portMAX_DELAY);

      if (first_square.image_type == second_square.image_type)
      {
        Game_Board[first_square.row][first_square.col].status = SQUARE_STATUS_FOUND;
        Game_Board[second_square.row][second_square.col].status = SQUARE_STATUS_FOUND;
        Score++;
        if (Score == 8)
        {
          game_over = true;
          state_active = false;
        }
      }
      else
      {
        vTaskDelay(1000);
        first_square.status = SQUARE_STATUS_NOT_SELECTED;
        // Reset and redraw first square to LCD
        xQueueSend(q_LCD, &first_square, portMAX_DELAY);
        second_square.status = SQUARE_STATUS_NOT_SELECTED;
        // Reset and redraw Second square to LCD
        xQueueSend(q_LCD, &second_square, portMAX_DELAY);

        xTaskNotifyGive(Task_Handle_State_Active);
        state_active = false;
      }
    }
    if (game_over)
    {
      xTaskNotifyGive(Task_Handle_State_Game_Over);// Go to Game Over is game is over
    }
    else
    {
      xTaskNotifyGive(Task_Handle_State_Active);// Go to Active state if other player's square did not match
    }
  }
}

void task_state_waiting_init(void)
{
  xTaskCreate(
      task_state_waiting,
      "Task State Waiting",
      5 * configMINIMAL_STACK_SIZE,
      NULL,
      HW03_TASK_PRIORITY_FSM,
      &Task_Handle_State_Waiting);
}
#endif
