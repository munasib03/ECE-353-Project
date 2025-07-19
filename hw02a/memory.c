/**
 * @file memory.c
 * @author M Sadman Sakib, Amer Salem
 * @brief
 * @version 0.1
 * @date 2024-03-08
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "memory.h"

#if defined(HW02A)

/******************************************************************************
 * Functions that you will need to complete
 ******************************************************************************/

/**
 * @brief
 * Sends REMOTE_CMD_ACK to the remote board
 */
void memory_ack_send(void)
{
    // Simply transmit the ACK byte, no event change.
    remote_uart_tx_char_async(REMOTE_CMD_ACK);
}

/**
 * @brief
 * Waits to receive an REMOTE_CMD_ACK
 */
void memory_ack_wait(void)
{
    // Stay in loop until the reciever has detected an ACK byte.
    while (EVENT_IPC != EVENT_IPC_RX_ACK)
    {
        ;
        ;
    }
    // Reset the event so when the next byte is received, the loop condition is true by defualt.
    EVENT_IPC = EVENT_NONE;
    return;
}

/**
 * @brief
 * Verifies that the data received in the data packet are valid
 * @param packet
 * An array of 6 bytes received from the remote board
 * @return true
 * All bytes are valid
 * @return false
 * Packet contained invalid data
 */
bool memory_packet_verify(uint8_t *packet)
{
    /*  PACKET CONTENTS: [START, ROW, COL, IMAGE, END]
        ROW and COL cannot be outside of grid,
        and there are only 7 valid images.             */

    // Check contents in order described above.
    if (packet[0] != REMOTE_CMD_PACKET_START)
    {
        return false;
    }

    if (packet[1] > 3 || packet[1] < 0)
    {
        return false;
    }

    if (packet[2] > 3 || packet[2] < 0)
    {
        return false;
    }

    if (packet[3] > 6 || packet[3] < 0)
    {
        return false;
    }

    if (packet[4] != REMOTE_CMD_PACKET_END)
    {
        return false;
    }
    // Only if data is in correct order and in bounds, return true.
    return true;
}

/**
 * @brief
 * Transmit data to the remote board. The format of the data being transmitted is as follows:
 *
 *       REMOTE_CMD_START
 *       REMOTE_CMD_TYPE_INIT or REMOTE_CMD_TYPE_SEL
 *       row
 *       col
 *       image
 *       REMOTE_CMD_END
 *
 * @param square
 *     Data Structure that contains the information about the square to transmit
 * @param remote_cmd_type
 *      Indicates if the square is being initialized or if the square was selected
 *      by the user
 */
void memory_square_transmit(Square_t *square, uint8_t remote_cmd_type)
{
    // Build a packet containing the square
    char packet[6];
    // Start with Start Byte, then add contents (row,col,image), then end byte
    packet[0] = REMOTE_CMD_PACKET_START;
    packet[1] = remote_cmd_type;
    packet[2] = square->row;
    packet[3] = square->col;
    packet[4] = square->image_type;
    packet[5] = REMOTE_CMD_PACKET_END;

    // Send each byte one at a time.
    for (int i = 0; i < 6; i++)
    {
        remote_uart_tx_char_async(packet[i]);
    }
    // Wait until the other board receives the byte before continuing any process.
    memory_ack_wait();
    return;
}

/**
 * @brief
 * Transmits 16 images to the remote board.
 *
 * After each of the squares is transmitted, wait until REMOTE_CMD_ACK byte
 * is received.
 *
 * @param squares
 * A array of 16 Square_t structs that contain the information needed to transmit the
 * images to the remote device.
 */
void memory_square_transmit_all(Square_t *squares, uint8_t remote_cmd_type)
{
    // Loop through all 16 squares on board
    for (uint8_t i = 0; i < 16; i++)
    {
        // Utilize the transmission of a single square so that each one is confirmed
        //  to be ACKd before continuing
        memory_square_transmit(squares, remote_cmd_type);
        squares++;
    }
}

/**
 * @brief
 * Draws a single square to the LCD.
 * @param square
 * A pointer to the square to be displayed.
 */
void memory_square_display(Square_t *square)
{
    staff_draw_image(square->row, square->col, square->image_type, square->fcolor, square->bcolor);
}

/**
 * @brief
 * Draws the game board and all of the squares that were randomly generated.
 * @param squares
 * A array of 16 Square_t structs that contain the information needed to draw the
 * images at each location in the game board.
 */
void memory_square_display_all(Square_t *squares)
{
    // Loops through board and prints squares one at a time.
    for (uint8_t i = 0; i < 16; i++)
    {
        memory_square_display(squares);
        squares++;
    }
}

#endif