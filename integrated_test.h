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

int run_all_integrated_tests(void);

#endif /* INTEGRATED_TEST_H */