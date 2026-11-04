/**
 * @file line_algo.h
 * @brief line sensor processing
 */
#ifndef LINE_ALGO_H
#define LINE_ALGO_H

#include <stdbool.h>
#include <stdint.h>

#define LINE_N          5U //5 IR sensors
#define LINE_ADC_MAX    4095U
#define LINE_NORM_MAX   1000 //Normalized value range: 0...1000
#define LINE_POS_MAX    2000 //Position weight range: -2000 -1000 0 +1000 +2000

typedef struct{
    uint16_t min[LINE_N];
    uint16_t max [LINE_N];
    bool inverted; //true: line is darker in ADC terms than background
} LineCal_t;

void LineAlgo_CalReset(LineCal_t *cal, bool inverted);
void LineAlgo_CalUpdate(LineCal_t *cal, const uint16_t raw[LINE_N]);

/**
 * @return true if every channel saw at least
 * @p min_span ADC counts of contrast.
 */
bool LineAlgo_IsCalibrated(const LineCal_t *cal, uint16_t min_span);

/**
 * Scale raw readings to 0 (background) ... 1000 (line)
 */
void LineAlgo_Normalize(const LineCal_t *cal, const uint16_t raw[LINE_N], int16_t out[LINE_N]);

/**
 * @brief Weighted-average line position
 * @param pos -2000 (line under left-most sensor) ... +2000 (right-most)
 * @return false if no sensor sees the line (line lost)
 */

bool LineAlgo_Position(const LineCal_t *cal, const uint16_t raw[LINE_N], int16_t *pos);

/**Number of sensors whose normalized value is >= @p threshold */
uint8_t LineAlgo_CountActive(const LineCal_t *cal, const uint16_t raw[LINE_N], int16_t threshold);

#endif /* LINE_ALGO_H*/
