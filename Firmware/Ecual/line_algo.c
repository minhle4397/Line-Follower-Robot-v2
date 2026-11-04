/**
 * @file line_algo.c
 * @brief line sensor processing
 */

#include "line_algo.h"
#define NOISE_FLOOR 100 //Normalized values below this are treated as 0

static const int16_t k_weight[LINE_N] = {-2000, -1000, 0, 1000, 2000};

void LineAlgo_CalReset(LineCal_t *cal, bool inverted){
    for(uint32_t i = 0U; i < LINE_N; i++){
        cal->min[i] = (uint16_t)LINE_ADC_MAX;
        cal->max[i] = 0U;
    }
    cal->inverted = inverted;
}
void LineAlgo_CalUpdate(LineCal_t *cal, const uint16_t raw[LINE_N]){
    for(uint16_t i = 0U; i < LINE_N; i++){
        if(raw[i] < cal->min[i]) { cal->min[i] = raw[i]; }
        if(raw[i] > cal->max[i]) { cal->max[i] = raw[i]; }
    }
}
bool LineAlgo_IsCalibrated(const LineCal_t *cal, uint16_t min_span){
    for(uint32_t i = 0U; i < LINE_N; i++){
        if((cal->max[i] <= cal->min[i]) || ((uint16_t)(cal->max[i] - cal->min[i]) < min_span))
        {
            return false;
        }
    }
    return true;
}
void LineAlgo_Normalize(const LineCal_t *cal, const uint16_t raw[LINE_N], int16_t out[LINE_N]){
    for(uint32_t i = 0U; i < LINE_N; i++){
        const int32_t span = (int32_t)cal->max[i] - (int32_t)cal->min[i];
        int32_t v = 0;

        if(span > 0){
            v = (((int32_t)raw[i] - (int32_t)cal->min[i])*LINE_NORM_MAX)/span;
        }
        if(v<0) v = 0;
        if(v>LINE_NORM_MAX) v = LINE_NORM_MAX;
        if(cal->inverted) v = LINE_NORM_MAX - v;

        out[i] = (int16_t)v;
    }
}

bool LineAlgo_Position(const LineCal_t *cal, const uint16_t raw[LINE_N], int16_t *pos){
    int16_t norm[LINE_N];
    int32_t sum = 0;
    int32_t wsum = 0;

    LineAlgo_Normalize(cal, raw, norm);
    for(uint32_t i = 0U; i<LINE_N; i++){
        const int32_t v = (norm[i] < NOISE_FLOOR) ? 0 : norm[i];
        sum += v;
        wsum += v*k_weight[i];
    }
    if(sum == 0){
        return false;
    }
    *pos = (int16_t)(wsum/sum);
    return true;
}

uint8_t LineAlgo_CountActive(const LineCal_t *cal, const uint16_t raw[LINE_N], int16_t threshold){
    int16_t norm[LINE_N];
    uint8_t n = 0U;

    LineAlgo_Normalize(cal, raw, norm);
    for(uint32_t i = 0U; i < LINE_N; i++){
        if(norm[i] >= threshold) n++;
    }
    return n;
}
