#include "task_sw.h"

#if defined(HW03A)

/**
 * @brief
 * Function used to detect when SW1 has been pressed.  The task should detect the initial button
 * press by debouncing the push button for 50mS.  Pressing the button for longer than 50mS should
 * only result in a single button event.
 *
 * When the button press is detected, send EVENT_UI_SW1 to eg_UI
 * @param param
 *  Unused
 */
void task_sw1(void *param)
{
  /* Allocate local variables used in this task */
  bool state_button_prev = false;
  bool state_button_curr = false;
  while (1)
  {
    vTaskDelay(50);
    /* Check the current state of the button */
    if ((PORT_BUTTONS->IN & SW1_MASK) == 0x00)
    {
      state_button_curr = true;
    }
    else
    {
      state_button_curr = false;
    }

    /* Detect if the button has been pressed */
    if (state_button_curr && !state_button_prev)
    {
      /*set Event Bits*/
      xEventGroupSetBits(eg_UI, EVENT_UI_SW1);
    }

    /* Update the previous button state */
    state_button_prev = state_button_curr;
  }
}
/**
 * @brief
 * Function used to detect when SW2 has been pressed.  The task should detect the initial button
 * press by debouncing the push button for 50mS.  Pressing the button for longer than 50mS should
 * only result in a single button event.
 *
 * When the button press is detected, send EVENT_UI_SW2 to eg_UI
 * @param param
 *  Unused
 */
void task_sw2(void *param)
{
  /* Allocate local variables used in this task */
  bool state_button_prev = false;
  bool state_button_curr = false;
  while (1)
  {
    vTaskDelay(50);
    /* Check the current state of the button */
    if ((PORT_BUTTONS->IN & SW2_MASK) == 0x00)
    {
      state_button_curr = true;
    }
    else
    {
      state_button_curr = false;
    }

    /* Detect if the button has been pressed */
    if (state_button_curr && !state_button_prev)
    {
      /*set Event Bits*/
      xEventGroupSetBits(eg_UI, EVENT_UI_SW2);
    }

    /* Update the previous button state */
    state_button_prev = state_button_curr;
  }
}

/**
 * @brief
 * Initializes User Push Buttons and creates the corresponding FreeRTOS tasks for SW1 and SW2
 */
void task_sw_init(void)
{
  push_buttons_init(false);

  // Register the Task with FreeRTOS
  xTaskCreate(
      task_sw1,
      "Task SW1",
      configMINIMAL_STACK_SIZE,
      NULL,
      HW03_TASK_PRIORITY_UI_INPUT,
      NULL);

  xTaskCreate(
      task_sw2,
      "Task SW2",
      configMINIMAL_STACK_SIZE,
      NULL,
      HW03_TASK_PRIORITY_UI_INPUT,
      NULL);
}

#endif
