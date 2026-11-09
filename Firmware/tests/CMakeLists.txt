cmake_minimum_required(VERSION 3.16)
project(line_follower_host C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG v2.6.0
)
FetchContent_MakeAvailable(unity)

set(ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(WARNINGS -Wall -Wextra -Wshadow -Wdouble-promotion -Werror)

add_library(lf_logic STATIC
${ROOT}/Ecual/line_algo.c
${ROOT}/Services/pid.c)
target_include_directories(lf_logic PUBLIC ${ROOT}/Ecual ${ROOT}/Services)
target_compile_options(lf_logic PRIVATE ${WARNINGS})

add_executable(unit_tests
test_main.c
test_line_algo.c
test_pid.c)


target_compile_options(unit_tests PRIVATE -Wall -Wextra -Werror)
target_link_libraries(unit_tests PRIVATE lf_logic unity)

add_executable(sim ${ROOT}/sim/sim.c)
target_include_directories(sim PRIVATE ${ROOT}/App)
target_compile_options(sim PRIVATE ${WARNING})
target_link_libraries(sim PRIVATE lf_logic m)

enable_testing()
add_test(NAME unit_tests COMMAND unit_tests)
add_test(NAME sim_converges COMMAND sim)

