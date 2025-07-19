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

#if defined(HW01B)



/*****************************************************************************/
/* Function Declarations                                                     */
/*****************************************************************************/
//void memory_get_image(mem_image_t image, uint32_t *bitmap_addr, uint16_t *height, uint16_t *width);

/*****************************************************************************/
/* Global Variables                                                          */
/*****************************************************************************/
char APP_DESCRIPTION[] = "ECE353: HW01B";


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
    uint16_t image = 0;
    uint16_t color = LCD_COLOR_BLUE;
    joystick_position_t pos_curr = JOYSTICK_POS_CENTER;
    joystick_position_t pos_prev = JOYSTICK_POS_CENTER;
    bool buzzer_on = false;

    memory_draw_grid(true);

    /* Draw 353 In the Text Area of the board */
    memory_print_number(3,(LOC_X_CENTER - 15), LOC_Y_TIME,LCD_COLOR_GREEN, LCD_COLOR_BLACK);
    memory_print_number(5,(LOC_X_CENTER -  0), LOC_Y_TIME,LCD_COLOR_GREEN, LCD_COLOR_BLACK);
    memory_print_number(3,(LOC_X_CENTER + 15), LOC_Y_TIME,LCD_COLOR_GREEN, LCD_COLOR_BLACK);

    /* Draw Initial Image*/
    memory_draw_image(row, col, (mem_image_t)image, color, LCD_COLOR_BLACK);
    
    /* Declaring variable to store currently pressed switch number*/
    uint8_t ACTIVE_SW = 0;
    while (1)
    {
      
        /* Save the previous position */
        pos_prev = pos_curr;
            

        /* Get the current position */
        pos_curr = joystick_get_pos();
        /* Check if joystick is being centered before every move*/
        if (pos_prev == JOYSTICK_POS_CENTER && pos_curr !=JOYSTICK_POS_CENTER )
            {
                /* Erase image from old position*/
                memory_erase_image(row, col);

                /* change row or col based on joystick movement*/
                if(pos_curr ==JOYSTICK_POS_LEFT && col > 0){
                    col--;
                }
                if(pos_curr ==JOYSTICK_POS_RIGHT && col < 3){
                    col++;
                }
                 if(pos_curr ==JOYSTICK_POS_UP && row > 0){
                    row--;
                }
                if(pos_curr ==JOYSTICK_POS_DOWN && row < 3){
                    row++;
                }

                /* Draw image on new position */
                memory_draw_image(row, col, (mem_image_t)image, color, LCD_COLOR_BLACK);
            } 

        /* if SW1 has been pressed, change Image */
        if(ALERT_SW1 == BUTTON_PRESSED)
            {
            
                ALERT_SW1 = BUTTON_INACTIVE;
                /* Change image only once per switch 1 press */
                if(ACTIVE_SW != 1){
                    // If all 4 images have passed, reset back to Clubs
                    image++;
                    if (image > 4)
                    {
                        image = 0;
                    }
                    /* Erase old image and draw switched new image*/
                     memory_erase_image(row, col);
                     memory_draw_image(row, col, (mem_image_t)image, color, LCD_COLOR_BLACK);
                }
                /* Set SW1 as currently pressed switch */
                ACTIVE_SW = 1;
            }
        /* if SW2 has been pressed, change Color */
        if(ALERT_SW2  == BUTTON_PRESSED)
            {
                ALERT_SW2 = BUTTON_INACTIVE;
                /* Change color only once per switch 2 press */
                if(ACTIVE_SW != 2){
                     // Now swap colors
                    if (color == LCD_COLOR_BLUE)
                    {
                        color = LCD_COLOR_RED;
                    }
                    else
                    {
                        color = LCD_COLOR_BLUE;
                    }
                    /* Erase old image and draw same image with swapped color*/
                     memory_erase_image(row, col);
                     memory_draw_image(row, col, (mem_image_t)image, color, LCD_COLOR_BLACK);
                }
                /* Set SW2 as currently pressed switch */
                 ACTIVE_SW = 2;
            }
        /* If switch 3 was pressed, toggle buzzer */
        if (ALERT_SW3 == BUTTON_PRESSED)
        {
            ALERT_SW3 = BUTTON_INACTIVE;

            /* Toggle buzzer only once per switch 3 press */
            if(ACTIVE_SW != 3){    
                // check current state of buzzer, if on, turn off.
                if (buzzer_on)
                {
                    pwm_buzzer_stop();
                    buzzer_on = false; // update tracker variable.
                }
                // if off, turn on.
                else
                {
                    pwm_buzzer_start();
                    buzzer_on = true; // update tracker variable
                }
            }
            /* Set SW3 as currently pressed switch */
            ACTIVE_SW = 3;
        }

        /* Resetting Active Switch when any of them are released */
        if(ALERT_SW1 == BUTTON_RELEASED ){
                    
            ALERT_SW1 = BUTTON_INACTIVE;
                   
            if(ACTIVE_SW == 1){
                
                ACTIVE_SW = 0;
            }
           
         }
        if(ALERT_SW2 == BUTTON_RELEASED ){
                    
            ALERT_SW2 = BUTTON_INACTIVE;
                   
            if(ACTIVE_SW == 2){
                
                ACTIVE_SW = 0;
            }
          
         }
        if(ALERT_SW3 == BUTTON_RELEASED ){
                    
            ALERT_SW3 = BUTTON_INACTIVE;
                   
            if(ACTIVE_SW == 3){
                
                ACTIVE_SW = 0;
            }
           
        }


    }
}

#endif
