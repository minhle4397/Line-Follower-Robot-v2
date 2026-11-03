/**
 * @file pid.h
 * @brief Discreate PID controller with integral anti-windup
 */
#ifndef PID_H
#define PID_H

typedef struct
{
    float kp;
    float ki;
    float kd;
    float i_max; //integral limit
    float out_max; //output limit
    float integ; //state
    float prev_err; //state
} Pid_t;

void Pid_reset(Pid_t *p);
float Pid_update(Pid_t *p, float err, float dt);

#endif
