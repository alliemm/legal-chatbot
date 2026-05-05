<script setup>
import { ref, onMounted } from "vue";
import { marked } from "marked";

const props = defineProps(["pageText"]);
const emit = defineEmits(["go"]);
const summary = ref("");
const loading = ref(true);

onMounted(async () => {
  if (!props.pageText || !props.pageText.trim()) {
    summary.value = "I couldn't read the Terms & Conditions from this page. Please reload the page and reopen the extension.";
    loading.value = false;
    return;
  }

  try {
    const res = await fetch("https://legal-chatbot-4t8e.onrender.com/analyze-tc", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ text: `You are a legal assistant. Summarize the following Terms & Conditions in 9-10 plain English bullet points. Do NOT return JSON. T&C text: ${props.pageText}` })
    });
    const data = await res.json();
    summary.value = marked.parse(data.reply);
  } catch (e) {
    console.error("Failed to load summary:", e);
  } finally {
    loading.value = false;
  }
});
</script>

<template>
  <div class="view">
    <div class="view-title">Summary</div>
    <div class="view-body">
      <div v-if="loading">Generating summary...</div>
      <div v-else v-html="summary"></div>
    </div>
    <button class="menu-btn" @click="emit('go', 'detected')">MENU</button>
  </div>
</template>