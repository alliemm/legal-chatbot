<script setup>
defineProps(["email"]);
const emit = defineEmits(["go"]);
</script>

<template>
  <h2 class="detected-title">Contract Detected!</h2>
  <div class="detected-email">{{ email }}</div>
  <div class="cards">
    <button class="card" @click="emit('go', 'clauses')">
      <div class="card-icon">⚠️</div>
      <div class="card-label">Alarming clauses</div>
    </button>
    <button class="card" @click="emit('go', 'chat')">
      <div class="card-icon">💬</div>
      <div class="card-label">Ask questions</div>
    </button>
    <button class="card" @click="emit('go', 'summary')">
      <div class="card-icon">📄</div>
      <div class="card-label">Summary</div>
    </button>
  </div>
</template>
