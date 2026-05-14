# IRQ Simulator - Software Architecture

## 1. Architecture Overview

本專案採用 **單層模組化架構 (Monolithic Modular Architecture)**，所有核心邏輯集中於 `main.c`，透過 `main.h` 對外暴露介面。

```mermaid
flowchart TD
    USER["User (stdin)"]
    PARSER["Input Parser<br/>數字模式 / b-mode / h-mode / exit / 0"]
    TRIGGER["irq_trigger()<br/>Set bit"]
    PENDING["irq_pending<br/>32-bit register"]
    PROCESS["irq_process_all()<br/>Priority loop"]
    HANDLER["irq_handler()<br/>Switch-case"]
    TICK["tick_irq_handler()"]
    EXCEPTION["exception_irq_handler()"]
    OTHER["其他 IRQ handlers"]
    TICK_COUNT["g_tick_count"]
    EXCEPTION_COUNT["exception_count"]

    USER --> PARSER
    PARSER --> TRIGGER
    PARSER --> PENDING
    TRIGGER --> PENDING
    PENDING --> PROCESS
    PROCESS --> HANDLER
    HANDLER --> TICK
    HANDLER --> EXCEPTION
    HANDLER --> OTHER
    TICK --> TICK_COUNT
    EXCEPTION --> EXCEPTION_COUNT
```

## 2. Module Decomposition

### 2.1 Core Modules

| 模組 | 檔案 | 職責 |
|------|------|------|
| IRQ Core | `main.c` | IRQ 觸發、處理、pending register 管理 |
| IRQ Interface | `main.h` | 函式宣告、常數定義 |
| Startup | `start.s` | 組合語言中斷向量表與處理常式 |

### 2.2 Key Data Structures

```
irq_pending (uint32_t)
  Bit 0  -> IRQ0  (System Timer)
  Bit 1  -> IRQ1  (UART0 RX)
  ...
  Bit 31 -> IRQ31 (Exception)

g_tick_count (unsigned int)
  系統 tick 計數器，每次主迴圈迭代 +1
```

### 2.3 Function Call Graph

```mermaid
flowchart TD
    MAIN["main()"]
    TICK_H["tick_irq_handler()<br/>每迴圈迭代"]
    FGETS["fgets()<br/>讀取使用者輸入"]
    STRCMP{"strcmp('exit')<br/>退出判斷"}
    PARSE{"Input Parse"}
    TRIGGER["irq_trigger()<br/>b-mode / 數字模式"]
    PENDING_SET["irq_pending |= bit"]
    HEX_SET["irq_pending |= hex_val<br/>h-mode 直接設定"]
    PROCESS["irq_process_all()<br/>處理所有 pending IRQ"]
    LOOP["for i = 0..31"]
    CHECK{"pending bit set?"}
    HANDLER["irq_handler(i)"]
    TICK_INNER["tick_irq_handler()<br/>IRQ0"]
    EXC_INNER["exception_irq_handler()<br/>IRQ31"]
    OTHER["其他 IRQ 行為"]
    CLEAR["irq_pending &= ~bit"]

    MAIN --> TICK_H
    TICK_H --> FGETS
    FGETS --> STRCMP
    STRCMP -->|"exit"| EXIT["return 0"]
    STRCMP -->|"other"| PARSE
    PARSE -->|"b-mode / num"| TRIGGER
    PARSE -->|"h-mode"| HEX_SET
    TRIGGER --> PENDING_SET
    PENDING_SET --> PROCESS
    HEX_SET --> PROCESS
    PROCESS --> LOOP
    LOOP --> CHECK
    CHECK -->|"Yes"| HANDLER
    CHECK -->|"No"| LOOP
    HANDLER --> TICK_INNER
    HANDLER --> EXC_INNER
    HANDLER --> OTHER
    TICK_INNER --> CLEAR
    EXC_INNER --> CLEAR
    OTHER --> CLEAR
    CLEAR --> LOOP
```

## 3. Data Flow

### 3.1 IRQ Trigger Flow (b-mode)

```mermaid
sequenceDiagram
    participant User
    participant Parser as Input Parser
    participant Trigger as irq_trigger()
    participant Pending as irq_pending
    participant Process as irq_process_all()
    participant Handler as irq_handler()

    User->>Parser: "b5"
    Parser->>Parser: 辨識為 b-mode
    Parser->>Trigger: irq_trigger(5)
    Trigger->>Pending: |= (1 << 5) → 0x00000020
    Parser->>Process: irq_process_all()
    Process->>Process: 掃描 bit 0..31
    Process->>Handler: irq_handler(5) [bit 5 set]
    Handler->>Handler: 執行 SPI0 處理行為
    Handler->>Pending: &= ~(1 << 5) → 清除 bit 5
```

### 3.2 Hex Multi-IRQ Flow

```mermaid
sequenceDiagram
    participant User
    participant Parser as Input Parser
    participant Pending as irq_pending
    participant Process as irq_process_all()
    participant Handler as irq_handler()

    User->>Parser: "h3"
    Parser->>Parser: 辨識為 h-mode
    Parser->>Pending: |= 0x03 (bit 0,1 同時設定)
    Parser->>Process: irq_process_all()
    Process->>Process: 掃描 bit 0..31
    Process->>Handler: irq_handler(0) → tick++
    Process->>Handler: irq_handler(1) → UART0 RX
    Process->>Pending: pending = 0
```

## 4. Build System

```mermaid
flowchart LR
    CMAKE["CMakeLists.txt"]
    PROJECT["project(trae_test C)"]
    EXE["trae_test.exe"]
    UNIT["unit_test.exe"]
    INTEG["integrated_test.exe"]
    MAIN["main.c + main.h"]
    START["start.s"]
    UT_SRC["unit_test.c + unit_test.h"]
    IT_SRC["integrated_test.c + integrated_test.h"]
    C11["C11 Standard"]
    NINJA["Ninja build generator"]

    CMAKE --> PROJECT
    PROJECT --> EXE
    PROJECT --> UNIT
    PROJECT --> INTEG
    MAIN --> EXE
    START --> EXE
    MAIN --> UNIT
    UT_SRC --> UNIT
    MAIN --> INTEG
    IT_SRC --> INTEG
    C11 --> CMAKE
    NINJA --> CMAKE
```

## 5. Test Architecture

```mermaid
flowchart TD
    subgraph UnitTest["Unit Test (unit_test.c/h)"]
        UT_TRIGGER["irq_trigger()"]
        UT_HANDLER["irq_handler()"]
        UT_PROCESS["irq_process_all()"]
        UT_TICK["tick_irq_handler()"]
        UT_EXC["exception_irq_handler()"]
        UT_ACCESS["存取函式<br/>irq_get_pending / irq_get_tick"]
    end

    subgraph IntegTest["Integration Test (integrated_test.c/h)"]
        IT_PARSE["輸入解析<br/>數字/b/h-mode"]
        IT_MULTI["多 IRQ 觸發"]
        IT_TICK["tick 遞增"]
        IT_RESET["狀態重置"]
        IT_BOUNDARY["邊界條件"]
    end

    subgraph Core["IRQ Core (main.c)"]
        CORE_FUNC["核心函式"]
    end

    UnitTest --> Core
    IntegTest --> Core
```