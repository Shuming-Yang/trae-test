# IRQ Simulator - ASPICE Software Development Process

## ASPICE V-Model Overview

本项目遵循 **ASPICE (Automotive SPICE)** 软件开发流程，采用 **V 形图 (V-Model)** 结构来组织软件开发生命周期中的各阶段产出文件。

```mermaid
flowchart TB
    subgraph LEFT["左侧：需求与设计 (Requirements & Design)"]
        direction TB
        SWE1["SWE.1<br/>软件需求分析<br/>Software Requirements Analysis"]
        SWE2["SWE.2<br/>软件架构设计<br/>Software Architectural Design"]
        SWE3["SWE.3<br/>软件详细设计<br/>Software Detailed Design"]
    end

    subgraph BOTTOM["底部：实现"]
        SWE4_IMPL["SWE.4<br/>软件单元实现<br/>Software Unit Construction"]
    end

    subgraph RIGHT["右侧：验证与测试 (Verification & Validation)"]
        direction BT
        SWE4_TEST["SWE.4<br/>软件单元验证<br/>Software Unit Verification"]
        SWE5["SWE.5<br/>软件集成与集成测试<br/>Software Integration Test"]
    end

    SWE1 --> SWE2
    SWE2 --> SWE3
    SWE3 --> SWE4_IMPL
    SWE4_IMPL --> SWE4_TEST
    SWE4_TEST --> SWE5
    SWE5 -.->|"追溯验证"| SWE1

    style SWE1 fill:#4A90D9,color:#fff
    style SWE2 fill:#4A90D9,color:#fff
    style SWE3 fill:#4A90D9,color:#fff
    style SWE4_IMPL fill:#7B68EE,color:#fff
    style SWE4_TEST fill:#50B86C,color:#fff
    style SWE5 fill:#50B86C,color:#fff
```

### V 形图说明

| 阶段 | 方向 | 说明 |
|------|------|------|
| **左侧 (下降)** | 需求 → 设计 | 从高层次需求逐步细化到详细设计，每个阶段产出对应规格文件 |
| **底部** | 实现 | 根据设计文件进行代码开发 |
| **右侧 (上升)** | 测试 → 验证 | 从单元测试逐步集成到系统测试，每个阶段验证对应的左侧规格 |
| **追溯性** | 水平对应 | 右侧测试用例须追溯至左侧对应层级的需求规格 |

---

## 文件对照表

| ASPICE 阶段 | 文件夹 | 文件 | 语言版本 |
|-------------|--------|------|----------|
| **SWE.1** 软件需求分析 | 01_software_requirements | IRQ Simulator 需求规格 | [EN](01_software_requirements/requirement_en.md) \| [CN](01_software_requirements/requirement_cn.md) \| [TW](01_software_requirements/requirement_tw.md) |
| **SWE.2** 软件架构设计 | 02_software_architecture | IRQ Simulator 软件架构 | [EN](02_software_architecture/software_architecture_en.md) \| [CN](02_software_architecture/software_architecture_cn.md) \| [TW](02_software_architecture/software_architecture_tw.md) |
| **SWE.3** 软件详细设计 | 03_software_detailed_design | IRQ Simulator 软件设计 | [EN](03_software_detailed_design/software_design_en.md) \| [CN](03_software_detailed_design/software_design_cn.md) \| [TW](03_software_detailed_design/software_design_tw.md) |
| **SWE.4** 软件单元验证 | 04_software_unit_verification | IRQ Simulator 单元测试计划 | [EN](04_software_unit_verification/unit_test_en.md) \| [CN](04_software_unit_verification/unit_test_cn.md) \| [TW](04_software_unit_verification/unit_test_tw.md) |
| **SWE.5** 软件集成测试 | 05_software_integration_test | IRQ Simulator 集成测试计划 | [EN](05_software_integration_test/integrated_test_en.md) \| [CN](05_software_integration_test/integrated_test_cn.md) \| [TW](05_software_integration_test/integrated_test_tw.md) |

---

## 追溯性矩阵 (Traceability Matrix)

```mermaid
flowchart LR
    subgraph REQ["SWE.1 需求"]
        FR01["FR-01: IRQ 触发机制"]
        FR02["FR-02: 输入模式"]
        FR03["FR-03: IRQ 优先权"]
        FR04["FR-04: IRQ 处理行为"]
        FR05["FR-05: Tick 计数器"]
        FR06["FR-06: 程序控制"]
    end

    subgraph ARCH["SWE.2 架构"]
        AM1["模块分解"]
        AM2["函数调用图"]
        AM3["数据流"]
    end

    subgraph DES["SWE.3 设计"]
        DD1["接口设计"]
        DD2["算法设计"]
        DD3["数据结构"]
    end

    subgraph UT["SWE.4 单元测试"]
        UT01["UT-01~07"]
    end

    subgraph IT["SWE.5 集成测试"]
        IT01["IT-01~07"]
    end

    REQ --> ARCH
    ARCH --> DES
    DES --> UT
    DES --> IT
    UT -.-> REQ
    IT -.-> REQ
```

---

## 目录结构

```
docs/
├── index_tw.md                          ← 繁体中文首页
├── index_cn.md                          ← 本文件 (简体中文首页)
├── index_en.md                          ← 英文首页
├── 01_software_requirements/            ← SWE.1 软件需求分析
│   ├── requirement_en.md
│   ├── requirement_cn.md
│   └── requirement_tw.md
├── 02_software_architecture/            ← SWE.2 软件架构设计
│   ├── software_architecture_en.md
│   ├── software_architecture_cn.md
│   └── software_architecture_tw.md
├── 03_software_detailed_design/         ← SWE.3 软件详细设计
│   ├── software_design_en.md
│   ├── software_design_cn.md
│   └── software_design_tw.md
├── 04_software_unit_verification/       ← SWE.4 软件单元验证
│   ├── unit_test_en.md
│   ├── unit_test_cn.md
│   └── unit_test_tw.md
└── 05_software_integration_test/        ← SWE.5 软件集成测试
    ├── integrated_test_en.md
    ├── integrated_test_cn.md
    └── integrated_test_tw.md
```