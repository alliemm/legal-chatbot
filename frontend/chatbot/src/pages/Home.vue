<script setup lang="ts">
import Logo from "@/components/Logo.vue";
import HomeStep from "@/components/HomeStep.vue";
import FeatureCircle from "@/components/FeatureCircle.vue";
import gavel from "@/assets/gavel.png";
import iconSetting from "@/assets/icons/setting.svg";
import iconActivity from "@/assets/icons/activity.svg";
import iconActivity2 from "@/assets/icons/activity-2.svg";
import iconChat from "@/assets/icons/chat.svg";
import iconChat2 from "@/assets/icons/chat-2.svg";
import iconCamera2 from "@/assets/icons/camera-2.svg";
import iconLightning from "@/assets/icons/lightning.svg";
import iconLogout from "@/assets/icons/logout.svg";
import iconInfo from "@/assets/icons/info.svg";
import divider from "@/assets/icons/divider.svg";
</script>

<template>
  <div class="min-h-screen bg-cream text-leaf-deep overflow-x-hidden">
    <!-- NAV -->
    <header class="mx-auto flex max-w-[1500px] items-center justify-between px-10 py-7">
      <div class="flex items-center gap-3">
        <Logo class="h-12 w-12 text-leaf-deep" />
        <span class="font-bold tracking-tight" style="font-family: Inter, sans-serif; font-size: 40px; color: #085041; text-shadow: 4px 4px 10px rgba(0,0,0,0.25)">Legaleye</span>
      </div>
      <nav class="hidden items-center gap-14 md:flex">
        <a href="#" class="text-[25px] font-semibold" style="font-family: Inter, sans-serif; color: #085041">Home</a>
        <a href="#" class="text-[25px] font-semibold opacity-85" style="font-family: Inter, sans-serif; color: #085041">How it works</a>
        <a href="#" class="text-[25px] font-semibold opacity-85" style="font-family: Inter, sans-serif; color: #085041">Services</a>
      </nav>
      <RouterLink to="/login" class="rounded-full bg-leaf-deep px-12 py-3 font-semibold text-white transition hover:opacity-90" style="font-family: Inter, sans-serif; font-size: 24px; box-shadow: -20px -20px 50px rgba(64,241,123,0.25), 20px 20px 50px rgba(19,235,76,0.25)">
        Login
      </RouterLink>
    </header>

    <!-- HERO -->
    <section class="relative mx-auto max-w-[1500px] px-6 pb-28 pt-6 md:px-10">
      <div class="relative mx-auto flex flex-col items-center text-center">
        <img :src="gavel" alt="" aria-hidden="true" class="pointer-events-none absolute left-1/2 top-0 w-[55%] max-w-[600px] -translate-x-1/2 -translate-y-[35%] opacity-95" />
        <h1 class="font-mega relative leading-[0.85]" style="font-family: 'Bebas Neue', sans-serif; font-weight: 400; font-size: clamp(5rem,16vw,18rem); letter-spacing: -0.05em; color: #78d5b9; text-shadow: 1px 0 0 #1e6d55, 0 1px 0 #1e6d55, -1px 0 0 #1e6d55, 0 -1px 0 #1e6d55">LEGALEYE</h1>
        <h2 class="relative mt-10 font-semibold tracking-tight" style="font-family: Inter, sans-serif; font-size: clamp(1.8rem,3.2vw,2.8rem); font-weight: 600; letter-spacing: -0.07em; color: #1e6d55; text-shadow: 0px 4px 4px rgba(0,0,0,0.25)">Legal Intelligence, Reimagined</h2>
        <p class="relative mt-5 max-w-3xl font-medium leading-relaxed" style="font-family: Poppins, sans-serif; font-size: clamp(1.1rem,1.8vw,2rem); font-weight: 500; letter-spacing: -0.06em; color: #1e6d55">
          Stay informed with AI-driven case analysis, contract intelligence, and instant legal summaries.
        </p>
        <RouterLink to="/signup" class="relative mt-10 rounded-full bg-leaf-deep px-14 py-4 font-semibold text-white transition hover:scale-[1.02]" style="font-family: Inter, sans-serif; font-size: 24px; box-shadow: -20px -20px 50px rgba(64,241,123,0.25), 20px 20px 50px rgba(19,235,76,0.25)">
          Get Started
        </RouterLink>
      </div>
    </section>

    <!-- DIVIDER -->
    <div class="flex justify-center pb-12">
      <img :src="divider" alt="" class="h-2.5 w-44 opacity-70" />
    </div>

    <!-- HOW IT WORKS -->
    <section class="relative mx-auto max-w-[1500px] px-6 py-16 md:px-10">
      <h2 class="text-center font-display font-extrabold leading-none" style="font-family: Newsreader, serif; font-size: clamp(3rem,7vw,90px); letter-spacing: -0.07em; color: #1e6d55; text-shadow: 0px 4px 4px rgba(0,0,0,0.25)">How it works</h2>
      <div class="relative mx-auto mt-20 grid max-w-6xl grid-cols-1 items-start gap-10 md:grid-cols-3">
        <HomeStep :n="1" title="Upload documents" :icon="iconActivity" body="Securely upload your documents, contracts and other files to our system" />
        <div class="md:mt-44">
          <HomeStep :n="2" title="AI Processing" :icon="iconSetting" body="Our advanced neural networks analyze your documents" />
        </div>
        <HomeStep :n="3" title="Get Insights" :icon="iconActivity2" body="Receive comprehensive analysis, actionable insights, and recommendations instantly." />
      </div>
    </section>

    <!-- DIVIDER -->
    <div class="flex justify-center py-14">
      <img :src="divider" alt="" class="h-2.5 w-44 opacity-70" />
    </div>

      <!-- SERVICES -->
    <section class="mx-auto max-w-[1500px] px-6 py-12 md:px-10">
      <div class="grid grid-cols-1 gap-14 md:grid-cols-3">
        <FeatureCircle :icon="iconActivity2" title="Legal Document Analysis" body="Upload any legal document — contracts, rental agreements, employment terms, loan documents — and get a breakdown of what it actually means, with risky clauses flagged automatically" />
        <FeatureCircle :icon="iconChat2" title="Chatbot Interaction" body="Ask any legal question and get a clear, jargon-free answer instantly. Follow up as many times as you need within the same session." />
        <FeatureCircle :icon="iconLogout" title="Personalized summaries" body="Get summaries tailored to what matters to you. Set your priorities during onboarding — data privacy, hidden fees, cancellation rights — and every analysis is filtered through your preferences." />
      </div>
    </section>

  
    <section class="mx-auto max-w-[1500px] px-6 py-20 md:px-10">
      <h2 class="text-center font-display font-extrabold leading-none" style="font-family: Newsreader, serif; font-size: clamp(2.5rem,6vw,75px); letter-spacing: -0.07em; color: #1e6d55; text-shadow: 0px 4px 4px rgba(0,0,0,0.25)">Services</h2>
      <div class="mt-16 grid grid-cols-1 gap-14 md:grid-cols-3">
        <FeatureCircle :icon="iconCamera2" title="Terms and Conditions Scanner" body='Our browser extension automatically detects Terms and Conditions pages while you browse and alerts you to concerning clauses before you click "I agree."' />
        <FeatureCircle :icon="iconChat2" title="Contract Draft Assistance" body="Drafting your own contract? Get feedback on missing clauses, risky language, and plain-English suggestions to strengthen your agreement." />
        <FeatureCircle :icon="iconInfo" title="Secure Document Storage" body="Every document you upload is stored in an encrypted, private vault — never shared, never used to train AI, and always deletable by you." />
      </div>
    </section>

    <footer class="mt-16 border-t border-leaf-deep/15 py-8 text-center text-xs text-leaf-deep/60" style="font-family: Inter, sans-serif">
      © {{ new Date().getFullYear() }} Legaleye. All rights reserved.
    </footer>

    <span class="hidden">
      <img :src="iconCamera2" alt="" />
      <img :src="iconLightning" alt="" />
    </span>
  </div>
</template>
