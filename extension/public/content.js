
const TC_KEYWORDS = [
  "terms and conditions",
  "terms of service",
  "terms of use",
  "privacy policy",
  "user agreement",
  "end user license agreement",
  "eula",
  "legal notice",
  "cookie policy",
  "acceptable use policy",
  "data processing agreement",
  "service level agreement",
];

const TC_URL_SLUGS = [
  "/terms",
  "/tos",
  "/privacy-policy",
  "/privacy",
  "/eula",
  "/legal",
  "/cookie-policy",
  "/user-agreement",
  "/acceptable-use",
];

function detectTermsAndConditions() {
  let score = 0;
  const matchedSignals = [];
  const titleLower = document.title.toLowerCase();
  const urlPath = location.pathname.toLowerCase();

  for (const keyword of TC_KEYWORDS) {
    if (titleLower.includes(keyword)) {
      score += 3;
      matchedSignals.push(`title:"${keyword}"`);
      break;
    }
  }

  for (const slug of TC_URL_SLUGS) {
    if (urlPath.includes(slug)) {
      score += 3;
      matchedSignals.push(`url:"${slug}"`);
      break;
    }
  }

  const h1Elements = document.querySelectorAll("h1");
  outerH1: for (const h1 of h1Elements) {
    const h1Text = h1.innerText.toLowerCase();
    for (const keyword of TC_KEYWORDS) {
      if (h1Text.includes(keyword)) {
        score += 2;
        matchedSignals.push(`h1:"${keyword}"`);
        break outerH1;
      }
    }
  }

  let subheadingPoints = 0;
  const subheadings = document.querySelectorAll("h2, h3");
  for (const el of subheadings) {
    if (subheadingPoints >= 2) break;
    const elText = el.innerText.toLowerCase();
    for (const keyword of TC_KEYWORDS) {
      if (elText.includes(keyword)) {
        subheadingPoints += 1;
        matchedSignals.push(`${el.tagName.toLowerCase()}:"${keyword}"`);
        break;
      }
    }
  }
  score += subheadingPoints;

  const bodyText = (document.body.innerText || "").slice(0, 5000).toLowerCase();
  const bodyMatches = TC_KEYWORDS.filter((kw) => bodyText.includes(kw));
  if (bodyMatches.length >= 2) {
    score += 1;
    matchedSignals.push(`body:${bodyMatches.length} keywords`);
  }

  return { detected: score >= 3, score, matchedSignals };
}

function sendResult(result) {
  chrome.runtime.sendMessage({
    type: "TC_DETECTION_RESULT",
    detected: result.detected,
    score: result.score,
    url: location.href,
    matchedSignals: result.matchedSignals,
    pageText: document.body.innerText,
  });
}

function injectPanel() {
  if (document.getElementById('lex-assist-host')) return;

  const host = document.createElement('div');
  host.id = 'lex-assist-host';
  document.body.appendChild(host);

  const shadow = host.attachShadow({ mode: 'open' });
  const wrapper = document.createElement('div');
  wrapper.innerHTML = `
    <style>
      .lex-popup { position: fixed; bottom: 30px; right: 30px; width: 320px; background: #ffffff; border-radius: 12px; box-shadow: 0 10px 30px rgba(0,0,0,0.2); font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; z-index: 2147483647; border: 1px solid #e0e0e0; overflow: hidden; animation: slideIn 0.4s ease-out; }
      @keyframes slideIn { from { transform: translateY(100px); opacity: 0; } to { transform: translateY(0); opacity: 1; } }
      .header { background: #0b4538; color: white; padding: 15px; font-weight: bold; display: flex; align-items: center; gap: 10px; }
      .body { padding: 15px; color: #333; font-size: 14px; line-height: 1.5; }
      .btn-start { background: #0b4538; color: white; border: none; padding: 10px 20px; border-radius: 6px; cursor: pointer; width: 100%; font-weight: 600; margin-top: 10px; transition: 0.2s; }
      .btn-start:hover { opacity: 0.9; }
      .btn-close { color: #999; cursor: pointer; font-size: 18px; margin-left: auto; }
    </style>
    <div class="lex-popup">
      <div class="header">
        <span>⚖️ LegalEye</span>
        <span id="close-x" class="btn-close">&times;</span>
      </div>
      <div class="body">
        <b>Terms Detected!</b><br>
        Click the extension icon to analyze this page's terms and conditions!
        <button id="start-lex" class="btn-start">Close</button>
      </div>
    </div>
  `;
  shadow.appendChild(wrapper);

  shadow.getElementById('start-lex').onclick = () => {
    chrome.runtime.sendMessage({ action: "OPEN_LEXASSIST" });
    host.remove();
  };
  shadow.getElementById('close-x').onclick = () => host.remove();
}

let debounceTimer = null;
let lastBodyTextLength = document.body.innerText.length;
let observerActive = true;

const observer = new MutationObserver(() => {
  clearTimeout(debounceTimer);
  debounceTimer = setTimeout(() => {
    const currentLength = document.body.innerText.length;
    if (Math.abs(currentLength - lastBodyTextLength) > 500) {
      lastBodyTextLength = currentLength;
      const result = detectTermsAndConditions();
      sendResult(result);
      if (result.detected) {
        injectPanel();
        observer.disconnect();
        observerActive = false;
      }
    }
  }, 1000);
});

function initialize() {
  const initialResult = detectTermsAndConditions();
  if (initialResult.detected) {
    injectPanel();
  }
  sendResult(initialResult);
  if (!initialResult.detected && document.body) {
    observer.observe(document.body, { childList: true, subtree: true });
  }
}

if (document.readyState === "complete") {
  initialize();
} else {
  window.addEventListener("load", initialize);
}

window.addEventListener("legaleye:logout", () => {
  chrome.runtime.sendMessage({ type: "LOGOUT" });
});

chrome.runtime.onMessage.addListener((message) => {
  if (message.type === "HIGHLIGHT_CLAUSES") {
    for (const clause of message.clauses) {
      document.body.innerHTML = document.body.innerHTML.replace(
        clause.text,
        `<mark style="background: rgba(255,0,0,0.3); border-radius: 3px;">${clause.text}</mark>`
      );
    }
  }
});