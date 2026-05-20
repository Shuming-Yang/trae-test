---
name: gitlab-ci-generator
description: # 技能名稱：gitlab-ci-generator
## 描述：根據當前專案的架構，自動編寫或修正符合團隊規範的 `.gitlab-ci.yml` 自動化流水線檔案。

## 觸發時機
- 當用戶提到「修改 CI 流程」、「新增部署階段」或「建立自動編譯腳本」時。
---

## 執行步驟
1. 結構檢查：確保流水線包含 `stages: [lint, test, build, deploy]` 四個核心階段。
2. 環境與映像檔：
   - 靜態檢查（Lint）與測試（Test）階段必須指定特定的輕量化映像檔（例如 `python:3.10-slim`）。
   - 啟用快取機制（Cache），鎖定 `pip` 或 `npm` 的快取資料夾以加速編譯。
3. 自動編譯與合規（Build 階段）：
   - 必須加入自動編譯驗證腳本（如 `bash autobuild.sh`）。
   - 必須包含靜態合規性掃描步驟（如執行驗證工具並產出報告）。
4. 產物管理（Artifacts）：
   - 編譯成功後的產物（Artifacts）必須設定過期時間（expire_in: 1 week），避免佔用 GitLab 伺服器空間。
5. 輸出結果：直接產生完整的 `.gitlab-ci.yml` 程式碼區塊，並詳細說明本次修改了哪些 Stage 與觸發條件。