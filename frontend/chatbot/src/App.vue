<template>
  <AppNav v-if="showNav" />
  <div :class="{ 'has-nav': showNav }">
    <router-view />
  </div>
  <ChatPanel />
</template>

<script>
import AppNav from './components/AppNav.vue'
import ChatPanel from './components/ChatPanel.vue'
export default {
  components: { AppNav, ChatPanel },
  computed: {
    showNav() {
      return !['/profile', '/survey'].includes(this.$route.path)
    }
  }
}
</script>

<style>
.has-nav { padding-top: 56px; }
</style>
