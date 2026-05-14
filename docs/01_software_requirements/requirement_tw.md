# IRQ Simulator - Requirement Specification

## 1. Overview

本專案為一個 **IRQ (Interrupt Request) 模擬器**，運行於 Host PC 環境，用於模擬嵌入式系統中的中斷處理機制。使用者透過命令列介面輸入指令來觸發 IRQ，系統依優先權順序處理待處理的中斷。

```mermaid
flowchart LR
    USER["User<br/>(stdin)"]
    SIM["IRQ Simulator"]
    PENDING["Pending Register<br/>32-bit"]
    HANDLER["IRQ Handlers<br/>IRQ0 ~ IRQ31"]
    TICK["Tick Counter"]
    LOG["Log Output<br/>[tick: N] ..."]

    USER -->|"1-32 / bN / hHEX / exit"| SIM
    SIM --> PENDING
    PENDING --> HANDLER
    HANDLER --> TICK
    SIM --> LOG
```

## 2. Functional Requirements

### FR-01: IRQ Trigger Mechanism
- 系統須支援 32 個 IRQ 通道 (IRQ0 ~ IRQ31)
- 每個 IRQ 以 32-bit pending register 中的一個 bit 表示
- IRQ 觸發後，對應 bit 設為 1，等待處理

### FR-02: Input Modes
系統須支援以下三種輸入模式：

| 模式 | 語法 | 說明 | 範例 |
|------|------|------|------|
| 預設數字模式 | `<1-32>` | 觸發單一 IRQ，輸入值減 1 對應 IRQ 編號 | `1` → IRQ0 |
| Bit 模式 | `b<N>` | 直接指定 IRQ 編號 (0-31) | `b5` → IRQ5 |
| Hex 模式 | `h<HEX>` | 以十六進位值直接設定 pending register | `h3` → IRQ0, IRQ1 |

### FR-03: IRQ Priority Handling
- IRQ0 具有最高優先權，IRQ31 最低
- 待處理 IRQ 依編號由小到大（優先權由高到低）依序處理
- 每個 IRQ 處理完畢後清除對應的 pending bit

### FR-04: IRQ Handler Behaviors
每個 IRQ 須有對應的模擬處理行為：

| IRQ | 模擬周邊 | 行為 |
|-----|---------|------|
| IRQ0 | System Timer | 呼叫 tick_irq_handler，遞增 tick 計數 |
| IRQ1 | UART0 RX | 資料接收 |
| IRQ2 | UART0 TX | 資料傳送 |
| IRQ3 | GPIO Port A | 腳位狀態變更 |
| IRQ4 | GPIO Port B | 腳位狀態變更 |
| IRQ5 | SPI0 | 傳輸完成 |
| IRQ6 | I2C0 | 交易完成 |
| IRQ7 | ADC | 轉換完成 |
| IRQ8~9 | DMA Ch0~1 | 傳輸完成 |
| IRQ10 | Watchdog | 計時器屆期 |
| IRQ11 | RTC | 鬧鐘觸發 |
| IRQ12 | USB | 裝置事件 |
| IRQ13 | CAN0 | 訊息接收 |
| IRQ14 | PWM | 週期結束 |
| IRQ15~16 | Timer1~2 | 比較匹配 / 溢位 |
| IRQ17~18 | UART1 RX/TX | 資料接收/傳送 |
| IRQ19 | SPI1 | 傳輸完成 |
| IRQ20 | I2C1 | 交易完成 |
| IRQ21~23 | External INT0~2 | 外部中斷 |
| IRQ24~25 | DMA Ch2~3 | 傳輸完成 |
| IRQ26 | CRC | 計算完成 |
| IRQ27 | AES | 加密完成 |
| IRQ28 | QSPI | 指令完成 |
| IRQ29 | SDIO | 卡片事件 |
| IRQ30 | Ethernet | 封包接收 |
| IRQ31 | Exception | 呼叫 exception_irq_handler |

### FR-05: Tick Counter
- 系統須維護一個全域 tick 計數器
- 每次主迴圈迭代時 tick 自動遞增
- IRQ0 處理時 tick 也會遞增
- 所有 log 輸出須帶有 `[tick: N]` 前綴

### FR-06: Program Control
- 輸入 `0`：手動處理所有 pending IRQ
- 輸入 `exit`：結束模擬器

## 3. Non-Functional Requirements

### NFR-01: Usability
- 提供清晰的命令提示與使用說明
- 對無效輸入提供明確的錯誤訊息

### NFR-02: Maintainability
- 程式碼遵循既有 coding style 與註解規範
- IRQ 處理邏輯集中於 switch-case，易於擴充

### NFR-03: Portability
- 使用標準 C11，不依賴特定平台 API
- 透過 CMake 建置系統管理編譯