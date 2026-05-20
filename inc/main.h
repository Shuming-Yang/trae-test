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

#define IRQ_COUNT   32U

/* Global function pointers for exception handlers (referenced in start.s) */
extern void (*rom_except_tick)(void);
extern void (*rom_except_int)(void);

/* Function declarations */

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
 * @brief Get current exception count value (for test access)
 * @retval type=[unsigned int] current exception_count value
 */
uint32_t exception_get_count(void);

/**
 * @brief Trigger a specific IRQ by setting the corresponding bit in irq_pending
 * @remark Test Criteria: IRQ bit set correctly in pending register.
 * @param [in] irq_num type=[unsigned int] R=[0-31] P=[0-31] N=[N/A] D=[IRQ number to trigger]
 * @retval type=[void] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 */
void irq_trigger(uint32_t irq_num);

/**
 * @brief Trigger multiple IRQs by raw mask (for h-mode / test use)
 * @param [in] mask type=[uint32_t] R=[0-0xFFFFFFFF] P=[N/A] N=[N/A] D=[bitmask of IRQs to trigger]
 * @retval type=[void] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 */
void irq_trigger_raw(uint32_t mask);

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

/**
 * @brief Get current IRQ pending register value (for test access)
 * @retval type=[uint32_t] current irq_pending value
 */
uint32_t irq_get_pending(void);

/**
 * @brief Get current tick count value (for test access)
 * @retval type=[unsigned int] current g_tick_count value
 */
uint32_t irq_get_tick(void);

/**
 * @brief Reset all IRQ state to initial values (for test setup)
 * @param [in] N/A type=[N/A] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 * @retval type=[void] R=[N/A] P=[N/A] N=[N/A] D=[N/A]
 */
void irq_reset_all(void);

#endif /* MAIN_H */