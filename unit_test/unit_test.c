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
 * UT_01: tick_irq_handler
 * ================================================================ */

/**
 * @brief Verify initial tick count is zero after reset
 * @remark Test Criteria: UT_01_01: initial tick should be 0.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_tick_initial(void) {
    UT_ASSERT_EQ(irq_get_tick(), 0, "UT_01_01: initial tick should be 0");
    return 0;
}

/**
 * @brief Verify tick increments by 1 after a single tick_irq_handler call
 * @remark Test Criteria: UT_01_02: tick should be 1 after one call.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_tick_single_call(void) {
    tick_irq_handler();
    UT_ASSERT_EQ(irq_get_tick(), 1, "UT_01_02: tick should be 1 after one call");
    return 0;
}

/**
 * @brief Verify tick increments correctly after multiple tick_irq_handler calls
 * @remark Test Criteria: UT_01_03: tick should be 5 after five calls.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_tick_multiple_calls(void) {
    tick_irq_handler();
    tick_irq_handler();
    tick_irq_handler();
    tick_irq_handler();
    tick_irq_handler();
    UT_ASSERT_EQ(irq_get_tick(), 5, "UT_01_03: tick should be 5 after five calls");
    return 0;
}

/**
 * @brief Verify tick count resets correctly and resumes counting after irq_reset_all
 * @remark Test Criteria: UT_01_04: tick should be 3 after reset + 3 calls.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_tick_after_reset(void) {
    tick_irq_handler();
    tick_irq_handler();
    tick_irq_handler();
    irq_reset_all();
    tick_irq_handler();
    tick_irq_handler();
    tick_irq_handler();
    UT_ASSERT_EQ(irq_get_tick(), 3, "UT_01_04: tick should be 3 after reset + 3 calls");
    return 0;
}

/* ================================================================
 * UT_02: exception_irq_handler
 * ================================================================ */

/**
 * @brief Verify exception_irq_handler executes without crashing
 * @remark Test Criteria: UT_02_01: exception_irq_handler should not crash.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_exception_no_crash(void) {
    exception_irq_handler();
    UT_ASSERT(1, "UT_02_01: exception_irq_handler should not crash");
    return 0;
}

/**
 * @brief Verify exception_irq_handler can be called multiple times without crashing
 * @remark Test Criteria: UT_02_02: exception_irq_handler x3 should not crash.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_exception_multiple_calls(void) {
    exception_irq_handler();
    exception_irq_handler();
    exception_irq_handler();
    UT_ASSERT(1, "UT_02_02: exception_irq_handler x3 should not crash");
    return 0;
}

/**
 * @brief Verify exception_count increments correctly with each exception_irq_handler call
 * @remark Test Criteria: UT_02_03: exception_count should increment per call.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_exception_count_increment(void) {
    UT_ASSERT_EQ(exception_get_count(), 0, "UT_02_03: initial exception_count should be 0");
    exception_irq_handler();
    UT_ASSERT_EQ(exception_get_count(), 1, "UT_02_03: exception_count should be 1 after one call");
    exception_irq_handler();
    exception_irq_handler();
    UT_ASSERT_EQ(exception_get_count(), 3, "UT_02_03: exception_count should be 3 after three calls");
    return 0;
}

/* ================================================================
 * UT_03: irq_trigger
 * ================================================================ */

/**
 * @brief Verify irq_trigger(0) sets bit 0 in pending register
 * @remark Test Criteria: UT_03_01: trigger IRQ0 sets pending to 0x00000001.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_trigger_irq0(void) {
    irq_trigger(0);
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0x00000001, "UT_03_01: trigger IRQ0");
    return 0;
}

/**
 * @brief Verify irq_trigger(5) sets bit 5 in pending register
 * @remark Test Criteria: UT_03_02: trigger IRQ5 sets pending to 0x00000020.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_trigger_irq5(void) {
    irq_trigger(5);
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0x00000020, "UT_03_02: trigger IRQ5");
    return 0;
}

/**
 * @brief Verify irq_trigger(31) sets bit 31 (MSB) in pending register
 * @remark Test Criteria: UT_03_03: trigger IRQ31 sets pending to 0x80000000.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_trigger_irq31(void) {
    irq_trigger(31);
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0x80000000, "UT_03_03: trigger IRQ31");
    return 0;
}

/**
 * @brief Verify multiple irq_trigger calls accumulate bits in pending register
 * @remark Test Criteria: UT_03_04: accumulate IRQ0+IRQ1 sets pending to 0x00000003.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_trigger_accumulate(void) {
    irq_trigger(0);
    irq_trigger(1);
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0x00000003, "UT_03_04: accumulate IRQ0+IRQ1");
    return 0;
}

/**
 * @brief Verify duplicate irq_trigger for same IRQ does not toggle the bit
 * @remark Test Criteria: UT_03_05: duplicate trigger IRQ0 keeps pending at 0x00000001.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_trigger_duplicate(void) {
    irq_trigger(0);
    irq_trigger(0);
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0x00000001, "UT_03_05: duplicate trigger IRQ0");
    return 0;
}

/**
 * @brief Verify irq_trigger with invalid IRQ number 32 does not modify pending register
 * @remark Test Criteria: UT_03_06: invalid IRQ32 should not change pending.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_trigger_invalid_32(void) {
    uint32_t before = irq_get_pending();
    irq_trigger(32);
    UT_ASSERT_HEX_EQ(irq_get_pending(), before, "UT_03_06: invalid IRQ32 should not change pending");
    return 0;
}

/**
 * @brief Verify irq_trigger with invalid IRQ number 99 does not modify pending register
 * @remark Test Criteria: UT_03_07: invalid IRQ99 should not change pending.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_trigger_invalid_99(void) {
    uint32_t before = irq_get_pending();
    irq_trigger(99);
    UT_ASSERT_HEX_EQ(irq_get_pending(), before, "UT_03_07: invalid IRQ99 should not change pending");
    return 0;
}

/* ================================================================
 * UT_04: irq_handler
 * ================================================================ */

/**
 * @brief Verify irq_handler(0) clears pending bit 0 and increments tick
 * @remark Test Criteria: UT_04_01: handler IRQ0 clears bit 0 and increments tick.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_handler_irq0(void) {
    irq_trigger(0);
    irq_handler(0);
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0x00000000, "UT_04_01: handler IRQ0 clears bit 0");
    UT_ASSERT_EQ(irq_get_tick(), 1, "UT_04_01: handler IRQ0 increments tick");
    return 0;
}

/**
 * @brief Verify irq_handler(5) clears pending bit 5
 * @remark Test Criteria: UT_04_02: handler IRQ5 clears bit 5.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_handler_irq5(void) {
    irq_trigger(5);
    irq_handler(5);
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0x00000000, "UT_04_02: handler IRQ5 clears bit 5");
    return 0;
}

/**
 * @brief Verify irq_handler(31) clears pending bit 31 (MSB)
 * @remark Test Criteria: UT_04_03: handler IRQ31 clears bit 31.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_handler_irq31(void) {
    irq_trigger(31);
    irq_handler(31);
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0x00000000, "UT_04_03: handler IRQ31 clears bit 31");
    return 0;
}

/**
 * @brief Verify pending register is cleared to 0 after irq_handler processes the IRQ
 * @remark Test Criteria: UT_04_04: pending should be 0 after handler.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_handler_clears_pending(void) {
    irq_trigger(0);
    irq_handler(0);
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0, "UT_04_04: pending should be 0 after handler");
    return 0;
}

/**
 * @brief Verify irq_handler with invalid IRQ number does not crash (default case)
 * @remark Test Criteria: UT_04_05: irq_handler(99) should not crash.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_handler_invalid_irq(void) {
    uint32_t before = irq_get_pending();
    irq_handler(99);
    UT_ASSERT(1, "UT_04_05: irq_handler(99) should not crash (default case)");
    return 0;
}

/* ================================================================
 * UT_05: irq_process_all
 * ================================================================ */

/**
 * @brief Verify irq_process_all is safe to call when no IRQs are pending
 * @remark Test Criteria: UT_05_01: process_all with no pending should be safe.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_process_all_empty(void) {
    irq_process_all();
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0, "UT_05_01: process_all with no pending should be safe");
    return 0;
}

/**
 * @brief Verify irq_process_all clears a single pending IRQ3
 * @remark Test Criteria: UT_05_02: process_all should clear single IRQ3.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_process_all_single(void) {
    irq_trigger(3);
    irq_process_all();
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0, "UT_05_02: process_all should clear single IRQ3");
    return 0;
}

/**
 * @brief Verify irq_process_all clears multiple pending IRQs (0, 5, 10)
 * @remark Test Criteria: UT_05_03: process_all should clear IRQ0,5,10.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_process_all_multiple(void) {
    irq_trigger(0);
    irq_trigger(5);
    irq_trigger(10);
    irq_process_all();
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0, "UT_05_03: process_all should clear IRQ0,5,10");
    return 0;
}

/**
 * @brief Verify irq_process_all clears all 32 IRQs when all are triggered
 * @remark Test Criteria: UT_05_04: process_all should clear all 32 IRQs.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_process_all_full(void) {
    unsigned int i;
    for (i = 0; i < IRQ_COUNT; i++) {
        irq_trigger(i);
    }
    irq_process_all();
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0, "UT_05_04: process_all should clear all 32 IRQs");
    return 0;
}

/* ================================================================
 * UT_06: irq_reset_all
 * ================================================================ */

/**
 * @brief Verify irq_reset_all clears the pending register
 * @remark Test Criteria: UT_06_01: reset clears pending.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_reset_pending(void) {
    irq_trigger(5);
    irq_reset_all();
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0, "UT_06_01: reset clears pending");
    return 0;
}

/**
 * @brief Verify irq_reset_all clears the tick count to zero
 * @remark Test Criteria: UT_06_02: reset clears tick.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_reset_tick(void) {
    tick_irq_handler();
    tick_irq_handler();
    tick_irq_handler();
    irq_reset_all();
    UT_ASSERT_EQ(irq_get_tick(), 0, "UT_06_02: reset clears tick");
    return 0;
}

/**
 * @brief Verify irq_reset_all clears both pending register and tick count simultaneously
 * @remark Test Criteria: UT_06_03: reset clears both pending and tick.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_reset_both(void) {
    irq_trigger(7);
    tick_irq_handler();
    tick_irq_handler();
    irq_reset_all();
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0, "UT_06_03: reset clears both pending and tick");
    UT_ASSERT_EQ(irq_get_tick(), 0, "UT_06_03: reset clears tick too");
    return 0;
}

/* ================================================================
 * UT_07: irq_get_pending / irq_get_tick
 * ================================================================ */

/**
 * @brief Verify irq_get_pending returns 0 after reset
 * @remark Test Criteria: UT_07_01: initial pending should be 0.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_get_pending_initial(void) {
    irq_reset_all();
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0, "UT_07_01: initial pending should be 0");
    return 0;
}

/**
 * @brief Verify irq_get_tick returns 0 after reset
 * @remark Test Criteria: UT_07_02: initial tick should be 0.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_get_tick_initial(void) {
    irq_reset_all();
    UT_ASSERT_EQ(irq_get_tick(), 0, "UT_07_02: initial tick should be 0");
    return 0;
}

/**
 * @brief Verify irq_get_pending returns correct value after triggering IRQ7
 * @remark Test Criteria: UT_07_03: pending after trigger IRQ7 should be 0x00000080.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_get_pending_after_trigger(void) {
    irq_reset_all();
    irq_trigger(7);
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0x00000080, "UT_07_03: pending after trigger IRQ7");
    return 0;
}

/**
 * @brief Verify irq_get_tick returns correct non-zero value after multiple tick_irq_handler calls
 * @remark Test Criteria: UT_07_04: tick should be 3 after three tick_irq_handler calls.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_get_tick_nonzero(void) {
    irq_reset_all();
    tick_irq_handler();
    tick_irq_handler();
    tick_irq_handler();
    UT_ASSERT_EQ(irq_get_tick(), 3, "UT_07_04: tick should be 3 after three tick_irq_handler calls");
    return 0;
}

/* ================================================================
 * UT_08: irq_trigger_raw
 * ================================================================ */

/**
 * @brief Verify irq_trigger_raw with single bit mask (0x01) sets only bit 0
 * @remark Test Criteria: UT_08_01: trigger_raw single bit (0x01).
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_trigger_raw_single_bit(void) {
    irq_trigger_raw(0x00000001);
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0x00000001, "UT_08_01: trigger_raw single bit (0x01)");
    return 0;
}

/**
 * @brief Verify irq_trigger_raw with multi-bit mask (0x0F) sets bits 0-3
 * @remark Test Criteria: UT_08_02: trigger_raw multi bit (0x0F).
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_trigger_raw_multi_bit(void) {
    irq_trigger_raw(0x0000000F);
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0x0000000F, "UT_08_02: trigger_raw multi bit (0x0F)");
    return 0;
}

/**
 * @brief Verify irq_trigger_raw performs cumulative OR with previously triggered IRQs
 * @remark Test Criteria: UT_08_03: trigger_raw cumulative OR with trigger(0).
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_trigger_raw_cumulative_or(void) {
    irq_trigger(0);
    irq_trigger_raw(0x0006);
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0x00000007, "UT_08_03: trigger_raw cumulative OR with trigger(0)");
    return 0;
}

/**
 * @brief Verify irq_trigger_raw with zero mask does not modify pending register
 * @remark Test Criteria: UT_08_04: trigger_raw zero mask should not change pending.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_trigger_raw_zero_mask(void) {
    uint32_t before = irq_get_pending();
    irq_trigger_raw(0x00000000);
    UT_ASSERT_HEX_EQ(irq_get_pending(), before, "UT_08_04: trigger_raw zero mask should not change pending");
    return 0;
}

/**
 * @brief Verify irq_trigger_raw with full mask (0xFFFFFFFF) sets all 32 bits
 * @remark Test Criteria: UT_08_05: trigger_raw full mask (0xFFFFFFFF).
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_trigger_raw_full_mask(void) {
    irq_trigger_raw(0xFFFFFFFF);
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0xFFFFFFFF, "UT_08_05: trigger_raw full mask (0xFFFFFFFF)");
    return 0;
}

/**
 * @brief Verify irq_trigger_raw with MSB-only mask (0x80000000) sets only bit 31
 * @remark Test Criteria: UT_08_06: trigger_raw MSB only (IRQ31).
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_trigger_raw_msb_only(void) {
    irq_trigger_raw(0x80000000);
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0x80000000, "UT_08_06: trigger_raw MSB only (IRQ31)");
    return 0;
}

/* ================================================================
 * UT_09: irq_handler (Boundary Cases)
 * ================================================================ */

/**
 * @brief Verify irq_handler called without pending IRQ does not crash or modify state
 * @remark Test Criteria: UT_09_01: handler without pending should not crash or change pending.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_handler_no_pending(void) {
    uint32_t before = irq_get_pending();
    irq_handler(0);
    UT_ASSERT_HEX_EQ(irq_get_pending(), before, "UT_09_01: handler without pending should not crash or change pending");
    return 0;
}

/**
 * @brief Verify irq_handler(15) correctly clears bit 15 for middle-range IRQ
 * @remark Test Criteria: UT_09_02: handler IRQ15 clears bit 15.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_handler_middle_irq15(void) {
    irq_trigger(15);
    irq_handler(15);
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0x00000000, "UT_09_02: handler IRQ15 clears bit 15");
    return 0;
}

/**
 * @brief Verify irq_handler clears only the target IRQ bit, leaving other bits intact
 * @remark Test Criteria: UT_09_03: handler IRQ0 clears only bit 0, bit 1 remains set.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_handler_clears_only_target(void) {
    irq_trigger(0);
    irq_trigger(1);
    irq_handler(0);
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0x00000002, "UT_09_03: handler IRQ0 clears only bit 0, bit 1 remains set");
    return 0;
}

/* ================================================================
 * UT_10: irq_process_all (Boundary Cases)
 * ================================================================ */

/**
 * @brief Verify irq_process_all handles highest priority IRQ0 and increments tick
 * @remark Test Criteria: UT_10_01: process_all highest priority IRQ0 only.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_process_all_highest_only(void) {
    irq_trigger(0);
    irq_process_all();
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0, "UT_10_01: process_all highest priority IRQ0 only");
    UT_ASSERT_EQ(irq_get_tick(), 1, "UT_10_01: IRQ0 handler increments tick");
    return 0;
}

/**
 * @brief Verify irq_process_all handles lowest priority IRQ31 correctly
 * @remark Test Criteria: UT_10_02: process_all lowest priority IRQ31 only.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_process_all_lowest_only(void) {
    irq_trigger(31);
    irq_process_all();
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0, "UT_10_02: process_all lowest priority IRQ31 only");
    return 0;
}

/**
 * @brief Verify irq_process_all respects priority order: IRQ0 processed before IRQ31
 * @remark Test Criteria: UT_10_03: process_all priority order IRQ0 before IRQ31.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_process_all_priority_order(void) {
    irq_trigger(31);
    irq_trigger(0);
    irq_process_all();
    UT_ASSERT_HEX_EQ(irq_get_pending(), 0, "UT_10_03: process_all priority order IRQ0 before IRQ31");
    UT_ASSERT_EQ(irq_get_tick(), 1, "UT_10_03: IRQ0 handler increments tick");
    return 0;
}

/* ================================================================
 * Test Runner
 * ================================================================ */

/**
 * @brief Execute all unit test suites and print pass/fail summary
 * @remark Test Criteria: All 10 unit test suites executed, results aggregated and reported.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0|1] P=[0=all passed, 1=some failed] N=[N/A] D=[exit code: 0 on success, 1 on failure]
 */
int run_all_unit_tests(void) {
    g_ut_passed = 0;
    g_ut_failed = 0;

    printf("\n========== Unit Tests ==========\n\n");

    printf("[UT_01] tick_irq_handler:\n");
    RUN_TEST(test_tick_initial);
    RUN_TEST(test_tick_single_call);
    RUN_TEST(test_tick_multiple_calls);
    RUN_TEST(test_tick_after_reset);

    printf("\n[UT_02] exception_irq_handler:\n");
    RUN_TEST(test_exception_no_crash);
    RUN_TEST(test_exception_multiple_calls);
    RUN_TEST(test_exception_count_increment);

    printf("\n[UT_03] irq_trigger:\n");
    RUN_TEST(test_trigger_irq0);
    RUN_TEST(test_trigger_irq5);
    RUN_TEST(test_trigger_irq31);
    RUN_TEST(test_trigger_accumulate);
    RUN_TEST(test_trigger_duplicate);
    RUN_TEST(test_trigger_invalid_32);
    RUN_TEST(test_trigger_invalid_99);

    printf("\n[UT_04] irq_handler:\n");
    RUN_TEST(test_handler_irq0);
    RUN_TEST(test_handler_irq5);
    RUN_TEST(test_handler_irq31);
    RUN_TEST(test_handler_clears_pending);
    RUN_TEST(test_handler_invalid_irq);

    printf("\n[UT_05] irq_process_all:\n");
    RUN_TEST(test_process_all_empty);
    RUN_TEST(test_process_all_single);
    RUN_TEST(test_process_all_multiple);
    RUN_TEST(test_process_all_full);

    printf("\n[UT_06] irq_reset_all:\n");
    RUN_TEST(test_reset_pending);
    RUN_TEST(test_reset_tick);
    RUN_TEST(test_reset_both);

    printf("\n[UT_07] irq_get_pending / irq_get_tick:\n");
    RUN_TEST(test_get_pending_initial);
    RUN_TEST(test_get_tick_initial);
    RUN_TEST(test_get_pending_after_trigger);
    RUN_TEST(test_get_tick_nonzero);

    printf("\n[UT_08] irq_trigger_raw:\n");
    RUN_TEST(test_trigger_raw_single_bit);
    RUN_TEST(test_trigger_raw_multi_bit);
    RUN_TEST(test_trigger_raw_cumulative_or);
    RUN_TEST(test_trigger_raw_zero_mask);
    RUN_TEST(test_trigger_raw_full_mask);
    RUN_TEST(test_trigger_raw_msb_only);

    printf("\n[UT_09] irq_handler (Boundary Cases):\n");
    RUN_TEST(test_handler_no_pending);
    RUN_TEST(test_handler_middle_irq15);
    RUN_TEST(test_handler_clears_only_target);

    printf("\n[UT_10] irq_process_all (Boundary Cases):\n");
    RUN_TEST(test_process_all_highest_only);
    RUN_TEST(test_process_all_lowest_only);
    RUN_TEST(test_process_all_priority_order);

    printf("\n========== Unit Test Results ==========\n");
    printf("  Total:  %u\n", g_ut_passed + g_ut_failed);
    printf("  Passed: %u\n", g_ut_passed);
    printf("  Failed: %u\n", g_ut_failed);
    printf("========================================\n");

    return (g_ut_failed > 0) ? 1 : 0;
}

/**
 * @brief Unit test program entry point
 * @remark Test Criteria: Program returns 0 when all tests pass, non-zero otherwise.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0|1] P=[0=all passed, 1=some failed] N=[N/A] D=[exit code from run_all_unit_tests]
 */
int main(void) {
    return run_all_unit_tests();
}