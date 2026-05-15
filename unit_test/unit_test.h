/**
 * @file   unit_test.h
 * @author Steven.Yang
 * @brief: Unit test header for IRQ Simulator
 * @par:   Initialized revision
 * @copyright Copyright (c):
 *    2026 OmniVision Co.,Ltd
 */

#ifndef UNIT_TEST_H
#define UNIT_TEST_H

#include <stdio.h>

#define UT_ASSERT(cond, msg) do { \
    if (!(cond)) { \
        printf("  [FAIL] %s (%s:%d)\n", msg, __FILE__, __LINE__); \
        return 1; \
    } \
} while (0)

#define UT_ASSERT_EQ(a, b, msg) do { \
    if ((a) != (b)) { \
        printf("  [FAIL] %s: expected %u, got %u (%s:%d)\n", \
               msg, (unsigned int)(b), (unsigned int)(a), __FILE__, __LINE__); \
        return 1; \
    } \
} while (0)

#define UT_ASSERT_HEX_EQ(a, b, msg) do { \
    if ((a) != (b)) { \
        printf("  [FAIL] %s: expected 0x%08X, got 0x%08X (%s:%d)\n", \
               msg, (unsigned int)(b), (unsigned int)(a), __FILE__, __LINE__); \
        return 1; \
    } \
} while (0)

/**
 * @brief Run all unit test suites and print results summary
 * @remark Test Criteria: All unit test suites executed and results reported.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 */
int run_all_unit_tests(void);

#endif /* UNIT_TEST_H */