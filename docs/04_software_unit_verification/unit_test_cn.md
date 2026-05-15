# IRQ Simulator - Unit Test Plan

## 1. Test Scope

单元测试针对 `main.c` 中的每个独立函数进行验证，确保各函数在隔离环境下行为正确。

## 2. Test Environment

- 编译器：GCC (MinGW)
- 语言标准：C11
- 测试框架：自定义 assert 宏（无外部依赖）
- 每个测试用例前调用 `irq_reset_all()` 重置状态

```mermaid
flowchart TD
    START(["unit_test main()"])
    RESET["irq_reset_all()"]
    RUN["执行测试函数"]
    ASSERT{"assert 检查"}
    PASS["PRINT: [PASS]"]
    FAIL["PRINT: [FAIL]"]
    NEXT{"还有测试?"}
    REPORT["输出统计: Total / Passed / Failed"]
    END_(["return 0"])

    START --> RESET
    RESET --> RUN
    RUN --> ASSERT
    ASSERT -->|"通过"| PASS
    ASSERT -->|"失败"| FAIL
    PASS --> NEXT
    FAIL --> NEXT
    NEXT -->|"Yes"| RESET
    NEXT -->|"No"| REPORT
    REPORT --> END_
```

## 3. Test Cases

### UT-01: tick_irq_handler

| ID | 测试项 | 输入 | 预期结果 |
|----|---------|------|---------|
| UT-01-01 | tick 初始值 | 无 | `irq_get_tick() == 0` |
| UT-01-02 | 单次调用 | `tick_irq_handler()` | `irq_get_tick() == 1` |
| UT-01-03 | 多次调用 | 调用 5 次 | `irq_get_tick() == 5` |
| UT-01-04 | 重置后调用 | reset → 调用 3 次 | `irq_get_tick() == 3` |

### UT-02: exception_irq_handler

| ID | 测试项 | 输入 | 预期结果 |
|----|---------|------|---------|
| UT-02-01 | 函数可被调用不崩溃 | `exception_irq_handler()` | 正常返回 |
| UT-02-02 | 多次调用 | 调用 3 次 | 正常返回，无副作用 |

### UT-03: irq_trigger

| ID | 测试项 | 输入 | 预期结果 |
|----|---------|------|---------|
| UT-03-01 | 触发 IRQ0 | `irq_trigger(0)` | `irq_get_pending() == 0x00000001` |
| UT-03-02 | 触发 IRQ5 | `irq_trigger(5)` | `irq_get_pending() == 0x00000020` |
| UT-03-03 | 触发 IRQ31 | `irq_trigger(31)` | `irq_get_pending() == 0x80000000` |
| UT-03-04 | 累积触发 | trigger(0), trigger(1) | `irq_get_pending() == 0x00000003` |
| UT-03-05 | 重复触发 | trigger(0), trigger(0) | `irq_get_pending() == 0x00000001` |
| UT-03-06 | 无效 IRQ (32) | `irq_trigger(32)` | pending 不变 |
| UT-03-07 | 无效 IRQ (99) | `irq_trigger(99)` | pending 不变 |

### UT-04: irq_handler

| ID | 测试项 | 输入 | 预期结果 |
|----|---------|------|---------|
| UT-04-01 | 处理 IRQ0 | trigger(0) → handler(0) | pending bit 0 清除，tick+1 |
| UT-04-02 | 处理 IRQ5 | trigger(5) → handler(5) | pending bit 5 清除 |
| UT-04-03 | 处理 IRQ31 | trigger(31) → handler(31) | pending bit 31 清除 |
| UT-04-04 | 处理后 pending 归零 | trigger(0) → handler(0) | `irq_get_pending() == 0` |

### UT-05: irq_process_all

| ID | 测试项 | 输入 | 预期结果 |
|----|---------|------|---------|
| UT-05-01 | 无 pending 时 | `irq_process_all()` | 直接返回，无动作 |
| UT-05-02 | 单一 IRQ | trigger(3) → process_all | IRQ3 被处理，pending=0 |
| UT-05-03 | 多重 IRQ | trigger(0), trigger(5), trigger(10) | 依次处理 0→5→10，pending=0 |
| UT-05-04 | 全 IRQ | trigger all 0-31 | 全部处理完毕，pending=0 |

### UT-06: irq_reset_all

| ID | 测试项 | 输入 | 预期结果 |
|----|---------|------|---------|
| UT-06-01 | 重置 pending | trigger(5) → reset | `irq_get_pending() == 0` |
| UT-06-02 | 重置 tick | tick++ x3 → reset | `irq_get_tick() == 0` |
| UT-06-03 | 重置两者 | trigger + tick → reset | pending=0, tick=0 |

### UT-07: irq_get_pending / irq_get_tick

| ID | 测试项 | 输入 | 预期结果 |
|----|---------|------|---------|
| UT-07-01 | 初始 pending | reset → get_pending | 返回 0 |
| UT-07-02 | 初始 tick | reset → get_tick | 返回 0 |
| UT-07-03 | 触发后 pending | trigger(7) → get_pending | 返回 0x00000080 |

### UT-08: irq_trigger_raw

| ID | 测试项 | 输入 | 预期结果 |
|----|---------|------|---------|
| UT-08-01 | 单 bit raw mask | `irq_trigger_raw(0x00000001)` | `irq_get_pending() == 0x00000001` |
| UT-08-02 | 多 bit raw mask | `irq_trigger_raw(0x0000000F)` | `irq_get_pending() == 0x0000000F` |
| UT-08-03 | 累积 OR 行为 | trigger(0), trigger_raw(0x0006) | `irq_get_pending() == 0x00000007` |
| UT-08-04 | 零掩码（无操作） | `irq_trigger_raw(0x00000000)` | pending 不变 |
| UT-08-05 | 全掩码（全部 32 bit） | `irq_trigger_raw(0xFFFFFFFF)` | `irq_get_pending() == 0xFFFFFFFF` |
| UT-08-06 | 边界：仅 MSB (IRQ31) | `irq_trigger_raw(0x80000000)` | `irq_get_pending() == 0x80000000` |

### UT-09: irq_handler（边界案例）

| ID | 测试项 | 输入 | 预期结果 |
|----|---------|------|---------|
| UT-09-01 | 无 pending bit 时调用 handler | `irq_handler(0)`（未触发） | 不崩溃，pending 不变 |
| UT-09-02 | 处理中间 IRQ (IRQ15) | trigger(15) → handler(15) | pending bit 15 清除 |
| UT-09-03 | handler 仅清除目标 bit | trigger(0), trigger(1) → handler(0) | bit 0 清除，bit 1 保持置位 |

### UT-10: irq_process_all（边界案例）

| ID | 测试项 | 输入 | 预期结果 |
|----|---------|------|---------|
| UT-10-01 | 仅最高优先级 (IRQ0) | trigger(0) → process_all | IRQ0 被处理，tick 递增 |
| UT-10-02 | 仅最低优先级 (IRQ31) | trigger(31) → process_all | IRQ31 被处理，pending=0 |
| UT-10-03 | 优先级顺序验证 | trigger(31), trigger(0) → process_all | IRQ0 先于 IRQ31 被处理 |

## 4. Expected Results

- 所有 UT-01 ~ UT-10 测试用例须全部通过
- 通过率：100%

## 5. 单元测试追溯表

| ID | 章节 | 追溯 SD | 描述 |
|----|------|---------|------|
| UT-01 | 3 (UT-01) | SD_009 | `tick_irq_handler`：初始值、单次调用、多次调用、重置后行为 |
| UT-02 | 3 (UT-02) | SD_006 | `exception_irq_handler`：可调用不崩溃、多次调用无副作用 |
| UT-03 | 3 (UT-03) | SD_004<br>SD_008<br>SD_010 | `irq_trigger`：边界 (IRQ0/IRQ5/IRQ31)、累积、重复、无效范围 (32, 99) |
| UT-04 | 3 (UT-04) | SD_006<br>SD_013 | `irq_handler`：分发 (IRQ0/IRQ5/IRQ31)、处理后 pending bit 清除 |
| UT-05 | 3 (UT-05) | SD_005 | `irq_process_all`：空 pending、单一 IRQ、多重 IRQ、全部 32 个 IRQ |
| UT-06 | 3 (UT-06) | SD_002<br>SD_009<br>SD_011 | `irq_reset_all`：重置 pending、重置 tick、同时重置两者 |
| UT-07 | 3 (UT-07) | SD_002<br>SD_011 | `irq_get_pending` / `irq_get_tick`：初始值、触发后回读 |
| UT-08 | 3 (UT-08) | SD_014<br>SD_010 | `irq_trigger_raw`：单/多/零/全掩码、累积 OR、MSB 边界 |
| UT-09 | 3 (UT-09) | SD_006<br>SD_013 | `irq_handler` 边界：无 pending 调用、中间 IRQ (IRQ15)、仅清除目标 bit |
| UT-10 | 3 (UT-10) | SD_005 | `irq_process_all` 边界：仅最高/最低优先级、优先级顺序验证 |

### SD 覆盖对照表

| SD 项 | 覆盖 UT | 状态 |
|---------|---------------|--------|
| SD_001 | UT-01 ~ UT-10（全部 9 个 API 函数已测） | ✅ 已覆盖 |
| SD_002 | UT-06, UT-07 | ✅ 已覆盖 |
| SD_003 | — | ⚠️ 日志输出（集成测试验证） |
| SD_004 | UT-03 | ✅ 已覆盖 |
| SD_005 | UT-05, UT-10 | ✅ 已覆盖 |
| SD_006 | UT-02, UT-04, UT-09 | ✅ 已覆盖 |
| SD_007 | — | ⚠️ 主循环解析（集成测试验证） |
| SD_008 | UT-03 | ✅ 已覆盖 |
| SD_009 | UT-01, UT-06 | ✅ 已覆盖 |
| SD_010 | UT-03, UT-08 | ✅ 已覆盖 |
| SD_011 | UT-06, UT-07 | ✅ 已覆盖 |
| SD_012 | — | ⚠️ 日志输出（集成测试验证） |
| SD_013 | UT-04, UT-09 | ✅ 已覆盖 |
| SD_014 | UT-08 | ✅ 已覆盖 |

> **缩写说明：**
>
> - **UT** = Unit Test（单元测试，为所有单元测试用例的统一编号）
> - **SD** = Software Detailed Design（软件详细设计，追溯至 SWE.3 详细设计项）