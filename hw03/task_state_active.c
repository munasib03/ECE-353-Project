#include "task_state_init_receive.h"

#if defined(HW03A)

TaskHandle_t Task_Handle_State_Active;

/**
 * @brief
 * The ACTIVE task will sleep (blocked state) until a Task Notification is received.
 *
 * When the ACTIVE task is woken up using the a Task Notification, it will will draw the LCD grid with
 * the active color. The ACTIVE task will update the game board to display the current location of the
 * active square
 *
 * The ACTIVE task will block on eg_UI until SW1 (EVENT_UI_SW1) has been pressed OR
 * the movement has been detected (EVENT_UI_MOVE).
 *
 * If EVENT_UI_MOVE is detected, the user can move the active square around the game board. All
 * movement information is received from q_Dir.  The active square must stay on the game board at
 * all times, so make sure to check the boundary conditions. The active square is displayed by
 * inverting the foreground and background colors.
 *
 * If EVENT_UI_SW1 is detected, the user can select the active square using SW1. If the active square
 * is successfully selected, reveal the image at that location, send the remote player the coordinates,
 * and wait for the ACK.
 *
 * If the active square has already been selected, turn on the buzzer (3.5Khz, 50% duty cycle) for 300mS
 *
 * Once the user has successfully selected two squares, check to see if the squares match.  If the
 * squares match, check to see if the game is over.  The game is over when all squares have been matched.
 * If the game is over, send a Task Notification to the Game Over task.  If both squares match but the game
 * is not over, remain in the ACTIVE task and allow the user to select additional squares.
 *
 * If the two selected squares do not match, show the selected images for 1 second (vTaskDelay(1000))
 * then send a Task Notification to the Waiting task and place the ACTIVE task into a blocked state
 * waiting on a Task Notification.
 *
 * @param param
 */
void task_state_active(void *param)
{
  bool state_active = false;
  /* Suppress warning for unused parameter */
  (void)param;

  /* Repeatedly running part of the task */
  for (;;)
  {
    /* Wait for the task notification to wake up */
    ulTaskNotifyTake(true, portMAX_DELAY);
    state_active = true;

    task_print_info("Entering State ACTIVE ");

    /* Clear Any Past Events from the Event Group. This helps to prevent the situation where
       the user was pressing SW1 or moving the joystick in one of the other states*/
    xEventGroupClearBits(eg_UI, 0xFFFFFF);

    /* Clear any messages that were received when the application is not in the active state*/
    xQueueReset(q_Dir);
    Square_t square = Game_Board[0][0];
    Square_t grid;
    uint8_t row = 0;
    uint8_t col = 0;
    Square_t selection1;
    Square_t selection2;
    EventBits_t uxBits;
    Direction_t direction;
    uint8_t counter = 0; // Count how many squares selected
    bool valid_move = true;
    bool game_over = false;
    Remote_Uart_Data_t Square_to_send;
    // MEM_IMAGE_GRID_ACTIVE
    while (state_active)
    { 
      /* Initialize Grid */
      if (counter == 0)
      {
        square.active = 1;
        grid.image_type = MEM_IMAGE_GRID_ACTIVE;
        xQueueSend(q_LCD, &grid, portMAX_DELAY);
        xQueueSend(q_LCD, &square, portMAX_DELAY);
        counter++;
      }
      /* When none or 1 squares selected */
      if (counter < 3)
      {
        // Enable active square selection through Joystick or IMU
        uxBits = xEventGroupWaitBits(eg_UI, EVENT_UI_MOVE | EVENT_UI_SW1, pdTRUE, pdFALSE, portMAX_DELAY);
        if ((uxBits & EVENT_UI_MOVE) != 0)
        {
          xQueueReceive(q_Dir, &direction, portMAX_DELAY);
          if (direction == DIR_RIGHT)
          {
           /* Check boundary conditions to determine if move is valid */
            if (col == 3)
            {
              valid_move = false;
            }
            else
            {
              col++;
              valid_move = true;
            }
          }
          else if (direction == DIR_LEFT)
          {

            if (col == 0)
            {
              valid_move = false;
            }
            else
            {
              col--;
              valid_move = true;
            }
          }
          else if (direction == DIR_UP)
          {
            if (row == 0)
            {
              valid_move = false;
            }
            else
            {
              row--;
              valid_move = true;
            }
          }

          else if (direction == DIR_DOWN)
          {
            if (row == 3)
            {
              valid_move = false;
            }
            else
            {
              row++;
              valid_move = true;
            }
          }
          if (valid_move)
          {
            square.active = 0;
            xQueueSend(q_LCD, &square, portMAX_DELAY);
            square = Game_Board[row][col];
            square.active = 1;
            xQueueSend(q_LCD, &square, portMAX_DELAY);
          }
        }

        else if ((uxBits & EVENT_UI_SW1) != 0)
        { 
          // Enable buzzer briefly if trying to select an already selected or found square
          if ((counter > 1 && square.row == selection1.row && square.col == selection1.col) || square.status == SQUARE_STATUS_FOUND)
          {
            // Adjust freq and duty cycle in driver
            pwm_buzzer_start();
            vTaskDelay(300);
            pwm_buzzer_stop();
          }
          else
          { // Select active square
            square.status = SQUARE_STATUS_SELECTED;
            if (counter == 1)
            {
              selection1 = square;
              Game_Board[square.row][square.col].status = SQUARE_STATUS_SELECTED;
            }
            else
            {
              selection2 = square;
              Game_Board[square.row][square.col].status = SQUARE_STATUS_SELECTED;
            }
            square.active = false;

            // USE TX TO SEND SQUARE FIRST
            Square_to_send.num_bytes = 6;
            Square_to_send.payload[0] = REMOTE_CMD_PACKET_START;
            Square_to_send.payload[1] = REMOTE_CMD_TYPE_SEL;
            Square_to_send.payload[2] = row;
            Square_to_send.payload[3] = col;
            Square_to_send.payload[4] = square.image_type;
            Square_to_send.payload[5] = REMOTE_CMD_PACKET_END;

            xQueueSend(q_Remote_Tx, &Square_to_send, portMAX_DELAY);
            if (xEventGroupWaitBits(eg_UI, EVENT_IPC_RX_ACK, pdTRUE, pdTRUE, portMAX_DELAY) == EVENT_IPC_RX_ACK)
            {
              square.active = true;
              xQueueSend(q_LCD, &square, portMAX_DELAY);
              counter++;
            }
          }
        }
      }
      else if (counter == 3) // 2 squares selected
      {
        // No match found
        if (selection1.image_type != selection2.image_type)
        {
          vTaskDelay(1000);
          selection1.active = false;
          selection2.active = false;
          selection1.status = SQUARE_STATUS_NOT_SELECTED;
          selection2.status = SQUARE_STATUS_NOT_SELECTED;
          Game_Board[selection1.row][selection1.col].status = SQUARE_STATUS_NOT_SELECTED;
          Game_Board[selection2.row][selection2.col].status = SQUARE_STATUS_NOT_SELECTED;
          xQueueSend(q_LCD, &selection1, portMAX_DELAY);
          xQueueSend(q_LCD, &selection2, portMAX_DELAY);
          state_active = false;
        }
        else
        { // Squares matched, update the board.
          selection2.active = false;
          selection1.status = SQUARE_STATUS_FOUND;
          selection2.status = SQUARE_STATUS_FOUND;
          Game_Board[selection1.row][selection1.col] = selection1;
          Game_Board[selection2.row][selection2.col] = selection2;
          // Update Score when there is a match
          Score++;
          Row++;
          xQueueSend(q_IO_Exp, &Row, portMAX_DELAY);
          if (Score == 8) // when there has been 8 matches
          {
            game_over = true;
            state_active = false;
          }
          else
          {
            counter = 1; // Reset to none square selected
          }
        }
      }
    }
    if (game_over)
    {
      xTaskNotifyGive(Task_Handle_State_Game_Over); // Go to Game Over task if game is over
    }
    else
    {
      xTaskNotifyGive(Task_Handle_State_Waiting); // Go to Waiting task if there was no match
    }
  }
}

void task_state_active_init(void)
{
  xTaskCreate(
      task_state_active,
      "Task State Active",
      5 * configMINIMAL_STACK_SIZE,
      NULL,
      HW03_TASK_PRIORITY_FSM,
      &Task_Handle_State_Active);
}

#endif
