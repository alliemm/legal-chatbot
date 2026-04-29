// content.js
const keywords = ["terms", "conditions", "privacy", "tos", "agreement"];

function injectLexAssist() {
    if (document.getElementById('lex-assist-host')) return; // Prevent double-loading

    const isLegal = keywords.some(k => window.location.href.toLowerCase().includes(k) || document.title.toLowerCase().includes(k));

    if (isLegal) {
        // 1. Create a "Host" element (The bridge to the webpage)
        const host = document.createElement('div');
        host.id = 'lex-assist-host';
        document.body.appendChild(host);

        // 2. Attach a Shadow Root (The "Privacy Bubble")
        const shadow = host.attachShadow({ mode: 'open' });

        // 3. Create the UI inside the Bubble
        const wrapper = document.createElement('div');
        wrapper.innerHTML = `
      <style>
        .lex-popup {
          position: fixed;
          bottom: 30px;
          right: 30px;
          width: 320px;
          background: #ffffff;
          border-radius: 12px;
          box-shadow: 0 10px 30px rgba(0,0,0,0.2);
          font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
          z-index: 2147483647;
          border: 1px solid #e0e0e0;
          overflow: hidden;
          animation: slideIn 0.4s ease-out;
        }
        @keyframes slideIn { from { transform: translateY(100px); opacity: 0; } to { transform: translateY(0); opacity: 1; } }
        .header { background: #1a2a3a; color: white; padding: 15px; font-weight: bold; display: flex; align-items: center; gap: 10px; }
        .body { padding: 15px; color: #333; font-size: 14px; line-height: 1.5; }
        .btn-start { background: #3498db; color: white; border: none; padding: 10px 20px; border-radius: 6px; cursor: pointer; width: 100%; font-weight: 600; margin-top: 10px; transition: 0.2s; }
        .btn-start:hover { background: #2980b9; }
        .btn-close { color: #999; cursor: pointer; font-size: 18px; margin-left: auto; }
      </style>
      <div class="lex-popup">
        <div class="header">
          <span>⚖️ LexAssist</span>
          <span id="close-x" class="btn-close">&times;</span>
        </div>
        <div class="body">
          <b>Terms Detected!</b><br>
          Would you like me to scan for red flags in the <b>Termination</b> and <b>Liability</b> clauses?
          <button id="start-lex" class="btn-start">Analyze with AI</button>
        </div>
      </div>
    `;

        shadow.appendChild(wrapper);

        // 4. Handle Actions
        shadow.getElementById('start-lex').onclick = () => {
            chrome.runtime.sendMessage({ action: "OPEN_LEXASSIST" });
            host.remove();
        };

        shadow.getElementById('close-x').onclick = () => host.remove();
    }
}

// Run once when the page is ready
setTimeout(injectLexAssist, 1500);