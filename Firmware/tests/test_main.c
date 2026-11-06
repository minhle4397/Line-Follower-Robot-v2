#include "unity.h"
#include "tests.h"

void setUp(void) {}
void tearDown(void) {}

int main(void){
    UNITY_BEGIN();
    run_line_algo_tests();
    run_pid_tests();
    return UNITY_END();
}
