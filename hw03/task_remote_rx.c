#include "task_remote_rx.h"

#if defined(HW03A)

extern cyhal_uart_t remote_uart_obj;

QueueHandle_t q_Remote_Rx;

/******************************************************************************
 * Static Functions
 *
 * These Functions should never be called outside of this file
 ******************************************************************************/

/**
 * @brief
 * Verifies that the data received in the data packet are valid
 * @param packet
 * An array of 6 bytes received from the remote board
 * @return true
 * All is valid
 * @return false
 * Packet contained invalid data
 */
static bool staff_image_verify(uint8_t *packet)
{
  /* Verify start of packet */
  if (packet[0] != REMOTE_CMD_PACKET_START)
  {
    return false;
  }

  /* Verify the command */
  if ((packet[1] != REMOTE_CMD_TYPE_INIT) && (packet[1] != REMOTE_CMD_TYPE_SEL))
  {
    return false;
  }

  /* Verify the row */
  if (packet[2] > 3)
  {
    return false;
  }

  /* Verify the column */
  if (packet[3] > 3)
  {
    return false;
  }

  /* Verify the image type*/
  if (
      (packet[4] != MEM_IMAGE_CLUB) &&
      (packet[4] != MEM_IMAGE_DIAMOND) &&
      (packet[4] != MEM_IMAGE_SPADE) &&
      (packet[4] != MEM_IMAGE_HEART))
  {
    return false;
  }

  /* Verify end of packet */
  if (packet[5] != REMOTE_CMD_PACKET_END)
  {
    return false;
  }

  return true;
}
/******************************************************************************
 * End of Static Functions
 ******************************************************************************/

/* This Global variable is used to store up to 6-bytes of data received on
 * the remote UART. As data is received, add the data to the payload and increment
 * the number of bytes received.
 *
 * Adding the next byte of data
 * Data_Remote_Rx.payload[Data_Remote_Rx.num_bytes] = c;
 *
 * Incrementing the number of bytes received
 * Data_Remote_Tx.num_bytes++;
 *
 */
static Remote_Uart_Data_t Data_Remote_Rx = {.num_bytes = 0, .payload = {0, 0, 0, 0, 0, 0}};

/**
 * @brief
 * This is the interrupt handler for the remote UART. Data is transmitted to the bottom
 * half task using q_Remote_Rx.
 *
 * The Handler should store the data locally in Data_Remote_Rx until one of the following
 * events occur:
 *
 *  1.) REMOTE_CMD_ACK is received.  Send a Remote_Uart_Data_t struct to the
 *      remote UART bottom half task.  Ensure that the num_bytes is set to 1
 *      and payload[0] is equal to REMOTE_CMD_ACK
 *
 *  2.) REMOTE_CMD_PLAYER_SEL is received.  Send a Remote_Uart_Data_t struct to the
 *      remote UART bottom half task.  Ensure that the num_bytes is set to 1
 *      and payload[0] is equal to REMOTE_CMD_PLAYER_SEL
 *
 *  3.) REMOTE_CMD_RESTART is received.  Send a Remote_Uart_Data_t struct to the
 *      remote UART bottom half task.  Ensure that the num_bytes is set to 1
 *      and payload[0] is equal to REMOTE_CMD_RESTART
 *
 *  4.) Data_Remote_Rx.num_bytes is equal to 6. Send a Remote_Uart_Data_t struct to the
 *      remote UART bottom half task.  Ensure that the num_bytes is set to 6
 *      and payload[] contains the 6 bytes received.
 *
 *  After sending data to the remote uart bottom half task, be sure to set Data_Remote_Rx.num_bytes
 *  back to 0.
 *
 * NOTE: This is an interrupt handler, so you need to remember to use xQueueSendFromISR() function
 *
 * @param handler_arg
 * @param event
 */
void Handler_FreeRTOS_Remote_UART(void *handler_arg, cyhal_uart_event_t event)
{
  (void)handler_arg;
  uint8_t data;
  portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
  if ((event & CYHAL_UART_IRQ_RX_NOT_EMPTY) == CYHAL_UART_IRQ_RX_NOT_EMPTY)
  { // Receive UART data
    cyhal_uart_getc(&remote_uart_obj, &data, 0);
    Data_Remote_Rx.payload[Data_Remote_Rx.num_bytes] = data;
    Data_Remote_Rx.num_bytes++;

    // REMOTE_CMD_ACK is received
    if (data == REMOTE_CMD_ACK)
    {
      Data_Remote_Rx.num_bytes = 1;
      Data_Remote_Rx.payload[0] = data;
      xQueueSendFromISR(q_Remote_Rx, &Data_Remote_Rx, &xHigherPriorityTaskWoken);
      Data_Remote_Rx.num_bytes = 0;
    }
    
    // REMOTE_CMD_PLAYER1_SEL is received
    else if (data == REMOTE_CMD_PLAYER1_SEL)
    {
      Data_Remote_Rx.num_bytes = 1;
      Data_Remote_Rx.payload[0] = data;
      xQueueSendFromISR(q_Remote_Rx, &Data_Remote_Rx, &xHigherPriorityTaskWoken);
      Data_Remote_Rx.num_bytes = 0;
    }

    // REMOTE_CMD_RESTART is received
    else if (data == REMOTE_CMD_RESTART)
    {
      Data_Remote_Rx.num_bytes = 1;
      Data_Remote_Rx.payload[0] = data;
      xQueueSendFromISR(q_Remote_Rx, &Data_Remote_Rx, &xHigherPriorityTaskWoken);
      Data_Remote_Rx.num_bytes = 0;
    }
    // Sending a Remote_Uart_Data_t struct to the remote UART bottom half task
    else if (Data_Remote_Rx.num_bytes == 6)
    {
      xQueueSendFromISR(q_Remote_Rx, &Data_Remote_Rx, &xHigherPriorityTaskWoken);
      Data_Remote_Rx.num_bytes = 0;
    }
  }

  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/**
 * @brief
 * The bottom half task for the remote UART will validate the data received.
 *
 * If REMOTE_CMD_ACK, REMOTE_CMD_PLAYER1_SEL, or REMOTE_CMD_RESTART were received, then
 * set the corresponding software event in eg_UI.  The software event
 * will be detected by one of the FSM tasks.
 *
 * If a valid 6-byte packet has been received, send a Square_t data structure to the FSM
 * tasks using q_Squares.  Be sure to fill in the corresponding fields of the
 * Square_t data structure with row, col, image_type, fcolor, and bcolor.
 *
 * @param param
 */
void task_remote_rx_bottom_half(void *param)
{
  Remote_Uart_Data_t rx_data;
  Square_t square;

  /* Suppress warning for unused parameter */
  (void)param;

  /* Repeatedly running part of the task */
  for (;;)
  {
    /* Wait to receive rx_data queue*/
    xQueueReceive(q_Remote_Rx, &rx_data, portMAX_DELAY);
    
    /* Set Event Bits based on rx data received */
    if (rx_data.num_bytes == 1)
    {
      if (rx_data.payload[0] == REMOTE_CMD_ACK)
      {
        xEventGroupSetBits(eg_UI, EVENT_IPC_RX_ACK);
      }
      else if (rx_data.payload[0] == REMOTE_CMD_PLAYER1_SEL)
      {
        xEventGroupSetBits(eg_UI, EVENT_IPC_RX_PLAYER1);
      }

      else if (rx_data.payload[0] == REMOTE_CMD_RESTART)
      {
        xEventGroupSetBits(eg_UI, EVENT_IPC_RX_RESTART);
      }
    }
    else if (rx_data.num_bytes == 6)
    {
      /* Verify the packet*/
      if (staff_image_verify((uint8_t *)(&rx_data.payload)))
      {
        square.active = false;
        square.random_num = 0;
        // SEND SQUARE TO FSM
        if (rx_data.payload[1] == REMOTE_CMD_TYPE_INIT)
        {
          square.status = SQUARE_STATUS_NOT_SELECTED;
        }
        else
        {
          square.status = SQUARE_STATUS_SELECTED;
        }
        square.row = rx_data.payload[2];        // row
        square.col = rx_data.payload[3];        // col
        square.image_type = rx_data.payload[4]; // Image type

        xQueueSend(q_Squares, &square, portMAX_DELAY);
      }
    }
    else
    {
      /* Unknown packet type --  ignore it*/
    }
  }
}

/**
 * @brief
 * Initializes the Remote UART interface so that interrupts are generated when bytes of data
 * are received by the remote UART.  This function registers a handler and bottom half task
 * that will be used to verify the data received is valid.  As part of the initialization, a
 * FreeRTOS Queue is created so send data between the Interrupt Handler and the bottom half
 * task.
 */
void task_remote_rx_init(void)
{

  /* Set the remote uart to 115200 8N1*/
  remote_uart_init();

  q_Remote_Rx = xQueueCreate(1, sizeof(Remote_Uart_Data_t));

  /* Register the Handler */
  cyhal_uart_register_callback(&remote_uart_obj, Handler_FreeRTOS_Remote_UART, NULL);

  /* Enable Rx Interrupts */
  cyhal_uart_enable_event(&remote_uart_obj, CYHAL_UART_IRQ_RX_NOT_EMPTY, 3, true);

  xTaskCreate(
      task_remote_rx_bottom_half,
      "Task Remote Rx",
      5 * configMINIMAL_STACK_SIZE,
      NULL,
      HW03_TASK_PRIORITY_BOTTOM_HALF,
      NULL);
}

#endif
