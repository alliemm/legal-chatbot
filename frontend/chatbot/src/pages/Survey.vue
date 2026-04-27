<script setup lang="ts">
import { ref, computed } from "vue";
import { useRouter } from "vue-router";
import gavel from "@/assets/gavel.png";
import surveyBlob from "@/assets/survey-blob.svg";

const router = useRouter();

const QUESTIONS = [
  {
    text: "How often do you deal with legal documents?",
    type: "single" as const,
    options: ["Rarely", "A few times a year", "Monthly", "Weekly"],
  },
  {
    text: "What types of documents do you most commonly need help with? (Select all that apply)",
    type: "multi" as const,
    options: [
      "Contracts and Employment agreements",
      "Leases and Rental agreements",
      "Court Filings",
      "Will and Estate agreements",
      "Business Formation documents",
      "NDAs",
    ],
  },
  {
    text: "When reviewing a legal document, what is your biggest concern? (Select all that apply)",
    type: "multi" as const,
    options: [
      "Hidden Clauses, fine print or financial risk",
      "Understanding my rights and obligations",
      "Deadlines and key dates",
      "Unfair or one-sided terms",
      "Whether the document is enforceable",
      "Plain-language explanation",
    ],
  },
  {
    text: "How comfortable are you with understanding legal Jargon?",
    type: "single" as const,
    options: ["Somewhat", "Not at all", "Very Comfortable"],
  },
  {
    text: "Have you ever worked with a lawyer before",
    type: "single" as const,
    options: ["Yes", "No", "Currently do"],
  },
] as const;

const step = ref(0);
const answers = ref<(string | string[] | null)[]>(Array(QUESTIONS.length).fill(null));

const current = computed(() => QUESTIONS[step.value]);

function isSelected(opt: string): boolean {
  const ans = answers.value[step.value];
  if (current.value.type === "multi") return Array.isArray(ans) && ans.includes(opt);
  return ans === opt;
}

function select(opt: string) {
  if (current.value.type === "multi") {
    const prev = (answers.value[step.value] as string[] | null) ?? [];
    answers.value[step.value] = prev.includes(opt)
      ? prev.filter((o) => o !== opt)
      : [...prev, opt];
  } else {
    answers.value[step.value] = answers.value[step.value] === opt ? null : opt;
  }
}

const canAdvance = computed(() => {
  const ans = answers.value[step.value];
  if (current.value.type === "multi") return Array.isArray(ans) && ans.length > 0;
  return ans !== null;
});

function next() {
  if (!canAdvance.value) return;
  if (step.value < QUESTIONS.length - 1) {
    step.value++;
  } else {
    router.push("/dashboard");
  }
}
</script>

<template>
  <div class="min-h-screen w-full bg-cream flex items-center justify-center px-4 py-10" style="font-family: Poppins, sans-serif">
    <div
      class="relative w-full max-w-[997px] rounded-[50px] overflow-hidden"
      style="background: rgba(120,213,185,0.55); box-shadow: -4px -4px 20px rgba(30,109,85,0.3), 4px 4px 20px rgba(30,109,85,0.3); min-height: 615px"
    >
      <div class="grid grid-cols-1 md:grid-cols-2 items-center gap-8 px-10 py-14">
        <!-- Left: gavel -->
        <div class="relative flex justify-center items-center">
          <img :src="surveyBlob" alt="" aria-hidden="true" class="absolute w-[420px] h-auto pointer-events-none select-none" />
          <img :src="gavel" alt="Gavel" class="relative w-[320px] h-auto object-contain" />
        </div>

        <!-- Right: question -->
        <div class="flex flex-col items-stretch">
          <h2 class="text-[22px] font-extrabold leading-snug" style="color: #154939; text-shadow: 0px 2px 6px rgba(0,0,0,0.15)">
            {{ current.text }}
          </h2>

          <div class="mt-6 flex flex-col gap-3 w-full max-w-[320px]">
            <button
              v-for="opt in current.options"
              :key="opt"
              @click="select(opt)"
              class="rounded-[15px] px-4 h-[49px] text-[14px] font-extrabold text-center transition hover:scale-[1.02] text-left px-5"
              :style="{
                backgroundColor: isSelected(opt) ? '#0e5c4a' : '#1e6d55',
                boxShadow: isSelected(opt)
                  ? 'inset 0 0 0 2px #86e3ce, 4px 4px 10px rgba(0,0,0,0.25)'
                  : '4px 4px 10px rgba(0,0,0,0.25)',
                color: '#86e3ce',
                textShadow: '0px 2px 4px rgba(0,0,0,0.2)',
              }"
            >{{ opt }}</button>
          </div>

          <button
            @click="next"
            :disabled="!canAdvance"
            class="mt-8 self-end rounded-[60px] bg-leaf-deep px-8 py-2 text-white text-[15px] font-semibold transition hover:opacity-90 disabled:opacity-40"
            style="width: 121px; height: 37px"
          >
            Next
          </button>
        </div>
      </div>
    </div>
  </div>
</template>
