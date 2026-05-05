<script setup>
import { ref } from "vue";

const emit = defineEmits(["go"]);
const email = ref("");
const password = ref("");
const error = ref("");
const isLoading = ref(false);

async function login() {
  if (!email.value || !password.value) return;
  isLoading.value = true;
  error.value = "";
  try {
    const res = await fetch("https://legal-chatbot-4t8e.onrender.com/login", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ email: email.value, password: password.value })
    });
    const data = await res.json();
    if (res.ok) {
      chrome.storage.local.set({ user_token: data.token });
      emit("go", "detected");
    } else {
      error.value = "Invalid email or password.";
    }
  } catch (e) {
    error.value = "Something went wrong. Try again.";
  } finally {
    isLoading.value = false;
  }
}
</script>

<template>
  <div class="unlock-logo"><img src="/icon.png" alt="Legaleye" /></div>
  <h1 class="unlock-title">Legaleye</h1>
  <input v-model="email" type="email" placeholder="Email" class="unlock-input" />
  <input v-model="password" type="password" placeholder="Password" class="unlock-input" @keydown.enter="login" />
  <p v-if="error" class="unlock-error">{{ error }}</p>
  <button class="btn btn-primary" @click="login" :disabled="isLoading">
    {{ isLoading ? "Logging in..." : "Login" }}
  </button>
</template>