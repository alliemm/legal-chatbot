<script setup>
import { ref } from "vue";
import UnlockScreen from "./components/UnlockScreen.vue";
import DetectedScreen from "./components/DetectedScreen.vue";
import ChatScreen from "./components/ChatScreen.vue";
import ClausesScreen from "./components/ClausesScreen.vue";
import SummaryScreen from "./components/SummaryScreen.vue";

const EMAIL = "saunter@getalby.com";
const screen = ref("unlock");
const go = (target) => (screen.value = target);
</script>

<template>
  <div class="app">
    <div class="screen" :class="{ unlock: screen === 'unlock' }">
      <div v-if="screen !== 'unlock'" class="header">
        <div class="brand">
          <span class="brand-logo"><img src="/icon.png" alt="" /></span>
          <span class="brand-name">Legaleye</span>
        </div>
        <span class="user-pill">{{ EMAIL }}</span>
      </div>

      <UnlockScreen v-if="screen === 'unlock'" @go="go" />
      <DetectedScreen v-else-if="screen === 'detected'" :email="EMAIL" @go="go" />
      <ChatScreen v-else-if="screen === 'chat'" @go="go" />
      <ClausesScreen v-else-if="screen === 'clauses'" @go="go" />
      <SummaryScreen v-else-if="screen === 'summary'" @go="go" />
    </div>
  </div>
</template>
