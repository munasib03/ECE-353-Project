#include "task_remote_tx.h"

#if defined(HW03A)

extern cyhal_uart_t remote_uart_obj;

QueueHandle_t q_Remote_Tx;

/**
 * @brief
 * All data transmitted to the remote board MUST send the data to this task.
 *
 * The Remote Tx task is used to transmit data to the remote board. This task will use cyhal_uart_putc() to
 * transmit each character using polling.
 *
 * Data is received by task_remote_tx using a Remote_Uart_Data_t data struture.  This data structure contains
 * the number of bytes to send (tx_data.num_bytes) and an array of up to 6 bytes to
 * transmit (tx_data.payload[]).
 *
 * @param param
 */
void task_remote_tx(void *param)
{
  /* Suppress warning for unused parameter */
  (void)param;
  Remote_Uart_Data_t tx_data;

  /* Repeatedly running part of the task */
  for (;;)
  { 
    /* Wait to receive Tx Queue */
    xQueueReceive(q_Remote_Tx, &tx_data, portMAX_DELAY);

    /* Transmit the Tx data through Polling*/
    for (int i = 0; i < tx_data.num_bytes; i++)
    {
      cyhal_uart_putc(&remote_uart_obj, tx_data.payload[i]);
    }
  }
}

/**
 * @brief
 * Used to create a FreeRTOS task that will manage all data being transmitted to the remote board.
 */
void task_remote_tx_init(void)
{
  q_Remote_Tx = xQueueCreate(1, sizeof(Remote_Uart_Data_t));

  xTaskCreate(
      task_remote_tx,
      "Task Remote Tx",
      5 * configMINIMAL_STACK_SIZE,
      NULL,
      HW03_TASK_PRIORITY_UI_OUTPUT,
      NULL);
}

#endif
