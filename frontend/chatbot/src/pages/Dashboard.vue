<script setup lang="ts">
import Logo from "@/components/Logo.vue";
import { Plus, Search, Settings, MoreVertical } from "lucide-vue-next";
import axios from "axios";
import {onMounted, ref} from "vue";
import {useRouter} from "vue-router";
const router = useRouter();

type Notebook = { id: string; title: string; date: string; sources: string; isNew?: boolean };

const NOTEBOOKS = ref<Notebook[]>([]);

const isLoading = ref(false);
const error = ref("");
const getNotebooks = async () => {
  try {
    isLoading.value = true;
    const token = localStorage.getItem('user_token');
    if(!token){
      error.value = "Please login to view this page"
      router.push('/login');
      return
    }
    const response = await axios.get('https://legal-chatbot-4t8e.onrender.com/notebooks', {
      headers: {
        Authorization: token
      }
    });

    const data = response.data;

    // Transform the Backend Object { "id": ["Title", "Date"] } into our Array
    const fetchedNotebooks = Object.entries(data)
        .map(([id, info]: any) => ({
          id: id,
          title: info[0], // Title from index 0
          date: info[1],  // Date from index 1
          sources: "0 sources", // Backend doesn't provide this yet, so we'll default it
          isNew: false
        }));

    NOTEBOOKS.value = [
      { id: "new", title: "Add new chat", date: "", sources: "", isNew: true },
      { id: "2", title: "Lease Agreement", date: "2 April 2026", sources: "3 sources" },
      { id: "3", title: "School Assignment", date: "2 April 2026", sources: "3 sources" },
      { id: "4", title: "School Assignment", date: "2 April 2026", sources: "2 sources" },
      { id: "5", title: "Job offer contract", date: "2 April 2026", sources: "2 sources" },
      ...fetchedNotebooks
    ];
  } catch (err) {
    console.error(err);
    error.value = "Failed to load notebooks.";
  } finally {
    isLoading.value = false;
  }
};

onMounted(() => {
  getNotebooks();
});
</script>

<template>
  <div class="min-h-screen w-full bg-cream" style="font-family: Poppins, sans-serif">
    <header class="w-full bg-cream" style="box-shadow: 0 4px 10px rgba(0,0,0,0.10)">
      <div class="mx-auto flex max-w-[1440px] items-center justify-between px-8 py-4">
        <RouterLink to="/" class="flex items-center gap-3">
          <Logo class="h-12 w-12 text-leaf-deep" />
          <span class="font-bold tracking-tight" style="font-family: Inter, sans-serif; font-size: 32px; color: #085041; text-shadow: 4px 4px 10px rgba(0,0,0,0.25)">Legaleye</span>
        </RouterLink>
        <div class="flex items-center gap-4">
          <button class="h-[55px] w-[55px] rounded-full hidden md:flex items-center justify-center" style="background-color: #b8e0d4; box-shadow: 4px 4px 15px rgba(0,0,0,0.25)" aria-label="Search">
            <Search class="h-6 w-6 text-leaf-deep" />
          </button>
          <RouterLink to="/chatbot" class="hidden md:flex items-center gap-2 h-[55px] rounded-full px-6 text-[18px]" style="background-color: #0e5c4a; color: #86e3ce; box-shadow: 4px 4px 15px rgba(0,0,0,0.25); font-family: Inter, sans-serif">
            <span>Add new chat</span>
            <Plus class="h-5 w-5" />
          </RouterLink>
          <button class="hidden md:flex items-center gap-2 h-[55px] rounded-full px-6 text-[18px]" style="background-color: rgba(184,224,212,0.8); color: #0e5c4a; box-shadow: 4px 4px 15px rgba(0,0,0,0.25); font-family: Inter, sans-serif">
            <Settings class="h-5 w-5" />
            <span>Settings</span>
          </button>
        </div>
      </div>
    </header>

    <main class="mx-auto max-w-[1440px] px-8 py-12">
      <h1 class="text-[40px] font-extrabold" style="color: #154939">Recent notebooks</h1>
      <div class="mt-10 grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-3 gap-10">
        <template v-for="(notebook, i) in NOTEBOOKS" :key="i">
          <RouterLink
            v-if="notebook.isNew"
            to="/chatbot"
            class="group relative flex flex-col items-center justify-center rounded-[50px] aspect-[368/348] transition hover:scale-[1.02]"
            style="background-color: rgba(120,213,185,0.7)"
          >
            <Plus class="h-24 w-24" style="color: #154939" :stroke-width="2.5" />
            <span class="mt-4 text-[32px] font-extrabold text-center px-6" style="color: #154939">Add new chat</span>
          </RouterLink>
          <RouterLink
            v-else
            to="/chatbot"
            class="group relative flex flex-col rounded-[50px] aspect-[368/348] p-8 transition hover:scale-[1.02]"
            style="background-color: rgba(120,213,185,0.7)"
          >
            <button class="absolute top-5 right-5 text-leaf-deep/70 hover:text-leaf-deep" @click.prevent aria-label="More options">
              <MoreVertical class="h-6 w-6" />
            </button>
            <div class="flex-1 flex items-center">
              <h3 class="text-[36px] font-extrabold leading-tight" style="color: #154939">{{ notebook.title }}</h3>
            </div>
            <div class="mt-auto" style="color: rgba(118,122,121,0.85)">
              <div class="text-[18px] font-extrabold">{{ notebook.date }}</div>
              <div class="text-[18px] font-extrabold mt-1">{{ notebook.sources }}</div>
            </div>
          </RouterLink>
        </template>
      </div>
    </main>
  </div>
</template>
