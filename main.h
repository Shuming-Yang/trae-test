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

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define IRQ_COUNT   32

/* Function declarations */
void tick_irq_handler(void);
void exception_irq_handler(void);

void irq_trigger(unsigned int irq_num);
void irq_handler(unsigned int irq_num);
void irq_process_all(void);

#endif /* MAIN_H */