<script setup>
import { ref, onMounted } from "vue";
import DetectedScreen from "./components/DetectedScreen.vue";
import ChatScreen from "./components/ChatScreen.vue";
import ClausesScreen from "./components/ClausesScreen.vue";
import SummaryScreen from "./components/SummaryScreen.vue";

const EMAIL = localStorage.getItem("user_token");
const screen = ref("detected");
const pageText = ref("");
const go = (target) => (screen.value = target);

onMounted(() => {
  chrome.tabs.query({ active: true, currentWindow: true }, (tabs) => {
    const tabId = tabs[0]?.id;
    if (tabId == null) return;
    chrome.runtime.sendMessage({ type: "GET_STATUS", tabId }, (response) => {
      if (response?.detected) {
        screen.value = "detected";
        pageText.value = response.pageText || "";
      }
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

      <DetectedScreen v-if="screen === 'detected'" :email="EMAIL" @go="go" />
      <ChatScreen v-else-if="screen === 'chat'" :pageText="pageText" @go="go" />
      <ClausesScreen v-else-if="screen === 'clauses'" :pageText="pageText" @go="go" />
      <SummaryScreen v-else-if="screen === 'summary'" :pageText="pageText" @go="go" />
    </div>
  </div>
</template>