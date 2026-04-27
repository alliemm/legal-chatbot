<script setup lang="ts">
defineProps<{ icon: string; title: string; body: string }>();
</script>

<template>
  <div class="relative flex flex-col items-center">
    <div
      class="relative flex aspect-square w-[380px] flex-col items-center justify-center rounded-full px-10 text-center shadow-[0_30px_70px_-20px_rgba(30,109,85,0.6)]"
      style="background-color: #1e6d55"
    >
      <img :src="icon" alt="" class="absolute -left-3 -top-6 h-28 w-28" />
      <h3 class="leading-tight" style="font-family: Newsreader, serif; font-weight: 800; font-size: clamp(1.5rem,2.2vw,2rem); letter-spacing: -0.07em; color: #4cc7a2">{{ title }}</h3>
      <p class="mt-4 leading-snug" style="font-family: Newsreader, serif; font-size: clamp(0.95rem,1.3vw,1.4rem); letter-spacing: -0.06em; color: #d2d2d2">{{ body }}</p>
    </div>
  </div>
</template>
