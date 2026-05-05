<script setup>
import { ref, onMounted } from "vue";
import UnlockScreen from "./components/UnlockScreen.vue";
import DetectedScreen from "./components/DetectedScreen.vue";
import ChatScreen from "./components/ChatScreen.vue";
import ClausesScreen from "./components/ClausesScreen.vue";
import SummaryScreen from "./components/SummaryScreen.vue";

const EMAIL = localStorage.getItem("user_token");
const screen = ref("loading");
const pageText = ref("");
const currentTabId = ref(null);

const go = (target) => {
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
    if (!user_token) {
      screen.value = "unlock";
      return;
    }
    chrome.tabs.query({ active: true, currentWindow: true }, (tabs) => {
      const tabId = tabs[0]?.id;
      if (tabId == null) { screen.value = "not-detected"; return; }
      currentTabId.value = tabId;
      chrome.runtime.sendMessage({ type: "GET_STATUS", tabId }, (response) => {
        pageText.value = response?.pageText || "";
        screen.value = response?.detected ? "detected" : "not-detected";
      });
    });
  });
});
</script>