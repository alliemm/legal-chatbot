const keywords = ["terms", "conditions", "privacy", "tos", "agreement"];
const isLegal = keywords.some(k => window.location.href.toLowerCase().includes(k));

if (isLegal) {
    chrome.runtime.sendMessage({ action: "OPEN_LEXASSIST" });
}