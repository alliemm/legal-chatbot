<script setup>
import { ref, nextTick } from "vue";
import { marked } from "marked";

const props = defineProps(["pageText"]);
const emit = defineEmits(["go"]);
const messages = ref([]);
const chatInput = ref("");
const chatArea = ref(null);
const isThinking = ref(false);

async function send() {
  const v = chatInput.value.trim();
  if (!v || isThinking.value) return;
  messages.value.push({ role: "me", text: v });
  chatInput.value = "";
  isThinking.value = true;
  await nextTick();
  if (chatArea.value) chatArea.value.scrollTop = chatArea.value.scrollHeight;

  try {
    const res = await fetch("http://localhost:18080/analyze-tc", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ text: `You are a legal assistant. Answer this question in plain English about the following Terms & Conditions. Do NOT use markdown, headers, asterisks, or bullet symbols. Write in plain paragraphs only. Do NOT return JSON. Question: "${v}". T&C text: ${props.pageText.slice(0, 4000)}` })
    });
    const data = await res.json();
    messages.value.push({ role: "ai", text: marked.parse(data.reply) });
  } catch (e) {
    messages.value.push({ role: "ai", text: "Something went wrong. Please try again." });
  } finally {
    isThinking.value = false;
    await nextTick();
    if (chatArea.value) chatArea.value.scrollTop = chatArea.value.scrollHeight;
  }
}
</script>

<template>
  <div class="view">
    <div class="view-title">New Chat</div>
    <div class="chat-area" ref="chatArea">
      <div v-if="messages.length === 0" class="notice">
        Reminder: This AI chatbot is for informational purposes only and does not constitute legal advice.
      </div>
      <div v-for="(m, i) in messages" :key="i" class="msg" :class="{ me: m.role === 'me' }">
        <span v-if="m.role === 'ai'" v-html="m.text"></span>
        <span v-else>{{ m.text }}</span>
      </div>
      <div v-if="isThinking" class="msg">Thinking...</div>
    </div>
    <div class="input-row">
      <input
        v-model="chatInput"
        placeholder="Type a new message here"
        @keydown.enter="send"
        :disabled="isThinking"
        autofocus
      />
      <button class="send-btn" @click="send" :disabled="isThinking" aria-label="Send">
        <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round">
          <line x1="22" y1="2" x2="11" y2="13" />
          <polygon points="22 2 15 22 11 13 2 9 22 2" />
        </svg>
      </button>
    </div>
    <button class="menu-btn" @click="emit('go', 'detected')">MENU</button>
  </div>
</template>