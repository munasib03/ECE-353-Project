#include "task_state_player_sel.h"

#if defined(HW03A)

TaskHandle_t Task_Handle_State_Player_Sel;

/**
 * @brief
 * This state is used to determine if the board is player 1.  This state will remaining
 * in a blocked state until it receives a Task Notification to wake up.
 *
 * Once awake, there are two events that this state will respond to.
 *
 *    SW1 is pressed
 *      1. The local board will become Player1.
 *      2. Send a REMOTE_CMD_PLAYER1_SEL command to the remote board
 *      3. Wait for an ACK
 *      4. Send a Task Notification to task_state_init_send
 *      5 Sleep (blocked state) until the task receives a Task Notification
 *
 *    Receive a EVENT_IPC_RX_PLAYER1 from the remote UART
 *      1. Send a REMOTE_CMD_ACK to the remote board.
 *      2. Send a Task Notification to task_state_init_receive
 *      3. Sleep (blocked state) until the task receives a Task Notification
 *
 * Remember, you CANNOT directly access the remote UART or SW1 from this task.
 * You will need to send/receive data from the UART using the Queues/EventGroups
 * dedicated for this purpose
 *
 * @param param
 */
void task_state_player_sel(void *param)
{
  /* Suppress warning for unused parameter */
  (void)param;

  task_print("\x1b[2J\x1b[;H");
  task_print("**************************************************\n\r");
  task_print("* %s\n\r", APP_DESCRIPTION);
  task_print("* Date: %s\n\r", __DATE__);
  task_print("* Time: %s\n\r", __TIME__);
  task_print("* Names:%s\n\r", STUDENTS);
  task_print("**************************************************\n\r");

  /* Repeatedly running part of the task */
  for (;;)
  {
    /* Wait for the task notification to wake up */
    ulTaskNotifyTake(true, portMAX_DELAY);

    bool STATE_ACTIVE = true;
    task_print_info("Entering State PLAYER_SEL");

    /* Clear Any Past Events from the Event Group. This helps to prevent the situation where
       the user was pressing SW1 or moving the joystick in one of the other states*/
    xEventGroupClearBits(eg_UI, 0xFFFFFF);

    /* ADD CODE */
    while (STATE_ACTIVE)
    {
      EventBits_t uxBits = xEventGroupWaitBits(eg_UI, (EVENT_UI_SW1 | EVENT_IPC_RX_PLAYER1), pdTRUE, pdFALSE, portMAX_DELAY);

      /* Check to see if the user has pressed SW1*/
      if ((uxBits & EVENT_UI_SW1) != 0)
      {
        // Send to the other board that you are player 1
        Remote_Uart_Data_t tx_data_plSelect;
        tx_data_plSelect.num_bytes = 1;
        tx_data_plSelect.payload[0] = REMOTE_CMD_PLAYER1_SEL;
        xQueueSend(q_Remote_Tx, &tx_data_plSelect, portMAX_DELAY);
        // Wait until the player byte is received before continuing.
        xEventGroupWaitBits(eg_UI, EVENT_IPC_RX_ACK, pdTRUE, pdTRUE, portMAX_DELAY);
        // Send task notification to task_state_init_send
        xTaskNotifyGive(Task_Handle_State_Init_Send);
        STATE_ACTIVE = false;
      }
      else if ((uxBits & EVENT_IPC_RX_PLAYER1) != 0)
      {
        // Send REMOTE_CMD_ACK to the other board
        Remote_Uart_Data_t tx_data;
        tx_data.num_bytes = 1;
        tx_data.payload[0] = REMOTE_CMD_ACK;
        xQueueSend(q_Remote_Tx, &tx_data, portMAX_DELAY);
        // Send task notification to task_state_init_send
        xTaskNotifyGive(Task_Handle_State_Init_Receive);
        STATE_ACTIVE = false;
      }
    }
  }
}

/**
 * @brief
 * Initializes the Player Select Task
 */
void task_state_player_sel_init(void)
{
  xTaskCreate(
      task_state_player_sel,
      "Task State Player Sel",
      5 * configMINIMAL_STACK_SIZE,
      NULL,
      HW03_TASK_PRIORITY_FSM,
      &Task_Handle_State_Player_Sel);
}
#endif
