<script setup lang="ts">
import { onMounted, ref } from "vue";
import Logo from "@/components/Logo.vue";
import { Plus, Settings, Search, Send, Smile, MessageSquare, ChevronDown, FileText } from "lucide-vue-next";
import axios from "axios";
import { useRouter, useRoute } from "vue-router";
import { API_BASE } from "@/api";
import MarkdownIt from "markdown-it";

type Source = { name: string; active?: boolean };
type Message = { from: "user" | "model"; text: string };

const sources = ref<Source[]>([]);
const messages = ref<Message[]>([]);

const md = new MarkdownIt();
const renderMarkdown = (text: string) => md.render(text);

const isThinking = ref(false);
const input = ref("");

const router = useRouter();
const route = useRoute();

const chatID = route.params.id as string;
const title = ref('New Notebook');
const error = ref("");
const showFileUpload = ref(false);
const selectedFile = ref<File | null>(null);
const isUploading = ref(false);
const uploadStatus = ref('');

const handleFileChange = (event: Event) => {
  const target = event.target as HTMLInputElement;
  if (target.files && target.files.length > 0) {
    selectedFile.value = target.files[0];
  }
};

const uploadFile = async () => {
  if (!selectedFile.value) return;

  const formData = new FormData();
  formData.append('chatid', chatID);
  formData.append('InputFile', selectedFile.value);

  try {
    isUploading.value = true;
    uploadStatus.value = '';
    const token = localStorage.getItem("user_token");

    await axios.post(`${API_BASE}/upload`, formData, {
      headers: {
        'Authorization': token,
        'Content-Type': 'multipart/form-data'
      }
    });

    uploadStatus.value = 'Successfully uploaded file';
    sources.value.push({ name: selectedFile.value.name });

    setTimeout(() => {
      showFileUpload.value = false;
      uploadStatus.value = '';
      selectedFile.value = null;
    }, 1500);

  } catch (err) {
    uploadStatus.value = 'Upload failed';
    console.error(err);
  } finally {
    isUploading.value = false;
  }
};

async function send() {
  const v = input.value.trim();
  if (!v || isThinking.value) return;
  messages.value.push({ from: "user", text: v });
  isThinking.value = true;
  const userQuery = v;
  input.value = "";
  try {
    const token = localStorage.getItem("user_token");
    if (!token) {
      error.value = "Please login to view this page";
      router.push("/login");
      return;
    }

    const aiResponse = await axios.post(
      `${API_BASE}/chat`,
      {
        message: userQuery,
        title : title.value
      },
      {
        headers: {
          Authorization: token,
          ChatID: chatID,
        },
      },
    );

    const data = aiResponse.data;
    messages.value.push({ from: "model", text: data.reply });
  } catch (err) {
    console.error("Failed to connect to backend:", err);
    messages.value.push({
      from: "model",
      text: "Sorry, I'm having trouble connecting to the Legaleye server.",
    });
  } finally {
    isThinking.value = false;
  }
}

const getMessages = async () => {
  try {
    const token = localStorage.getItem("user_token");
    if (!token) {
      error.value = "Please login to view this page";
      router.push("/login");
      return;
    }
    const response = await axios.get(`${API_BASE}/chatHistory`, {
      headers: {
        Authorization: token,
        ChatID: chatID,
      },
    });
    const data = response.data;
    const chatHistory = data.map((message: any) => ({
      from: message[0] === "user" ? "user" : "model",
      text: message[1],
    }));
    messages.value.push(...chatHistory);
  } catch (err) {
    console.error(err);
    error.value = "Failed to load chat history.";
  }
};
const getSources = async () => {
  try {
    const token = localStorage.getItem("user_token");
    if (!token) {
      error.value = "Please login to view this page";
      router.push("/login");
      return;
    }
    const response = await axios.get(`${API_BASE}/sourcesHistory`, {
      headers: {
        Authorization: token,
        ChatID: chatID,
      },
    });
    const data = response.data;
    sources.value = data.map((name: string) => ({ name, active: false }));
  } catch (err) {
    console.error(err);
    error.value = "Failed to load chat history.";
  }
};
const saveTitle = async () => {
  const navigationState = window.history.state;

  if (navigationState && navigationState.notebookTitle) {
    title.value = navigationState.notebookTitle;
  }
  console.log("Title", title.value);
}
onMounted(() => {
  getMessages();
  saveTitle();
  getSources();
});
</script>

<template>
  <div class="min-h-screen w-full bg-cream flex" style="font-family: Montserrat, Inter, sans-serif">

    <div v-if="showFileUpload" class="fixed inset-0 z-50 flex items-center justify-center bg-black/50 backdrop-blur-sm">
      <div class="bg-white p-8 rounded-2xl shadow-2xl w-full max-w-md border-4 border-leaf-deep relative">
        <button @click="showFileUpload = false" class="absolute top-4 right-4 text-gray-500 hover:text-black text-xl">✕</button>
        <h3 class="text-2xl font-bold mb-4 text-leaf-deep">Upload Document</h3>
        <div class="flex flex-col gap-4">
          <input type="file" @change="handleFileChange" class="block w-full text-sm text-gray-500 file:mr-4 file:py-2 file:px-4 file:rounded-full file:border-0 file:text-sm file:font-semibold file:bg-mint-soft file:text-leaf-deep hover:file:bg-mint-soft/60" />
          <button @click="uploadFile" :disabled="!selectedFile || isUploading" class="bg-leaf-deep text-white py-3 rounded-xl font-bold disabled:opacity-50 disabled:cursor-not-allowed hover:bg-opacity-90 transition-all">
            {{ isUploading ? 'Uploading...' : 'Confirm Upload' }}
          </button>
          <p v-if="uploadStatus" class="text-center text-sm font-medium mt-2" :class="uploadStatus.includes('Successfully') ? 'text-green-600' : 'text-red-600'">
            {{ uploadStatus }}
          </p>
        </div>
      </div>
    </div>

    <!-- SIDEBAR -->
    <aside class="hidden md:flex flex-col w-[360px] m-6 rounded-2xl text-white/80 relative overflow-hidden" style="background-color: #0b4538; border: 3px solid #f8f8ff; box-shadow: 0 4px 6px rgba(15,11,11,0.1); min-height: calc(100vh - 48px)">
      <div class="absolute inset-1 rounded-2xl pointer-events-none" style="border: 3px solid #f5f5f5" />
      <div class="relative p-6 flex flex-col gap-3 flex-1">
        <button @click="showFileUpload = true" class="flex items-center gap-3 px-4 h-[44px] rounded-md text-white text-[15px] font-medium" style="background-color: #1e6d55; border: 2px solid #282934">
          <Plus class="h-5 w-5" />
          <span>Add new source</span>
        </button>
        <button v-for="(s, i) in sources" :key="i" class="flex items-center justify-end gap-2 px-4 h-[44px] rounded-md text-white text-[15px] font-medium text-right" :style="{ backgroundColor: s.active ? '#1e6d55' : '#0a0c19', border: '2px solid #282934' }">
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
            <button class="h-12 w-12 rounded-full flex items-center justify-center hover:bg-mint-soft/40"><Search class="h-6 w-6" /></button>
            <button class="h-12 w-12 rounded-full flex items-center justify-center hover:bg-mint-soft/40"><Settings class="h-6 w-6" /></button>
          </div>
        </div>
      </header>

      <div class="px-6 md:px-10 pt-6">
        <h1 class="text-[32px] md:text-[40px] font-extrabold" style="font-family: Poppins, sans-serif; color: #154939">{{title}}</h1>
      </div>

      <div class="flex-1 px-6 md:px-10 py-8 overflow-y-auto">
        <div class="mx-auto max-w-3xl flex flex-col gap-6">
          <div v-for="(m, i) in messages" :key="i" class="flex items-end gap-3" :class="m.from === 'user' ? 'justify-end' : 'justify-start'">
            <div v-if="m.from === 'model'" class="h-10 w-10 rounded-lg bg-leaf-deep flex items-center justify-center text-white text-sm font-bold">L</div>
            <div class="relative max-w-[80%] rounded-[10px] px-5 py-4 text-[18px] leading-snug bg-white markdown-container" style="border: 1px solid #ddd; color: rgba(41,41,41,0.85); box-shadow: 0px 1px 2.29px rgba(0,0,0,0.13)">
              <div v-if="m.from === 'model'" v-html="renderMarkdown(m.text)"></div>
              <span v-else>{{ m.text }}</span>
            </div>
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
            <button class="h-11 w-11 rounded-full flex items-center justify-center hover:bg-mint-soft/40"><MessageSquare class="h-5 w-5" /></button>
            <button class="h-11 w-11 rounded-full flex items-center justify-center hover:bg-mint-soft/40"><Smile class="h-5 w-5" /></button>
            <button @click="send" class="h-11 w-11 rounded-full flex items-center justify-center bg-leaf-deep text-white hover:opacity-90"><Send class="h-5 w-5" /></button>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped>
/* Styles for the rendered Markdown */
.markdown-container :deep(h1),
.markdown-container :deep(h2),
.markdown-container :deep(h3) {
  font-weight: 700;
  margin-top: 0.5rem;
  margin-bottom: 0.5rem;
  color: #0b4538;
}

.markdown-container :deep(p) {
  margin-bottom: 0.75rem;
}

.markdown-container :deep(p:last-child) {
  margin-bottom: 0;
}

.markdown-container :deep(ul),
.markdown-container :deep(ol) {
  margin-left: 1.5rem;
  margin-bottom: 0.75rem;
}

.markdown-container :deep(li) {
  list-style-type: disc;
}

.markdown-container :deep(strong) {
  font-weight: 800;
  color: #000;
}
</style>

