# IRQ Simulator - Software Design

## 1. Design Overview

本文件描述 IRQ 模拟器的详细设计，包含接口定义、数据结构、算法与关键设计决策。

## 2. Interface Design

### 2.1 Public API (main.h)

```c
#define IRQ_COUNT 32

void tick_irq_handler(void);          /* Tick 中断处理 */
void exception_irq_handler(void);     /* 异常中断处理 */
void irq_trigger(unsigned int num);   /* 触发指定 IRQ */
void irq_trigger_raw(uint32_t mask);  /* 以 raw mask 触发多个 IRQ */
void irq_handler(unsigned int num);   /* 处理指定 IRQ */
void irq_process_all(void);           /* 处理所有 pending IRQ */

/* Test Accessors */
uint32_t irq_get_pending(void);       /* 读取 pending register */
unsigned int irq_get_tick(void);      /* 读取 tick 计数 */
void irq_reset_all(void);             /* 重置所有状态 */
```

### 2.2 Internal State

```c
static uint32_t irq_pending = 0;        /* IRQ pending register */
static unsigned int g_tick_count = 0;   /* 全局 tick 计数器 */
```

### 2.3 Logging Macro

```c
#define TICK_PRINTF(fmt, ...) \
    printf("[tick: %05u] " fmt, g_tick_count, ##__VA_ARGS__)
```

## 3. Algorithm Design

### 3.1 IRQ Trigger Algorithm

```mermaid
flowchart TD
    START(["irq_trigger(irq_num)"])
    CHECK{"irq_num < 32?"}
    SET["irq_pending |= (1 << irq_num)"]
    LOG_OK["LOG: IRQ triggered"]
    LOG_ERR["LOG: Invalid IRQ number"]
    END_(["return"])

    START --> CHECK
    CHECK -->|"Yes"| SET
    CHECK -->|"No"| LOG_ERR
    SET --> LOG_OK
    LOG_OK --> END_
    LOG_ERR --> END_
```

### 3.2 IRQ Process-All Algorithm (Priority-Based)

```mermaid
flowchart TD
    START(["irq_process_all()"])
    CHECK_EMPTY{"irq_pending == 0?"}
    LOG_START["LOG: Processing pending IRQs"]
    INIT["i = 0"]
    LOOP{"i < 32?"}
    CHECK_BIT{"irq_pending & (1 << i)?"}
    CALL_HANDLER["irq_handler(i)"]
    NEXT["i++"]
    LOG_END["LOG: All IRQs processed"]
    END_(["return"])

    START --> CHECK_EMPTY
    CHECK_EMPTY -->|"Yes"| END_
    CHECK_EMPTY -->|"No"| LOG_START
    LOG_START --> INIT
    INIT --> LOOP
    LOOP -->|"Yes"| CHECK_BIT
    LOOP -->|"No"| LOG_END
    CHECK_BIT -->|"Yes"| CALL_HANDLER
    CHECK_BIT -->|"No"| NEXT
    CALL_HANDLER --> NEXT
    NEXT --> LOOP
    LOG_END --> END_
```

### 3.3 IRQ Handler Dispatch Algorithm

```mermaid
flowchart TD
    START(["irq_handler(irq_num)"])
    LOG["LOG: Handling IRQ<num>"]
    SWITCH{"irq_num?"}
    CASE0["tick_irq_handler()"]
    CASE1["LOG: UART0 RX"]
    CASE2["LOG: UART0 TX"]
    CASE3["LOG: GPIO Port A"]
    CASE4["LOG: GPIO Port B"]
    CASE5["LOG: SPI0"]
    CASE6["LOG: I2C0"]
    CASE7["LOG: ADC"]
    CASE8["LOG: DMA Ch0"]
    CASE9["LOG: DMA Ch1"]
    CASE10["LOG: Watchdog"]
    CASE11["LOG: RTC"]
    CASE12["LOG: USB"]
    CASE13["LOG: CAN0"]
    CASE14["LOG: PWM"]
    CASE15["LOG: Timer1"]
    CASE16["LOG: Timer2"]
    CASE17["LOG: UART1 RX"]
    CASE18["LOG: UART1 TX"]
    CASE19["LOG: SPI1"]
    CASE20["LOG: I2C1"]
    CASE21["LOG: External INT0"]
    CASE22["LOG: External INT1"]
    CASE23["LOG: External INT2"]
    CASE24["LOG: DMA Ch2"]
    CASE25["LOG: DMA Ch3"]
    CASE26["LOG: CRC"]
    CASE27["LOG: AES"]
    CASE28["LOG: QSPI"]
    CASE29["LOG: SDIO"]
    CASE30["LOG: Ethernet"]
    CASE31["exception_irq_handler()"]
    CLEAR["irq_pending &= ~(1 << irq_num)"]
    END_(["return"])

    START --> LOG
    LOG --> SWITCH
    SWITCH -->|"0"| CASE0
    SWITCH -->|"1"| CASE1
    SWITCH -->|"2"| CASE2
    SWITCH -->|"3"| CASE3
    SWITCH -->|"4"| CASE4
    SWITCH -->|"5"| CASE5
    SWITCH -->|"6"| CASE6
    SWITCH -->|"7"| CASE7
    SWITCH -->|"8"| CASE8
    SWITCH -->|"9"| CASE9
    SWITCH -->|"10"| CASE10
    SWITCH -->|"11"| CASE11
    SWITCH -->|"12"| CASE12
    SWITCH -->|"13"| CASE13
    SWITCH -->|"14"| CASE14
    SWITCH -->|"15"| CASE15
    SWITCH -->|"16"| CASE16
    SWITCH -->|"17"| CASE17
    SWITCH -->|"18"| CASE18
    SWITCH -->|"19"| CASE19
    SWITCH -->|"20"| CASE20
    SWITCH -->|"21"| CASE21
    SWITCH -->|"22"| CASE22
    SWITCH -->|"23"| CASE23
    SWITCH -->|"24"| CASE24
    SWITCH -->|"25"| CASE25
    SWITCH -->|"26"| CASE26
    SWITCH -->|"27"| CASE27
    SWITCH -->|"28"| CASE28
    SWITCH -->|"29"| CASE29
    SWITCH -->|"30"| CASE30
    SWITCH -->|"31"| CASE31
    CASE0 --> CLEAR
    CASE1 --> CLEAR
    CASE2 --> CLEAR
    CASE3 --> CLEAR
    CASE4 --> CLEAR
    CASE5 --> CLEAR
    CASE6 --> CLEAR
    CASE7 --> CLEAR
    CASE8 --> CLEAR
    CASE9 --> CLEAR
    CASE10 --> CLEAR
    CASE11 --> CLEAR
    CASE12 --> CLEAR
    CASE13 --> CLEAR
    CASE14 --> CLEAR
    CASE15 --> CLEAR
    CASE16 --> CLEAR
    CASE17 --> CLEAR
    CASE18 --> CLEAR
    CASE19 --> CLEAR
    CASE20 --> CLEAR
    CASE21 --> CLEAR
    CASE22 --> CLEAR
    CASE23 --> CLEAR
    CASE24 --> CLEAR
    CASE25 --> CLEAR
    CASE26 --> CLEAR
    CASE27 --> CLEAR
    CASE28 --> CLEAR
    CASE29 --> CLEAR
    CASE30 --> CLEAR
    CASE31 --> CLEAR
    CLEAR --> END_
```

### 3.4 Input Parsing Algorithm

```mermaid
flowchart TD
    START(["main_loop()"])
    TICK["tick_irq_handler()<br/>每次迭代 tick+1"]
    READ["READ line from stdin"]
    EXIT_CHECK{"line == 'exit'?"}
    BREAK(["BREAK loop"])
    B_CHECK{"line[0] == 'b'?"}
    H_CHECK{"line[0] == 'h'?"}
    B_PARSE["parse number"]
    B_TRIGGER["irq_trigger(num)"]
    H_PARSE["parse hex"]
    H_TRIGGER["irq_pending |= hex"]
    NUM_PARSE["parse number"]
    ZERO_CHECK{"num == 0?"}
    RANGE_CHECK{"1 <= num <= 32?"}
    NUM_TRIGGER["irq_trigger(num-1)"]
    LOG_ERR["LOG: Invalid input"]
    PROCESS["irq_process_all()"]

    START --> TICK
    TICK --> READ
    READ --> EXIT_CHECK
    EXIT_CHECK -->|"Yes"| BREAK
    EXIT_CHECK -->|"No"| B_CHECK
    B_CHECK -->|"Yes"| B_PARSE
    B_CHECK -->|"No"| H_CHECK
    B_PARSE --> B_TRIGGER
    B_TRIGGER --> PROCESS
    H_CHECK -->|"Yes"| H_PARSE
    H_CHECK -->|"No"| NUM_PARSE
    H_PARSE --> H_TRIGGER
    H_TRIGGER --> PROCESS
    NUM_PARSE --> ZERO_CHECK
    ZERO_CHECK -->|"Yes"| PROCESS
    ZERO_CHECK -->|"No"| RANGE_CHECK
    RANGE_CHECK -->|"Yes"| NUM_TRIGGER
    RANGE_CHECK -->|"No"| LOG_ERR
    NUM_TRIGGER --> PROCESS
    PROCESS --> TICK
    LOG_ERR --> TICK
```

## 4. Data Structure Design

### 4.1 IRQ Pending Register

```mermaid
block-beta
    columns 32
    block:bit31:1 B31["31"]
    block:bit30:1 B30["30"]
    block:bit29:1 B29["29"]
    block:bit28:1 B28["28"]
    block:bit27:1 B27["27"]
    block:bit26:1 B26["26"]
    block:bit25:1 B25["25"]
    block:bit24:1 B24["24"]
    block:bit23:1 B23["23"]
    block:bit22:1 B22["22"]
    block:bit21:1 B21["21"]
    block:bit20:1 B20["20"]
    block:bit19:1 B19["19"]
    block:bit18:1 B18["18"]
    block:bit17:1 B17["17"]
    block:bit16:1 B16["16"]
    block:bit15:1 B15["15"]
    block:bit14:1 B14["14"]
    block:bit13:1 B13["13"]
    block:bit12:1 B12["12"]
    block:bit11:1 B11["11"]
    block:bit10:1 B10["10"]
    block:bit9:1 B9["9"]
    block:bit8:1 B8["8"]
    block:bit7:1 B7["7"]
    block:bit6:1 B6["6"]
    block:bit5:1 B5["5"]
    block:bit4:1 B4["4"]
    block:bit3:1 B3["3"]
    block:bit2:1 B2["2"]
    block:bit1:1 B1["1"]
    block:bit0:1 B0["0"]
```

```txt
Bit 0  = IRQ0  (最高优先权)
Bit 31 = IRQ31 (最低优先权)
```

### 4.2 Tick Counter Lifecycle

```mermaid
stateDiagram
    [*] --> Init: 程序启动
    Init --> Running: g_tick_count = 0
    Running --> Running: 主循环迭代: g_tick_count++
    Running --> Running: IRQ0 处理: g_tick_count++
    Running --> Init: irq_reset_all()
```

## 5. Error Handling Design

| 场景 | 处理方式 |
|------|---------|
| IRQ 编号超出范围 (>=32) | 输出错误消息，不修改 pending register |
| b-mode 参数无效 | 输出 "Invalid bit mode" |
| h-mode 参数无效 | 输出 "Invalid hex mode" |
| 纯数字超出 1-32 | 输出 "Invalid IRQ number" |
| 无法解析的输入 | 输出 "Invalid input" |
| stdin EOF | 正常退出循环 |

## 6. Design Decisions

### DD-01: 为何使用 static 文件级变量而非全局变量？
- 限制变量的可见范围，避免外部模块意外修改
- 通过 test accessor 函数提供受控的读取/重置接口

### DD-02: 为何使用宏 TICK_PRINTF 而非包装函数？
- 宏可在编译期展开，无函数调用 overhead
- 使用 `##__VA_ARGS__` (GNU extension) 支持无参数情况
- 统一所有 log 输出的格式

### DD-03: 为何 IRQ 处理后立即清除 pending bit 而非批量清除？
- 模拟真实硬件行为：ISR 执行后清除中断标志
- 避免同一个 IRQ 被重复处理

### DD-04: 为何 h-mode 使用 `|=` 而非 `=`？
- 允许累积触发：先触发一些 IRQ，再用 h-mode 追加
- 更贴近真实硬件中断控制器的行为

## 7. 详细设计追溯表

| ID | 章节 | 追溯 SA | 追溯 SR | 描述 |
|----|------|---------|---------|------|
| SD_001 | 2.1 | SA_003 | SR_001<br>SR_044 | Public API (`main.h`)：9 个函数声明 + `IRQ_COUNT` 常量定义 |
| SD_002 | 2.2 | SA_005<br>SA_006 | SR_001<br>SR_002<br>SR_003<br>SR_036<br>SR_037<br>SR_038 | 内部状态：`irq_pending` (static uint32_t) 与 `g_tick_count` (static unsigned int) 作为文件级变量 |
| SD_003 | 2.3 | SA_023 | SR_039 | `TICK_PRINTF` 宏：`printf` 带 `[tick: N]` 前缀，使用 `##__VA_ARGS__` 支持零参数 |
| SD_004 | 3.1 | SA_009 | SR_003<br>SR_004<br>SR_005<br>SR_042 | `irq_trigger()` 算法：范围检查 (`irq_num < 32`) → 位设置 (`1 << irq_num`) → 日志 |
| SD_005 | 3.2 | SA_011 | SR_007<br>SR_008 | `irq_process_all()` 算法：空检查 → 优先级循环 (IRQ0→IRQ31) → 逐 pending bit 分发处理 |
| SD_006 | 3.3 | SA_012<br>SA_013<br>SA_014 | SR_009<br>SR_010<br>SR_035<br>SR_045 | `irq_handler()` 分发：switch-case 覆盖 32 个 IRQ 行为 → 处理后清除 pending bit |
| SD_007 | 3.4 | SA_007<br>SA_008 | SR_004<br>SR_005<br>SR_006<br>SR_037<br>SR_040<br>SR_041<br>SR_042<br>SR_043 | 输入解析算法：tick 递增 → 读取 stdin → 解析 (b/h/数字/0/exit) → 触发 → 处理 |
| SD_008 | 4.1 | SA_005 | SR_001<br>SR_002<br>SR_003 | IRQ Pending Register 布局：32-bit，Bit 0=IRQ0（最高优先级）至 Bit 31=IRQ31（最低优先级） |
| SD_009 | 4.2 | SA_006 | SR_036<br>SR_037<br>SR_038 | Tick 计数器生命周期：Init (g_tick_count=0) → Running (循环/IRQ0 递增) → Reset (irq_reset_all) |
| SD_010 | 5 | SA_025 | SR_042<br>SR_043 | 错误处理设计：6 种场景（范围越界、无效 b/h 模式、无效数字、无法解析、EOF） |
| SD_011 | 6 | SA_002<br>SA_003 | SR_044 | DD-01：static 文件级变量 — 限制可见范围，通过 test accessor 函数受控访问 |
| SD_012 | 6 | SA_023 | SR_039 | DD-02：TICK_PRINTF 宏 vs 包装函数 — 零调用开销、`##__VA_ARGS__`、统一日志格式 |
| SD_013 | 6 | SA_024 | SR_009 | DD-03：立即清除 pending bit — 模拟真实硬件 ISR 行为，防止重复处理 |
| SD_014 | 6 | SA_010 | SR_003<br>SR_006 | DD-04：h-mode `|=` vs `=` — 累积触发，贴近真实中断控制器行为 |

### 章节对照表

| 章节 | SD 范围 | 数量 | 内容 |
|------|---------|------|------|
| 2 | SD_001 ~ SD_003 | 3 | 接口设计 |
| 3 | SD_004 ~ SD_007 | 4 | 算法设计 |
| 4 | SD_008 ~ SD_009 | 2 | 数据结构设计 |
| 5 | SD_010 | 1 | 错误处理设计 |
| 6 | SD_011 ~ SD_014 | 4 | 设计决策 |

> **缩写说明：**
>
> - **SD** = Software Detailed Design（软件详细设计，为所有详细设计项的统一编号）
> - **SA** = Software Architecture（软件架构，追溯至 SWE.2 架构项）
> - **SR** = Software Requirement（软件需求，追溯至 SWE.1 需求项）