/**
 * @file   unit_test.c
 * @author Steven.Yang
 * @brief: Unit tests for IRQ Simulator core functions
 * @par:   Initialized revision
 * @copyright Copyright (c):
 *    2026 OmniVision Co.,Ltd
 */

#include "unit_test.h"
#include "main.h"

static unsigned int g_ut_passed = 0;
static unsigned int g_ut_failed = 0;

#define RUN_TEST(name) do { \
    printf("  Running %s...\n", #name); \
    irq_reset_all(); \
    if (name() == 0) { \
        printf("  [PASS] %s\n", #name); \
        g_ut_passed++; \
    } else { \
        g_ut_failed++; \
    } \
} while (0)

/* ================================================================
 * UT-01: tick_irq_handler
 * ================================================================ */

static int test_tick_initial(void) {
    UT_ASSERT_EQ(irq_get_tick(), 0, "UT-01-01: initial tick should be 0");
    return 0;
}

static int test_tick_single_call(void) {
    tick_irq_handler();
    UT_ASSERT_EQ(irq_get_tick(), 1, "UT-01-02: tick should be 1 after one call");
    return 0;
}

static int test_tick_multiple_calls(void) {
    tick_irq_handler();
    tick_irq_handler();
    tick_irq_handler();
    tick_irq_handler();
    tick_irq_handler();
    UT_ASSERT_EQ(irq_get_tick(), 5, "UT-01-03: tick should be 5 after five calls");
    return 0;
}

static int test_tick_after_reset(void) {
    tick_irq_handler();
    tick_irq_handler();
    tick_irq_handler();
    irq_reset_all();
    tick_irq_handler();
    tick_irq_handler();
    tick_irq_handler();
    UT_ASSERT_EQ(irq_get_tick(), 3, "UT-01-04: tick should be 3 after reset + 3 calls");
    return 0;
}

/* ================================================================
 * UT-02: exception_irq_handler
 * ================================================================ */

static int test_exception_no_crash(void) {
    exception_irq_handler();
    UT_ASSERT(1, "UT-02-01: exception_irq_handler should not crash");
    return 0;
}

static int test_exception_multiple_calls(void) {
    exception_irq_handler();
    exception_irq_handler();
    exception_irq_handler();
    UT_ASSERT(1, "UT-02-02: exception_irq_handler x3 should not crash");
    return 0;
}

/* ================================================================
 * UT-03: irq_trigger
 * ================================================================ */

static int test_trigger_irq0(void) {
    irq_trigger(0);
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0x00000001, "UT-03-01: trigger IRQ0");
    return 0;
}

static int test_trigger_irq5(void) {
    irq_trigger(5);
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0x00000020, "UT-03-02: trigger IRQ5");
    return 0;
}

static int test_trigger_irq31(void) {
    irq_trigger(31);
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0x80000000, "UT-03-03: trigger IRQ31");
    return 0;
}

static int test_trigger_accumulate(void) {
    irq_trigger(0);
    irq_trigger(1);
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0x00000003, "UT-03-04: accumulate IRQ0+IRQ1");
    return 0;
}

static int test_trigger_duplicate(void) {
    irq_trigger(0);
    irq_trigger(0);
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0x00000001, "UT-03-05: duplicate trigger IRQ0");
    return 0;
}

static int test_trigger_invalid_32(void) {
    uint32_t before = irq_get_pending();
    irq_trigger(32);
    UT_ASSERT_HEX_EQ(irq_get_pending(), before, "UT-03-06: invalid IRQ32 should not change pending");
    return 0;
}

static int test_trigger_invalid_99(void) {
    uint32_t before = irq_get_pending();
    irq_trigger(99);
    UT_ASSERT_HEX_EQ(irq_get_pending(), before, "UT-03-07: invalid IRQ99 should not change pending");
    return 0;
}

/* ================================================================
 * UT-04: irq_handler
 * ================================================================ */

static int test_handler_irq0(void) {
    irq_trigger(0);
    irq_handler(0);
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0x00000000, "UT-04-01: handler IRQ0 clears bit 0");
    UT_ASSERT_EQ(irq_get_tick(), 1, "UT-04-01: handler IRQ0 increments tick");
    return 0;
}

static int test_handler_irq5(void) {
    irq_trigger(5);
    irq_handler(5);
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0x00000000, "UT-04-02: handler IRQ5 clears bit 5");
    return 0;
}

static int test_handler_irq31(void) {
    irq_trigger(31);
    irq_handler(31);
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0x00000000, "UT-04-03: handler IRQ31 clears bit 31");
    return 0;
}

static int test_handler_clears_pending(void) {
    irq_trigger(0);
    irq_handler(0);
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0, "UT-04-04: pending should be 0 after handler");
    return 0;
}

/* ================================================================
 * UT-05: irq_process_all
 * ================================================================ */

static int test_process_all_empty(void) {
    irq_process_all();
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0, "UT-05-01: process_all with no pending should be safe");
    return 0;
}

static int test_process_all_single(void) {
    irq_trigger(3);
    irq_process_all();
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0, "UT-05-02: process_all should clear single IRQ3");
    return 0;
}

static int test_process_all_multiple(void) {
    irq_trigger(0);
    irq_trigger(5);
    irq_trigger(10);
    irq_process_all();
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0, "UT-05-03: process_all should clear IRQ0,5,10");
    return 0;
}

static int test_process_all_full(void) {
    unsigned int i;
    for (i = 0; i < IRQ_COUNT; i++) {
        irq_trigger(i);
    }
    irq_process_all();
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0, "UT-05-04: process_all should clear all 32 IRQs");
    return 0;
}

/* ================================================================
 * UT-06: irq_reset_all
 * ================================================================ */

static int test_reset_pending(void) {
    irq_trigger(5);
    irq_reset_all();
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0, "UT-06-01: reset clears pending");
    return 0;
}

static int test_reset_tick(void) {
    tick_irq_handler();
    tick_irq_handler();
    tick_irq_handler();
    irq_reset_all();
    UT_ASSERT_EQ(irq_get_tick(), 0, "UT-06-02: reset clears tick");
    return 0;
}

static int test_reset_both(void) {
    irq_trigger(7);
    tick_irq_handler();
    tick_irq_handler();
    irq_reset_all();
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0, "UT-06-03: reset clears both pending and tick");
    UT_ASSERT_EQ(irq_get_tick(), 0, "UT-06-03: reset clears tick too");
    return 0;
}

/* ================================================================
 * UT-07: irq_get_pending / irq_get_tick
 * ================================================================ */

static int test_get_pending_initial(void) {
    irq_reset_all();
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0, "UT-07-01: initial pending should be 0");
    return 0;
}

static int test_get_tick_initial(void) {
    irq_reset_all();
    UT_ASSERT_EQ(irq_get_tick(), 0, "UT-07-02: initial tick should be 0");
    return 0;
}

static int test_get_pending_after_trigger(void) {
    irq_reset_all();
    irq_trigger(7);
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0x00000080, "UT-07-03: pending after trigger IRQ7");
    return 0;
}

/* ================================================================
 * Test Runner
 * ================================================================ */

int run_all_unit_tests(void) {
    g_ut_passed = 0;
    g_ut_failed = 0;

    printf("\n========== Unit Tests ==========\n\n");

    printf("[UT-01] tick_irq_handler:\n");
    RUN_TEST(test_tick_initial);
    RUN_TEST(test_tick_single_call);
    RUN_TEST(test_tick_multiple_calls);
    RUN_TEST(test_tick_after_reset);

    printf("\n[UT-02] exception_irq_handler:\n");
    RUN_TEST(test_exception_no_crash);
    RUN_TEST(test_exception_multiple_calls);

    printf("\n[UT-03] irq_trigger:\n");
    RUN_TEST(test_trigger_irq0);
    RUN_TEST(test_trigger_irq5);
    RUN_TEST(test_trigger_irq31);
    RUN_TEST(test_trigger_accumulate);
    RUN_TEST(test_trigger_duplicate);
    RUN_TEST(test_trigger_invalid_32);
    RUN_TEST(test_trigger_invalid_99);

    printf("\n[UT-04] irq_handler:\n");
    RUN_TEST(test_handler_irq0);
    RUN_TEST(test_handler_irq5);
    RUN_TEST(test_handler_irq31);
    RUN_TEST(test_handler_clears_pending);

    printf("\n[UT-05] irq_process_all:\n");
    RUN_TEST(test_process_all_empty);
    RUN_TEST(test_process_all_single);
    RUN_TEST(test_process_all_multiple);
    RUN_TEST(test_process_all_full);

    printf("\n[UT-06] irq_reset_all:\n");
    RUN_TEST(test_reset_pending);
    RUN_TEST(test_reset_tick);
    RUN_TEST(test_reset_both);

    printf("\n[UT-07] irq_get_pending / irq_get_tick:\n");
    RUN_TEST(test_get_pending_initial);
    RUN_TEST(test_get_tick_initial);
    RUN_TEST(test_get_pending_after_trigger);

    printf("\n========== Unit Test Results ==========\n");
    printf("  Total:  %u\n", g_ut_passed + g_ut_failed);
    printf("  Passed: %u\n", g_ut_passed);
    printf("  Failed: %u\n", g_ut_failed);
    printf("========================================\n");

    return (g_ut_failed > 0) ? 1 : 0;
}

int main(void) {
    return run_all_unit_tests();
}