#include "unity.h"
#include "tests.h"
#include "line_algo.h"

static const uint16_t k_white[LINE_N] = {200, 200, 200, 200, 200};
static const uint16_t k_black[LINE_N] = {3000, 3000, 3000, 3000, 3000};

static LineCal_t make_cal(bool inverted){
    LineCal_t cal;
    LineAlgo_CalReset(&cal, inverted);
    LineAlgo_CalUpdate(&cal, k_white);
    LineAlgo_CalUpdate(&cal, k_black);
    return cal;
}
static void test_center(void){
    LineCal_t cal = make_cal(false);
    const uint16_t raw[LINE_N] = {200, 200, 3000, 200, 200};
    int16_t pos = 999;
    TEST_ASSERT_TRUE(LineAlgo_Position(&cal, raw, &pos));
    TEST_ASSERT_EQUAL_INT16(0, pos);
}

static void test_far_left_and_far_right(void){
    LineCal_t cal = make_cal(false);
    const uint16_t left[LINE_N] = {3000, 200, 200, 200, 200};
    const uint16_t right[LINE_N] = {200, 200, 200, 200, 3000};
    int16_t pos = 0;
    TEST_ASSERT_TRUE(LineAlgo_Position(&cal, left, &pos));
    TEST_ASSERT_EQUAL_INT16(-2000, pos);
    TEST_ASSERT_TRUE(LineAlgo_Position(&cal, right, &pos));
    TEST_ASSERT_EQUAL_INT16(2000, pos);
}

static void test_between_two_sensors(void){
    LineCal_t cal = make_cal(false);
    const uint16_t raw[LINE_N] = {200, 200, 3000, 3000, 200};
    int16_t pos = 0;
    TEST_ASSERT_TRUE(LineAlgo_Position(&cal, raw, &pos));
    TEST_ASSERT_EQUAL_INT16(500, pos);
}

static void test_integral_division_truncates(void){
    LineCal_t cal = make_cal(false);
    const uint16_t raw[LINE_N] = {200, 3000, 1600, 200, 200};
    int16_t pos = 0;
    TEST_ASSERT_TRUE(LineAlgo_Position(&cal, raw, &pos));
    TEST_ASSERT_EQUAL_INT16(-666, pos);
}

static void test_line_lost(void){
    LineCal_t cal = make_cal(false);
    int16_t pos = 123;
    TEST_ASSERT_FALSE(LineAlgo_Position(&cal, k_white, &pos));
    TEST_ASSERT_EQUAL_INT16(123, pos);
}

static void test_noise_below_floor_is_ignored(void){
    LineCal_t cal = make_cal(false);
    const uint16_t raw[LINE_N] = {400, 200, 200, 200, 200};
    int16_t pos = 0;
    TEST_ASSERT_FALSE(LineAlgo_Position(&cal, raw, &pos));
}

static void test_inverted_line(void){
    LineCal_t cal = make_cal(true);
    const uint16_t raw[LINE_N] = {3000, 3000, 3000, 3000, 200};
    int16_t pos = 0;
    TEST_ASSERT_TRUE(LineAlgo_Position(&cal, raw, &pos));
    TEST_ASSERT_EQUAL_INT16(2000, pos);
}

static void test_normalize_clamps(void){
    LineCal_t cal = make_cal(false);
    const uint16_t raw[LINE_N] = {0, 200, 1600, 3000, 4095};
    int16_t n[LINE_N];
    LineAlgo_Normalize(&cal, raw, n);
    TEST_ASSERT_EQUAL_INT16(0, n[0]);
    TEST_ASSERT_EQUAL_INT16(0, n[1]);
    TEST_ASSERT_EQUAL_INT16(500, n[2]);
    TEST_ASSERT_EQUAL_INT16(1000, n[3]);
    TEST_ASSERT_EQUAL_INT16(1000, n[4]);
}

static void test_is_calibrated(void){
    LineCal_t cal;
    LineAlgo_CalReset(&cal, false);
    TEST_ASSERT_FALSE(LineAlgo_IsCalibrated(&cal, 300U));
    LineAlgo_CalUpdate(&cal, k_white);
    TEST_ASSERT_FALSE(LineAlgo_IsCalibrated(&cal, 300U));
    LineAlgo_CalUpdate(&cal, k_black);
    TEST_ASSERT_TRUE(LineAlgo_IsCalibrated(&cal, 300U));
}

static void test_count_active(void){
    LineCal_t cal = make_cal(false);
    const uint16_t raw[LINE_N] = {3000, 3000, 200, 2500, 200};
    TEST_ASSERT_EQUAL_UINT8(3, LineAlgo_CountActive(&cal, raw, 700));
    TEST_ASSERT_EQUAL_UINT8(5, LineAlgo_CountActive(&cal, k_black, 700));
}
void run_line_algo_tests(void){
    RUN_TEST(test_center);
    RUN_TEST(test_far_left_and_far_right);
    RUN_TEST(test_between_two_sensors);
    RUN_TEST(test_integral_division_truncates);
    RUN_TEST(test_line_lost);
    RUN_TEST(test_noise_below_floor_is_ignored);
    RUN_TEST(test_inverted_line);
    RUN_TEST(test_normalize_clamps);
    RUN_TEST(test_is_calibrated);
    RUN_TEST(test_count_active);
}
