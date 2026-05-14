(function() {
    // 語系對照表：當前頁面 → 其他語系頁面
    var LANG_MAP = {
        'en': { label: 'English', suffix: '' },
        'cn': { label: '简体中文', suffix: '_cn' },
        'tw': { label: '繁體中文', suffix: '_tw' }
    };

    // 從當前 URL 判斷語系
    function detectLang(path) {
        if (/_cn\.html$/.test(path) || /_cn\/$/.test(path)) return 'cn';
        if (/_tw\.html$/.test(path) || /_tw\/$/.test(path)) return 'tw';
        return 'en';
    }

    // 取得當前頁面的基礎名稱（去掉語系後綴）
    function getBasePath(path, lang) {
        if (lang === 'en') {
            // index.html → index.md, 其他 _en.html → _en.md
            return path;
        }
        // requirement_cn.html → requirement_en.html 的對應
        return path.replace('_' + lang, '_en');
    }

    // 建立下拉選單
    function createSwitcher() {
        var currentPath = window.location.pathname;
        var currentLang = detectLang(currentPath);

        // 計算其他語系的 URL
        var urls = {};
        for (var key in LANG_MAP) {
            if (key === currentLang) continue;
            var info = LANG_MAP[key];
            if (currentLang === 'en') {
                // 從 index.html → index_cn.html, requirement_en.html → requirement_cn.html
                urls[key] = currentPath.replace(/\.html$/, info.suffix + '.html')
                                       .replace(/\/$/, info.suffix + '/');
            } else {
                // 從 requirement_cn.html → requirement_en.html 或 requirement_tw.html
                urls[key] = currentPath.replace('_' + currentLang, '_' + key);
            }
        }

        // 建立 DOM
        var select = document.createElement('select');
        select.style.cssText = 'padding:4px 8px;border:1px solid #ccc;border-radius:4px;font-size:13px;margin-left:12px;background:#fff;cursor:pointer;';

        // 當前語系（selected）
        var opt = document.createElement('option');
        opt.value = '';
        opt.textContent = LANG_MAP[currentLang].label;
        opt.selected = true;
        opt.disabled = true;
        select.appendChild(opt);

        // 其他語系
        for (var key in urls) {
            var o = document.createElement('option');
            o.value = urls[key];
            o.textContent = LANG_MAP[key].label;
            select.appendChild(o);
        }

        select.addEventListener('change', function() {
            if (this.value) window.location.href = this.value;
        });

        // 插入到 header 右側
        var header = document.querySelector('.md-header__inner');
        if (header) {
            var wrap = document.createElement('div');
            wrap.style.cssText = 'display:flex;align-items:center;';
            wrap.appendChild(select);
            header.appendChild(wrap);
        }
    }

    // 等 DOM 載入完
    if (document.readyState === 'loading') {
        document.addEventListener('DOMContentLoaded', createSwitcher);
    } else {
        createSwitcher();
    }
})();
