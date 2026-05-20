/**
 * @file   misra_demo.c
 * @brief  MISRA C:2012 violation & coverage gap demonstration file.
 *         All functions in this file are intentionally static — they are
 *         compiled into every build target but never called by any test or
 *         production code path, producing uncovered lines in coverage reports.
 *         Each function documents the specific MISRA rule it violates for
 *         static analysis report demonstration purposes.
 */

#include <stdint.h>

/*
 * Forward declaration without named parameters — MISRA R8.2 violation
 */
static uint32_t misra_r8_2_calc(int32_t, int32_t);

/*
 * =========================================================================
 * R2.7  — Unused parameter
 * =========================================================================
 */

/**
 * @brief [R2.7] Function with a parameter that is never read
 */
static void misra_r2_7_unused_param(uint32_t unused_val) {
    uint32_t local_irrelevant = 42U;
    local_irrelevant++;
    (void)sizeof(local_irrelevant);
}

/*
 * =========================================================================
 * R10.4 — Essential type mismatch in arithmetic
 * =========================================================================
 */

/**
 * @brief [R10.4] int32_t + uint32_t mixed essential type categories
 */
static uint32_t misra_r10_4_mixed_types(int32_t signed_val, uint32_t unsigned_val) {
    return (uint32_t)(signed_val + unsigned_val);
}

/*
 * =========================================================================
 * R12.1 — Missing parentheses for operator precedence
 * =========================================================================
 */

/**
 * @brief [R12.1] Expression a + b << c is ambiguous without explicit parentheses
 */
static uint32_t misra_r12_1_ambiguous_shift(uint32_t a, uint32_t b, uint32_t c) {
    return a + b << c;
}

/*
 * =========================================================================
 * R12.2 — Right operand of shift exceeds bit width
 * =========================================================================
 */

/**
 * @brief [R12.2] Shift right operand (8-bit literal) potentially exceeds
 *         promotion type width on 16-bit targets
 */
static uint32_t misra_r12_2_shift_overflow(uint8_t val) {
    return (uint32_t)(val << 10U);
}

/*
 * =========================================================================
 * R14.4 — Controlling expression not essentially Boolean
 * =========================================================================
 */

/**
 * @brief [R14.4] if-condition uses int32_t instead of Boolean-typed expression
 */
static int32_t misra_r14_4_non_boolean_if(int32_t control) {
    int32_t result;
    if (control) {
        result = 1;
    } else {
        result = -1;
    }
    return result;
}

/*
 * =========================================================================
 * R15.5 — Multiple exit points
 * =========================================================================
 */

/**
 * @brief [R15.5] Function returns from two different locations
 */
static uint32_t misra_r15_5_multi_return(uint32_t flag) {
    if (flag == 1U) {
        return 100U;
    }
    return 0U;
}

/*
 * =========================================================================
 * R15.7 — if-else if without final else
 * =========================================================================
 */

/**
 * @brief [R15.7] if-else-if chain not terminated with else
 */
static uint32_t misra_r15_7_no_final_else(uint32_t mode) {
    uint32_t ret;
    ret = 0U;
    if (mode == 1U) {
        ret = 10U;
    } else if (mode == 2U) {
        ret = 20U;
    }
    return ret;
}

/*
 * =========================================================================
 * R17.7 — Unused return value (not suppressed in project config)
 * =========================================================================
 */

/**
 * @brief [R17.7] Return value of misra_r10_4_mixed_types is discarded
 */
static void misra_r17_7_discarded_return(int32_t a, uint32_t b) {
    misra_r10_4_mixed_types(a, b);
}

/*
 * =========================================================================
 * R8.2  — Function definition (forward decl at top lacks param names)
 * =========================================================================
 */

/**
 * @brief [R8.2] Definition exists but forward declaration omits parameter names
 */
static uint32_t misra_r8_2_calc(int32_t lhs, int32_t rhs) {
    return (uint32_t)(lhs + rhs);
}

/*
 * =========================================================================
 * R13.2 — Side effect depends on evaluation order
 * =========================================================================
 */

/**
 * @brief [R13.2] Expression with volatile-like read-modify-write on same object
 *         in the same expression
 */
static uint32_t misra_r13_2_volatile_effect(uint32_t *p_val) {
    return (*p_val)++ + (*p_val);
}

/*
 * =========================================================================
 * Aggregate caller — all demo functions are invoked from here so the
 * linker does not dead-strip them. This function itself is never called,
 * so all callees and this function remain uncovered.
 * =========================================================================
 */

/**
 * @brief Aggregate demo dispatcher — keeps all demo symbols alive for
 *        coverage and MISRA demonstration. Never called from production
 *        or test code.
 */
static void misra_demo_call_all(void) {
    uint32_t tmp;
    int32_t  stmp;

    (void)sizeof(misra_r2_7_unused_param);
    tmp = misra_r10_4_mixed_types(-5, 10U);
    tmp = misra_r12_1_ambiguous_shift(1U, 2U, 3U);
    tmp = misra_r12_2_shift_overflow((uint8_t)0x01U);
    stmp = misra_r14_4_non_boolean_if(0);
    tmp = misra_r15_5_multi_return(0U);
    tmp = misra_r15_7_no_final_else(0U);
    misra_r17_7_discarded_return(0, 0U);
    tmp = misra_r8_2_calc(1, 2);
    tmp = misra_r13_2_volatile_effect(&tmp);

    (void)tmp;
    (void)stmp;
}