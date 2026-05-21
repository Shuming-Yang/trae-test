# 工作流：標準互動式 Code Review (General)

本工作流適用於一般專案的大範圍掃描。請嚴格執行「分階段處理」策略，主動與使用者同步進度。

## 運作準則與步驟

### Step 0: 回溯檢查 (Context Retrieval)
1. 讀取專案中的 `code_review.md`（若不存在，提示使用者提供或由本步驟結束後建立）。
2. 識別所有狀態為 `pending` 或 `❌ ignored` 的歷史 Issue。
3. 比對原始碼：
   - **已修正**：若程式碼已符合最佳實踐，標記為 `**fixed**` 並註記日期。
   - **未修正**：列出該 Issue，並詢問使用者是否「維持 pending」、「改為 ignored」或「忽略此次提醒」。

### 大規模掃描規範（如審查多個資料夾或 src/、inc/）
- **拆分任務**：先建立一個「審查清單 (Review Roadmap)」。
- **互動式進度**：每審查完一個邏輯模組（例如一個資料夾），請輸出該階段的 Summary，並詢問使用者是否繼續。
- **跳過已掃描內容**：若該檔案在 `code_review.md` 中且狀態為 `fixed` 或 `ignored`，則自動跳過，僅分析未紀錄的檔案。

### 核心審查三步驟
- **Step 1: 靜態分析**：執行語法掃描與邏輯檢查，檢測未處理的 Exception、潛在記憶體洩漏或資源未釋放問題。
- **Step 2: 資安與權限掃描**：評估是否存在資安漏洞（如：不安全的 API 使用、明文傳輸、輸入驗證不足、未符合 MISRA 規範等）。
- **Step 3: 效能評估**：識別效能瓶頸（如 $O(N^2)$ 迴圈、過度的 API 呼叫、資源鎖競爭）。

### Step 4: 建議與重構
針對發現的 Issue，提供精確的重構對比：
- **問題清單**：列出有問題的行數、Severity (Critical/Warning/Info) 與原因。
- **優化對比**：提供 `Before` vs `After` 的程式碼區塊。
- **效益說明**：說明優化後預期的具體好處（如：防止緩衝區溢位、提升可維護性）。

### Step 5: 紀錄維護 (輸出格式)
審查完成或階段暫停時，輸出以下 Markdown 表格供使用者手動更新：
`# | Severity | File | Line(s) | Category | Issue | Status | Fix Date`

---

## 應用範例

若使用者要求 Review `src/` 與 `inc/`，輸出範例如下：

```text
正在執行 Step 0: 回溯檢查...
偵測到歷史 Issue #12 仍未修復。請問是否將該 Issue 改為 ❌ ignored 或維持 pending？
```

```c
// Before
strcpy(dest, src);

// After
strncpy(dest, src, sizeof(dest) - 1);
dest[sizeof(dest) - 1] = '\0';
```
