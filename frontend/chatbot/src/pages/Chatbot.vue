<script setup lang="ts">
import {onMounted, ref} from "vue";
import Logo from "@/components/Logo.vue";
import { Plus, Settings, Search, Send, Smile, MessageSquare, ChevronDown, FileText } from "lucide-vue-next";
import axios from "axios";
import {useRouter, useRoute} from "vue-router";

type Source = { name: string; active?: boolean };
type Message = { from: "user" | "model"; text: string };
const API_URL = "https://legal-chatbot-4t8e.onrender.com/chat";
const sources = ref<Source[]>([{ name: "Source 1", active: true }, { name: "Source 2" }]);
const messages = ref<Message[]>([
  { from: "user", text: "Rephrase 'This is an ai chatbot generated for better communication and simpler work flows'" },
  { from: "model", text: "Thank You :)" },
]);
const isThinking = ref(false);
const input = ref("");

const router = useRouter();
const route = useRoute()

const chatID = route.params.id as string

const error = ref("");

async function send() {
  const v = input.value.trim();
  if (!v || isThinking.value) return;
  messages.value.push({ from: "user", text: v })
  isThinking.value = true;
  const userQuery = v;
  input.value = "";
  try {
    // 2. GET USER PREFERENCES (The "Secret Sauce")
    // Replace 'fetchPrefsFromDB' with whatever your teammate named their function
    // Or call your C++ endpoint that returns the DB values
    const prefsResponse = await fetch("http://localhost:18080/api/user-preferences");
    const userPrefs = await prefsResponse.json();

    // 3. SEND TO AI WITH CONTEXT
    const aiResponse = await fetch(API_URL, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({
        message: userQuery,
        preferences: userPrefs, // This tells the AI what the user likes!
        context: "The user is currently looking at a Job Offer Agreement."
      })
    });

    const data = await aiResponse.json();

    // Push the actual AI response to the chat
    messages.value.push({ from: "model", text: data.reply });

  } catch (error) {
    console.error("Failed to connect to backend:", error);
    messages.value.push({
      from: "model",
      text: "Sorry, I'm having trouble connecting to the Legaleye server."
    });
  } finally {
    isThinking.value = false; //stops loading
  }
}
const getMessages = async () => {
  try {
    const token = localStorage.getItem('user_token');
    if(!token){
      error.value = "Please login to view this page"
      router.push('/login');
      return
    }
    const response = await axios.get('https://legal-chatbot-4t8e.onrender.com/chatHistory', {
      headers: {
        Authorization: token,
        ChatID : chatID
      }
    });
    const data = response.data;
    const chatHistory = data.map((message: any) =>({
      from: message[0] === 'user' ? 'user' : 'model',
      text: message[1]
    }))
    messages.value.push(...chatHistory);

  } catch (err) {
    console.error(err);
    error.value = "Failed to load chat history.";
  }
};
onMounted(() => {
  getMessages();
});
</script>

<template>
  <div class="min-h-screen w-full bg-cream flex" style="font-family: Montserrat, Inter, sans-serif">
    <!-- SIDEBAR -->
    <aside class="hidden md:flex flex-col w-[360px] m-6 rounded-2xl text-white/80 relative overflow-hidden" style="background-color: #0b4538; border: 3px solid #f8f8ff; box-shadow: 0 4px 6px rgba(15,11,11,0.1); min-height: calc(100vh - 48px)">
      <div class="absolute inset-1 rounded-2xl pointer-events-none" style="border: 3px solid #f5f5f5" />
      <div class="relative p-6 flex flex-col gap-3 flex-1">
        <RouterLink to="/dashboard" class="flex items-center gap-3 px-4 h-[44px] rounded-md text-white text-[15px] font-medium" style="background-color: #1e6d55; border: 2px solid #282934">
          <Plus class="h-5 w-5" />
          <span>Add new source</span>
        </RouterLink>
        <button
          v-for="(s, i) in sources"
          :key="i"
          class="flex items-center justify-end gap-2 px-4 h-[44px] rounded-md text-white text-[15px] font-medium text-right"
          :style="{ backgroundColor: s.active ? '#1e6d55' : '#0a0c19', border: '2px solid #282934' }"
        >
          <FileText class="h-4 w-4 opacity-70" />
          <span>{{ s.name }}</span>
        </button>
      </div>
      <div class="relative p-4">
        <div class="flex items-center gap-3 rounded-xl px-3 py-3" style="background-color: #f9fafa; border: 1.2px solid #e8eff7">
          <div class="h-10 w-10 rounded-lg bg-leaf-deep flex items-center justify-center text-white font-bold">U</div>
          <div class="flex-1 min-w-0">
            <div class="text-[12px] font-medium" style="color: rgba(2,4,15,0.7)">Welcome back,</div>
            <div class="text-[18px] font-semibold truncate" style="color: #02040f">User</div>
          </div>
          <ChevronDown class="h-5 w-5" style="color: rgba(2,4,15,0.7)" />
        </div>
      </div>
    </aside>

    <!-- MAIN -->
    <div class="flex-1 flex flex-col min-w-0">
      <header class="bg-cream" style="box-shadow: 0 4px 10px rgba(0,0,0,0.10)">
        <div class="flex items-center justify-between px-6 md:px-10 py-4">
          <RouterLink to="/dashboard" class="flex items-center gap-3">
            <Logo class="h-10 w-10 text-leaf-deep" />
            <span class="font-bold" style="font-family: Inter, sans-serif; font-size: 24px; color: #085041">Legaleye</span>
          </RouterLink>
          <div class="flex items-center gap-4 text-leaf-deep">
            <button class="h-12 w-12 rounded-full flex items-center justify-center hover:bg-mint-soft/40" aria-label="Search"><Search class="h-6 w-6" /></button>
            <button class="h-12 w-12 rounded-full flex items-center justify-center hover:bg-mint-soft/40" aria-label="Settings"><Settings class="h-6 w-6" /></button>
          </div>
        </div>
      </header>

      <div class="px-6 md:px-10 pt-6">
        <h1 class="text-[32px] md:text-[40px] font-extrabold" style="font-family: Poppins, sans-serif; color: #154939">Job offer Agreement</h1>
      </div>

      <div class="flex-1 px-6 md:px-10 py-8 overflow-y-auto">
        <div class="mx-auto max-w-3xl flex flex-col gap-6">
          <div v-for="(m, i) in messages" :key="i" class="flex items-end gap-3" :class="m.from === 'user' ? 'justify-end' : 'justify-start'">
            <div v-if="m.from === 'model'" class="h-10 w-10 rounded-lg bg-leaf-deep flex items-center justify-center text-white text-sm font-bold">L</div>
            <div class="relative max-w-[80%] rounded-[10px] px-5 py-4 text-[18px] leading-snug bg-white" style="border: 1px solid #ddd; color: rgba(41,41,41,0.85); box-shadow: 0px 1px 2.29px rgba(0,0,0,0.13)">{{ m.text }}</div>
            <div v-if="m.from === 'user'" class="h-10 w-10 rounded-lg bg-mint-soft flex items-center justify-center text-leaf-deep text-sm font-bold">U</div>
          </div>
          <div v-if="isThinking" class="flex items-end gap-3 justify-start">
            <div class="h-10 w-10 rounded-lg bg-leaf-deep flex items-center justify-center text-white text-sm font-bold">L</div>
            <div class="italic text-gray-400 text-sm">LexAssist is thinking...</div>
          </div>
        </div>
      </div>

      <div class="px-6 md:px-10 pb-8">
        <div class="mx-auto max-w-5xl rounded-xl bg-white relative" style="border: 2px solid #02040f; box-shadow: 0 1px 2.29px rgba(0,0,0,0.13)">
          <input v-model="input" @keydown.enter="send" placeholder="Type a new message here" class="w-full bg-transparent px-6 py-6 pr-40 text-[16px] outline-none" style="color: #424242" />
          <div class="absolute right-4 top-1/2 -translate-y-1/2 flex items-center gap-2 text-leaf-deep">
            <button class="h-11 w-11 rounded-full flex items-center justify-center hover:bg-mint-soft/40" aria-label="Conversations"><MessageSquare class="h-5 w-5" /></button>
            <button class="h-11 w-11 rounded-full flex items-center justify-center hover:bg-mint-soft/40" aria-label="Emoji"><Smile class="h-5 w-5" /></button>
            <button @click="send" class="h-11 w-11 rounded-full flex items-center justify-center bg-leaf-deep text-white hover:opacity-90" aria-label="Send"><Send class="h-5 w-5" /></button>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>
