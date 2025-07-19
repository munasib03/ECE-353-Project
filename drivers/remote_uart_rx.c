/**
 * @file remote_uart_rx.c
 * @author Joe Krachey (jkrachey@wisc.edu)
 * @brief
 * @version 0.1
 * @date 2023-10-06
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "remote_uart.h"
/*******************************************************************************
 * Macros
 ******************************************************************************/

/*******************************************************************************
 * Global Variables
 ******************************************************************************/
extern cyhal_uart_t remote_uart_obj;

Circular_Buffer *Rx_Circular_Buffer;
volatile bool ALERT_UART_RX = false;
//volatile memory_event_t EVENT_IPC = EVENT_NONE;

/**
 * @brief
 * Return a message from the circular buffer
 * @param msg
 * Starting address to place the message
 * @param max_size
 * Max number of characters to copy
 * @return true
 * Message copied successfully
 * @return false
 * No message was found in circular buffer
 */
bool remote_uart_rx_data_async(char *msg, uint16_t max_size)
{
    /* If the msg pointer is NULL, return false*/
    if (msg == NULL)
    {
        return false;
    }
    /* If the circular buffer is empty, return false */
    if (circular_buffer_empty(Rx_Circular_Buffer))
    {
        return false;
    }
    /* Disable interrupts -- Disable NVIC */
    __disable_irq();
    /* Grab characters from the circular buffer until the buffer
     *  is empty OR the maximum number of characters are copied*/
    for (int i = 0; i < max_size; i++)
    {
        if (!circular_buffer_empty(Rx_Circular_Buffer))
        {
            msg[i] = circular_buffer_remove(Rx_Circular_Buffer);
        }
    }
    /* Re-enable interrupts -- Enable NVIC */
    __enable_irq();
    /* Return true */
    return true;
}

/**
 * @brief
 * Enables Rx Interrupts for the remote UART.
 *  of data will utilize circular buffers to help minimize the amount
 * of time spent waiting for the UART interface.
 */
void remote_uart_rx_interrupts_init(void)
{
    /* Initialize Rx_Circular_Buffer */
    Rx_Circular_Buffer = circular_buffer_init(RX_BUFFER_SIZE);
    /* Turn on Rx interrupts */
    cyhal_uart_enable_event(&remote_uart_obj, (cyhal_uart_event_t)(CYHAL_UART_IRQ_RX_NOT_EMPTY), RX_IRQ_PRIORITY, true);
}

/**
 * @brief
 * Implements the Rx portion of the UART Handler
 */
void remote_uart_event_handler_process_rx(void)
{
    // Variables to hold char and the status of pulling the character from UART.
    cy_rslt_t status;
    uint8_t c;

    // Get character from UART
    status = cyhal_uart_getc(&remote_uart_obj, &c, 1);

    // If successfully received, check whether the character received indicated an event type.
    // Set the Event IPC if so, else add the char to the circular buffer.
    if (status == CY_RSLT_SUCCESS)
    {
        /*THESE DO NOT GO INTO BUFFER*/
/*
        // Initialize Square
        if (c == REMOTE_CMD_TYPE_INIT)
        {
            EVENT_IPC = EVENT_IPC_RX_SQUARE_INIT;
        }
        // ACK byte received
        else if (c == REMOTE_CMD_ACK)
        {
            EVENT_IPC = EVENT_IPC_RX_ACK;
        }
        // Player 1 select received
        else if (c == REMOTE_CMD_PLAYER1_SEL)
        {
            EVENT_IPC = EVENT_IPC_RX_PLAYER1;
        }

        // Else, add to buffer as it is data for a game board square
        else
        {
            circular_buffer_add(Rx_Circular_Buffer, c);
        }
   */ } 
}