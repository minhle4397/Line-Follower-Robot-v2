/**
 * @file app_config.h
 * @brief tuning parameter
 */

#ifndef APP_CONFIG_H
#define APP_CONFIG_H

 //Task period
#define APP_CTRL_PERIOD_MS 5U //line following loop: 200Hz
#define APP_CTRL_DT_S      0.005f

//Steering PID
#define APP_STEER_KP    0.08f
#define APP_STEER_KI    0.0f
#define APP_STEER_KD    0.002f
#define APP_STEER_I_MAX 100.0f
#define APP_STEER_O_MAX 400.0f
#define APP_BASE_SPEED  400

#endif
