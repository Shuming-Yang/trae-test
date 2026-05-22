<a id="top"></a>

[English](#english) | [简体中文](#简体中文) | [繁體中文](#繁體中文)   ← 頁首語系切換

---

# IRQ Simulator

<a id="english"></a>

## English

An **embedded firmware IRQ (Interrupt Request) simulator** developed under the **ASPICE V-Model** software development process. The project simulates a 32-channel interrupt controller on a Host PC, allowing developers to trigger, handle, and verify IRQ behavior via a command-line interface before deploying to target hardware.

### Project Overview

| Aspect | Detail |
|--------|--------|
| **Language** | C11 (ANSI C) |
| **Build System** | CMake 3.10+ + Ninja |
| **CI/CD** | GitLab CI/CD (Docker: `atb-compiler-env:v1`) |
| **Documentation** | MkDocs Material + PDF (EN / 简体中文 / 繁體中文) |
| **Code Quality** | MISRA C:2012 via cppcheck, Lizard complexity analysis |
| **Testing** | Unit Test + Integration Test with gcovr HTML coverage |
| **Code Review** | Automated per-issue tracking via `tools/gen_code_review.py` |

#### ASPICE V-Model Phases

| Phase | Folder | Deliverable |
|-------|--------|-------------|
| **SWE.1** Requirements | `docs/01_software_requirements/` | IRQ Simulator Requirement Specification |
| **SWE.2** Architecture | `docs/02_software_architecture/` | Software Architecture Design |
| **SWE.3** Detailed Design | `docs/03_software_detailed_design/` | Function-level design + auto-generated flowcharts |
| **SWE.4** Unit Verification | `docs/04_software_unit_verification/` | Unit test plan + coverage report |
| **SWE.5** Integration Test | `docs/05_software_integration_test/` | Integration test plan + MISRA + complexity reports |

### Project Structure

```
├── src/                          # C source code
│   ├── main.c                    # Core IRQ simulator logic + CLI main loop
│   ├── start.s                   # Assembly startup & interrupt vector stubs
│   └── make_misra_error.c        # Intentionally MISRA-noncompliant demo file
├── inc/
│   └── main.h                    # Public API, type definitions, test visibility macros
├── unit_test/
│   ├── unit_test.c               # Unit tests for all core functions (UT_01 ~ UT_07)
│   └── unit_test.h               # Unit test assertion macros
├── integration_test/
│   ├── integrated_test.c         # End-to-end integration tests (IT_01 ~ IT_07)
│   └── integrated_test.h         # Integration test assertion macros
├── tools/
│   ├── gen_func_docs.py          # Auto-generate function docs + Mermaid flowcharts
│   ├── gen_coverage_report.py    # Build, run unit_test, generate gcovr HTML coverage
│   ├── gen_integration_coverage_report.py  # Build, run integrated_test, generate coverage
│   ├── gen_misra_report.py       # Run cppcheck with MISRA addon, generate report
│   ├── gen_code_review.py        # Detect src/ changes, manage per-issue review records
│   ├── run_lizard.py             # Generate cyclomatic complexity / NLOC report
│   └── ninja/                    # Prebuilt Ninja binaries (linux, win, mac, arm64)
├── docs/                         # MkDocs documentation (ASPICE V-Model structure)
│   ├── index.md                  # Documentation home (EN)
│   ├── 01_software_requirements/ # SWE.1: Requirement specs (EN/CN/TW)
│   ├── 02_software_architecture/ # SWE.2: Architecture docs (EN/CN/TW)
│   ├── 03_software_detailed_design/ # SWE.3: Detailed design + auto-generated function docs
│   ├── 04_software_unit_verification/ # SWE.4: Unit test plan + coverage report
│   ├── 05_software_integration_test/  # SWE.5: Integration test + MISRA + lizard reports
│   └── code_review_record/       # Per-issue code review history
├── CMakeLists.txt                # CMake build configuration (3 targets)
├── mkdocs.yml                    # MkDocs Material theme + PDF plugin config
├── .gitlab-ci.yml                # GitLab CI/CD pipeline definition
└── README.md                     # This file
```

### Build & Run

#### Prerequisites

- **CMake** 3.10+
- **Ninja** build system (prebuilt binaries included in `tools/ninja/`)
- **GCC** or compatible C11 compiler

#### Build

```bash
# Configure with Ninja generator
cmake -B build -G Ninja

# Build the main executable
cmake --build build --target trae_test

# Build unit tests
cmake --build build --target unit_test

# Build integration tests
cmake --build build --target integrated_test
```

#### Run

```bash
# Launch the IRQ Simulator CLI
./build/trae_test
```

#### Run Tests Locally

```bash
# Unit tests
cmake -B build -G Ninja -DENABLE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build --target unit_test
./build/unit_test

# Integration tests
cmake --build build --target integrated_test
./build/integrated_test

# Generate HTML coverage report
python tools/gen_coverage_report.py
```

### IRQ Simulator CLI

The simulator provides an interactive command-line interface for triggering and processing interrupts.

```
========== IRQ Simulator ==========
  <num>   : trigger single IRQ (1-32 -> IRQ0-IRQ31)
  b<num>  : bit mode, trigger single IRQ (b1 -> IRQ1)
  h<hex>  : hex mode, trigger multiple IRQs (h3 -> IRQ0,1)
  0       : process all pending IRQs manually
  exit    : quit simulator
===================================
```

#### Input Modes

| Mode | Syntax | Example | Description |
|------|--------|---------|-------------|
| Numeric | `1` ~ `32` | `1` → triggers IRQ0 | Input value minus 1 maps to IRQ number |
| Bit | `b0` ~ `b31` | `b5` → triggers IRQ5 | Direct IRQ number specification |
| Hex | `h01` ~ `hFFFFFFFF` | `h3` → triggers IRQ0 + IRQ1 | Raw bitmask written to pending register |
| Process | `0` | `0` | Manually process all currently pending IRQs |

#### IRQ Channel Map (32 channels)

| IRQ | Peripheral | IRQ | Peripheral |
|-----|-----------|-----|-----------|
| 0 | System Timer (tick++) | 16 | Timer2 (overflow) |
| 1 | UART0 RX | 17 | UART1 RX |
| 2 | UART0 TX | 18 | UART1 TX |
| 3 | GPIO Port A | 19 | SPI1 |
| 4 | GPIO Port B | 20 | I2C1 |
| 5 | SPI0 | 21 | External INT0 |
| 6 | I2C0 | 22 | External INT1 |
| 7 | ADC | 23 | External INT2 |
| 8 | DMA Ch0 | 24 | DMA Ch2 |
| 9 | DMA Ch1 | 25 | DMA Ch3 |
| 10 | Watchdog | 26 | CRC |
| 11 | RTC | 27 | AES |
| 12 | USB | 28 | QSPI |
| 13 | CAN0 | 29 | SDIO |
| 14 | PWM | 30 | Ethernet |
| 15 | Timer1 | 31 | Exception |

### GitLab CI/CD Pipeline

The pipeline runs in the `atb-compiler-env:v1` Docker image and consists of two stages:

#### Stage: `build`

| Job | Trigger | Description |
|-----|---------|-------------|
| `build_on_commit` | Push with `.c`/`.h`/`CMakeLists.txt` changes | Compile `trae_test` executable |
| `weekly_build_check` | Scheduled (Mon-Fri 18:00) | Periodic compilation verification |

#### Stage: `pages`

| Job | Trigger | Steps |
|-----|---------|-------|
| `pages` | Push to `main`/`master` with source/doc changes | 1. Auto-generate function docs (flowcharts)<br>2. Run unit tests + coverage report<br>3. Run integration tests + coverage report<br>4. Run MISRA C:2012 static analysis<br>5. Run Lizard complexity analysis<br>6. Generate code review entry<br>7. Build MkDocs site + PDF |

The documentation site is automatically deployed to **GitLab Pages**.

### Code Quality & Standards

#### MISRA C:2012 Compliance

Static analysis is performed via **cppcheck** with the MISRA C:2012 addon. A deviation/suppression strategy is documented inline:

- `<stdio.h>` and `<stdarg.h>` are explicitly suppressed for CLI debug output (R21.6, R17.1)
- MISRA rule checklist is maintained per code review entry

#### Code Review Process

- On each `src/` change, `tools/gen_code_review.py` detects modified files and creates a new review entry
- Issues are tracked per-item with status: `pending`, `fixed (YYYY-MM-DD)`, or `❌ ignored`
- Review history is stored in `docs/code_review_record/code_review.md`

#### Complexity Analysis

**Lizard** generates cyclomatic complexity, NLOC, and parameter count reports per function, published under SWE.5 documentation.

### Documentation

#### Local Preview

```bash
pip install mkdocs mkdocs-material mkdocs-glightbox mkdocs-include-markdown mkdocs-with-pdf

# Start local dev server
mkdocs serve
```

Open http://127.0.0.1:8000 in your browser.

#### Build Static Site

```bash
mkdocs build --site-dir public
```

A combined PDF (`IRQ_Simulator_ASPICE_Documentation.pdf`) is auto-generated via the `mkdocs-with-pdf` plugin.

### Build Targets

| Target | Sources | Description |
|--------|---------|-------------|
| `trae_test` | `src/main.c` + `src/start.s` + `src/make_misra_error.c` | Main CLI executable |
| `unit_test` | `unit_test/unit_test.c` + `src/main.c` | Unit test executable (defines `TEST_BUILD`) |
| `integrated_test` | `integration_test/integrated_test.c` + `src/main.c` | Integration test executable (defines `TEST_BUILD`) |

The `TEST_BUILD` macro controls symbol visibility:
- **Production**: `FW_STATIC` expands to `static` (MISRA R8.7)
- **Test**: `FW_STATIC` expands to nothing, exposing internal symbols for the test harness

### License

Copyright (c) 2026 OmniVision Technologies, Inc. All rights reserved.

[↑ Back to top](#english)

---

[↑ 回到页首](#top)

<a id="简体中文"></a>

## 简体中文

**IRQ 模拟器**（嵌入式固件中断请求模拟器），遵循 **ASPICE V-Model** 软件开发流程。该项目在主机 PC 上模拟 32 通道中断控制器，让开发者可以在部署到目标硬件之前，通过命令行界面触发、处理和验证 IRQ 行为。

### 项目概览

| 项目 | 详情 |
|------|------|
| **开发语言** | C11 (ANSI C) |
| **构建系统** | CMake 3.10+ + Ninja |
| **CI/CD** | GitLab CI/CD (Docker: `atb-compiler-env:v1`) |
| **文档系统** | MkDocs Material + PDF (英文 / 简体中文 / 繁体中文) |
| **代码质量** | MISRA C:2012 (cppcheck)、Lizard 圈复杂度分析 |
| **测试体系** | 单元测试 + 集成测试，gcovr HTML 覆盖率报告 |
| **代码审查** | 基于 `tools/gen_code_review.py` 的逐项自动追踪 |

#### ASPICE V-Model 阶段

| 阶段 | 目录 | 交付物 |
|------|------|--------|
| **SWE.1** 需求分析 | `docs/01_software_requirements/` | IRQ 模拟器需求规格说明书 |
| **SWE.2** 架构设计 | `docs/02_software_architecture/` | 软件架构设计文档 |
| **SWE.3** 详细设计 | `docs/03_software_detailed_design/` | 函数级设计 + 自动生成流程图 |
| **SWE.4** 单元验证 | `docs/04_software_unit_verification/` | 单元测试计划 + 覆盖率报告 |
| **SWE.5** 集成测试 | `docs/05_software_integration_test/` | 集成测试计划 + MISRA + 复杂度报告 |

### 项目结构

```
├── src/                          # C 源代码
│   ├── main.c                    # IRQ 模拟器核心逻辑 + CLI 主循环
│   ├── start.s                   # 汇编启动代码 & 中断向量桩
│   └── make_misra_error.c        # 故意违反 MISRA 的演示文件
├── inc/
│   └── main.h                    # 公开 API、类型定义、测试可见性宏
├── unit_test/
│   ├── unit_test.c               # 所有核心函数的单元测试 (UT_01 ~ UT_07)
│   └── unit_test.h               # 单元测试断言宏
├── integration_test/
│   ├── integrated_test.c         # 端到端集成测试 (IT_01 ~ IT_07)
│   └── integrated_test.h         # 集成测试断言宏
├── tools/
│   ├── gen_func_docs.py          # 自动生成函数文档 + Mermaid 流程图
│   ├── gen_coverage_report.py    # 构建、运行单元测试，生成 gcovr HTML 覆盖率
│   ├── gen_integration_coverage_report.py  # 构建、运行集成测试，生成覆盖率
│   ├── gen_misra_report.py       # 运行 cppcheck + MISRA 插件，生成报告
│   ├── gen_code_review.py        # 检测 src/ 变更，管理逐项审查记录
│   ├── run_lizard.py             # 生成圈复杂度 / NLOC 报告
│   └── ninja/                    # 预编译 Ninja 二进制文件 (linux, win, mac, arm64)
├── docs/                         # MkDocs 文档 (ASPICE V-Model 结构)
│   ├── index.md                  # 文档首页 (英文)
│   ├── 01_software_requirements/ # SWE.1: 需求规格 (英文/简体/繁体)
│   ├── 02_software_architecture/ # SWE.2: 架构文档 (英文/简体/繁体)
│   ├── 03_software_detailed_design/ # SWE.3: 详细设计 + 自动生成函数文档
│   ├── 04_software_unit_verification/ # SWE.4: 单元测试计划 + 覆盖率报告
│   ├── 05_software_integration_test/  # SWE.5: 集成测试 + MISRA + lizard 报告
│   └── code_review_record/       # 逐项代码审查历史
├── CMakeLists.txt                # CMake 构建配置 (3 个目标)
├── mkdocs.yml                    # MkDocs Material 主题 + PDF 插件配置
├── .gitlab-ci.yml                # GitLab CI/CD 流水线定义
└── README.md                     # 本文件
```

### 构建与运行

#### 环境要求

- **CMake** 3.10+
- **Ninja** 构建系统 (`tools/ninja/` 中已包含预编译二进制)
- **GCC** 或兼容的 C11 编译器

#### 构建

```bash
# 使用 Ninja 生成器配置
cmake -B build -G Ninja

# 构建主程序
cmake --build build --target trae_test

# 构建单元测试
cmake --build build --target unit_test

# 构建集成测试
cmake --build build --target integrated_test
```

#### 运行

```bash
# 启动 IRQ 模拟器 CLI
./build/trae_test
```

#### 本地运行测试

```bash
# 单元测试
cmake -B build -G Ninja -DENABLE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build --target unit_test
./build/unit_test

# 集成测试
cmake --build build --target integrated_test
./build/integrated_test

# 生成 HTML 覆盖率报告
python tools/gen_coverage_report.py
```

### IRQ 模拟器 CLI

模拟器提供交互式命令行界面，用于触发和处理中断。

```
========== IRQ Simulator ==========
  <num>   : 触发单个 IRQ (1-32 -> IRQ0-IRQ31)
  b<num>  : 位模式，触发单个 IRQ (b1 -> IRQ1)
  h<hex>  : 十六进制模式，触发多个 IRQ (h3 -> IRQ0,1)
  0       : 手动处理所有挂起的 IRQ
  exit    : 退出模拟器
===================================
```

#### 输入模式

| 模式 | 语法 | 示例 | 说明 |
|------|------|------|------|
| 数字模式 | `1` ~ `32` | `1` → 触发 IRQ0 | 输入值减 1 映射到 IRQ 编号 |
| 位模式 | `b0` ~ `b31` | `b5` → 触发 IRQ5 | 直接指定 IRQ 编号 |
| 十六进制模式 | `h01` ~ `hFFFFFFFF` | `h3` → 触发 IRQ0 + IRQ1 | 原始位掩码写入挂起寄存器 |
| 处理模式 | `0` | `0` | 手动处理当前所有挂起的 IRQ |

#### IRQ 通道映射表（32 通道）

| IRQ | 外设 | IRQ | 外设 |
|-----|------|-----|------|
| 0 | 系统定时器 (tick++) | 16 | 定时器2 (溢出) |
| 1 | UART0 接收 | 17 | UART1 接收 |
| 2 | UART0 发送 | 18 | UART1 发送 |
| 3 | GPIO 端口 A | 19 | SPI1 |
| 4 | GPIO 端口 B | 20 | I2C1 |
| 5 | SPI0 | 21 | 外部中断 INT0 |
| 6 | I2C0 | 22 | 外部中断 INT1 |
| 7 | ADC | 23 | 外部中断 INT2 |
| 8 | DMA 通道0 | 24 | DMA 通道2 |
| 9 | DMA 通道1 | 25 | DMA 通道3 |
| 10 | 看门狗 | 26 | CRC |
| 11 | RTC | 27 | AES |
| 12 | USB | 28 | QSPI |
| 13 | CAN0 | 29 | SDIO |
| 14 | PWM | 30 | 以太网 |
| 15 | 定时器1 | 31 | 异常中断 |

### GitLab CI/CD 流水线

流水线运行在 `atb-compiler-env:v1` Docker 镜像中，分为两个阶段：

#### Stage: `build`

| 作业 | 触发条件 | 说明 |
|------|----------|------|
| `build_on_commit` | 推送包含 `.c`/`.h`/`CMakeLists.txt` 变更 | 编译 `trae_test` 可执行文件 |
| `weekly_build_check` | 定时触发（周一至周五 18:00） | 周期性编译验证 |

#### Stage: `pages`

| 作业 | 触发条件 | 步骤 |
|------|----------|------|
| `pages` | 推送到 `main`/`master` 分支，包含源码或文档变更 | 1. 自动生成函数文档（流程图）<br>2. 运行单元测试 + 覆盖率报告<br>3. 运行集成测试 + 覆盖率报告<br>4. 运行 MISRA C:2012 静态分析<br>5. 运行 Lizard 复杂度分析<br>6. 生成代码审查条目<br>7. 构建 MkDocs 站点 + PDF |

文档站点自动部署到 **GitLab Pages**。

### 代码质量与标准

#### MISRA C:2012 合规

通过 **cppcheck** 配合 MISRA C:2012 插件进行静态分析。偏离/抑制策略以内联注释方式记录：

- `<stdio.h>` 和 `<stdarg.h>` 针对 CLI 调试输出做了显式抑制 (R21.6, R17.1)
- 每次代码审查均维护 MISRA 规则检查清单

#### 代码审查流程

- 每次 `src/` 变更时，`tools/gen_code_review.py` 检测修改文件并创建新的审查条目
- 问题逐项追踪，状态包括：`pending`、`fixed (YYYY-MM-DD)` 或 `❌ ignored`
- 审查历史保存在 `docs/code_review_record/code_review.md`

#### 复杂度分析

**Lizard** 生成每个函数的圈复杂度、NLOC 和参数数量报告，发布在 SWE.5 文档下。

### 文档系统

#### 本地预览

```bash
pip install mkdocs mkdocs-material mkdocs-glightbox mkdocs-include-markdown mkdocs-with-pdf

# 启动本地开发服务器
mkdocs serve
```

在浏览器中打开 http://127.0.0.1:8000。

#### 构建静态站点

```bash
mkdocs build --site-dir public
```

合并版 PDF（`IRQ_Simulator_ASPICE_Documentation.pdf`）通过 `mkdocs-with-pdf` 插件自动生成。

### 构建目标

| 目标 | 源文件 | 说明 |
|------|--------|------|
| `trae_test` | `src/main.c` + `src/start.s` + `src/make_misra_error.c` | 主 CLI 可执行文件 |
| `unit_test` | `unit_test/unit_test.c` + `src/main.c` | 单元测试可执行文件 (定义 `TEST_BUILD`) |
| `integrated_test` | `integration_test/integrated_test.c` + `src/main.c` | 集成测试可执行文件 (定义 `TEST_BUILD`) |

`TEST_BUILD` 宏控制符号可见性：
- **生产构建**：`FW_STATIC` 展开为 `static`（MISRA R8.7）
- **测试构建**：`FW_STATIC` 展开为空，将内部符号暴露给测试框架

### 许可证

Copyright (c) 2026 OmniVision Technologies, Inc. 保留所有权利。

[↑ 回顶部](#简体中文)

---

[↑ 回到頁首](#top)

<a id="繁體中文"></a>

## 繁體中文

**IRQ 模擬器**（嵌入式韌體中斷請求模擬器），遵循 **ASPICE V-Model** 軟體開發流程。本專案在主機 PC 上模擬 32 通道中斷控制器，讓開發者可以在部署到目標硬體之前，透過命令列介面觸發、處理和驗證 IRQ 行為。

### 專案概覽

| 項目 | 詳情 |
|------|------|
| **開發語言** | C11 (ANSI C) |
| **建構系統** | CMake 3.10+ + Ninja |
| **CI/CD** | GitLab CI/CD (Docker: `atb-compiler-env:v1`) |
| **文件系統** | MkDocs Material + PDF (英文 / 簡體中文 / 繁體中文) |
| **程式碼品質** | MISRA C:2012 (cppcheck)、Lizard 迴圈複雜度分析 |
| **測試體系** | 單元測試 + 整合測試，gcovr HTML 覆蓋率報告 |
| **程式碼審查** | 基於 `tools/gen_code_review.py` 的逐項自動追蹤 |

#### ASPICE V-Model 階段

| 階段 | 目錄 | 交付物 |
|------|------|--------|
| **SWE.1** 需求分析 | `docs/01_software_requirements/` | IRQ 模擬器需求規格說明書 |
| **SWE.2** 架構設計 | `docs/02_software_architecture/` | 軟體架構設計文件 |
| **SWE.3** 詳細設計 | `docs/03_software_detailed_design/` | 函式級設計 + 自動產生流程圖 |
| **SWE.4** 單元驗證 | `docs/04_software_unit_verification/` | 單元測試計畫 + 覆蓋率報告 |
| **SWE.5** 整合測試 | `docs/05_software_integration_test/` | 整合測試計畫 + MISRA + 複雜度報告 |

### 專案結構

```
├── src/                          # C 原始碼
│   ├── main.c                    # IRQ 模擬器核心邏輯 + CLI 主迴圈
│   ├── start.s                   # 組合語言啟動程式 & 中斷向量樁
│   └── make_misra_error.c        # 刻意違反 MISRA 的展示檔案
├── inc/
│   └── main.h                    # 公開 API、型別定義、測試可見性巨集
├── unit_test/
│   ├── unit_test.c               # 所有核心函式的單元測試 (UT_01 ~ UT_07)
│   └── unit_test.h               # 單元測試斷言巨集
├── integration_test/
│   ├── integrated_test.c         # 端到端整合測試 (IT_01 ~ IT_07)
│   └── integrated_test.h         # 整合測試斷言巨集
├── tools/
│   ├── gen_func_docs.py          # 自動產生函式文件 + Mermaid 流程圖
│   ├── gen_coverage_report.py    # 建構、執行單元測試，產生 gcovr HTML 覆蓋率
│   ├── gen_integration_coverage_report.py  # 建構、執行整合測試，產生覆蓋率
│   ├── gen_misra_report.py       # 執行 cppcheck + MISRA 附加元件，產生報告
│   ├── gen_code_review.py        # 偵測 src/ 變更，管理逐項審查紀錄
│   ├── run_lizard.py             # 產生迴圈複雜度 / NLOC 報告
│   └── ninja/                    # 預編譯 Ninja 二進位檔 (linux, win, mac, arm64)
├── docs/                         # MkDocs 文件 (ASPICE V-Model 結構)
│   ├── index.md                  # 文件首頁 (英文)
│   ├── 01_software_requirements/ # SWE.1: 需求規格 (英文/簡體/繁體)
│   ├── 02_software_architecture/ # SWE.2: 架構文件 (英文/簡體/繁體)
│   ├── 03_software_detailed_design/ # SWE.3: 詳細設計 + 自動產生函式文件
│   ├── 04_software_unit_verification/ # SWE.4: 單元測試計畫 + 覆蓋率報告
│   ├── 05_software_integration_test/  # SWE.5: 整合測試 + MISRA + lizard 報告
│   └── code_review_record/       # 逐項程式碼審查歷史
├── CMakeLists.txt                # CMake 建構配置 (3 個目標)
├── mkdocs.yml                    # MkDocs Material 主題 + PDF 外掛配置
├── .gitlab-ci.yml                # GitLab CI/CD 管線定義
└── README.md                     # 本文件
```

### 建構與執行

#### 環境需求

- **CMake** 3.10+
- **Ninja** 建構系統 (`tools/ninja/` 中已包含預編譯二進位檔)
- **GCC** 或相容的 C11 編譯器

#### 建構

```bash
# 使用 Ninja 產生器配置
cmake -B build -G Ninja

# 建構主程式
cmake --build build --target trae_test

# 建構單元測試
cmake --build build --target unit_test

# 建構整合測試
cmake --build build --target integrated_test
```

#### 執行

```bash
# 啟動 IRQ 模擬器 CLI
./build/trae_test
```

#### 本地執行測試

```bash
# 單元測試
cmake -B build -G Ninja -DENABLE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build --target unit_test
./build/unit_test

# 整合測試
cmake --build build --target integrated_test
./build/integrated_test

# 產生 HTML 覆蓋率報告
python tools/gen_coverage_report.py
```

### IRQ 模擬器 CLI

模擬器提供互動式命令列介面，用於觸發和處理中斷。

```
========== IRQ Simulator ==========
  <num>   : 觸發單一 IRQ (1-32 -> IRQ0-IRQ31)
  b<num>  : 位元模式，觸發單一 IRQ (b1 -> IRQ1)
  h<hex>  : 十六進位模式，觸發多個 IRQ (h3 -> IRQ0,1)
  0       : 手動處理所有掛起的 IRQ
  exit    : 退出模擬器
===================================
```

#### 輸入模式

| 模式 | 語法 | 範例 | 說明 |
|------|------|------|------|
| 數字模式 | `1` ~ `32` | `1` → 觸發 IRQ0 | 輸入值減 1 映射到 IRQ 編號 |
| 位元模式 | `b0` ~ `b31` | `b5` → 觸發 IRQ5 | 直接指定 IRQ 編號 |
| 十六進位模式 | `h01` ~ `hFFFFFFFF` | `h3` → 觸發 IRQ0 + IRQ1 | 原始位元遮罩寫入掛起暫存器 |
| 處理模式 | `0` | `0` | 手動處理目前所有掛起的 IRQ |

#### IRQ 通道對照表（32 通道）

| IRQ | 週邊裝置 | IRQ | 週邊裝置 |
|-----|---------|-----|---------|
| 0 | 系統計時器 (tick++) | 16 | 計時器2 (溢位) |
| 1 | UART0 接收 | 17 | UART1 接收 |
| 2 | UART0 傳送 | 18 | UART1 傳送 |
| 3 | GPIO 埠 A | 19 | SPI1 |
| 4 | GPIO 埠 B | 20 | I2C1 |
| 5 | SPI0 | 21 | 外部中斷 INT0 |
| 6 | I2C0 | 22 | 外部中斷 INT1 |
| 7 | ADC | 23 | 外部中斷 INT2 |
| 8 | DMA 通道0 | 24 | DMA 通道2 |
| 9 | DMA 通道1 | 25 | DMA 通道3 |
| 10 | 看門狗 | 26 | CRC |
| 11 | RTC | 27 | AES |
| 12 | USB | 28 | QSPI |
| 13 | CAN0 | 29 | SDIO |
| 14 | PWM | 30 | 乙太網路 |
| 15 | 計時器1 | 31 | 異常中斷 |

### GitLab CI/CD 管線

管線執行於 `atb-compiler-env:v1` Docker 映像檔中，分為兩個階段：

#### Stage: `build`

| 作業 | 觸發條件 | 說明 |
|------|----------|------|
| `build_on_commit` | 推送包含 `.c`/`.h`/`CMakeLists.txt` 變更 | 編譯 `trae_test` 執行檔 |
| `weekly_build_check` | 排程觸發（週一至週五 18:00） | 週期性編譯驗證 |

#### Stage: `pages`

| 作業 | 觸發條件 | 步驟 |
|------|----------|------|
| `pages` | 推送到 `main`/`master` 分支，包含原始碼或文件變更 | 1. 自動產生函式文件（流程圖）<br>2. 執行單元測試 + 覆蓋率報告<br>3. 執行整合測試 + 覆蓋率報告<br>4. 執行 MISRA C:2012 靜態分析<br>5. 執行 Lizard 複雜度分析<br>6. 產生程式碼審查條目<br>7. 建構 MkDocs 站台 + PDF |

文件站台自動部署到 **GitLab Pages**。

### 程式碼品質與標準

#### MISRA C:2012 合規

透過 **cppcheck** 搭配 MISRA C:2012 附加元件進行靜態分析。偏離／抑制策略以內聯註解方式記錄：

- `<stdio.h>` 和 `<stdarg.h>` 針對 CLI 除錯輸出做了顯式抑制 (R21.6, R17.1)
- 每次程式碼審查均維護 MISRA 規則檢查清單

#### 程式碼審查流程

- 每次 `src/` 變更時，`tools/gen_code_review.py` 偵測修改檔案並建立新的審查條目
- 問題逐項追蹤，狀態包括：`pending`、`fixed (YYYY-MM-DD)` 或 `❌ ignored`
- 審查歷史儲存於 `docs/code_review_record/code_review.md`

#### 複雜度分析

**Lizard** 產生每個函式的迴圈複雜度、NLOC 和參數數量報告，發布於 SWE.5 文件下。

### 文件系統

#### 本地預覽

```bash
pip install mkdocs mkdocs-material mkdocs-glightbox mkdocs-include-markdown mkdocs-with-pdf

# 啟動本地開發伺服器
mkdocs serve
```

在瀏覽器中開啟 http://127.0.0.1:8000。

#### 建構靜態站台

```bash
mkdocs build --site-dir public
```

合併版 PDF（`IRQ_Simulator_ASPICE_Documentation.pdf`）透過 `mkdocs-with-pdf` 外掛自動產生。

### 建構目標

| 目標 | 來源檔案 | 說明 |
|------|----------|------|
| `trae_test` | `src/main.c` + `src/start.s` + `src/make_misra_error.c` | 主 CLI 執行檔 |
| `unit_test` | `unit_test/unit_test.c` + `src/main.c` | 單元測試執行檔 (定義 `TEST_BUILD`) |
| `integrated_test` | `integration_test/integrated_test.c` + `src/main.c` | 整合測試執行檔 (定義 `TEST_BUILD`) |

`TEST_BUILD` 巨集控制符號可見性：
- **生產建構**：`FW_STATIC` 展開為 `static`（MISRA R8.7）
- **測試建構**：`FW_STATIC` 展開為空，將內部符號暴露給測試框架

### 授權條款

Copyright (c) 2026 OmniVision Technologies, Inc. 保留所有權利。

[↑ 回頂部](#繁體中文)
