<script setup>
import { ref , onMounted } from "vue";

const props = defineProps(["pageText"]);
const clauses = ref([]);
const emit = defineEmits(["go"]);
const tooltipActive = ref(false);
const loading = ref(true);

onMounted(async () => {
  try {
    const res = await fetch("http://localhost:18080/analyze-tc", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ text: `Analyze this Terms & Conditions text and return ONLY a valid JSON array with no markdown, no extra text, nothing else. Each item must have "text" (the exact risky clause) and "risk" ("high", "medium", or "low"). T&C text: ${props.pageText}` })
    });
    const data = await res.json();
    clauses.value = JSON.parse(data.reply);
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
      <p v-for="(c, i) in clauses" :key="i" class="clause" :class="c.risk">{{ c.text }}</p>
    </div>
    <button class="menu-btn" @click="emit('go', 'detected')">MENU</button>
  </div>
</template>
