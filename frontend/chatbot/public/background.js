chrome.runtime.onMessage.addListener((msg, sender) => {
    if (msg.action === "OPEN_LEXASSIST") {
        chrome.sidePanel.open({ tabId: sender.tab.id });
    }
});