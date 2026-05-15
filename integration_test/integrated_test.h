/**
 * @file   integrated_test.h
 * @author Steven.Yang
 * @brief: Integration test header for IRQ Simulator
 * @par:   Initialized revision
 * @copyright Copyright (c):
 *    2026 OmniVision Co.,Ltd
 */

#ifndef INTEGRATED_TEST_H
#define INTEGRATED_TEST_H

#include <stdio.h>
#include <string.h>

#define IT_ASSERT(cond, msg) do { \
    if (!(cond)) { \
        printf("  [FAIL] %s (%s:%d)\n", msg, __FILE__, __LINE__); \
        return 1; \
    } \
} while (0)

#define IT_ASSERT_EQ(a, b, msg) do { \
    if ((a) != (b)) { \
        printf("  [FAIL] %s: expected %u, got %u (%s:%d)\n", \
               msg, (unsigned int)(b), (unsigned int)(a), __FILE__, __LINE__); \
        return 1; \
    } \
} while (0)

#define IT_ASSERT_HEX_EQ(a, b, msg) do { \
    if ((a) != (b)) { \
        printf("  [FAIL] %s: expected 0x%08X, got 0x%08X (%s:%d)\n", \
               msg, (unsigned int)(b), (unsigned int)(a), __FILE__, __LINE__); \
        return 1; \
    } \
} while (0)

/**
 * @brief Run all integration test suites and print results summary
 * @remark Test Criteria: All integration test suites executed and results reported.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0|1] P=[0=all passed, 1=some failed] N=[N/A] D=[exit code: 0 on success, 1 on failure]
 */
int run_all_integrated_tests(void);

#endif /* INTEGRATED_TEST_H */