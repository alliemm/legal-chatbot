<script setup>
import { ref, onMounted } from "vue";
import { marked } from "marked";

const props = defineProps(["pageText"]);
const clauses = ref([]);
const emit = defineEmits(["go"]);
const tooltipActive = ref(false);
const loading = ref(true);

onMounted(async () => {
  try {
    const { user_token: EMAIL } = await 
    chrome.storage.local.get("user_token");

    const prefsRes = await fetch("https://legal-chatbot-4t8e.onrender.com/userPreferences", {
      headers: { "Authorization": EMAIL }
    });
    const prefs = await prefsRes.json();
    const prefContext = `User profile — concerns: ${prefs.concerns || "general"}, document types: ${prefs.docTypes || "general"}, jargon comfort: ${prefs.jargonComfort || "moderate"}.`;

    const res = await fetch("https://legal-chatbot-4t8e.onrender.com/analyze-tc", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ text: `${prefContext} Analyze this Terms & Conditions text and return ONLY a valid JSON array with no markdown, no extra text, nothing else. Each item must have "text" (the exact risky clause) and "risk" ("high", "medium", or "low"). T&C text: ${props.pageText.slice(0, 4000)}` })
    });
    const data = await res.json();
    const raw = data.reply.replace(/```json\n?|\n?```/g, "").trim();
    clauses.value = JSON.parse(raw).map(c => ({ ...c, text: marked.parse(c.text) }));
  } catch (e) {
    console.error("Failed to load clauses:", e);
  } finally {
    loading.value = false;
  }
});
</script>

<template>
  <div class="view">
    <div class="view-title">
      Alarming clauses
      <span
        class="info-dot"
        :class="{ active: tooltipActive }"
        @click.stop="tooltipActive = !tooltipActive"
      >
        i
        <div class="tooltip">These clauses are tailored to you based on preferences from your settings</div>
      </span>
    </div>
    <div class="view-body">
      <div v-if="loading">Analyzing clauses...</div>
      <p v-else-if="clauses.length === 0">No risky clauses found.</p>
      <p v-for="(c, i) in clauses" :key="i" class="clause" :class="c.risk" v-html="c.text"></p>
    </div>
    <button class="menu-btn" @click="emit('go', 'detected')">MENU</button>
  </div>
</template>


