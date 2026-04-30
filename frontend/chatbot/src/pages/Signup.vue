<script setup lang="ts">
import { ref } from "vue";
import { useRouter } from "vue-router";
import lawBooks from "@/assets/law-books.png";
import surveyBlob from "@/assets/survey-blob.svg";
import axios from "axios";

const router = useRouter();
const name = ref("");
const email = ref("");
const password = ref("");
const confirm = ref("");
const isLoading = ref(false);
const error = ref("");

const signUp = async() =>{
  isLoading.value = true;
  error.value = "";
  try{
    const response = await axios.post('https://legal-chatbot-4t8e.onrender.com/signup', {
      "name": name.value,
      "email": email.value,
      "password": password.value
    });
    const token = response.data.token;
    localStorage.setItem('user_token', token);
    
    router.push('survey');
  }catch(err: any){
    error.value = "An error occurred during sign up";
    console.error("Error during signup", err);
  }finally{
    isLoading.value = false;
  }

}
</script>

<template>
  <div class="min-h-screen w-full bg-cream flex items-center justify-center px-4 py-10" style="font-family: Poppins, sans-serif">
    <div
      class="relative w-full max-w-[997px] rounded-[50px] bg-white/40 backdrop-blur-sm"
      style="box-shadow: -4px -4px 20px rgba(30,109,85,0.3), 4px 4px 20px rgba(30,109,85,0.3); min-height: 615px"
    >
      <div class="grid grid-cols-1 md:grid-cols-2 items-center gap-8 px-10 py-14">
        <!-- Left: image -->
        <div class="relative flex justify-center items-center">
          <img :src="surveyBlob" alt="" aria-hidden="true" class="absolute w-[420px] h-auto pointer-events-none select-none" />
          <img :src="lawBooks" alt="Stack of law books" class="relative w-[373px] h-[373px] object-contain" />
        </div>

        <!-- Right: form -->
        <div class="flex flex-col">
          <h1 class="text-[40px] font-extrabold leading-none" style="color: #154939">Sign Up</h1>

          <label class="mt-6 text-[15px] font-semibold" style="color: rgba(69,108,91,0.95)">Full Name</label>
          <input v-model="name" type="text" class="mt-2 h-[36px] w-full max-w-[277px] rounded-[10px] bg-white px-3 outline-none text-[14px] text-leaf-deep" style="box-shadow: 4px 4px 10px rgba(0,0,0,0.25)" />

          <label class="mt-4 text-[15px] font-semibold" style="color: rgba(69,108,91,0.95)">Email Address</label>
          <input v-model="email" type="email" class="mt-2 h-[36px] w-full max-w-[277px] rounded-[10px] bg-white px-3 outline-none text-[14px] text-leaf-deep" style="box-shadow: 4px 4px 10px rgba(0,0,0,0.25)" />

          <label class="mt-4 text-[15px] font-semibold" style="color: #456c5b">Password</label>
          <input v-model="password" type="password" class="mt-2 h-[36px] w-full max-w-[277px] rounded-[10px] bg-white px-3 outline-none text-[14px] text-leaf-deep" style="box-shadow: 4px 4px 10px rgba(0,0,0,0.25)" />

          <label class="mt-4 text-[15px] font-semibold" style="color: #456c5b">Confirm Password</label>
          <input v-model="confirm" type="password" class="mt-2 h-[36px] w-full max-w-[277px] rounded-[10px] bg-white px-3 outline-none text-[14px] text-leaf-deep" style="box-shadow: 4px 4px 10px rgba(0,0,0,0.25)" />

          <div class="mt-4 flex w-full max-w-[277px] items-center justify-between text-[10px] font-semibold" style="font-family: Inter, sans-serif; color: #669782">
            <span>Already have an account?</span>
            <RouterLink to="/login" class="hover:underline">Login</RouterLink>
          </div>

          <button
            @click="signUp"
            :disabled="isLoading || !name.trim() || !email.trim() || !password.trim() || !confirm.trim() || password !== confirm"
            class="mt-6 self-end rounded-[60px] bg-leaf-deep px-8 py-2 text-white text-[15px] font-semibold transition hover:opacity-90"
            style="width: 140px; height: 37px"
          >
            Sign Up
          </button>

          <RouterLink to="/" class="mt-6 text-[12px] text-leaf-deep/70 hover:underline self-start">← Back to home</RouterLink>
        </div>
      </div>
    </div>
  </div>
</template>
