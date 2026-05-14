(function() {
    var LANGS = {
        'en': { label: 'English', suffix: '_en' },
        'cn': { label: '简体中文', suffix: '_cn' },
        'tw': { label: '繁體中文', suffix: '_tw' }
    };

    // 從 URL 偵測當前語系
    function detectLang(path) {
        if (/_cn\.html$/.test(path) || /_cn\/$/.test(path)) return 'cn';
        if (/_tw\.html$/.test(path) || /_tw\/$/.test(path)) return 'tw';
        return 'en';
    }

    // 剝離語系後綴，得到基礎路徑
    function getBasePath(path, lang) {
        if (lang === 'en') {
            return path.replace('_en', '');
        }
        return path.replace('_' + lang, '');
    }

    // 根據目標語系組合完整路徑
    function getTargetPath(basePath, targetLang) {
        if (targetLang === 'en') {
            return basePath;
        }
        return basePath.replace(/\.html$/, '_' + targetLang + '.html')
                       .replace(/\/$/, '_' + targetLang + '/');
    }

    function createSwitcher() {
        var path = window.location.pathname;
        var currentLang = detectLang(path);
        var basePath = getBasePath(path, currentLang);

        var select = document.createElement('select');
        select.style.cssText =
            'padding:4px 8px;border:1px solid var(--md-default-fg-color--lightest);' +
            'border-radius:4px;font-size:13px;margin-left:12px;' +
            'background:var(--md-default-bg-color);color:var(--md-default-fg-color);cursor:pointer;';

        // 當前語系（disabled, 顯示用）
        var cur = document.createElement('option');
        cur.value = '';
        cur.textContent = LANGS[currentLang].label;
        cur.selected = true;
        cur.disabled = true;
        select.appendChild(cur);

        // 其他語系選項
        for (var key in LANGS) {
            if (key === currentLang) continue;
            var opt = document.createElement('option');
            opt.value = getTargetPath(basePath, key);
            opt.textContent = LANGS[key].label;
            select.appendChild(opt);
        }

        select.addEventListener('change', function() {
            if (this.value) window.location.href = this.value;
        });

        var header = document.querySelector('.md-header__inner');
        if (header) {
            var wrap = document.createElement('div');
            wrap.style.cssText = 'display:flex;align-items:center;';
            wrap.appendChild(select);
            header.appendChild(wrap);
        }
    }

    if (document.readyState === 'loading') {
        document.addEventListener('DOMContentLoaded', createSwitcher);
    } else {
        createSwitcher();
    }
})();
