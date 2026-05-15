# IRQ Simulator - Integration Test Plan

## 1. Test Scope

集成测试验证多个模块之间的交互行为，包含输入解析、IRQ 触发与处理的端到端流程、tick 计数的跨模块一致性。

## 2. Test Environment

- 编译器：GCC (MinGW)
- 语言标准：C11
- 测试框架：自定义 assert 宏
- 每个测试用例前调用 `irq_reset_all()` 重置状态

```mermaid
flowchart TD
    START(["integrated_test main()"])
    RESET["irq_reset_all()"]
    SIM["simulate_input()<br/>模拟用户输入"]
    ASSERT{"assert 检查"}
    PASS["PRINT: [PASS]"]
    FAIL["PRINT: [FAIL]"]
    NEXT{"还有测试?"}
    REPORT["输出统计: Total / Passed / Failed"]
    END_(["return 0"])

    START --> RESET
    RESET --> SIM
    SIM --> ASSERT
    ASSERT -->|"通过"| PASS
    ASSERT -->|"失败"| FAIL
    PASS --> NEXT
    FAIL --> NEXT
    NEXT -->|"Yes"| RESET
    NEXT -->|"No"| REPORT
    REPORT --> END_
```

## 3. Test Cases

### IT_01: 数字模式输入解析

| ID | 测试项 | 模拟输入 | 预期结果 |
|----|---------|---------|---------|
| IT_01_01 | 输入 1 触发 IRQ0 | `"1"` | pending=0x01, IRQ0 被处理, pending=0 |
| IT_01_02 | 输入 32 触发 IRQ31 | `"32"` | pending=0x80000000, IRQ31 被处理 |
| IT_01_03 | 输入 0 手动处理 | trigger(3) → `"0"` | IRQ3 被处理 |
| IT_01_04 | 无效数字 33 | `"33"` | pending 不变，输出错误消息 |
| IT_01_05 | 无效数字 -5 | `"-5"` | pending 不变，输出错误消息 |

### IT_02: b-mode 输入解析

| ID | 测试项 | 模拟输入 | 预期结果 |
|----|---------|---------|---------|
| IT_02_01 | b0 触发 IRQ0 | `"b0"` | pending=0x01, IRQ0 被处理 |
| IT_02_02 | b5 触发 IRQ5 | `"b5"` | pending=0x20, IRQ5 被处理 |
| IT_02_03 | b31 触发 IRQ31 | `"b31"` | pending=0x80000000, IRQ31 被处理 |
| IT_02_04 | B10 (大写) | `"B10"` | pending=0x400, IRQ10 被处理 |
| IT_02_05 | 无效 b32 | `"b32"` | pending 不变，输出错误 |
| IT_02_06 | 无效 b-1 | `"b-1"` | pending 不变，输出错误 |

### IT_03: h-mode 输入解析

| ID | 测试项 | 模拟输入 | 预期结果 |
|----|---------|---------|---------|
| IT_03_01 | h1 触发 IRQ0 | `"h1"` | pending=0x01, IRQ0 被处理 |
| IT_03_02 | h3 触发 IRQ0,1 | `"h3"` | IRQ0, IRQ1 依次被处理 |
| IT_03_03 | hFF 触发 IRQ0~7 | `"hFF"` | IRQ0~7 全部依次处理 |
| IT_03_04 | h80000000 触发 IRQ31 | `"h80000000"` | IRQ31 被处理 |
| IT_03_05 | H0A (大写+hex) | `"H0A"` | pending=0x0A, IRQ1,3 被处理 |
| IT_03_06 | 无效 hGG | `"hGG"` | pending 不变，输出错误 |

### IT_04: 累积触发与优先权

| ID | 测试项 | 步骤 | 预期结果 |
|----|---------|------|---------|
| IT_04_01 | 先触发再 h-mode 追加 | trigger(0) → `"h6"` | IRQ0,1,2 依次处理 |
| IT_04_02 | 多次 b-mode 累积 | `"b10"` → `"b5"` → `"0"` | IRQ5,10 依次处理 |
| IT_04_03 | 优先权顺序验证 | `"h80000001"` | IRQ0 先于 IRQ31 处理 |

### IT_05: Tick 计数一致性

| ID | 测试项 | 步骤 | 预期结果 |
|----|---------|------|---------|
| IT_05_01 | 初始 tick 为 0 | reset → get_tick | tick == 0 |
| IT_05_02 | 触发 IRQ0 后 tick+1 | trigger(0) → process | tick 增加 (IRQ0 handler +1) |
| IT_05_03 | 非 IRQ0 不影响 tick | trigger(5) → process | tick 不因 IRQ5 而增加 |
| IT_05_04 | 多次 IRQ0 tick 累计 | trigger(0)→process, trigger(0)→process, trigger(0)→process | tick 正确累加 3 |

### IT_06: exit 与边界条件

| ID | 测试项 | 模拟输入 | 预期结果 |
|----|---------|---------|---------|
| IT_06_01 | exit 正常退出 | `"exit"` | 返回 0，输出 goodbye |
| IT_06_02 | 空行输入 | `""` | 输出错误提示，不崩溃 |
| IT_06_03 | 乱码输入 | `"xyz"` | 输出错误提示，不崩溃 |

### IT_07: 端到端完整流程

| ID | 测试项 | 步骤 | 预期结果 |
|----|---------|------|---------|
| IT_07_01 | 完整操作序列 | `"1"` → `"b5"` → `"h3"` → `"exit"` | 所有 IRQ 正确处理，正常退出 |

## 4. Expected Results

- 所有 IT_01 ~ IT_07 测试用例须全部通过
- 通过率：100%

## 5. 整合测试追溯表

| ID | 章节 | 追溯 SD | 追溯 UT | 描述 |
|----|------|---------|---------|------|
| IT_01 | 3 (IT_01) | SD_007<br>SD_010 | UT_03<br>UT_04<br>UT_05 | 数字模式输入解析：IRQ0/IRQ31 触发、手动处理 (0)、无效范围 (33, -5) |
| IT_02 | 3 (IT_02) | SD_007<br>SD_010 | UT_03<br>UT_04<br>UT_05 | b-mode 输入解析：b0/b5/b31 触发、大写 B10、无效范围 (b32, b-1) |
| IT_03 | 3 (IT_03) | SD_007<br>SD_010<br>SD_014 | UT_05<br>UT_08 | h-mode 输入解析：h1/h3/hFF/h80000000 触发、大写 H0A、无效 hGG |
| IT_04 | 3 (IT_04) | SD_004<br>SD_005<br>SD_007<br>SD_014 | UT_03<br>UT_05<br>UT_08<br>UT_10 | 累积触发与优先权：trigger+h-mode 追加、多次 b-mode 累积、IRQ0 优先于 IRQ31 |
| IT_05 | 3 (IT_05) | SD_002<br>SD_006<br>SD_009 | UT_01<br>UT_04<br>UT_07 | Tick 计数一致性：初始为 0、IRQ0 递增、非 IRQ0 不影响、多次累加 |
| IT_06 | 3 (IT_06) | SD_007<br>SD_010 | — | exit 与边界条件：正常退出、空行输入、乱码输入（主循环解析层验证） |
| IT_07 | 3 (IT_07) | SD_004<br>SD_005<br>SD_006<br>SD_007<br>SD_014 | UT_03<br>UT_04<br>UT_05<br>UT_08 | 端到端完整流程：数字→b-mode→h-mode→exit 全操作序列 |

### SD 覆盖对照表（整合测试）

| SD 项 | 覆盖 IT | 状态 |
|---------|---------------|--------|
| SD_001 | — | ⚠️ API 声明（单元测试验证） |
| SD_002 | IT_05 | ✅ 已覆盖 |
| SD_003 | IT_01 ~ IT_07（全部 IT 验证日志输出） | ✅ 已覆盖 |
| SD_004 | IT_04, IT_07 | ✅ 已覆盖 |
| SD_005 | IT_04, IT_07 | ✅ 已覆盖 |
| SD_006 | IT_05, IT_07 | ✅ 已覆盖 |
| SD_007 | IT_01, IT_02, IT_03, IT_04, IT_06, IT_07 | ✅ 已覆盖 |
| SD_008 | — | ⚠️ 数据结构（单元测试验证） |
| SD_009 | IT_05 | ✅ 已覆盖 |
| SD_010 | IT_01, IT_02, IT_03, IT_06 | ✅ 已覆盖 |
| SD_011 | — | ⚠️ 设计决策（单元测试验证） |
| SD_012 | IT_01 ~ IT_07（全部 IT 验证 TICK_PRINTF 宏） | ✅ 已覆盖 |
| SD_013 | — | ⚠️ 设计决策（单元测试验证） |
| SD_014 | IT_03, IT_04, IT_07 | ✅ 已覆盖 |

> **缩写说明：**
>
> - **IT** = Integration Test（整合测试，为所有整合测试用例的统一编号）
> - **UT** = Unit Test（单元测试，追溯至 SWE.4 单元测试项）
> - **SD** = Software Detailed Design（软件详细设计，追溯至 SWE.3 详细设计项）