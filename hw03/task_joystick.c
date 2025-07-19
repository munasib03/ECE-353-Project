#include "task_joystick.h"

#if defined(HW03A)

/**
 * @brief
 * The Joystick Task will monitor the ADC channels connected to the joystick. The joystick
 * should be examined every 100mS.  The joystick task is the ONLY task that is allowed to
 * directly access the ADC peripheral.
 *
 * This task will determine when the joystick has been moved out of the center position
 * to either UP, DOWN, RIGHT, or LEFT. You should only inform the FSM that the joystick
 * has been moved from the CENTER postion to UP, DOWN, RIGHT, or LEFT.  DO NOT report the
 * joystick moving back to the CENTER.
 *
 * When the task determines that the joystick has been moved UP, DOWN, LEFT, or RIGHT,
 * you will need to:
 *
 *  1.) Send the current position to q_Dir
 *  2.) Wake up the FSM by sending EVENT_UI_MOVE
 *
 * NOTE: You will need to send the joystick position to q_Dir first,
 * THEN send then set the corresponding bit in eg_UI.
 *
 * @param param
 *  Unused
 */
void task_joystick(void *param)
{
  joystick_position_t cur_pos = JOYSTICK_POS_CENTER;
  joystick_position_t prev_pos = JOYSTICK_POS_CENTER;
  while (1)
  {
    /* The Joystick will be ready every 100mS*/
    vTaskDelay(100);

    /* Allocate local variables used in this task */
    Direction_t direction;
    /* Get the current position of the joystick */
    cur_pos = joystick_get_pos();

    /* If the position of the joystick has changed, send a message to the joystick queue*/
    if (cur_pos != JOYSTICK_POS_CENTER && prev_pos == JOYSTICK_POS_CENTER)
    {
      if (cur_pos != prev_pos)
      {
        if (cur_pos == JOYSTICK_POS_RIGHT)
        {
          direction = DIR_RIGHT;             // Move Active square one position to Right
        }
        else if (cur_pos == JOYSTICK_POS_LEFT)
        {
          direction = DIR_LEFT;              // Move Active square one position to Left
        }
        else if (cur_pos == JOYSTICK_POS_DOWN)
        {
          direction = DIR_DOWN;             // Move Active square one position to Down
        }
        else if (cur_pos == JOYSTICK_POS_UP)
        {
          direction = DIR_UP;              // Move Active square one position to Up
        }
      }
      /* Send a message to the joystick queue */
      xEventGroupSetBits(eg_UI, EVENT_UI_MOVE);
      xQueueSend(q_Dir, &direction, portMAX_DELAY);
    }
    prev_pos = cur_pos; // Ipdate previous position
  }
}

/**
 * @brief
 */
void task_joystick_init(void)
{
  joystick_init();

  // Register the Task with FreeRTOS
  xTaskCreate(
      task_joystick,
      "Task Joystick",
      configMINIMAL_STACK_SIZE,
      NULL,
      HW03_TASK_PRIORITY_UI_INPUT,
      NULL);
}

#endif
