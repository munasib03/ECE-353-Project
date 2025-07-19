/**
 * @file hw01a.c
 * @author
 * @brief
 * @version 0.1
 * @date 2024-01-08
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "hw01.h"

#if defined(HW01A)

/*****************************************************************************/
/* Function Declarations                                                     */
/*****************************************************************************/

/*****************************************************************************/
/* Global Variables                                                          */
/*****************************************************************************/
char APP_DESCRIPTION[] = "ECE353: HW01A";

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
    uint8_t x = 0;
    uint8_t y = 0;
    uint16_t color = LCD_COLOR_BLUE;
    mem_image_t image = MEM_IMAGE_CLUB;

    memory_draw_grid(true);

    /* Draw Initial Image*/
    memory_draw_image(x, y, image, LCD_COLOR_BLUE, LCD_COLOR_BLACK);
    cyhal_system_delay_ms(200);

    while (1)
    {
        /* Write the code that will display the images as described in the HW01A
         * document
         */
        // Erase the previously drawn shape using the old x-y coords.
        memory_erase_image(x, y);
        // increment the y-axis so that the next row is selected.
        y++;
        // If row 4 has already been completed, the entire column has been drawn to, go to first row of next col.
        if (y > 3)
        {
            y = 0; // Resets row.
            x++;   // Goes to next column on grid.
        }
        // If the 4th column is complete, reset back to (0,0) and update the image.
        if (x > 3)
        {
            x = 0;   // Resets column
            image++; // Select next image.

            // Now swap colors
            if (color == LCD_COLOR_BLUE)
            {
                color = LCD_COLOR_RED;
            }
            else
            {
                color = LCD_COLOR_BLUE;
            }
        }
        // If all 4 images have passed, reset back to Clubs
        if (image > 3)
        {
            image = 0;
        }
        // After validating coords, shape, and color, draw to next grid on the LCD.
        memory_draw_image(x, y, image, color, LCD_COLOR_BLACK);

        cyhal_system_delay_ms(200);
    }
}

#endif
