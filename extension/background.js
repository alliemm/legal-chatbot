// Service worker for the Chrome extension (Manifest V3)
// Tracks terms & conditions detection state per tab

// Map from tabId (number) -> detection result data
const tabStates = new Map();

// Listen for messages from content scripts and the popup
chrome.runtime.onMessage.addListener((message, sender, sendResponse) => {
  // --- Content script reports detection result ---
  if (message.type === "TC_DETECTION_RESULT") {
    const tabId = sender.tab?.id;
    if (tabId == null) return;

    // Store the latest detection data for this tab
    tabStates.set(tabId, message);

    if (message.detected) {
      // Show a red "!" badge to alert the user
      chrome.action.setBadgeText({ text: "!", tabId });
      chrome.action.setBadgeBackgroundColor({ color: "#DC2626", tabId });
    } else {
      // Clear the badge when no T&C detected
      chrome.action.setBadgeText({ text: "", tabId });
    }
    return;
  }

  // --- Popup requests the current status for a tab ---
  if (message.type === "GET_STATUS") {
    const state = tabStates.get(message.tabId) ?? { detected: false };
    sendResponse(state);
    // Return true to keep the message channel open for the async sendResponse
    return true;
  }
});

// When a tab starts loading a new page, reset its state and badge
chrome.tabs.onUpdated.addListener((tabId, changeInfo) => {
  if (changeInfo.status === "loading") {
    tabStates.delete(tabId);
    chrome.action.setBadgeText({ text: "", tabId });
  }
});

// Clean up state when a tab is closed
chrome.tabs.onRemoved.addListener((tabId) => {
  tabStates.delete(tabId);
});
