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

#### English
```
├── .clinerules                   # Cline System Instructions for project context, rules, skills, and workflow
├── .gitattributes                # Defines attributes per path, mainly for Git LFS and line ending normalization
├── .gitignore                    # Specifies intentionally untracked files to ignore
├── .gitlab-ci.yml                # GitLab CI/CD pipeline definition for automated builds, tests, and documentation deployment
├── .vscodeignore                 # Specifies files/folders to ignore in VS Code extensions
├── AGENTS.md                     # Defines advanced AI collaborator roles, technical stacks, and working principles
├── CMakeLists.txt                # CMake build configuration for main executable, unit tests, and integration tests
├── mkdocs.yml                    # MkDocs configuration for documentation site generation (Material theme, PDF plugin)
├── README.md                     # This file (project overview, build instructions, CLI usage, and documentation links)
├── requirements-mcp.txt          # List of Python packages required for MCP servers
├── docs/                         # MkDocs documentation (ASPICE V-Model structure) - contains project specifications, designs, and reports
│   ├── index.md                  # Documentation homepage (English)
│   ├── index_cn.md               # Documentation homepage (Simplified Chinese)
│   ├── index_tw.md               # Documentation homepage (Traditional Chinese)
│   ├── 01_software_requirements/ # SWE.1: Software Requirement Specification documents (EN/CN/TW)
│   │   ├── requirement_cn.md     # Simplified Chinese version of requirements
│   │   ├── requirement_en.md     # English version of requirements
│   │   └── requirement_tw.md     # Traditional Chinese version of requirements
│   ├── 02_software_architecture/ # SWE.2: Software Architecture Design documents (EN/CN/TW) with various versions
│   │   ├── software_architecture_cn_cline.md # Simplified Chinese architecture document (Cline generated)
│   │   ├── software_architecture_cn_gemini.md # Simplified Chinese architecture document (Gemini generated)
│   │   ├── software_architecture_cn.md # Simplified Chinese architecture document
│   │   ├── software_architecture_en_cline.md # English architecture document (Cline generated)
│   │   ├── software_architecture_en_gemini.md # English architecture document (Gemini generated)
│   │   ├── software_architecture_en.md # English architecture document
│   │   ├── software_architecture_tw_cline.md # Traditional Chinese architecture document (Cline generated)
│   │   ├── software_architecture_tw_gemini.md # Traditional Chinese architecture document (Gemini generated)
│   │   └── software_architecture_tw.md # Traditional Chinese architecture document
│   ├── 03_software_detailed_design/ # SWE.3: Software Detailed Design documents (EN/CN/TW) and auto-generated function docs
│   │   ├── software_design_cn_cline.md # Simplified Chinese detailed design (Cline generated)
│   │   ├── software_design_cn_gemini.md # Simplified Chinese detailed design (Gemini generated)
│   │   ├── software_design_cn.md # Simplified Chinese detailed design
│   │   ├── software_design_en_cline.md # English detailed design (Cline generated)
│   │   ├── software_design_en_gemini.md # English detailed design (Gemini generated)
│   │   ├── software_design_en.md # English detailed design
│   │   ├── software_design_tw_cline.md # Traditional Chinese detailed design (Cline generated)
│   │   ├── software_design_tw_gemini.md # Traditional Chinese detailed design (Gemini generated)
│   │   ├── software_design_tw.md # Traditional Chinese detailed design
│   │   └── auto_generated/       # Auto-generated function documentation and flowcharts
│   │       ├── func_main.md      # Auto-generated documentation for main.c functions
│   │       └── index.md          # Index for auto-generated function documentation
│   ├── 04_software_unit_verification/ # SWE.4: Unit Verification documents, test plans, and coverage reports
│   │   ├── coverage_report.md    # Markdown file for unit test coverage report
│   │   ├── unit_test_cn_cline.md # Simplified Chinese unit test plan (Cline generated)
│   │   ├── unit_test_cn_gemini.md # Simplified Chinese unit test plan (Gemini generated)
│   │   ├── unit_test_cn.md       # Simplified Chinese unit test plan
│   │   ├── unit_test_en_cline.md # English unit test plan (Cline generated)
│   │   ├── unit_test_en_gemini.md # English unit test plan (Gemini generated)
│   │   ├── unit_test_en.md       # English unit test plan
│   │   ├── unit_test_tw_cline.md # Traditional Chinese unit test plan (Cline generated)
│   │   ├── unit_test_tw_gemini.md # Traditional Chinese unit test plan (Gemini generated)
│   │   └── unit_test_tw.md       # Traditional Chinese unit test plan
│   ├── 05_software_integration_test/  # SWE.5: Integration Test documents, test plans, MISRA, and complexity reports
│   │   ├── coverage_report.md    # Markdown file for integration test coverage report
│   │   ├── integrated_test_cn_cline.md # Simplified Chinese integration test plan (Cline generated)
│   │   ├── integrated_test_cn_gemini.md # Simplified Chinese integration test plan (Gemini generated)
│   │   ├── integrated_test_cn.md # Simplified Chinese integration test plan
│   │   ├── integrated_test_en_cline.md # English integration test plan (Cline generated)
│   │   ├── integrated_test_en_gemini.md # English integration test plan (Gemini generated)
│   │   ├── integrated_test_en.md # English integration test plan
│   │   ├── integrated_test_tw_cline.md # Traditional Chinese integration test plan (Cline generated)
│   │   ├── integrated_test_tw_gemini.md # Traditional Chinese integration test plan (Gemini generated)
│   │   ├── integrated_test_tw.md # Traditional Chinese integration test plan
│   │   ├── lizard_report.md      # Report for cyclomatic complexity and NLOC analysis
│   │   └── misra_report.md       # Report for MISRA C static analysis
│   ├── code_review_record/       # Stores per-issue code review history
│   │   └── code_review.md        # Detailed code review records
│   ├── javascripts/              # JavaScript files for documentation site functionality
│   │   └── lang-switcher.js      # Script for language switching functionality
│   ├── stylesheets/              # CSS files for documentation site styling
│   │   └── extra.css             # Additional custom CSS styles
│   ├── misra/                    # MISRA C rule definitions or related files
│   │   └── MISRA_Rule.txt        # Text file containing MISRA rule descriptions
│   └── trae/                     # Placeholder directory (e.g., for project-specific assets)
│       └── .gitkeep              # Ensures the directory is tracked by Git even if empty
├── inc/                          # C header files
│   └── main.h                    # Public API, type definitions, and test visibility macros for the IRQ simulator
├── integration_test/             # Integration test source files
│   ├── integrated_test.c         # End-to-end integration tests (IT_01 ~ IT_07) for the IRQ simulator CLI
│   └── integrated_test.h         # Header file with integration test assertion macros
├── src/                          # C source code
│   ├── main.c                    # Core IRQ simulator logic, CLI main loop, and interrupt handling functions
│   ├── main.c.ctu-info           # Clang Tidy Unused file (tool-generated for static analysis)
│   ├── make_misra_error.c        # Intentionally MISRA-noncompliant demo file to showcase MISRA violations
│   └── start.s                   # Assembly startup code and interrupt vector stubs
├── tools/                        # Python automation tools and utilities
│   ├── gen_code_review.py        # Detects src/ changes and manages per-issue code review records
│   ├── gen_coverage_report.py    # Builds and runs unit tests, then generates gcovr HTML coverage reports
│   ├── gen_func_docs.py          # Auto-generates function documentation with Mermaid flowcharts from C source
│   ├── gen_integration_coverage_report.py  # Builds and runs integration tests, then generates coverage reports
│   ├── gen_misra_report.py       # Runs cppcheck with MISRA addon and generates MISRA C 2012 static analysis reports
│   ├── run_lizard.py             # Generates cyclomatic complexity / NLOC / parameter count reports using Lizard
│   └── ninja/                    # Directory containing prebuilt Ninja binaries for various platforms
│       ├── ninja_env.cmake       # CMake environment script for Ninja build system
│       ├── linux/                # Ninja binary for Linux
│       ├── linux-aarch64/        # Ninja binary for Linux (ARM64)
│       ├── mac/                  # Ninja binary for macOS
│       ├── win/                  # Ninja binary for Windows
│       └── winarm64/             # Ninja binary for Windows (ARM64)
└── unit_test/                    # Unit test source files
    ├── unit_test.c               # Unit tests for all core functions (UT_01 ~ UT_07) of the IRQ simulator
    └── unit_test.h               # Header file with unit test assertion macros
```

#### 简体中文
```
├── .clinerules                   # Cline 系统指令：包含项目背景、规则、技能和工作流程
├── .gitattributes                # 定义按路径划分的属性，主要用于 Git LFS 和行尾符标准化
├── .gitignore                    # 指定要忽略的未追踪文件
├── .gitlab-ci.yml                # GitLab CI/CD 流水线定义，用于自动化构建、测试和文档部署
├── .vscodeignore                 # 指定 VS Code 扩展中要忽略的文件/文件夹
├── AGENTS.md                     # 定义高级 AI 协作者角色、技术栈和工作原则
├── CMakeLists.txt                # CMake 构建配置，用于主可执行文件、单元测试和集成测试
├── mkdocs.yml                    # MkDocs 文档网站生成配置（Material 主题，PDF 插件）
├── README.md                     # 本文件（项目概览、构建说明、CLI 使用方法和文档链接）
├── requirements-mcp.txt          # MCP 服务器所需的 Python 包列表
├── docs/                         # MkDocs 文档（ASPICE V 形图结构）- 包含项目规范、设计和报告
│   ├── index.md                  # 文档首页（英文）
│   ├── index_cn.md               # 文档首页（简体中文）
│   ├── index_tw.md               # 文档首页（繁体中文）
│   ├── 01_software_requirements/ # SWE.1: 软件需求规格说明书文档（EN/CN/TW）
│   │   ├── requirement_cn.md     # 简体中文版需求
│   │   ├── requirement_en.md     # 英文版需求
│   │   └── requirement_tw.md     # 繁体中文版需求
│   ├── 02_software_architecture/ # SWE.2: 软件架构设计文档（EN/CN/TW），包含不同版本
│   │   ├── software_architecture_cn_cline.md # 简体中文架构文档（Cline 生成）
│   │   ├── software_architecture_cn_gemini.md # 简体中文架构文档（Gemini 生成）
│   │   ├── software_architecture_cn.md # 简体中文架构文档
│   │   ├── software_architecture_en_cline.md # 英文架构文档（Cline 生成）
│   │   ├── software_architecture_en_gemini.md # 英文架构文档（Gemini 生成）
│   │   ├── software_architecture_en.md # 英文架构文档
│   │   ├── software_architecture_tw_cline.md # 繁体中文架构文档（Cline 生成）
│   │   ├── software_architecture_tw_gemini.md # 繁体中文架构文档（Gemini 生成）
│   │   └── software_architecture_tw.md # 繁体中文架构文档
│   ├── 03_software_detailed_design/ # SWE.3: 软件详细设计文档（EN/CN/TW）和自动生成的函数文档
│   │   ├── software_design_cn_cline.md # 简体中文详细设计（Cline 生成）
│   │   ├── software_design_cn_gemini.md # 简体中文详细设计（Gemini 生成）
│   │   ├── software_design_cn.md # 简体中文详细设计
│   │   ├── software_design_en_cline.md # 英文详细设计（Cline 生成）
│   │   ├── software_design_en_gemini.md # 英文详细设计（Gemini 生成）
│   │   ├── software_design_en.md # 英文详细设计
│   │   ├── software_design_tw_cline.md # 繁体中文详细设计（Cline 生成）
│   │   ├── software_design_tw_gemini.md # 繁体中文详细设计（Gemini 生成）
│   │   ├── software_design_tw.md # 繁体中文详细设计
│   │   └── auto_generated/       # 自动生成的函数文档和流程图
│   │       ├── func_main.md      # main.c 函数的自动生成文档
│   │       └── index.md          # 自动生成的函数文档索引
│   ├── 04_software_unit_verification/ # SWE.4: 单元验证文档、测试计划和覆盖率报告
│   │   ├── coverage_report.md    # 单元测试覆盖率报告的 Markdown 文件
│   │   ├── unit_test_cn_cline.md # 简体中文单元测试计划（Cline 生成）
│   │   ├── unit_test_cn_gemini.md # 简体中文单元测试计划（Gemini 生成）
│   │   ├── unit_test_cn.md       # 简体中文单元测试计划
│   │   ├── unit_test_en_cline.md # 英文单元测试计划（Cline 生成）
│   │   ├── unit_test_en_gemini.md # 英文单元测试计划（Gemini 生成）
│   │   ├── unit_test_en.md       # 英文单元测试计划
│   │   ├── unit_test_tw_cline.md # 繁体中文单元测试计划（Cline 生成）
│   │   ├── unit_test_tw_gemini.md # 繁体中文单元测试计划（Gemini 生成）
│   │   └── unit_test_tw.md       # 繁体中文单元测试计划
│   ├── 05_software_integration_test/  # SWE.5: 集成测试文档、测试计划、MISRA 和复杂度报告
│   │   ├── coverage_report.md    # 集成测试覆盖率报告的 Markdown 文件
│   │   ├── integrated_test_cn_cline.md # 简体中文集成测试计划（Cline 生成）
│   │   ├── integrated_test_cn_gemini.md # 简体中文集成测试计划（Gemini 生成）
│   │   ├── integrated_test_cn.md # 简体中文集成测试计划
│   │   ├── integrated_test_en_cline.md # 英文集成测试计划（Cline 生成）
│   │   ├── integrated_test_en_gemini.md # 英文集成测试计划（Gemini 生成）
│   │   ├── integrated_test_en.md # 英文集成测试计划
│   │   ├── integrated_test_tw_cline.md # 繁体中文集成测试计划（Cline 生成）
│   │   ├── integrated_test_tw_gemini.md # 繁体中文集成测试计划（Gemini 生成）
│   │   ├── integrated_test_tw.md # 繁体中文集成测试计划
│   │   ├── lizard_report.md      # 圈复杂度与 NLOC 分析报告
│   │   └── misra_report.md       # MISRA C 静态分析报告
│   ├── code_review_record/       # 存储逐项代码审查历史
│   │   └── code_review.md        # 详细代码审查记录
│   ├── javascripts/              # 文档网站功能的 JavaScript 文件
│   │   └── lang-switcher.js      # 语言切换功能的脚本
│   ├── stylesheets/              # 文档网站样式的 CSS 文件
│   │   └── extra.css             # 额外的自定义 CSS 样式
│   ├── misra/                    # MISRA C 规则定义或相关文件
│   │   └── MISRA_Rule.txt        # 包含 MISRA 规则描述的文本文件
│   └── trae/                     # 占位符目录（例如，用于项目特定资产）
│       └── .gitkeep              # 确保即使为空目录也能被 Git 追踪
├── inc/                          # C 头文件
│   └── main.h                    # IRQ 模拟器的公共 API、类型定义和测试可见性宏
├── integration_test/             # 集成测试源文件
│   ├── integrated_test.c         # IRQ 模拟器 CLI 的端到端集成测试（IT_01 ~ IT_07）
│   └── integrated_test.h         # 包含集成测试断言宏的头文件
├── src/                          # C 源代码
│   ├── main.c                    # IRQ 模拟器核心逻辑、CLI 主循环和中断处理函数
│   ├── main.c.ctu-info           # Clang Tidy Unused 文件（工具生成的静态分析文件）
│   ├── make_misra_error.c        # 故意违反 MISRA 的演示文件，用于展示 MISRA 违规
│   └── start.s                   # 汇编启动代码和中断向量桩
├── tools/                        # Python 自动化工具和实用程序
│   ├── gen_code_review.py        # 检测 src/ 变更并管理逐项代码审查记录
│   ├── gen_coverage_report.py    # 构建和运行单元测试，然后生成 gcovr HTML 覆盖率报告
│   ├── gen_func_docs.py          # 从 C 源代码自动生成带 Mermaid 流程图的函数文档
│   ├── gen_integration_coverage_report.py  # 构建和运行集成测试，然后生成覆盖率报告
│   ├── gen_misra_report.py       # 运行 cppcheck 与 MISRA 附加组件，生成 MISRA C 2012 静态分析报告
│   ├── run_lizard.py             # 使用 Lizard 生成圈复杂度 / NLOC / 参数计数报告
│   └── ninja/                    # 包含适用于不同平台的预编译 Ninja 二进制文件的目录
│       ├── ninja_env.cmake       # Ninja 构建系统的 CMake 环境脚本
│       ├── linux/                # Linux 平台的 Ninja 二进制文件
│       ├── linux-aarch64/        # Linux (ARM64) 平台的 Ninja 二进制文件
│       ├── mac/                  # macOS 平台的 Ninja 二进制文件
│       ├── win/                  # Windows 平台的 Ninja 二进制文件
│       └── winarm64/             # Windows (ARM64) 平台的 Ninja 二进制文件
└── unit_test/                    # 单元测试源文件
    ├── unit_test.c               # IRQ 模拟器所有核心函数（UT_01 ~ UT_07）的单元测试
    └── unit_test.h               # 包含单元测试断言宏的头文件
```

#### 繁體中文
```
├── .clinerules                   # Cline 系統指令：包含專案背景、規則、技能和工作流程
├── .gitattributes                # 定義按路徑劃分的屬性，主要用於 Git LFS 和換行符標準化
├── .gitignore                    # 指定要忽略的未追蹤檔案
├── .gitlab-ci.yml                # GitLab CI/CD 管線定義，用於自動化建構、測試和文件部署
├── .vscodeignore                 # 指定 VS Code 擴充功能中要忽略的檔案/資料夾
├── AGENTS.md                     # 定義進階 AI 協作者角色、技術棧和工作原則
├── CMakeLists.txt                # CMake 建構配置，用於主執行檔、單元測試和整合測試
├── mkdocs.yml                    # MkDocs 文件網站生成配置（Material 主題，PDF 外掛）
├── README.md                     # 本檔案（專案概覽、建構說明、CLI 使用方法和文件連結）
├── requirements-mcp.txt          # MCP 伺服器所需的 Python 套件列表
├── docs/                         # MkDocs 文件（ASPICE V 形圖結構）- 包含專案規格、設計和報告
│   ├── index.md                  # 文件首頁（英文）
│   ├── index_cn.md               # 文件首頁（簡體中文）
│   ├── index_tw.md               # 文件首頁（繁體中文）
│   ├── 01_software_requirements/ # SWE.1: 軟體需求規格說明書文件（EN/CN/TW）
│   │   ├── requirement_cn.md     # 簡體中文版需求
│   │   ├── requirement_en.md     # 英文版需求
│   │   └── requirement_tw.md     # 繁體中文版需求
│   ├── 02_software_architecture/ # SWE.2: 軟體架構設計文件（EN/CN/TW），包含不同版本
│   │   ├── software_architecture_cn_cline.md # 簡體中文架構文件（Cline 生成）
│   │   ├── software_architecture_cn_gemini.md # 簡體中文架構文件（Gemini 生成）
│   │   ├── software_architecture_cn.md # 簡體中文架構文件
│   │   ├── software_architecture_en_cline.md # 英文架構文件（Cline 生成）
│   │   ├── software_architecture_en_gemini.md # 英文架構文件（Gemini 生成）
│   │   ├── software_architecture_en.md # 英文架構文件
│   │   ├── software_architecture_tw_cline.md # 繁體中文架構文件（Cline 生成）
│   │   ├── software_architecture_tw_gemini.md # 繁體中文架構文件（Gemini 生成）
│   │   └── software_architecture_tw.md # 繁體中文架構文件
│   ├── 03_software_detailed_design/ # SWE.3: 軟體細部設計文件（EN/CN/TW）和自動生成的函式文件
│   │   ├── software_design_cn_cline.md # 簡體中文細部設計（Cline 生成）
│   │   ├── software_design_cn_gemini.md # 簡體中文細部設計（Gemini 生成）
│   │   ├── software_design_cn.md # 簡體中文細部設計
│   │   ├── software_design_en_cline.md # 英文細部設計（Cline 生成）
│   │   ├── software_design_en_gemini.md # 英文細部設計（Gemini 生成）
│   │   ├── software_design_en.md # 英文細部設計
│   │   ├── software_design_tw_cline.md # 繁體中文細部設計（Cline 生成）
│   │   ├── software_design_tw_gemini.md # 繁體中文細部設計（Gemini 生成）
│   │   ├── software_design_tw.md # 繁體中文細部設計
│   │   └── auto_generated/       # 自動生成的函式文件和流程圖
│   │       ├── func_main.md      # main.c 函式的自動生成文件
│   │       └── index.md          # 自動生成的函式文件索引
│   ├── 04_software_unit_verification/ # SWE.4: 單元驗證文件、測試計畫和覆蓋率報告
│   │   ├── coverage_report.md    # 單元測試覆蓋率報告的 Markdown 檔案
│   │   ├── unit_test_cn_cline.md # 簡體中文單元測試計畫（Cline 生成）
│   │   ├── unit_test_cn_gemini.md # 簡體中文單元測試計畫（Gemini 生成）
│   │   ├── unit_test_cn.md       # 簡體中文單元測試計畫
│   │   ├── unit_test_en_cline.md # 英文單元測試計畫（Cline 生成）
│   │   ├── unit_test_en_gemini.md # 英文單元測試計畫（Gemini 生成）
│   │   ├── unit_test_en.md       # 英文單元測試計畫
│   │   ├── unit_test_tw_cline.md # 繁體中文單元測試計畫（Cline 生成）
│   │   ├── unit_test_tw_gemini.md # 繁體中文單元測試計畫（Gemini 生成）
│   │   └── unit_test_tw.md       # 繁體中文單元測試計畫
│   ├── 05_software_integration_test/  # SWE.5: 整合測試文件、測試計畫、MISRA 和複雜度報告
│   │   ├── coverage_report.md    # 整合測試覆蓋率報告的 Markdown 檔案
│   │   ├── integrated_test_cn_cline.md # 簡體中文整合測試計畫（Cline 生成）
│   │   ├── integrated_test_cn_gemini.md # 簡體中文整合測試計畫（Gemini 生成）
│   │   ├── integrated_test_cn.md # 簡體中文整合測試計畫
│   │   ├── integrated_test_en_cline.md # 英文整合測試計畫（Cline 生成）
│   │   ├── integrated_test_en_gemini.md # 英文整合測試計畫（Gemini 生成）
│   │   ├── integrated_test_en.md # 英文整合測試計畫
│   │   ├── integrated_test_tw_cline.md # 繁體中文整合測試計畫（Cline 生成）
│   │   ├── integrated_test_tw_gemini.md # 繁體中文整合測試計畫（Gemini 生成）
│   │   ├── integrated_test_tw.md # 繁體中文整合測試計畫
│   │   ├── lizard_report.md      # 迴圈複雜度與 NLOC 分析報告
│   │   └── misra_report.md       # MISRA C 靜態分析報告
│   ├── code_review_record/       # 儲存逐項程式碼審查歷史
│   │   └── code_review.md        # 詳細程式碼審查記錄
│   ├── javascripts/              # 文件網站功能的 JavaScript 檔案
│   │   └── lang-switcher.js      # 語言切換功能的腳本
│   ├── stylesheets/              # 文件網站樣式的 CSS 檔案
│   │   └── extra.css             # 額外的自訂 CSS 樣式
│   ├── misra/                    # MISRA C 規則定義或相關檔案
│   │   └── MISRA_Rule.txt        # 包含 MISRA 規則描述的文字檔案
│   └── trae/                     # 佔位符目錄（例如，用於專案特定資產）
│       └── .gitkeep              # 確保即使為空目錄也能被 Git 追蹤
├── inc/                          # C 標頭檔
│   └── main.h                    # IRQ 模擬器的公共 API、型別定義和測試可見性巨集
├── integration_test/             # 整合測試原始檔
│   ├── integrated_test.c         # IRQ 模擬器 CLI 的端到端整合測試（IT_01 ~ IT_07）
│   └── integrated_test.h         # 包含整合測試斷言巨集的標頭檔
├── src/                          # C 原始碼
│   ├── main.c                    # IRQ 模擬器核心邏輯、CLI 主迴圈和中斷處理函式
│   ├── main.c.ctu-info           # Clang Tidy Unused 檔案（工具生成的靜態分析檔案）
│   ├── make_misra_error.c        # 刻意違反 MISRA 的展示檔案，用於展示 MISRA 違規
│   └── start.s                   # 組合語言啟動程式和中斷向量樁
├── tools/                        # Python 自動化工具和實用程式
│   ├── gen_code_review.py        # 偵測 src/ 變更並管理逐項程式碼審查記錄
│   ├── gen_coverage_report.py    # 建構和執行單元測試，然後生成 gcovr HTML 覆蓋率報告
│   ├── gen_func_docs.py          # 從 C 原始碼自動生成帶 Mermaid 流程圖的函式文件
│   ├── gen_integration_coverage_report.py  # 建構和執行整合測試，然後生成覆蓋率報告
│   ├── gen_misra_report.py       # 執行 cppcheck 與 MISRA 附加元件，生成 MISRA C 2012 靜態分析報告
│   ├── run_lizard.py             # 使用 Lizard 生成迴圈複雜度 / NLOC / 參數計數報告
│   └── ninja/                    # 包含適用於不同平台的預編譯 Ninja 二進位檔案的目錄
│       ├── ninja_env.cmake       # Ninja 建構系統的 CMake 環境腳本
│       ├── linux/                # Linux 平台的 Ninja 二進位檔案
│       ├── linux-aarch64/        # Linux (ARM64) 平台的 Ninja 二進位檔案
│       ├── mac/                  # macOS 平台的 Ninja 二進位檔案
│       ├── win/                  # Windows 平台的 Ninja 二進位檔案
│       └── winarm64/             # Windows (ARM64) 平台的 Ninja 二進位檔案
└── unit_test/                    # 單元測試原始檔
    ├── unit_test.c               # IRQ 模擬器所有核心函式（UT_01 ~ UT_07）的單元測試
    └── unit_test.h               # 包含單元測試斷言巨集的標頭檔
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
