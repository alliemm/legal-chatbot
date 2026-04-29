<script setup lang="ts">
import { ref } from "vue";
import { useRouter } from "vue-router";
import lawBooks from "@/assets/law-books.png";
import surveyBlob from "@/assets/survey-blob.svg";

const router = useRouter();
const email = ref("");
const password = ref("");
</script>

<template>
  <div class="min-h-screen w-full bg-cream flex items-center justify-center px-4 py-10" style="font-family: Poppins, sans-serif">
    <div class="relative w-full max-w-[997px] rounded-[50px] bg-white/40 backdrop-blur-sm" style="box-shadow: -4px -4px 20px rgba(30,109,85,0.3), 4px 4px 20px rgba(30,109,85,0.3); min-height: 615px">
      <div class="grid grid-cols-1 md:grid-cols-2 items-center gap-8 px-10 py-14">
        <div class="relative flex justify-center items-center">
          <img :src="surveyBlob" alt="" aria-hidden="true" class="absolute w-[420px] pointer-events-none select-none" />
          <img :src="lawBooks" alt="Stack of law books" class="relative w-[373px] h-[373px] object-contain" />
        </div>
        <div class="flex flex-col">
          <h1 class="text-[40px] font-extrabold leading-none" style="color: #154939">Login</h1>

          <label class="mt-8 text-[15px] font-semibold" style="color: rgba(69,108,91,0.95)">Email Address</label>
          <input v-model="email" type="email" class="mt-2 h-[36px] w-full max-w-[277px] rounded-[10px] bg-white px-3 outline-none text-[14px] text-leaf-deep" style="box-shadow: 4px 4px 10px rgba(0,0,0,0.25)" />

          <label class="mt-6 text-[15px] font-semibold" style="color: #456c5b">Password</label>
          <input v-model="password" type="password" class="mt-2 h-[36px] w-full max-w-[277px] rounded-[10px] bg-white px-3 outline-none text-[14px] text-leaf-deep" style="box-shadow: 4px 4px 10px rgba(0,0,0,0.25)" />

          <div class="mt-4 flex w-full max-w-[277px] items-center justify-between text-[10px] font-semibold" style="font-family: Inter, sans-serif; color: #669782">
            <span>Don't have an account? <RouterLink to="/signup" class="hover:underline">Sign Up</RouterLink></span>
            <a href="#" class="hover:underline">Forgot Password?</a>
          </div>

          <button
           @click="router.push('/dashboard')" 
           :disabled="!email.trim() || !password.trim()"
           class="mt-6 self-end rounded-[60px] bg-leaf-deep px-8 py-2 text-white text-[15px] font-semibold transition hover:opacity-90" style="width: 121px; height: 37px">
            Login
          </button>

          <RouterLink to="/" class="mt-6 text-[12px] text-leaf-deep/70 hover:underline self-start">← Back to home</RouterLink>
        </div>
      </div>
    </div>
  </div>
</template>
