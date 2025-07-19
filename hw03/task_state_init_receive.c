#include "task_state_init_receive.h"

#if defined(HW03A)

TaskHandle_t Task_Handle_State_Init_Receive;

/**
 * @brief
 * The Init Receive task is used to receive 16 squares from the remote board.
 *
 * This state will need to:
 *  1. Sleep (blocked state) until the task receives a Task Notification
 *  2. Receive a square from the remote board via q_Squares
 *  3. Send an Ack the remote board indicating that the packet has been received
 *  4. Each square will need to be drawn on the LCD
 *  5. Repeat Steps 1-4 until all 16 squares have been received
 *  6. Once all squares have been Acked and Displayed, send a Task Notification to task_state_waiting
 *  7. Return to step 1
 *
 * @param param
 */
void task_state_init_receive(void *param)
{
  /* Suppress warning for unused parameter */
  (void)param;
  bool state_active = false;

  /* Repeatedly running part of the task */
  for (;;)
  {
    /* Wait for the task notification to wake up */

    ulTaskNotifyTake(true, portMAX_DELAY);

    state_active = true;

    task_print_info("Entering State INIT_RECEIVE");
    Score = 0;
    Row = 0;
    xQueueSend(q_IO_Exp, &Row, portMAX_DELAY);
    /* Clear Any Past Events from the Event Group. This helps to prevent the situation where
   the user was pressing SW1 or moving the joystick in one of the other states*/
    xEventGroupClearBits(eg_UI, 0xFFFFFF);
    Square_t square;
    Remote_Uart_Data_t tx_data_SendAck;
    tx_data_SendAck.num_bytes = 1;
    tx_data_SendAck.payload[0] = REMOTE_CMD_ACK;
    uint8_t row;
    uint8_t col;
    uint8_t counter = 0;
    while (state_active)
    {
      /* ADD CODE */
      // Receive a square from the remote board via q_Squares
      xQueueReceive(q_Squares, &square, portMAX_DELAY);
      // Send an Ack the remote board indicating that the packet has been received
      xQueueSend(q_Remote_Tx, &tx_data_SendAck, portMAX_DELAY);
      // Each square will need to be drawn on the LCD
      square.active = false;
      square.status = SQUARE_STATUS_NOT_SELECTED;
      Game_Board[square.row][square.col] = square;
      xQueueSend(q_LCD, &square, portMAX_DELAY);
      counter++;
      if (counter == 16)
      {
        state_active = false;
      }
    }
    xTaskNotifyGive(Task_Handle_State_Waiting); // Go to Waiting task when ACKing all squares is done 
  }
}

void task_state_init_receive_init(void)
{
  xTaskCreate(
      task_state_init_receive,
      "Task State Init Rx",
      5 * configMINIMAL_STACK_SIZE,
      NULL,
      HW03_TASK_PRIORITY_FSM,
      &Task_Handle_State_Init_Receive);
}
#endif
