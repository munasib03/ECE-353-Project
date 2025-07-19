/**
 * @file io-sw.c
 * @author
 * @brief 
 * @version 0.1
 * @date 2023-09-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "io-sw.h"

/*****************************************************************************/
/* Public Global Variables */
/*****************************************************************************/
button_state_t ALERT_SW1 = BUTTON_INACTIVE;
button_state_t ALERT_SW2 = BUTTON_INACTIVE;
button_state_t ALERT_SW3 = BUTTON_INACTIVE;

/*****************************************************************************/
/* Static Function Declarations */
/*****************************************************************************/
static void Handler_Timer_Buttons(void *handler_arg, cyhal_timer_event_t event);
static void push_buttons_timer_init(void);

/*****************************************************************************/
/* ICE 02 START */
/*****************************************************************************/
void push_buttons_init(bool enable_timer_irq)
{
    /* ADD CODE */
    /* Initialize the IO Pins connected to the push buttons as inputs */
     cy_rslt_t rslt;
    
     rslt = cyhal_gpio_init(PIN_SW1,CYHAL_GPIO_DIR_INPUT,CYHAL_GPIO_DRIVE_NONE, false);
     rslt = cyhal_gpio_init(PIN_SW2,CYHAL_GPIO_DIR_INPUT,CYHAL_GPIO_DRIVE_NONE, false);
     rslt = cyhal_gpio_init(PIN_SW3,CYHAL_GPIO_DIR_INPUT,CYHAL_GPIO_DRIVE_NONE, false);
    /* ADD CODE */
    if(enable_timer_irq)
    {
        push_buttons_timer_init();
    }
} 
/*****************************************************************************/
/* ICE 02 END */
/*****************************************************************************/

/*****************************************************************************/
/* ICE 06 START */
/*****************************************************************************/

/*
 * Handler used to debounce the push buttons
*/
static void Handler_Timer_Buttons(void *handler_arg, cyhal_timer_event_t event)
{
    /* Static Vars */
    
    uint32_t reg_val = PORT_BUTTONS->IN;

    /* Many mechanical buttons will bounce when they are pressed.
    *  The signal may oscillate between 0 and 1 for several milliseconds.
    *
    * We will debounce a button by sampling the button and detecting the first
    * 50mS interval of time when the button is low.  When that period of time is
    * detected, we will alert the main application using ALERT_SW1, ALERT_SW2, 
    * and ALERT_SW3.
    */

   /* ADD CODE */

  
    //Create counters and initialize to 0 to keep track of interrupts. Each tick = 10 ms, press registered at 50 ms
   static uint8_t timer_count_sw1 = 0;
   static uint8_t timer_count_sw2 = 0;
   static uint8_t timer_count_sw3 = 0;
   
        //Buttons are active low, check status with mask and if pressed, increment.
       if ((reg_val & SW1_MASK) == 0x00)
        {
            timer_count_sw1++;
        }
        
        if ((reg_val & SW2_MASK) == 0x00)
        {
             timer_count_sw2++;
        }
        
        if ((reg_val & SW3_MASK) == 0x00)
        {
             timer_count_sw3++;
        }


        

            //Once threshold reached, mark as pressed.
             if ((reg_val & SW1_MASK) == 0x00 && timer_count_sw1 == 5)
            {
                ALERT_SW1 = BUTTON_PRESSED;
             

            }
             if ((reg_val & SW2_MASK) == 0x00 && timer_count_sw2 == 5)
            {
                ALERT_SW2 = BUTTON_PRESSED;
           
           
            }
             if ((reg_val & SW3_MASK) == 0x00 && timer_count_sw3 == 5)
            {
                 ALERT_SW3 = BUTTON_PRESSED;
           
           
            }
       
           

            
            //Release switches after 50 ms if not being pressed
             if ((reg_val & SW1_MASK) != 0x00 && timer_count_sw1 > 5)
            {
                ALERT_SW1 = BUTTON_RELEASED;
                timer_count_sw1 = 0;
             

            }
             if ((reg_val & SW2_MASK) != 0x00 && timer_count_sw2 > 5)
            {
                ALERT_SW2 = BUTTON_RELEASED;
                timer_count_sw2 = 0;
           
            }
             if ((reg_val & SW3_MASK) != 0x00 && timer_count_sw3 > 5)
            {
                 ALERT_SW3 = BUTTON_RELEASED;
                 timer_count_sw3 = 0;
           
            }
            
         
 

}

/* ADD CODE */
/* Allocate static variables for a cyhal_timer_t and cyhal_timer_cfg_t that will be used
 * to configure the button timer*/
const cyhal_timer_cfg_t timer_cfg =
    {
        .compare_value = 0,              /* Timer compare value, not used */
        .period = 1000000,              /* number of timer ticks */
        .direction = CYHAL_TIMER_DIR_UP, /* Timer counts up */
        .is_compare = false,             /* Don't use compare mode */
        .is_continuous = true,           /* Run timer indefinitely */
        .value = 0                       /* Initial value of counter */
};

/* Timer object used */
cyhal_timer_t timer_obj;

static void push_buttons_timer_init(void)
{
    /*ADD CODE*/
    /*
     * Use timer_init() to configure a timer that generates an interrupt every 10mS
     * This timer will be used to debounce the push buttons  on the ECE353 Dev Board
     */
    timer_init(&timer_obj, &timer_cfg, 1000000,&Handler_Timer_Buttons);
}
/*****************************************************************************/
/* ICE 06 END */
/*****************************************************************************/
