#include "task_imu.h"

#if defined(HW03A)

/**
 * @brief
 * The IMU Task will monitor the IMU every 100mS.  The IMU task is the ONLY task that is
 * allowed to directly access the IMU device.
 *
 * This task will determine when the IMU has been moved in the X or Y axis
 *
 * You should only inform the FSM that the IMU has been moved from the CENTER postion to
 * UP, DOWN, RIGHT, or LEFT.  DO NOT report the IMU moving back to the CENTER.
 *
 * When the task determines that the IMU has been moved UP, DOWN, LEFT, or RIGHT,
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
void task_imu(void *param)
{
  // Initializing IMU
  static uint8_t whoamI, rst;
  static uint8_t tx_buffer[1000];
  spi_init();
  lsm6dsm_init();
  uint8_t x_data = 0;
  uint8_t y_data = 0;
  uint8_t x_prev_data = 0;
  uint8_t y_prev_data = 0;
  while (1)
  {
    vTaskDelay(100); // Monitor Task every 100 mS
    lsm6dsm_read_reg(&dev_ctx, LSM6DSM_OUTX_H_XL, &x_data, 1); // Read x axis register
    lsm6dsm_read_reg(&dev_ctx, LSM6DSM_OUTY_H_XL, &y_data, 1); // Read y axis register
    Direction_t dir = DIR_CENTER; // Default position center
    // Check boundary conditions
    if ((x_prev_data <= 15 || x_prev_data >= 240) && (y_prev_data <= 15 || y_prev_data >= 240))
    {
      if(x_prev_data <= 15 || x_prev_data >= 240)
      {
        if (x_data > 15 && x_data < 60)
        {
          // Move Active square one position to Left
          dir = DIR_LEFT;
          xQueueSend(q_Dir, &dir, portMAX_DELAY);
          xEventGroupSetBits(eg_UI, EVENT_UI_MOVE);
        }

        else if (x_data < 240 && x_data > 195)
        {
          // Move Active square one position to Right
          dir = DIR_RIGHT;
          xQueueSend(q_Dir, &dir, portMAX_DELAY);
          xEventGroupSetBits(eg_UI, EVENT_UI_MOVE);
        }
      } 
    
    if (y_prev_data <= 15 || y_prev_data >= 240)
    {
      if (y_data > 15 && y_data < 60)
      {
        // Move Active square one position to Down
        dir = DIR_DOWN;
        xQueueSend(q_Dir, &dir, portMAX_DELAY);
        xEventGroupSetBits(eg_UI, EVENT_UI_MOVE);
      }

      else if (y_data < 240 && y_data > 195)
      {
        // Move Active square one position to Up
        dir = DIR_UP;
        xQueueSend(q_Dir, &dir, portMAX_DELAY);
        xEventGroupSetBits(eg_UI, EVENT_UI_MOVE);
      }
    }
   } 
   
   // Update previous position
   x_prev_data = x_data;
   y_prev_data = y_data;

  }
}

/**
 * @brief
 */
void task_imu_init(void)
{
  
  xTaskCreate(
      task_imu,
      "Task_imu",
      10 * configMINIMAL_STACK_SIZE,
      NULL,
      HW03_TASK_PRIORITY_UI_INPUT,
      NULL);
}

#endif
