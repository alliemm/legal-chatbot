<script setup>
import { ref, nextTick } from "vue";

const emit = defineEmits(["go"]);
const messages = ref([]);
const chatInput = ref("");
const chatArea = ref(null);

async function send() {
  const v = chatInput.value.trim();
  if (!v) return;
  messages.value.push({ role: "me", text: v });
  messages.value.push({ role: "ai", text: "Thanks — analyzing this against your contract..." });
  chatInput.value = "";
  await nextTick();
  if (chatArea.value) chatArea.value.scrollTop = chatArea.value.scrollHeight;
}
</script>

<template>
  <div class="view">
    <div class="view-title">New Chat</div>
    <div class="chat-area" ref="chatArea">
      <div v-if="messages.length === 0" class="notice">
        Reminder: This is an AI chatbot generated for better communication and simpler word base!
      </div>
      <div v-for="(m, i) in messages" :key="i" class="msg" :class="{ me: m.role === 'me' }">
        {{ m.text }}
      </div>
    </div>
    <div class="input-row">
      <input
        v-model="chatInput"
        placeholder="Type a new message here"
        @keydown.enter="send"
        autofocus
      />
      <button class="send-btn" @click="send" aria-label="Send">
        <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round">
          <line x1="22" y1="2" x2="11" y2="13" />
          <polygon points="22 2 15 22 11 13 2 9 22 2" />
        </svg>
      </button>
    </div>
    <button class="menu-btn" @click="emit('go', 'detected')">MENU</button>
  </div>
</template>
