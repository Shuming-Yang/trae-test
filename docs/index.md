# IRQ Simulator - ASPICE Software Development Process

## ASPICE V-Model Overview

This project follows the **ASPICE (Automotive SPICE)** software development process, organized using the **V-Model** structure to manage deliverables across the software development lifecycle.

```mermaid
flowchart TB
    subgraph LEFT["Left Side: Requirements & Design"]
        direction TB
        SWE1["SWE.1<br/>Software Requirements Analysis"]
        SWE2["SWE.2<br/>Software Architectural Design"]
        SWE3["SWE.3<br/>Software Detailed Design"]
    end

    subgraph BOTTOM["Bottom: Implementation"]
        SWE4_IMPL["SWE.4<br/>Software Unit Construction"]
    end

    subgraph RIGHT["Right Side: Verification & Validation"]
        direction BT
        SWE4_TEST["SWE.4<br/>Software Unit Verification"]
        SWE5["SWE.5<br/>Software Integration & Integration Test"]
    end

    SWE1 --> SWE2
    SWE2 --> SWE3
    SWE3 --> SWE4_IMPL
    SWE4_IMPL --> SWE4_TEST
    SWE4_TEST --> SWE5
    SWE5 -.->|"Traceability Verification"| SWE1

    style SWE1 fill:#4A90D9,color:#fff
    style SWE2 fill:#4A90D9,color:#fff
    style SWE3 fill:#4A90D9,color:#fff
    style SWE4_IMPL fill:#7B68EE,color:#fff
    style SWE4_TEST fill:#50B86C,color:#fff
    style SWE5 fill:#50B86C,color:#fff
```

### V-Model Description

| Side | Direction | Description |
|------|------|------|
| **Left (Descending)** | Requirements → Design | Progressive refinement from high-level requirements to detailed design, with specification documents at each stage |
| **Bottom** | Implementation | Code development based on design documents |
| **Right (Ascending)** | Testing → Verification | Progressive integration from unit testing to system testing, each stage verifying the corresponding left-side specification |
| **Traceability** | Horizontal Mapping | Right-side test cases must trace back to the corresponding left-side requirement level |

---

## Document Mapping

| ASPICE Phase | Folder | Document | Language Versions |
|-------------|--------|------|----------|
| **SWE.1** Software Requirements Analysis | 01_software_requirements | IRQ Simulator Requirement Specification | [EN](01_software_requirements/requirement_en.md) \| [CN](01_software_requirements/requirement_cn.md) \| [TW](01_software_requirements/requirement_tw.md) |
| **SWE.2** Software Architectural Design | 02_software_architecture | IRQ Simulator Software Architecture | [EN](02_software_architecture/software_architecture_en.md) \| [CN](02_software_architecture/software_architecture_cn.md) \| [TW](02_software_architecture/software_architecture_tw.md) |
| **SWE.3** Software Detailed Design | 03_software_detailed_design | IRQ Simulator Software Design | [EN](03_software_detailed_design/software_design_en.md) \| [CN](03_software_detailed_design/software_design_cn.md) \| [TW](03_software_detailed_design/software_design_tw.md) |
| **SWE.4** Software Unit Verification | 04_software_unit_verification | IRQ Simulator Unit Test Plan | [EN](04_software_unit_verification/unit_test_en.md) \| [CN](04_software_unit_verification/unit_test_cn.md) \| [TW](04_software_unit_verification/unit_test_tw.md) |
| **SWE.5** Software Integration Test | 05_software_integration_test | IRQ Simulator Integration Test Plan | [EN](05_software_integration_test/integrated_test_en.md) \| [CN](05_software_integration_test/integrated_test_cn.md) \| [TW](05_software_integration_test/integrated_test_tw.md) |

---

## Traceability Matrix

```mermaid
flowchart TB
    subgraph REQ["SWE.1 Requirements"]
        FR01["FR-01: IRQ Trigger Mechanism"]
        FR02["FR-02: Input Modes"]
        FR03["FR-03: IRQ Priority"]
        FR04["FR-04: IRQ Handler Behaviors"]
        FR05["FR-05: Tick Counter"]
        FR06["FR-06: Program Control"]
    end

    subgraph ARCH["SWE.2 Architecture"]
        AM1["Module Decomposition"]
        AM2["Function Call Graph"]
        AM3["Data Flow"]
    end

    subgraph DES["SWE.3 Design"]
        DD1["Interface Design"]
        DD2["Algorithm Design"]
        DD3["Data Structures"]
    end

    subgraph UT["SWE.4 Unit Test"]
        UT01["UT-01~07"]
    end

    subgraph IT["SWE.5 Integration Test"]
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

## Directory Structure

```
docs/
├── index_tw.md                          ← Traditional Chinese Homepage
├── index_cn.md                          ← Simplified Chinese Homepage
├── index_en.md                          ← This file (English Homepage)
├── 01_software_requirements/            ← SWE.1 Software Requirements Analysis
│   ├── requirement_en.md
│   ├── requirement_cn.md
│   └── requirement_tw.md
├── 02_software_architecture/            ← SWE.2 Software Architectural Design
│   ├── software_architecture_en.md
│   ├── software_architecture_cn.md
│   └── software_architecture_tw.md
├── 03_software_detailed_design/         ← SWE.3 Software Detailed Design
│   ├── software_design_en.md
│   ├── software_design_cn.md
│   └── software_design_tw.md
├── 04_software_unit_verification/       ← SWE.4 Software Unit Verification
│   ├── unit_test_en.md
│   ├── unit_test_cn.md
│   └── unit_test_tw.md
└── 05_software_integration_test/        ← SWE.5 Software Integration Test
    ├── integrated_test_en.md
    ├── integrated_test_cn.md
    └── integrated_test_tw.md
```