<script setup lang="ts">
import Logo from "@/components/Logo.vue";
import { Plus, Search, Settings, MoreVertical, X, LogOut } from "lucide-vue-next";
import axios from "axios";
import { onMounted, ref } from "vue";
import { useRouter } from "vue-router";
import { nanoid } from "nanoid";
import { API_BASE } from "@/api";

const router = useRouter();

type Notebook = { id: string; title: string; date: string; sources: string; isNew?: boolean };

const NOTEBOOKS = ref<Notebook[]>([]);

const error = ref("");
const showNamePopup = ref(false);
const notebookName = ref("");

function openPopup() {
  notebookName.value = "";
  showNamePopup.value = true;
}

function handleCreate() {
  if (!notebookName.value.trim()) return;
  showNamePopup.value = false;
  const newId = nanoid(25);
  router.push({
    path: `/chatbot/${newId}`,
    state: { notebookTitle: notebookName.value.trim() }
  });
}
function openOld(cur:Notebook){
  router.push({
    path: `/chatbot/${cur.id}`,
    state: { notebookTitle: cur.title }
  });
}

function logout() {
  localStorage.removeItem("user_token");
  router.push("/login");
}

const getNotebooks = async () => {
  try {
    const token = localStorage.getItem("user_token");
    if (!token) {
      error.value = "Please login to view this page";
      router.push("/login");
      return;
    }
    const response = await axios.get(`${API_BASE}/notebooks`, {
      headers: {
        Authorization: token,
      },
    });

    const data = response.data;
    const fetchedNotebooks = Object.entries(data).map(([id, info]: any) => ({
      id: id,
      title: info[0],
      date: info[1],
      sources: info[2] + " sources",
      isNew: false,
    }));
    NOTEBOOKS.value = [
      { id: "new", title: "Add new chat", date: "", sources: "", isNew: true },
      ...fetchedNotebooks,
    ];
  } catch (err) {
    console.error(err);
    error.value = "Failed to load notebooks.";
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
          <button type="button" @click="openPopup" class="hidden md:flex items-center gap-2 h-[55px] rounded-full px-6 text-[18px]" style="background-color: #0e5c4a; color: #86e3ce; box-shadow: 4px 4px 15px rgba(0,0,0,0.25); font-family: Inter, sans-serif">
            <span>Add new chat</span>
            <Plus class="h-5 w-5" />
          </button>
          <RouterLink to="/profile" class="hidden md:flex items-center gap-2 h-[55px] rounded-full px-6 text-[18px]" style="background-color: rgba(184,224,212,0.8); color: #0e5c4a; box-shadow: 4px 4px 15px rgba(0,0,0,0.25); font-family: Inter, sans-serif">
          <Settings class="h-5 w-5" />
          <span>Settings</span>
        </RouterLink>
        <button @click="logout" class="hidden md:flex items-center gap-2 h-[55px] rounded-full px-6 text-[18px]" style="background-color: #0e5c4a; color: #86e3ce; box-shadow: 4px 4px 15px rgba(0,0,0,0.25); font-family: Inter, sans-serif">
          <LogOut class="h-5 w-5" />
          <span>Logout</span>
        </button>
        </div>
      </div>
    </header>

    <main class="mx-auto max-w-[1440px] px-8 py-12">
      <h1 class="text-[40px] font-extrabold" style="color: #154939">Recent notebooks</h1>
      <div class="mt-10 grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-3 gap-10">
        <template v-for="notebook in NOTEBOOKS" :key="notebook.id">

          <div
              v-if="notebook.isNew"
              @click="openPopup"
              class="group relative flex flex-col items-center justify-center rounded-[50px] aspect-[368/348] transition hover:scale-[1.02] cursor-pointer"
              style="background-color: rgba(120,213,185,0.7)"
          >
            <Plus class="h-24 w-24" style="color: #154939" :stroke-width="2.5" />
            <span class="mt-4 text-[32px] font-extrabold text-center px-6" style="color: #154939">
          Add new chat
        </span>
          </div>

          <div
              v-else
              @click="openOld(notebook)"
              class="group relative flex flex-col rounded-[50px] aspect-[368/348] p-8 transition hover:scale-[1.02] cursor-pointer"
              style="background-color: rgba(120,213,185,0.7)"
          >
            <button
                class="absolute top-5 right-5 text-leaf-deep/70 hover:text-leaf-deep z-10"
                @click.stop
                aria-label="More options"
            >
              <MoreVertical class="h-6 w-6" />
            </button>

            <div class="flex-1 flex items-center">
              <h3 class="text-[36px] font-extrabold leading-tight" style="color: #154939">
                {{ notebook.title }}
              </h3>
            </div>

            <div class="mt-auto" style="color: rgba(118,122,121,0.85)">
              <div class="text-[18px] font-extrabold">{{ notebook.date }}</div>
              <div class="text-[18px] font-extrabold mt-1">{{ notebook.sources }}</div>
            </div>
          </div>

        </template>
      </div>
    </main>

    <!-- NAME NOTEBOOK POPUP -->
    <Teleport to="body">
      <div
        v-if="showNamePopup"
        class="fixed inset-0 z-50 flex items-center justify-center px-4"
        style="background-color: rgba(0,0,0,0.4)"
        @click="showNamePopup = false"
      >
        <div
          class="w-full max-w-[520px] rounded-[28px] p-8 relative"
          style="background-color: #78d5b9; box-shadow: 0 20px 50px rgba(0,0,0,0.25)"
          @click.stop
        >
          <button type="button" @click="showNamePopup = false" class="absolute top-4 right-4 text-leaf-deep/70 hover:text-leaf-deep" aria-label="Close">
            <X class="h-5 w-5" />
          </button>

          <div class="flex items-center gap-2 mb-6">
            <Logo class="h-7 w-7 text-leaf-deep" />
            <span class="font-bold" style="font-family: Inter, sans-serif; font-size: 18px; color: #085041">Legaleye</span>
          </div>

          <label for="notebook-name" class="block text-[20px] font-bold mb-3" style="color: #0d2b22">Name your notebook :</label>
          <input
            id="notebook-name"
            v-model="notebookName"
            type="text"
            autofocus
            placeholder="Type text here"
            maxlength="100"
            class="w-full rounded-full bg-white px-6 py-3 text-[16px] outline-none border border-black/10 focus:border-leaf-deep"
            style="color: #0d2b22"
            @keydown.enter="handleCreate"
          />

          <div class="mt-6 flex justify-end">
            <button
              type="button"
              @click="handleCreate"
              :disabled="!notebookName.trim()"
              class="flex items-center gap-2 h-[48px] rounded-full px-6 text-[16px] disabled:opacity-50"
              style="background-color: #0e5c4a; color: #86e3ce; box-shadow: 4px 4px 15px rgba(0,0,0,0.25); font-family: Inter, sans-serif"
            >
              <span>Add new chat</span>
              <Plus class="h-4 w-4" />
            </button>
          </div>
        </div>
      </div>
    </Teleport>
  </div>
</template>