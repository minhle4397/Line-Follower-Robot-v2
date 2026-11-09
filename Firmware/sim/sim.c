/**
 * @file sim.c
 * @brief software-in-the-loop simulation of the line follower
 * The result is a starting point for tuning
 * Usage: ./sim -> runs all scenarios, exit code 0 if all pass
 *        ./sim csv -> prints a CSV file for plotting
 */

#include "math.h"
#include "stdbool.h"
#include "stdio.h"
#include "string.h"

#include "app_config.h"
#include "line_algo.h"
#include "pid.h"

//Mechanical parameters
#define SENSOR_PITCH_M  0.012f //distance between 2 IR sensors
#define LOOKAHEAD_M     0.080f //sensor array in front of the wheel axle
#define WHEEL_BASE_M    0.130f //distance between 2 wheel
#define V_MAX_MPS       1.0f //speed at duty = 1000
#define LINE_SIGMA_M    0.006f //half the line width

#define SIM_STEPS       600 //600*5ms = 3s
#define PASS_ERROR_MM   2.0f

//Fake ADC
static void fake_adc(float line_offset_m, uint16_t raw[LINE_N]){
    for(int i = 0; i < (int)LINE_N; i++){
        const float d = line_offset_m - (float)(i-2)*SENSOR_PITCH_M;
        raw[i] = (uint16_t)(300.0f + 2500.0f * expf(-(d*d)/(2.0f * LINE_SIGMA_M*LINE_SIGMA_M)));
    }
}

// y : line position relative to axle centre of wheel [m], + mean: line on the RIGHT
// th (theta): car heading relative to the line [rad], + mean: turned LEFT (counter clock wise)
static bool run_scenario(const char *name, float y, float th, bool csv){
    LineCal_t cal;
    LineAlgo_CalReset(&cal, false);
    for(float o = -0.04f; o <= 0.04f; o+= 0.001f){  //calib sweep
        uint16_t r[LINE_N];
        fake_adc(o, r);
        LineAlgo_CalUpdate(&cal, r);
    }
    Pid_t steer = {.kp = APP_STEER_KP, .ki = APP_STEER_KI, .kd = APP_STEER_KD,
                    .i_max = APP_STEER_I_MAX, .out_max = APP_STEER_O_MAX};
    Pid_reset(&steer);
    if(csv){ printf("t,y_mm,pos,duty_l,duty_r\n");}

    for(int k = 0; k < SIM_STEPS; k++){
        uint16_t raw[LINE_N];
        int16_t pos;

        fake_adc(y + LOOKAHEAD_M*sinf(th), raw);
        if(!LineAlgo_Position(&cal, raw, &pos)){
            printf("[FAIL] %-22s line lost at t=%.3f s\n", name, (double)((float)k*APP_CTRL_DT_S));
            return false;
        }
        const float u = Pid_update(&steer, (float)pos, APP_CTRL_DT_S);
        float dl = (float)APP_BASE_SPEED + u;
        float dr = (float)APP_BASE_SPEED - u;
        dl = fminf(fmaxf(dl, -1000.0f), 1000.0f);
        dr = fminf(fmaxf(dr, -1000.0f), 1000.0f);

        const float vl = dl / 1000.0f * V_MAX_MPS;
        const float vr = dr / 1000.0f * V_MAX_MPS;
        const float v = 0.5f * (vl+vr);
        const float w = (vr-vl)/WHEEL_BASE_M;
        th += w*APP_CTRL_DT_S;
        y += v*sinf(th)*APP_CTRL_DT_S;

        if(csv){
            printf("%.3f, %.3f, %d, %.0f, %.0f\n", (double)((float)k*APP_CTRL_DT_S),
                    (double)(y*1000.0f), pos, (double)dl, (double)dr);
        }
    }
    const float err_mm = fabsf(y*1000.0f);
    const bool ok = err_mm < PASS_ERROR_MM;
    if(!csv){
        printf("[%s] %-22s final error %.2f mm\n", ok ? "OK":"FAIL", name, (double)err_mm);
    }
    return ok;
}

int main(int argc, char **argv){
    if((argc > 1) && (strcmp(argv[1], "--csv")==0)){
        return run_scenario("offset 15mm", 0.015f, 0.0f, true) ? 0:1;
    }

    bool ok = true;
    ok &= run_scenario("offset +15mm", 0.015f, 0.0f, false);
    ok &= run_scenario("offset -20mm", -0.020f, 0.0f, false);
    ok &= run_scenario("heading +10 deg", 0.0f, 0.175f, false);
    ok &= run_scenario("offset+heading", 0.010f, -0.12f, false);
    return ok ? 0 : 1;
}
