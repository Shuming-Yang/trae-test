/**
 * @file   main.c
 * @author Steven.Yang
 * @brief: Main entry point for test-trae project
 * @par:   Initialized revision
 * @copyright Copyright (c):
 *    2026 OmniVision Co.,Ltd
 */

#include <stdio.h>

/**
 * @brief Main entry point of the program
 * @remark Test Criteria: Program execution result.
 * @param [in] argc type=[int] R=[1-0x7fffffff] P=[Vail address] N=[0] D=[argument count]
 * @param [in] argv type=[char**] R=[Vail address] P=[Vail address] N=[NULL] D=[argument vector]
 * @retval type=[int] R=[0-0xff] P=[N/A] N=[N/A] D=[exit status]
 */
int main(int argc, char* argv[]) {
    printf("Hello, TRAE IDE!\n");
    printf("This is a C project built with CMake.\n");
    return 0;
}