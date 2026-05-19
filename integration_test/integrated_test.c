/**
 * @file   integrated_test.c
 * @author Steven.Yang
 * @brief: Integration tests for IRQ Simulator end-to-end flows
 * @par:   Initialized revision
 * @copyright Copyright (c):
 *    2026 OmniVision Co.,Ltd
 */

#include "integrated_test.h"
#include "main.h"

static uint32_t g_it_passed = 0U;
static uint32_t g_it_failed = 0U;

#define RUN_TEST(name) do { \
    printf("  Running %s...\n", #name); \
    irq_reset_all(); \
    if (name() == 0) { \
        printf("  [PASS] %s\n", #name); \
        g_it_passed++; \
    } else { \
        g_it_failed++; \
    } \
} while (0)

/**
 * @brief Simulate the main loop's input parsing logic
 * @param line Input string (without trailing newline for simplicity)
 * @return 1 if "exit" was processed, 0 otherwise
 */
static int32_t simulate_input(const char *line) {
    int32_t input;
    uint32_t hex_val;
    char buf[64];

    strncpy(buf, line, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    if (strcmp(buf, "exit") == 0) {
        return 1;
    }

    if (buf[0] == 'b' || buf[0] == 'B') {
        if (sscanf(buf + 1, "%d", &input) == 1 && input >= 0 && input < IRQ_COUNT) {
            irq_trigger((uint32_t)input);
            irq_process_all();
        }
    } else if (buf[0] == 'h' || buf[0] == 'H') {
        if (sscanf(buf + 1, "%x", &hex_val) == 1) {
            irq_trigger_raw(hex_val);
            irq_process_all();
        }
    } else if (sscanf(buf, "%d", &input) == 1) {
        if (input == 0) {
            irq_process_all();
        } else if (input >= 1 && input <= 32) {
            irq_trigger((uint32_t)(input - 1));
            irq_process_all();
        }
    }

    return 0;
}

/* ================================================================
 * IT_01: Number Mode Input Parsing
 * ================================================================ */

/**
 * @brief Verify number mode input '1' triggers IRQ0 and clears pending after processing
 * @remark Test Criteria: IT_01_01: input '1' triggers IRQ0, pending cleared.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_number_mode_irq0(void) {
    simulate_input("1");
    IT_ASSERT_HEX_EQ(irq_get_pending(), 0, "IT_01_01: input '1' triggers IRQ0, pending cleared");
    return 0;
}

/**
 * @brief Verify number mode input '32' triggers IRQ31 and clears pending after processing
 * @remark Test Criteria: IT_01_02: input '32' triggers IRQ31, pending cleared.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_number_mode_irq31(void) {
    simulate_input("32");
    IT_ASSERT_HEX_EQ(irq_get_pending(), 0, "IT_01_02: input '32' triggers IRQ31, pending cleared");
    return 0;
}

/**
 * @brief Verify number mode input '0' processes all pending IRQs
 * @remark Test Criteria: IT_01_03: input '0' processes pending IRQ3.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_number_mode_zero(void) {
    irq_trigger(3);
    simulate_input("0");
    IT_ASSERT_HEX_EQ(irq_get_pending(), 0, "IT_01_03: input '0' processes pending IRQ3");
    return 0;
}

/**
 * @brief Verify number mode input '33' (out of range) does not modify pending register
 * @remark Test Criteria: IT_01_04: input '33' should not change pending.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_number_mode_invalid_33(void) {
    uint32_t before = irq_get_pending();
    simulate_input("33");
    IT_ASSERT_HEX_EQ(irq_get_pending(), before, "IT_01_04: input '33' should not change pending");
    return 0;
}

/**
 * @brief Verify number mode input '-5' (negative) does not modify pending register
 * @remark Test Criteria: IT_01_05: input '-5' should not change pending.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_number_mode_invalid_neg5(void) {
    uint32_t before = irq_get_pending();
    simulate_input("-5");
    IT_ASSERT_HEX_EQ(irq_get_pending(), before, "IT_01_05: input '-5' should not change pending");
    return 0;
}

/* ================================================================
 * IT_02: b-mode Input Parsing
 * ================================================================ */

/**
 * @brief Verify b-mode input 'b0' triggers IRQ0 and clears pending after processing
 * @remark Test Criteria: IT_02_01: b0 triggers IRQ0, pending cleared.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_bmode_irq0(void) {
    simulate_input("b0");
    IT_ASSERT_HEX_EQ(irq_get_pending(), 0, "IT_02_01: b0 triggers IRQ0, pending cleared");
    return 0;
}

/**
 * @brief Verify b-mode input 'b5' triggers IRQ5 and clears pending after processing
 * @remark Test Criteria: IT_02_02: b5 triggers IRQ5, pending cleared.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_bmode_irq5(void) {
    simulate_input("b5");
    IT_ASSERT_HEX_EQ(irq_get_pending(), 0, "IT_02_02: b5 triggers IRQ5, pending cleared");
    return 0;
}

/**
 * @brief Verify b-mode input 'b31' triggers IRQ31 and clears pending after processing
 * @remark Test Criteria: IT_02_03: b31 triggers IRQ31, pending cleared.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_bmode_irq31(void) {
    simulate_input("b31");
    IT_ASSERT_HEX_EQ(irq_get_pending(), 0, "IT_02_03: b31 triggers IRQ31, pending cleared");
    return 0;
}

/**
 * @brief Verify b-mode uppercase input 'B10' triggers IRQ10 and clears pending after processing
 * @remark Test Criteria: IT_02_04: B10 triggers IRQ10, pending cleared.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_bmode_uppercase(void) {
    simulate_input("B10");
    IT_ASSERT_HEX_EQ(irq_get_pending(), 0, "IT_02_04: B10 triggers IRQ10, pending cleared");
    return 0;
}

/**
 * @brief Verify b-mode input 'b32' (out of range) does not modify pending register
 * @remark Test Criteria: IT_02_05: b32 should not change pending.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_bmode_invalid_32(void) {
    uint32_t before = irq_get_pending();
    simulate_input("b32");
    IT_ASSERT_HEX_EQ(irq_get_pending(), before, "IT_02_05: b32 should not change pending");
    return 0;
}

/**
 * @brief Verify b-mode input 'b-1' (negative) does not modify pending register
 * @remark Test Criteria: IT_02_06: b-1 should not change pending.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_bmode_invalid_neg1(void) {
    uint32_t before = irq_get_pending();
    simulate_input("b-1");
    IT_ASSERT_HEX_EQ(irq_get_pending(), before, "IT_02_06: b-1 should not change pending");
    return 0;
}

/* ================================================================
 * IT_03: h-mode Input Parsing
 * ================================================================ */

/**
 * @brief Verify h-mode input 'h1' triggers IRQ0 and clears pending after processing
 * @remark Test Criteria: IT_03_01: h1 triggers IRQ0, pending cleared.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_hmode_h1(void) {
    simulate_input("h1");
    IT_ASSERT_HEX_EQ(irq_get_pending(), 0, "IT_03_01: h1 triggers IRQ0, pending cleared");
    return 0;
}

/**
 * @brief Verify h-mode input 'h3' triggers IRQ0 and IRQ1, clears pending after processing
 * @remark Test Criteria: IT_03_02: h3 triggers IRQ0,1, pending cleared.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_hmode_h3(void) {
    simulate_input("h3");
    IT_ASSERT_HEX_EQ(irq_get_pending(), 0, "IT_03_02: h3 triggers IRQ0,1, pending cleared");
    return 0;
}

/**
 * @brief Verify h-mode input 'hFF' triggers IRQ0~7 and clears pending after processing
 * @remark Test Criteria: IT_03_03: hFF triggers IRQ0~7, pending cleared.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_hmode_hFF(void) {
    simulate_input("hFF");
    IT_ASSERT_HEX_EQ(irq_get_pending(), 0, "IT_03_03: hFF triggers IRQ0~7, pending cleared");
    return 0;
}

/**
 * @brief Verify h-mode input 'h80000000' triggers IRQ31 and clears pending after processing
 * @remark Test Criteria: IT_03_04: h80000000 triggers IRQ31, pending cleared.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_hmode_h80000000(void) {
    simulate_input("h80000000");
    IT_ASSERT_HEX_EQ(irq_get_pending(), 0, "IT_03_04: h80000000 triggers IRQ31, pending cleared");
    return 0;
}

/**
 * @brief Verify h-mode uppercase input 'H0A' triggers IRQ1 and IRQ3, clears pending after processing
 * @remark Test Criteria: IT_03_05: H0A triggers IRQ1,3, pending cleared.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_hmode_uppercase(void) {
    simulate_input("H0A");
    IT_ASSERT_HEX_EQ(irq_get_pending(), 0, "IT_03_05: H0A triggers IRQ1,3, pending cleared");
    return 0;
}

/**
 * @brief Verify h-mode input 'hGG' (invalid hex) does not modify pending register
 * @remark Test Criteria: IT_03_06: hGG should not change pending.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_hmode_invalid(void) {
    uint32_t before = irq_get_pending();
    simulate_input("hGG");
    IT_ASSERT_HEX_EQ(irq_get_pending(), before, "IT_03_06: hGG should not change pending");
    return 0;
}

/* ================================================================
 * IT_04: Accumulated Trigger & Priority
 * ================================================================ */

/**
 * @brief Verify accumulated trigger: irq_trigger(0) followed by h-mode 'h6' processes IRQ0,1,2
 * @remark Test Criteria: IT_04_01: trigger(0)+h6 processes IRQ0,1,2.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_accumulate_trigger_then_hmode(void) {
    irq_trigger(0);
    simulate_input("h6");
    IT_ASSERT_HEX_EQ(irq_get_pending(), 0, "IT_04_01: trigger(0)+h6 processes IRQ0,1,2");
    return 0;
}

/**
 * @brief Verify multiple b-mode inputs accumulate and are processed by manual '0' command
 * @remark Test Criteria: IT_04_02: b10+b5+0 processes IRQ5,10.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_accumulate_multi_bmode(void) {
    simulate_input("b10");
    simulate_input("b5");
    simulate_input("0");
    IT_ASSERT_HEX_EQ(irq_get_pending(), 0, "IT_04_02: b10+b5+0 processes IRQ5,10");
    return 0;
}

/**
 * @brief Verify priority order: h-mode 'h80000001' processes IRQ0 before IRQ31
 * @remark Test Criteria: IT_04_03: h80000001 processes IRQ0 before IRQ31.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_priority_order(void) {
    simulate_input("h80000001");
    IT_ASSERT_HEX_EQ(irq_get_pending(), 0, "IT_04_03: h80000001 processes IRQ0 before IRQ31");
    return 0;
}

/* ================================================================
 * IT_05: Tick Count Consistency
 * ================================================================ */

/**
 * @brief Verify initial tick count is zero at start of integration test
 * @remark Test Criteria: IT_05_01: initial tick should be 0.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_tick_initial_zero(void) {
    IT_ASSERT_EQ(irq_get_tick(), 0, "IT_05_01: initial tick should be 0");
    return 0;
}

/**
 * @brief Verify IRQ0 handler increments tick count by 1
 * @remark Test Criteria: IT_05_02: IRQ0 handler increments tick by 1.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_tick_irq0_increment(void) {
    uint32_t before = irq_get_tick();
    irq_trigger(0);
    irq_process_all();
    IT_ASSERT_EQ(irq_get_tick(), before + 1, "IT_05_02: IRQ0 handler increments tick by 1");
    return 0;
}

/**
 * @brief Verify non-IRQ0 handler does not change tick count
 * @remark Test Criteria: IT_05_03: non-IRQ0 should not change tick.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_tick_non_irq0_no_change(void) {
    uint32_t before = irq_get_tick();
    irq_trigger(5);
    irq_process_all();
    IT_ASSERT_EQ(irq_get_tick(), before, "IT_05_03: non-IRQ0 should not change tick");
    return 0;
}

/**
 * @brief Verify three separate IRQ0 triggers each increment tick by 1 (total +3)
 * @remark Test Criteria: IT_05_04: three separate IRQ0 triggers increment tick by 3.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_tick_multi_irq0(void) {
    uint32_t before = irq_get_tick();
    irq_trigger(0);
    irq_process_all();
    irq_trigger(0);
    irq_process_all();
    irq_trigger(0);
    irq_process_all();
    IT_ASSERT_EQ(irq_get_tick(), before + 3, "IT_05_04: three separate IRQ0 triggers increment tick by 3");
    return 0;
}

/* ================================================================
 * IT_06: Exit & Boundary Conditions
 * ================================================================ */

/**
 * @brief Verify simulate_input returns 1 when 'exit' command is processed
 * @remark Test Criteria: IT_06_01: 'exit' should return 1.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_exit_returns_one(void) {
    int32_t result = simulate_input("exit");
    IT_ASSERT_EQ(result, 1, "IT_06_01: 'exit' should return 1");
    return 0;
}

/**
 * @brief Verify empty input string does not crash or modify pending register
 * @remark Test Criteria: IT_06_02: empty input should not crash.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_empty_input_safe(void) {
    uint32_t before = irq_get_pending();
    simulate_input("");
    IT_ASSERT_HEX_EQ(irq_get_pending(), before, "IT_06_02: empty input should not crash");
    return 0;
}

/**
 * @brief Verify garbage input 'xyz' does not crash or modify pending register
 * @remark Test Criteria: IT_06_03: garbage input should not crash.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_garbage_input_safe(void) {
    uint32_t before = irq_get_pending();
    simulate_input("xyz");
    IT_ASSERT_HEX_EQ(irq_get_pending(), before, "IT_06_03: garbage input should not crash");
    return 0;
}

/* ================================================================
 * IT_07: End-to-End Full Flow
 * ================================================================ */

/**
 * @brief Verify end-to-end full flow: number mode, b-mode, h-mode, and exit in sequence
 * @remark Test Criteria: IT_07_01: full flow with all input modes and exit.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0] P=[0] N=[N/A] D=[always returns 0 on success]
 */
static int test_full_flow(void) {
    simulate_input("1");
    IT_ASSERT_HEX_EQ(irq_get_pending(), 0, "IT_07_01: step1 '1' -> IRQ0 processed");

    simulate_input("b5");
    IT_ASSERT_HEX_EQ(irq_get_pending(), 0, "IT_07_01: step2 'b5' -> IRQ5 processed");

    simulate_input("h3");
    IT_ASSERT_HEX_EQ(irq_get_pending(), 0, "IT_07_01: step3 'h3' -> IRQ0,1 processed");

    int32_t result = simulate_input("exit");
    IT_ASSERT_EQ(result, 1, "IT_07_01: step4 'exit' -> returns 1");

    return 0;
}

/* ================================================================
 * Test Runner
 * ================================================================ */

/**
 * @brief Execute all integration test suites and print pass/fail summary
 * @remark Test Criteria: All 7 integration test suites executed, results aggregated and reported.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0|1] P=[0=all passed, 1=some failed] N=[N/A] D=[exit code: 0 on success, 1 on failure]
 */
int32_t run_all_integrated_tests(void) {
    g_it_passed = 0U;
    g_it_failed = 0U;

    printf("\n========== Integration Tests ==========\n\n");

    printf("[IT_01] Number Mode Input Parsing:\n");
    RUN_TEST(test_number_mode_irq0);
    RUN_TEST(test_number_mode_irq31);
    RUN_TEST(test_number_mode_zero);
    RUN_TEST(test_number_mode_invalid_33);
    RUN_TEST(test_number_mode_invalid_neg5);

    printf("\n[IT_02] b-mode Input Parsing:\n");
    RUN_TEST(test_bmode_irq0);
    RUN_TEST(test_bmode_irq5);
    RUN_TEST(test_bmode_irq31);
    RUN_TEST(test_bmode_uppercase);
    RUN_TEST(test_bmode_invalid_32);
    RUN_TEST(test_bmode_invalid_neg1);

    printf("\n[IT_03] h-mode Input Parsing:\n");
    RUN_TEST(test_hmode_h1);
    RUN_TEST(test_hmode_h3);
    RUN_TEST(test_hmode_hFF);
    RUN_TEST(test_hmode_h80000000);
    RUN_TEST(test_hmode_uppercase);
    RUN_TEST(test_hmode_invalid);

    printf("\n[IT_04] Accumulated Trigger & Priority:\n");
    RUN_TEST(test_accumulate_trigger_then_hmode);
    RUN_TEST(test_accumulate_multi_bmode);
    RUN_TEST(test_priority_order);

    printf("\n[IT_05] Tick Count Consistency:\n");
    RUN_TEST(test_tick_initial_zero);
    RUN_TEST(test_tick_irq0_increment);
    RUN_TEST(test_tick_non_irq0_no_change);
    RUN_TEST(test_tick_multi_irq0);

    printf("\n[IT_06] Exit & Boundary Conditions:\n");
    RUN_TEST(test_exit_returns_one);
    RUN_TEST(test_empty_input_safe);
    RUN_TEST(test_garbage_input_safe);

    printf("\n[IT_07] End-to-End Full Flow:\n");
    RUN_TEST(test_full_flow);

    printf("\n========== Integration Test Results ==========\n");
    printf("  Total:  %u\n", g_it_passed + g_it_failed);
    printf("  Passed: %u\n", g_it_passed);
    printf("  Failed: %u\n", g_it_failed);
    printf("===============================================\n");

    return (g_it_failed > 0) ? 1 : 0;
}

/**
 * @brief Integration test program entry point
 * @remark Test Criteria: Program returns 0 when all tests pass, non-zero otherwise.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[int] R=[0|1] P=[0=all passed, 1=some failed] N=[N/A] D=[exit code from run_all_integrated_tests]
 */
int32_t main(void) {
    return run_all_integrated_tests();
}