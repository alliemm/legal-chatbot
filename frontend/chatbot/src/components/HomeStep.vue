<script setup lang="ts">
defineProps<{ n: number; title: string; body: string; icon: string }>();
</script>

<template>
  <div class="relative flex flex-col items-center">
    <div class="absolute -top-2 left-6 z-20">
      <div
        class="flex h-10 w-10 items-center justify-center rounded-full text-base font-bold text-white"
        style="background-color: rgba(38,111,89,0.83); font-family: Poppins, sans-serif"
      >{{ n }}</div>
    </div>
    <div
      class="relative flex aspect-square w-[330px] flex-col items-center justify-center rounded-full px-10 text-center shadow-[0_28px_70px_-20px_rgba(30,109,85,0.55)]"
      style="background-color: #1e6d55"
    >
      <img :src="icon" alt="" class="absolute -left-2 -top-4 h-24 w-24" />
      <h3 class="leading-tight" style="font-family: Newsreader, serif; font-weight: 800; font-size: clamp(1.5rem,2.2vw,2rem); letter-spacing: -0.07em; color: #4cc7a2">{{ title }}</h3>
      <p class="mt-3 leading-snug" style="font-family: Newsreader, serif; font-size: clamp(0.95rem,1.3vw,1.4rem); letter-spacing: -0.06em; color: #b8e0d4">{{ body }}</p>
    </div>
  </div>
</template>
