<script setup>
const emit = defineEmits(["go"]);
</script>

<template>
  <div class="view">
    <div class="view-title">Summary</div>
    <div class="view-body">
      <ol class="summary-list">
        <li>
          There wasn't a bird in the sky, but that was not what caught her attention. It was the
          clouds. The deep green that isn't the color of clouds, but came with these. She knew what
          was coming and she hoped she was prepared.
        </li>
        <li>
          It wasn't that he hated her. It was simply that he didn't like her much. It was difficult
          for him to explain this to her, and even more difficult for her to truly understand. She
          was in love and wanted him to feel the same way. He didn't, and no matter how he tried to
          explain to her she refused to listen or to understand.
        </li>
      </ol>
    </div>
    <button class="menu-btn" @click="emit('go', 'detected')">MENU</button>
  </div>
</template>
