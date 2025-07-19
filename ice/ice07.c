/**
 * @file ice01.c
 * @author
 * @brief
 * @version 0.1
 * @date 2023-08-25
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "../main.h"

#if defined(ICE07)

/*****************************************************************************/
/*  Global Variables                                                         */
/*****************************************************************************/
char APP_DESCRIPTION[] = "ECE353: ICE07 - Timer Interrupts";


/**
 * @brief
 * This function will initialize all of the hardware resources for
 * the ICE
 */
void peripheral_init(void)
{
    /* Initialize the push buttons and turn on a timer to debounce the 
     * pushbuttons
     */
    push_buttons_init(true);

}

/*****************************************************************************/
/* Application Code                                                          */
/*****************************************************************************/
/**
 * @brief
 * This function implements the behavioral requirements for the ICE
 */
void main_app(void)
{
     /* Enter Infinite Loop*/
     uint8_t ACTIVE_SW = 0;
    
    while (1)
    {
        /* ADD CODE */
        

        /* Print if SW1 has been pressed. */
        if(ALERT_SW1 == BUTTON_PRESSED)
        {
           
            ALERT_SW1 = BUTTON_INACTIVE;
            if(ACTIVE_SW != 1){
                printf("SW1 has been pressed\n\r");
            }
             ACTIVE_SW = 1;
        }
        /* Print if SW2 has been pressed. */
         if(ALERT_SW2  == BUTTON_PRESSED)
        {
            ALERT_SW2 = BUTTON_INACTIVE;
             if(ACTIVE_SW != 2){
                printf("SW2 has been pressed\n\r");
            }
             ACTIVE_SW = 2;
        }
        
        /* Print if SW3 has been pressed. */

         if(ALERT_SW3  == BUTTON_PRESSED)
        {
            ALERT_SW3 = BUTTON_INACTIVE;
            if(ACTIVE_SW != 3){
                printf("SW3 has been pressed\n\r");
            }
             ACTIVE_SW = 3;
        }
     ////

         if(ALERT_SW1 == BUTTON_RELEASED ){
                    
                   ALERT_SW1 = BUTTON_INACTIVE;
                   
                   if(ACTIVE_SW == 1){
                printf("SW1 has been released\n\r");
                ACTIVE_SW = 0;
            }
           
         }
         if(ALERT_SW2 == BUTTON_RELEASED ){
                    
                   ALERT_SW2 = BUTTON_INACTIVE;
                   
                   if(ACTIVE_SW == 2){
                printf("SW2 has been released\n\r");
                ACTIVE_SW = 0;
            }
          
         }
         if(ALERT_SW3 == BUTTON_RELEASED ){
                    
                   ALERT_SW3 = BUTTON_INACTIVE;
                   
                   if(ACTIVE_SW == 3){
                printf("SW3 has been released\n\r");
                ACTIVE_SW = 0;
            }
           
         }

    }   
}
#endif
