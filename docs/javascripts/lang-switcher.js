(function() {
    var LANGS = {
        'en': { label: 'English' },
        'cn': { label: '简体中文' },
        'tw': { label: '繁體中文' }
    };

    function detectLang(path) {
        if (/_cn\.html$/.test(path) || /_cn\/$/.test(path)) return 'cn';
        if (/_tw\.html$/.test(path) || /_tw\/$/.test(path)) return 'tw';
        return 'en';
    }

    function switchTo(path, targetLang) {
        var hasSuffix = /_(en|cn|tw)(?=\.html|\/|$)/.test(path);

        if (hasSuffix) {
            if (targetLang === 'en') {
                return path.replace(/_(en|cn|tw)(?=\.html|\/|$)/, '');
            }
            return path.replace(/_(en|cn|tw)(?=\.html|\/|$)/, '_' + targetLang);
        }

        // 無後綴 = 英文頁面
        if (targetLang === 'en') return path;

        // 英文首頁特殊處理: /trae-test/ → /trae-test/index_cn/
        if (path === '/' || /\/$/.test(path) || /\/index\.html$/.test(path)) {
            return path.replace(/(\/?)(index\.html)?$/, '/index_' + targetLang + '/');
        }

        return path.replace(/(\.html)$/, '_' + targetLang + '.html');
    }

    function createSwitcher() {
        var path = window.location.pathname;
        var currentLang = detectLang(path);

        var select = document.createElement('select');
        select.style.cssText =
            'padding:4px 8px;border:1px solid var(--md-default-fg-color--lightest);' +
            'border-radius:4px;font-size:13px;margin-left:12px;' +
            'background:var(--md-default-bg-color);color:var(--md-default-fg-color);cursor:pointer;';

        var cur = document.createElement('option');
        cur.value = '';
        cur.textContent = LANGS[currentLang].label;
        cur.selected = true;
        cur.disabled = true;
        select.appendChild(cur);

        for (var key in LANGS) {
            if (key === currentLang) continue;
            var opt = document.createElement('option');
            opt.value = switchTo(path, key);
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
