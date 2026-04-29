// content.js
const keywords = ["terms", "conditions", "privacy", "tos", "agreement"];

if (keywords.some(k => window.location.href.toLowerCase().includes(k))) {
    // Create the "Popup" element
    const banner = document.createElement('div');
    banner.id = "lex-assist-trigger";
    banner.innerHTML = `
        <div style="padding: 15px; background: #2c3e50; color: white; border-radius: 8px; box-shadow: 0 4px 15px rgba(0,0,0,0.3); font-family: sans-serif;">
            <p style="margin: 0 0 10px 0;">⚖️ <b>LexAssist Detected a Legal Document</b></p>
            <button id="lex-start-btn" style="background: #3498db; color: white; border: none; padding: 8px 12px; border-radius: 4px; cursor: pointer;">
                Analyze Risks Now
            </button>
            <button id="lex-close-btn" style="background: transparent; color: #bdc3c7; border: none; margin-left: 10px; cursor: pointer;">Dismiss</button>
        </div>
    `;

    // Style the banner to float in the bottom right
    Object.assign(banner.style, {
        position: 'fixed',
        bottom: '20px',
        right: '20px',
        zIndex: '999999'
    });

    document.body.appendChild(banner);

    // Make the buttons actually do something
    document.getElementById('lex-start-btn').onclick = () => {
        chrome.runtime.sendMessage({ action: "OPEN_SIDE_PANEL" });
        banner.remove(); // Clean up after opening
    };

    document.getElementById('lex-close-btn').onclick = () => banner.remove();
}