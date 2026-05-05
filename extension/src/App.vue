<script setup>
const EMAIL = localStorage.getItem("user_token");
const screen = ref("loading");
const pageText = ref("");
const currentTabId = ref(null);   // ← add this

const go = (target) => {
  // ← replace the old single-line go with this:
  if (["chat", "clauses", "summary"].includes(target) && currentTabId.value != null) {
    chrome.runtime.sendMessage({ type: "GET_STATUS", tabId: currentTabId.value }, (response) => {
      pageText.value = response?.pageText || "";
      screen.value = target;
    });
  } else {
    screen.value = target;
  }
};

onMounted(() => {
  chrome.storage.local.get("user_token", ({ user_token }) => {
    if (!user_token) { screen.value = "unlock"; return; }
    chrome.tabs.query({ active: true, currentWindow: true }, (tabs) => {
      const tabId = tabs[0]?.id;
      if (tabId == null) { screen.value = "not-detected"; return; }
      currentTabId.value = tabId;   // ← store it
      chrome.runtime.sendMessage({ type: "GET_STATUS", tabId }, (response) => {
        pageText.value = response?.pageText || "";
        screen.value = response?.detected ? "detected" : "not-detected";
      });
    });
  });
});
</script>

<template>
  <div class="app">
    <div class="screen">
      <div class="header">
        <div class="brand">
          <span class="brand-logo"><img src="/icon.png" alt="" /></span>
          <span class="brand-name">Legaleye</span>
        </div>
      </div>
      <UnlockScreen v-if="screen === 'unlock'" @go="go" />
      <div v-if="screen === 'loading'" class="notice">Loading...</div>
      <div v-else-if="screen === 'not-detected'" class="notice">No Terms & Conditions detected on this page.</div>
      <DetectedScreen v-if="screen === 'detected'" :email="EMAIL" @go="go" />
      <ChatScreen v-else-if="screen === 'chat'" :pageText="pageText" @go="go" />
      <ClausesScreen v-else-if="screen === 'clauses'" :pageText="pageText" @go="go" />
      <SummaryScreen v-else-if="screen === 'summary'" :pageText="pageText" @go="go" />
    </div>
  </div>
</template>