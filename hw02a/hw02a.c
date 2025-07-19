/**
 * @file hw01a.c
 * @author M Sadman Sakib, Amer Salem
 * @brief
 * @version 0.1
 * @date 2024-01-08
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "memory.h"

#if defined(HW02A)

/*****************************************************************************/
/* Function Declarations                                                     */
/*****************************************************************************/
void memory_mode_player_selection(Game_State_t *state);
void memory_mode_init_send(Game_State_t *state);
void memory_mode_init_receive(Game_State_t *state);
void memory_mode_active(Game_State_t *state, uint8_t *row, uint8_t *col);
void memory_mode_waiting(Game_State_t *state, uint8_t *row, uint8_t *col);
void memory_mode_game_over(Game_State_t *state, uint8_t *row, uint8_t *col);

/*****************************************************************************/
/* Global Variables                                                          */
/*****************************************************************************/
char APP_DESCRIPTION[] = "ECE353: HW02A";
cyhal_trng_t trng_obj;

Square_t Squares[16];

uint8_t Score = 0;

/**
 * @brief
 * This function will initialize all of the hardware resources for
 * the ICE.
 *
 * This function is implemented in the iceXX.c file for the ICE you are
 * working on.
 */
void peripheral_init(void)
{
    /* Enable the LCD */
    ece353_enable_lcd();

    /* Enable SW1, SW2, and SW3*/
    push_buttons_init(true);

    /* Enable Inter-Board communications*/
    remote_uart_init();
    remote_uart_enable_interrupts();

    /* Initialize the hardware random number generator*/
    cyhal_trng_init(&trng_obj);
}

/*****************************************************************************/
/* Application Code                                                          */
/*****************************************************************************/
/**
 * @brief
 * This function implements the behavioral requirements for the ICE
 *
 * This function is implemented in the iceXX.c file for the ICE you are
 * working on.
 */
void main_app(void)
{
    uint8_t row = 0;
    uint8_t col = 0;
    Game_State_t game_state = GAME_STATE_PLAYER_SELECTION;

    staff_set_grid_color(LCD_COLOR_ORANGE);

    while (1)
    {
        switch (game_state)
        {
        case GAME_STATE_PLAYER_SELECTION:
        {
            memory_mode_player_selection(&game_state);
            break;
        }
        case GAME_STATE_INIT_SEND:
        {
            memory_mode_init_send(&game_state);
            break;
        }
        case GAME_STATE_INIT_RECEIVE:
        {
            memory_mode_init_receive(&game_state);
            break;
        }
        case GAME_STATE_ACTIVE:
        {
            memory_mode_active(&game_state, &row, &col);
            break;
        }
        case GAME_STATE_WAITING:
        {
            memory_mode_waiting(&game_state, &row, &col);
            break;
        }
        case GAME_STATE_GAME_OVER:
        {
            memory_mode_game_over(&game_state, &row, &col);
            break;
        }
        default:
        {
            printf("Error, unknown mode\n\r");
            while (1)
            {
            };
        }
        }
    }
}

/**
 * @brief
 * The player selection mode determines which board is associated with player 1.
 *
 * The first player to press SW1 will become Player1. This function will send a
 * REMOTE_CMD_PLAYER1_SEL command to the other board. After sending
 * REMOTE_CMD_PLAYER1_SEL, wait for the other board to acknowlege the command
 * by returning a REMOTE_CMD_ACK byte.  Once REMOTE_CMD_ACK has been received, The game
 * state will be set to GAME_STATE_INIT_SEND and return.
 *
 * If REMOTE_CMD_PLAYER1_SEL is received from the remote board, the function will
 * send a REMOTE_CMD_ACK and set the game state to GAME_STATE_INIT_RECEIVE.
 *
 * @param state
 * @param row
 * @param col
 */
void memory_mode_player_selection(Game_State_t *state)
{
    printf("Entering GAME_STATE_PLAYER_SELECTION\n\r");

    while (*state == GAME_STATE_PLAYER_SELECTION)
    {
        if (ALERT_SW2 == BUTTON_PRESSED)
        {
            /* If the user presses SW2 do nothing.
             * We still need to set the state back to BUTTON INACTIVE
             */
            ALERT_SW2 = BUTTON_INACTIVE;
        }

        if (ALERT_SW3 == BUTTON_PRESSED)
        {
            /* If the user presses SW3 do nothing.
             * We still need to set the state back to BUTTON INACTIVE
             */
            ALERT_SW3 = BUTTON_INACTIVE;
        }

        /* Check to see if the user has pressed SW1*/
        if (ALERT_SW1 == BUTTON_PRESSED)
        {
            // Set button to INACTIVE so that it is properly released
            ALERT_SW1 = BUTTON_INACTIVE;
            // Send to the other board that you are player 1
            remote_uart_tx_char_async(REMOTE_CMD_PLAYER1_SEL);
            // Wait until the player byte is received before continuing.
            memory_ack_wait();
            // Switch state to being the transmitter
            *state = GAME_STATE_INIT_SEND;
            return;
        }
        else
        {
            /* Check to see if data has been received */
            // EVENT_NONE is the default event, if changed, then data has been received
            if (EVENT_IPC != EVENT_NONE)
            {
                if (EVENT_IPC == EVENT_IPC_RX_PLAYER1)
                {
                    // If the player 1 byte was received, ACK that it was received and go to
                    // player 2 state (receiver)
                    memory_ack_send();
                    *state = GAME_STATE_INIT_RECEIVE;
                }
                // Return event back to none
                EVENT_IPC = EVENT_NONE;
            }
        }
    }
}

/**
 * @brief
 * Player1 will generate the location of each game piece. This function will
 * display the images in each square and send the information about each
 * square to the remote board.
 *
 * Use the function staff_randomize_images(Square_t *squares); to generate an array
 * of 16 random images that will be used to determine the position of each image.
 *
 * Once all of the information has been transmitted to the remote board, this
 * function will exit and set the state of the game to GAME_STATE_ACTIVE.
 * @param state
 */
void memory_mode_init_send(Game_State_t *state)
{
    printf("Entering GAME_STATE_INIT_SEND\n\r");

    if (ALERT_SW1 == BUTTON_PRESSED)
    {
        /* If the user presses SW1 do nothing.
         * We still need to set the state back to BUTTON INACTIVE
         */
        ALERT_SW1 = BUTTON_INACTIVE;
    }

    if (ALERT_SW2 == BUTTON_PRESSED)
    {
        /* If the user presses SW2 do nothing.
         * We still need to set the state back to BUTTON INACTIVE
         */
        ALERT_SW2 = BUTTON_INACTIVE;
    }

    if (ALERT_SW3 == BUTTON_PRESSED)
    {
        /* If the user presses SW3 do nothing.
         * We still need to set the state back to BUTTON INACTIVE
         */
        ALERT_SW3 = BUTTON_INACTIVE;
    }

    if (circular_buffer_num_entries(Rx_Circular_Buffer) != 0)
    {
        /* If for some reason we receive data while trying
         * to initialize the other board, ignore the data
         * and print an error
         */
        EVENT_IPC = EVENT_NONE;
        printf("Unexpected data received during init_send\n\r");
        while (1)
        {
        };
    }

    /* Generate random locations for all images */
    staff_randomize_images(Squares);

    /* Transmit the images to the remote board */
    memory_square_transmit_all(Squares, REMOTE_CMD_TYPE_INIT);

    /* Draw the images on the local board.*/
    memory_square_display_all(Squares);

    *state = GAME_STATE_ACTIVE;
}

/**
 * @brief
 * Player2 will receive the information for all 16 squares from the remote board.
 * These data packets contain the information needed to initialize the game board.
 *
 * When a valid 6-byte packet is received, update the LCD with the image for that
 * packet and then send an REMOTE_CMD_ACK.
 *
 * After all 16 packets have been received, the game state will be set to
 * GAME_STATE_WAITING
 *
 * @param state
 */
void memory_mode_init_receive(Game_State_t *state)
{
    uint8_t init_count = 0;
    uint8_t packet[6];
    uint8_t row;
    uint8_t col;
    uint8_t image;
    uint16_t color;

    printf("Entering GAME_STATE_INIT_RECEIVE\n\r");

    /* Draw the grid */
    staff_set_grid_color(LCD_COLOR_GRAY);

    /* The application will remain in the GAME_STATE_INIT_RECEIVE state until
     * 16 valid images are received
     */
    while (*state == GAME_STATE_INIT_RECEIVE)
    {
        if (ALERT_SW1 == BUTTON_PRESSED)
        {
            /* If the user presses SW1 do nothing.
             * We still need to set the state back to BUTTON INACTIVE
             */
            ALERT_SW1 = BUTTON_INACTIVE;
        }

        if (ALERT_SW2 == BUTTON_PRESSED)
        {
            /* If the user presses SW2 do nothing.
             * We still need to set the state back to BUTTON INACTIVE
             */
            ALERT_SW2 = BUTTON_INACTIVE;
        }

        if (ALERT_SW3 == BUTTON_PRESSED)
        {
            /* If the user presses SW3 do nothing.
             * We still need to set the state back to BUTTON INACTIVE
             */
            ALERT_SW3 = BUTTON_INACTIVE;
        }
        /* Check to see if a 6-byte data has been received */
        else if (circular_buffer_num_entries(Rx_Circular_Buffer) > 4)
        {

            /* Parse the data received, verify the data, update the
             * LCD Display, and ACK valid packets.
             */
            for (uint8_t i = 0; i < 5; i++)
            {
                packet[i] = circular_buffer_remove(Rx_Circular_Buffer);
            }

            // Verify the packet and make sure all content is valid before displaying
            if (memory_packet_verify(packet))
            {
                // Update the characteristics of the square
                Squares[init_count].row = packet[1];
                Squares[init_count].col = packet[2];
                Squares[init_count].image_type = packet[3];

                // Determine color (0,1 == blue (spade/clove))
                if (packet[3] == 0 || packet[3] == 1)
                {
                    Squares[init_count].fcolor = LCD_COLOR_BLUE;
                }
                // (2,3 == red (Heart/Diamond))
                else if (packet[3] == 2 || packet[3] == 3)
                {
                    Squares[init_count].fcolor = LCD_COLOR_RED;
                }
                // Background color is black by default
                Squares[init_count].bcolor = LCD_COLOR_BLACK;
                // Display the current square
                memory_square_display(&Squares[init_count]);
                // Move on to the next square in the array, which may not be next square on board.
                init_count++;
                memory_ack_send();
            }

            /* When 16 valid packets have been received, update state to be
             * GAME_STATE_ACTIVE */
            if (init_count == 16)
            {
                *state = GAME_STATE_ACTIVE;
            }
        }
    }
}

/**
 * @brief
 * In active mode, the game will detect when the user selects an active square.  When
 * a square is selected, the LCD will display the image and send a data packet to the
 * remote board indicating which square was selected.  After two squares have been selected,
 * your application will determine if a match was found.  If a match is NOT found, the game
 * state is set to GAME_STATE_WAITING to allow the remote player to take control of the
 * selection process.
 * @param state
 * @param row
 * @param col
 */
void memory_mode_active(Game_State_t *state, uint8_t *row, uint8_t *col)
{
    printf("Entering GAME_STATE_ACTIVE\n\r");

    while (*state == GAME_STATE_ACTIVE)
    {
    }
}

/**
 * @brief
 * In Waiting mode, the device receives remote messages from the other device.  These messages
 * indicate which squares were selected.  If the other player selects a match, you will remaining
 * in Waiting mode.  If the squares do NOT match, then the game state is set to GAME_STATE_ACTIVE
 * and allows the local player to take control of the selection process.
 * @param state
 * @param row
 * @param col
 */
void memory_mode_waiting(Game_State_t *state, uint8_t *row, uint8_t *col)
{
    printf("Entering GAME_STATE_WAITING\n\r");

    while (*state == GAME_STATE_WAITING)
    {
    }
}

void memory_mode_game_over(Game_State_t *state, uint8_t *row, uint8_t *col)
{
    printf("Entering GAME_STATE_GAME_OVER\n\r");

    while (*state == GAME_STATE_GAME_OVER)
    {
    }
}

#endif