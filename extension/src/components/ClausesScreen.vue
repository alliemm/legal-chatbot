<script setup>
import { ref } from "vue";

const emit = defineEmits(["go"]);
const tooltipActive = ref(false);

const sampleClause =
  "It's always good to bring a slower friend with you on a hike. If you happen to come across bears, the whole group doesn't have to worry. Only the slowest in the group do. That was the lesson they were about to...";
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
      <p class="clause">{{ sampleClause }}</p>
      <p class="clause">{{ sampleClause }}</p>
      <p class="clause bold">{{ sampleClause }} learn that day.</p>
      <p class="clause">{{ sampleClause }}</p>
    </div>
    <button class="menu-btn" @click="emit('go', 'detected')">MENU</button>
  </div>
</template>
