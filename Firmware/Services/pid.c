/**
 * @file pid.c
 */

#include "pid.h"

static float clampf(float x, float lim){
    if(x > lim) { return lim;}
    if(x< -lim) {return -lim;}
    return x;
}

void Pid_reset(Pid_t *p){
    p->integ = 0.0f;
    p->prev_err = 0.0f;
}

float Pid_update(Pid_t *p, float err, float dt){
    if(dt <= 0.0f){
        return 0.0f;
    }
    p->integ = clampf(p->integ + (err*dt), p->i_max); //anti windup

    const float out = (p->kp *err) + (p->ki*p->integ) + (p->kd*(err-p->prev_err)/dt);
    p->prev_err = err;
    return clampf(out, p->out_max);
}

