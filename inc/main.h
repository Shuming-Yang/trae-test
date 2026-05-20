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
 * @brief IRQ handler: process a specific IRQ and clear its pending bit
 * @remark Test Criteria: IRQ handled and pending bit cleared.
 * @param [in] irq_num type=[unsigned int] R=[0-31] P=[0-31] N=[N/A] D=[IRQ number to handle]
 * @retval type=[void] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 */
void irq_handler(uint32_t irq_num);

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

extern uint32_t irq_pending;
extern uint32_t g_tick_count;
extern uint32_t exception_count;

FW_STATIC void tick_printf(const char *fmt, ...);
FW_STATIC uint32_t exception_get_count(void);
FW_STATIC void irq_trigger_raw(uint32_t mask);
FW_STATIC uint32_t irq_get_pending(void);
FW_STATIC uint32_t irq_get_tick(void);
FW_STATIC void irq_reset_all(void);

#endif /* TEST_BUILD */

#endif /* MAIN_H */