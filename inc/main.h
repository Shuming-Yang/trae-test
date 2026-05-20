/**
 * @file   main.h
 * @author Steven.Yang
 * @brief: Main header file for test-trae project
 * @par:   Initialized revision
 * @copyright Copyright (c):
 *    2026 OmniVision Co.,Ltd
 */

#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>

/*
 * FW_STATIC: firmware-level static modifier.
 *   Production build: expands to 'static' (MISRA R8.7 compliant).
 *   Test build (TEST_BUILD defined): expands to nothing, making symbols
 *   externally visible for unit/integration test harnesses.
 */
#ifdef TEST_BUILD
#define FW_STATIC
#else
#define FW_STATIC static
#endif

#define IRQ_COUNT   32U

/* Global function pointers for exception handlers (referenced in start.s) */
extern void (*rom_except_tick)(void);
extern void (*rom_except_int)(void);

/*
 * ---------------------------------------------------------------------------
 * Public API — always visible to all translation units
 * ---------------------------------------------------------------------------
 */

/**
 * @brief Disable global interrupts (platform stub — replace with arch-specific intrinsic)
 * @remark Placeholder for real hardware; no-op in simulator builds.
 * @retval type=[void] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 */
void __disable_irq(void);

/**
 * @brief Enable global interrupts (platform stub — replace with arch-specific intrinsic)
 * @remark Placeholder for real hardware; no-op in simulator builds.
 * @retval type=[void] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 */
void __enable_irq(void);

/**
 * @brief Tick interrupt handler function
 * @remark Test Criteria: Function executed on tick interrupt.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 */
void tick_irq_handler(void);

/**
 * @brief Exception interrupt handler function
 * @remark Test Criteria: Function executed on external interrupt.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 */
void exception_irq_handler(void);

/**
 * @brief Trigger a specific IRQ by setting the corresponding bit in irq_pending
 * @remark Test Criteria: IRQ bit set correctly in pending register.
 * @param [in] irq_num type=[unsigned int] R=[0-31] P=[0-31] N=[N/A] D=[IRQ number to trigger]
 * @retval type=[void] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 */
void irq_trigger(uint32_t irq_num);

/**
 * @brief Process all pending IRQs from lowest to highest priority (IRQ0 = highest)
 * @remark Test Criteria: All pending IRQs processed in priority order.
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[void] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 */
void irq_process_all(void);

/*
 * ---------------------------------------------------------------------------
 * Test-only declarations — visible only when TEST_BUILD is defined.
 * In production builds these symbols have internal linkage (FW_STATIC → static)
 * and must not appear in the public header.
 * ---------------------------------------------------------------------------
 */
#ifdef TEST_BUILD

/**
 * @brief IRQ pending register: each bit represents a triggered-but-unhandled IRQ
 */
extern uint32_t irq_pending;

/**
 * @brief System tick counter, incremented on each tick_irq_handler() call
 */
extern uint32_t g_tick_count;

/**
 * @brief External exception interrupt counter
 */
extern uint32_t exception_count;

/**
 * @brief Debug logging wrapper: prefix output with tick timestamp, then delegate to printf / vprintf
 * @remark R17.1 deviation: va_list / va_start / va_end are required for variadic debug output.
 *         This function serves CLI interactive development only and is excluded from production firmware.
 * @param [in] fmt type=[const char*] R=[N/A] P=[N/A] N=[N/A] D=[printf-compatible format string]
 * @param [in] ... type=[variable] R=[N/A] P=[N/A] N=[N/A] D=[variable arguments matching fmt]
 * @retval type=[void] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 */
FW_STATIC void tick_printf(const char *fmt, ...);

/**
 * @brief Get current exception count value (for test access)
 * @retval type=[uint32_t] current exception_count value
 */
FW_STATIC uint32_t exception_get_count(void);

/**
 * @brief Trigger multiple IRQs by raw mask (for h-mode / test use)
 * @param [in] mask type=[uint32_t] R=[0-0xFFFFFFFF] P=[N/A] N=[N/A] D=[bitmask of IRQs to trigger]
 * @retval type=[void] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 */
FW_STATIC void irq_trigger_raw(uint32_t mask);

/**
 * @brief Get current IRQ pending register value (for test access)
 * @retval type=[uint32_t] current irq_pending value
 */
FW_STATIC uint32_t irq_get_pending(void);

/**
 * @brief Get current tick count value (for test access)
 * @retval type=[uint32_t] current g_tick_count value
 */
FW_STATIC uint32_t irq_get_tick(void);

/**
 * @brief Reset all IRQ state to initial values (for test setup)
 */
FW_STATIC void irq_reset_all(void);

/**
 * @brief IRQ handler: process a specific IRQ and clear its pending bit
 * @remark Test Criteria: IRQ handled and pending bit cleared.
 * @param [in] irq_num type=[unsigned int] R=[0-31] P=[0-31] N=[N/A] D=[IRQ number to handle]
 * @retval type=[void] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 */
FW_STATIC void irq_handler(uint32_t irq_num);
#endif /* TEST_BUILD */

#endif /* MAIN_H */