<script setup>
const emit = defineEmits(["go"]);
</script>

<template>
  <div class="unlock-logo"><img src="/icon.png" alt="Legaleye" /></div>
  <h1 class="unlock-title">Legaleye</h1>
  <button class="btn btn-primary" @click="emit('go', 'detected')">Login</button>
</template>
