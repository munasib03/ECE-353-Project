/**
 * @file hw01b.c
 * @author
 * @brief
 * @version 0.1
 * @date 2024-01-08
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "hw01.h"

#if defined(HW01C)

/*****************************************************************************/
/* Function Declarations                                                     */
/*****************************************************************************/
// void memory_get_image(mem_image_t image, uint32_t *bitmap_addr, uint16_t *height, uint16_t *width);

/*****************************************************************************/
/* Global Variables                                                          */
/*****************************************************************************/
char APP_DESCRIPTION[] = "ECE353: HW01C";

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

    ece353_enable_lcd();

    joystick_init();

    push_buttons_init(true);

    pwm_buzzer_init();
}

// Square Struct to keep track of if its features and whether it has been hidden or selected.
typedef struct
{
    /* Data */
    bool hidden;   // True if unmatched and unselected.
    bool selected; // If hovering and SW1 pressed, then the image has been selected.
    uint8_t row;
    uint8_t col;
    uint16_t image;
    uint16_t fcolor;
    uint16_t bcolor;
} Square;

// 2D array storing each image, accessed by position on board
Square Memory[4][4];

bool GAME_OVER;    // Only true when user has matched 8 pairs.
uint8_t MATCH_COUNTER; // Used to keep track of how many pairs have been made

/**
 * @brief
 * A function for which square the user is currently over. If hovering over a square,
 * the color of the image will be inverted with its background.
 * @param s1, square to invert
 */
void isHovering(Square s1)
{
    uint16_t temp_color;
    uint16_t temp_image;

    // If not hidden, use the actual attributes of the image, else, use the question mark shape.
    if (s1.hidden == false)
    {
        temp_color = s1.fcolor;
        temp_image = s1.image;
    }
    else
    {
        temp_color = LCD_COLOR_GREEN;
        temp_image = 4;
    }
    s1.fcolor = LCD_COLOR_BLACK;
    s1.bcolor = temp_color;
    // Now that correct shape and color selected, print to screen
    memory_draw_image(s1.row, s1.col, (mem_image_t)temp_image, s1.fcolor, s1.bcolor);
    return;
}

/**
 * @brief
 * Similar to the isHovering logic, however, this one uninverts a square after the user uses the
 * joystick to hover over a different square.
 * @param s1, square to revert
 */
void notHovering(Square s1)
{
    uint16_t temp_color;
    uint16_t temp_image;
    // Checks whether the image was hidden or not so that the correct state of the game is maintained.
    if (s1.hidden == false)
    {
        temp_color = s1.fcolor;
        temp_image = s1.image;
    }
    else
    {
        temp_color = LCD_COLOR_GREEN;
        temp_image = 4;
    }

    // After state has been decided, uninvert the image.
    s1.fcolor = temp_color;
    s1.bcolor = LCD_COLOR_BLACK;
    memory_draw_image(s1.row, s1.col, (mem_image_t)temp_image, s1.fcolor, s1.bcolor);
    return;
}

/**
 * @brief
 * This is the event handler for SW1. When a square is selected, it's real shape is shown until another
 * square is selected. When two squares are selected, their shapes are compared. If there is a match,
 * the match counter is updated and the images do not revert to question marks. Else, they show for 1 second
 * and revert.
 * @param row, the row of the shape that was selected
 * @param col, the colomn the row was selected
 */
void selectionMade(uint8_t row, uint8_t col)
{
    // Variables to keep track of the number of squares selected to determine whether to compare or not, as well
    // as the currently selected squares.
    static uint8_t count = 0;
    static Square S1;
    Square current = Memory[row][col];

    // If already selected, ring buzzer for 300ms and do nothing else.
    if (current.selected)
    {
        pwm_buzzer_start();
        cyhal_system_delay_ms(300);
        pwm_buzzer_stop();
        return;
    }

    // Else,  Erase ? and draw image.
    memory_erase_image(row, col);
    Memory[row][col].hidden = false;
    Memory[row][col].selected = true;
    memory_draw_image(row, col, (mem_image_t)current.image, LCD_COLOR_BLACK, current.fcolor);

    // If only 1 selected, save that square and wait for another selection.
    if (count == 0)
    {
        S1 = current;
        count++;
        return;
    }

    else
    {
        // If images do not match, rehide them after 1 second and keep the most recently selected square inverted.
        if (S1.image != current.image)
        {
            // WAIT 1 sec, turn HIDDEN back to TRUE and SELECTED back to FALSE.
            cyhal_system_delay_ms(1000);
            Memory[S1.row][S1.col].hidden = true;
            Memory[row][col].hidden = true;
            Memory[S1.row][S1.col].selected = false;
            Memory[row][col].selected = false;

            // REDRAW QUESTION MARKS, current is currently being hovered over.
            memory_erase_image(S1.row, S1.col);
            memory_erase_image(current.row, current.col);

            memory_draw_image(S1.row, S1.col, (mem_image_t)4, LCD_COLOR_GREEN, LCD_COLOR_BLACK);
            memory_draw_image(row, col, (mem_image_t)4, LCD_COLOR_BLACK, LCD_COLOR_GREEN);
        }
        // If images match, update the MATCH_COUNTER to keep track of when the game is finished.
        else
        {
            MATCH_COUNTER++;
            // After 8 matches, the game over screen will appear
            if (MATCH_COUNTER == 8)
            {
                gameWon();
            }
        }
    }
    count = 0;
    return;
}

/**
 * @brief
 * Prints the game over screen and sets the game status to OVER.
 */
void gameWon()
{
    memory_draw_image(5, 5, (mem_image_t)5, LCD_COLOR_BLUE2, LCD_COLOR_BLACK);
    GAME_OVER = true;
    return;
}

/**
 * @brief
 * Redraws the board back to the initialized state and the match counter plus game status are reset as well.
 */
void resetGame()
{
   main_app();
   return;
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
    uint16_t image = 4;
    uint16_t color = LCD_COLOR_GREEN;
    joystick_position_t pos_curr = JOYSTICK_POS_CENTER;
    joystick_position_t pos_prev = JOYSTICK_POS_CENTER;
    GAME_OVER = false;
    MATCH_COUNTER = 0;

    memory_draw_grid(true);

    // Populate Square Struct images and color
    Memory[0][0].image = 0;
    Memory[0][0].fcolor = LCD_COLOR_BLUE;
    Memory[0][1].image = 2;
    Memory[0][1].fcolor = LCD_COLOR_RED;
    Memory[0][2].image = 1;
    Memory[0][2].fcolor = LCD_COLOR_BLUE;
    Memory[0][3].image = 2;
    Memory[0][3].fcolor = LCD_COLOR_RED;
    Memory[1][0].image = 1;
    Memory[1][0].fcolor = LCD_COLOR_BLUE;
    Memory[1][1].image = 3;
    Memory[1][1].fcolor = LCD_COLOR_RED;
    Memory[1][2].image = 3;
    Memory[1][2].fcolor = LCD_COLOR_RED;
    Memory[1][3].image = 0;
    Memory[1][3].fcolor = LCD_COLOR_BLUE;
    Memory[2][0].image = 2;
    Memory[2][0].fcolor = LCD_COLOR_RED;
    Memory[2][1].image = 1;
    Memory[2][1].fcolor = LCD_COLOR_BLUE;
    Memory[2][2].image = 3;
    Memory[2][2].fcolor = LCD_COLOR_RED;
    Memory[2][3].image = 2;
    Memory[2][3].fcolor = LCD_COLOR_RED;
    Memory[3][0].image = 1;
    Memory[3][0].fcolor = LCD_COLOR_BLUE;
    Memory[3][1].image = 0;
    Memory[3][1].fcolor = LCD_COLOR_BLUE;
    Memory[3][2].image = 0;
    Memory[3][2].fcolor = LCD_COLOR_BLUE;
    Memory[3][3].image = 3;
    Memory[3][3].fcolor = LCD_COLOR_RED;

    /* Draw 353 In the Text Area of the board */
    memory_print_number(3, (LOC_X_CENTER - 15), LOC_Y_TIME, LCD_COLOR_GREEN, LCD_COLOR_BLACK);
    memory_print_number(5, (LOC_X_CENTER - 0), LOC_Y_TIME, LCD_COLOR_GREEN, LCD_COLOR_BLACK);
    memory_print_number(3, (LOC_X_CENTER + 15), LOC_Y_TIME, LCD_COLOR_GREEN, LCD_COLOR_BLACK);

    /* Draw Initial Image*/
    for (row = 0; row < 4; row++)
    {
        for (col = 0; col < 4; col++)
        {
            Memory[row][col].row = row;
            Memory[row][col].col = col;
            Memory[row][col].hidden = true;
            Memory[row][col].selected = false;
            memory_draw_image(row, col, (mem_image_t)image, color, LCD_COLOR_BLACK);
        }
    }
    row = 0;
    col = 0;
    /* Declaring variable to store currently pressed switch number*/
    uint8_t ACTIVE_SW = 0;
    isHovering(Memory[0][0]);

    while (1)
    {
        /* Save the previous position */
        pos_prev = pos_curr;

        /* Get the current position */
        pos_curr = joystick_get_pos();
        /* Joystick can be only active when game is being played*/
      if(GAME_OVER == false){  
        /* Check if joystick is being centered before every move*/
        if (pos_prev == JOYSTICK_POS_CENTER && pos_curr != JOYSTICK_POS_CENTER)
        {
            /* Erase image from old position*/
            notHovering(Memory[row][col]);

            /* change row or col based on joystick movement*/
            if (pos_curr == JOYSTICK_POS_LEFT && col > 0)
            {
                col--;
            }
            if (pos_curr == JOYSTICK_POS_RIGHT && col < 3)
            {
                col++;
            }
            if (pos_curr == JOYSTICK_POS_UP && row > 0)
            {
                row--;
            }
            if (pos_curr == JOYSTICK_POS_DOWN && row < 3)
            {
                row++;
            }

            /* Draw image on new position */
            isHovering(Memory[row][col]);
        }
      }
        /* if SW1 has been pressed, select Image */
        if (ALERT_SW1 == BUTTON_PRESSED)
        {

            ALERT_SW1 = BUTTON_INACTIVE;
            /* Select image only once per switch 1 press */
            if (ACTIVE_SW != 1)
            {
                selectionMade(row, col);
            }
            /* Set SW1 as currently pressed switch */
            ACTIVE_SW = 1;
        }
        /* if SW2 has been pressed, reset game */
        if (ALERT_SW2 == BUTTON_PRESSED)
        {
            ALERT_SW2 = BUTTON_INACTIVE;
            /* Reset game only once per switch 2 press */
            if (ACTIVE_SW != 2)
            {
                // Reset game if and only if the game is over
                if (GAME_OVER)
                {
                    resetGame();
                }
            }
            /* Set SW2 as currently pressed switch */
            ACTIVE_SW = 2;
        }
       
        /* Resetting Active Switch when any of them are released */
        if (ALERT_SW1 == BUTTON_RELEASED)
        {

            ALERT_SW1 = BUTTON_INACTIVE;

            if (ACTIVE_SW == 1)
            {

                ACTIVE_SW = 0;
            }
        }
        if (ALERT_SW2 == BUTTON_RELEASED)
        {

            ALERT_SW2 = BUTTON_INACTIVE;

            if (ACTIVE_SW == 2)
            {

                ACTIVE_SW = 0;
            }
        }
      
    }
}

#endif
