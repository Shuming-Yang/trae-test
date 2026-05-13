# TRAE 測試專案

## 專案介紹

這是一個使用 **TRAE IDE** 進行開發的測試專案。TRAE 是一個 AI 原生的整合開發環境，深度整合 AI 大語言模型，提供智慧問答、程式碼自動補全與 AI 自動程式設計能力。

## 專案目的

- 學習與熟悉 TRAE IDE 的各項功能
- 測試 AI 輔助開發的工作流程
- 建立 TRAE 專案開發的基礎範本

## 關於 TRAE IDE

TRAE（讀作 /treɪ/）是字節跳動推出的 AI 原生整合開發環境，主要特色包括：

- 🤖 **內建 AI 助手**：支援 Chat（問答）和 Builder（專案建構）兩種模式
- 🌐 **全中文介面**：無需安裝漢化外掛，開箱即用
- 🔌 **VS Code 生態相容**：可一鍵匯入外掛、快捷鍵、設定檔
- 🖥️ **多平台支援**：Windows 和 macOS
- 🆓 **完全免費**：無需付費訂閱即可使用 AI 功能

## 開始使用

### 環境需求

- Windows 10/11 或 macOS 10.15+
- 記憶體：建議 8GB 以上
- 磁碟空間：2GB 可用空間

### 安裝 TRAE IDE

1. 造訪 TRAE 官網：https://www.trae.com.cn/
2. 下載對應作業系統的安裝檔
3. 依照安裝精靈完成安裝
4. 使用手機號碼或稀土掘金帳號登入

### 開啟專案

```bash
# 在終端中使用 trae 指令開啟此專案
trae .
```

或者在 TRAE IDE 中選擇「檔案」→「開啟資料夾」，選擇此專案目錄。

## 專案結構

```
LOCAL_trae-test/
├── .gitlab-ci.yml     # GitLab CI/CD 設定
├── CMakeLists.txt     # CMake 建構設定
├── README.md          # 專案說明文件
├── generate_docs.py   # 自動文件產生腳本
├── main.c             # 主程式碼與中斷處理
├── main.h             # 主標頭檔案
└── start.s            # 組合語言啟動程式
```

## 建構與執行

### 使用 CMake 建構

```bash
# 建立建構目錄
mkdir build
cd build

# 設定 CMake 專案
cmake ..

# 編譯
cmake --build .

# 執行程式
./trae_test  # Linux/macOS
# 或
.\trae_test.exe  # Windows
```

### 開發時快速建構 (Windows)

```bash
cmake -B build
cmake --build build
.\build\trae_test.exe
```

## 開發模式

TRAE IDE 提供兩種主要的 AI 輔助開發模式：

### Chat 模式
適用於開發過程中的隨時問答，例如：
- 解釋程式碼的作用
- 尋找 Bug 並提供修復方案
- 詢問 API 使用方式
- 優化程式碼邏輯

### Builder 模式
適用於從零開始建構專案，只需用自然語言描述需求，AI 會自動：
- 建立專案目錄結構
- 生成所需檔案
- 執行終端指令
- 分析指令執行狀態

## 常用快捷鍵

| 功能 | Windows/Linux | macOS |
|------|---------------|-------|
| 開啟側邊對話 | Ctrl + U | Cmd + U |
| 內聯 AI 聊天 | Ctrl + I | Cmd + I |
| 切換終端 | Ctrl + ` | Cmd + ` |
| 開啟命令選擇區 | Ctrl + Shift + P | Cmd + Shift + P |
| 快速開啟檔案 | Ctrl + P | Cmd + P |

## GitLab CI/CD

本專案已設定 GitLab CI/CD 自動建構與文件產生：

### 自動編譯
- **提交時觸發**：當 `.c`、`.h` 或 `CMakeLists.txt` 變更時自動編譯
- **排程檢查**：每週一至週五傍晚 18:00 自動執行編譯檢查

### 設定 GitLab 排程
1. 進入 GitLab 專案 → **Build** → **Pipeline schedules**
2. 新增排程，Cron 語法：`0 18 * * 1-5`

## GitLab Pages 自動文件

本專案使用 `generate_docs.py` 自動掃描所有 `.c` 和 `.h` 檔案，並產生文件網站：

### 功能特色
- 🤖 **全自動**：無需手動維護 Markdown 檔案
- 📊 **函式表格**：自動產生函式定義總覽表格
- 📈 **Mermaid 圖表**：包含架構圖與流程圖
- 📄 **PDF 匯出**：提供完整文件 PDF 下載

### 文件網站
文件網站會自動部署到 GitLab Pages，網址形式：
```
https://<username>.gitlab.io/<project-name>/
```

### 本地測試文件產生
您也可以在本地測試文件產生：
```bash
# 安裝相依套件
pip install mkdocs mkdocs-material pymdown-extensions

# 執行文件產生腳本
python generate_docs.py

# 啟動本地預覽
mkdocs serve
```

### generate_docs.py 功能
- 自動掃描專案內所有 `.c` 和 `.h` 檔案（排除 build 目錄）
- 解析 Doxygen 註解格式（`@brief`、`@param`、`@retval` 等）
- 動態產生函式定義表格與流程圖
- 自動建立 mkdocs 設定檔

## 後續計畫

- [x] 建立專案基礎結構
- [x] 設定 GitLab CI/CD
- [x] 實作中斷處理函式
- [x] 建立自動文件產生系統
- [x] 設定 GitLab Pages
- [ ] 建立更多測試功能

## 相關資源

- [TRAE 官方網站](https://www.trae.com.cn/)
- [TRAE 教學文件](https://www.runoob.com/ai-agent/trae-quick-star.html)
- [GitLab 文件](https://docs.gitlab.com/)

## 授權

本專案僅供學習與測試使用。