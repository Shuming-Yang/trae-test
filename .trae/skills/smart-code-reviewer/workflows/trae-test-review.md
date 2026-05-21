# 工作流：自動化工具鏈 Code Review (trae_test 專用)

本工作流專用於 `trae_test` 專案，結合本地的 Python 自動化指令進行紀錄的讀取與維護。

## 運作準則與步驟

### Step 0: 回溯比對舊 Issue（強制優先執行）
1. 讀取 `docs/code_review_record/code_review.md`。
2. 找出最近一筆 review entry 中所有狀態為 `pending` 或 `❌ ignored` 的 issue。
3. 對每一個 issue 讀取其 `File` 與 `Line(s)` 並比對當前原始碼：
   - **若已修正** ➔ **主動執行本地終端機指令**：`python tools/gen_code_review.py --fix-issue N` 自動將狀態改為 `**fixed**`。
   - **若尚未修正** ➔ 逐個詢問使用者：「Issue #N 仍未修復，是否維持 `pending` 或改為 `❌ ignored`？」
   - **若先前為 `❌ ignored` 但已被修正** ➔ 執行指令改為 `**fixed**`。
4. 回溯比對確認完成後，方可進入 Step 1。

### 核心審查流程
- **Step 1: 靜態分析**：檢查程式碼是否存在語法錯誤、未處理的異常（Exception）或潛在的記憶體洩漏。
- **Step 2: 資安漏洞掃描**：評估是否有明文傳輸、權限控管不當或未符合 MISRA C 規範的問題。
- **Step 3: 效能評估**：檢查是否有不必要的迴圈（例如 $O(N^2)$）、重複的 API 請求。

### Step 4: 提供重構建議
- 先用列表指出有問題的程式碼行數與原因。
- 提供優化後的程式碼對比（Before vs After）。
- 說明優化後帶來的具體好處（如：讀取時間減少、更易於維護）。

### Step 5: 自動寫入 Review 紀錄
- 如果 `docs/code_review_record/code_review.md` 中尚無本次變動檔案的 review entry，**先調用指令建立**：`python tools/gen_code_review.py`。
- 將本次 review 的所有新 issue 以表格格式自動寫入該檔案，初始狀態設定為 `pending`。
- 表格必備欄位：`# | Severity | File | Line(s) | Category | Issue | Status | Fix Date`。
