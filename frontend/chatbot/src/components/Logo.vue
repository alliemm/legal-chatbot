<script setup lang="ts">
import logoSrc from "@/assets/legaleye-logo.png";
defineProps<{ class?: string }>();
</script>

<template>
  <span :class="`${$props.class ?? 'h-10 w-10'} inline-flex items-center justify-center rounded-full bg-black p-1.5`">
    <img :src="logoSrc" alt="Legaleye" class="h-full w-full object-contain" />
  </span>
</template>
