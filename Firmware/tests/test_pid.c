#include "unity.h"
#include "tests.h"
#include "pid.h"

static void test_proportional_only(void){
    Pid_t p = {.kp = 2.0f, .out_max = 1000.0f};
    Pid_reset(&p);
    TEST_ASSERT_EQUAL_FLOAT(20.0f, Pid_update(&p, 10.0f, 0.01f));
}

static void test_output_saturates(void){
    Pid_t p = {.kp = 10.0f, .out_max = 100.0f};
    Pid_reset(&p);
    TEST_ASSERT_EQUAL_FLOAT(100.0f, Pid_update(&p, 50.0f, 0.005f));
    TEST_ASSERT_EQUAL_FLOAT(-100.0f, Pid_update(&p, -50.0f, 0.005f));
}
static void test_anti_windup(void){
    Pid_t p = {.ki = 1.0f, .i_max = 2.0f, .out_max = 1000.0f};
    Pid_reset(&p);
    for(int i = 0; i < 1000; i++) {(void)Pid_update(&p, 100.0f, 0.01f);}
    TEST_ASSERT_EQUAL_FLOAT(2.0f, p.integ);
}
static void test_derivative_on_error_change(void){
    Pid_t p = {.kd = 0.1f, .out_max = 1000.0f};
    Pid_reset(&p);
    (void)Pid_update(&p, 0.0f, 0.01f);
    TEST_ASSERT_FLOAT_WITHIN(1e-3f, 10.0f, Pid_update(&p, 1.0f, 0.01f));
    TEST_ASSERT_FLOAT_WITHIN(1e-3f, 0.0f, Pid_update(&p, 1.0f, 0.01f));
}
static void test_invalid_dt_returns_zero(void){
    Pid_t p = {.kp = 1.0f, .out_max = 1000.0f};
    Pid_reset(&p);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, Pid_update(&p, 5.0f, 0.0f));
}
void run_pid_tests(void){
    RUN_TEST(test_proportional_only);
    RUN_TEST(test_output_saturates);
    RUN_TEST(test_anti_windup);
    RUN_TEST(test_derivative_on_error_change);
    RUN_TEST(test_invalid_dt_returns_zero);
}
