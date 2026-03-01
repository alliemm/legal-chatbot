<template>
  <div class="survey-page">
    <div class="survey-card">

      <!-- Header -->
      <div class="survey-header">
        <div class="brand">
          <div class="brand-icon">
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
              <path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z"/>
              <polyline points="14 2 14 8 20 8"/>
              <circle cx="10" cy="15" r="2.5"/>
              <line x1="12" y1="17" x2="14.5" y2="19.5"/>
            </svg>
          </div>
          <span class="brand-name">LexAssist</span>
        </div>

        <h1 class="title">Quick survey</h1>
        <p class="subtitle">Help us personalize your experience — takes 2 minutes</p>

        <!-- Step dots -->
        <div class="steps">
          <div
            v-for="(_, i) in questions"
            :key="i"
            class="step-dot"
            :class="{
              active: i === current,
              done: i < current
            }"
          >
            <svg v-if="i < current" class="check" viewBox="0 0 12 12" fill="none">
              <polyline points="2,6 5,9 10,3" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/>
            </svg>
            <span v-else>{{ i + 1 }}</span>
          </div>
        </div>

        <!-- Progress bar -->
        <div class="progress-track">
          <div class="progress-fill" :style="{ width: progressPct + '%' }"></div>
        </div>
        <p class="progress-label">Question {{ current + 1 }} of {{ questions.length }}</p>
      </div>

      <!-- Questions -->
      <form @submit.prevent="submit" class="form">
        <Transition name="slide" mode="out-in">
          <div :key="current" class="question">

            <p class="question-text">{{ questionTexts[current] }}</p>
            <p v-if="questionHints[current]" class="hint">{{ questionHints[current] }}</p>

            <!-- Q1 -->
            <div v-if="current === 0" class="option-group">
              <label v-for="opt in q1Options" :key="opt" class="option-label" :class="{ selected: answers.q1 === opt }">
                <input type="radio" v-model="answers.q1" :value="opt" />
                <span class="option-text">{{ opt }}</span>
                <svg v-if="answers.q1 === opt" class="option-check" viewBox="0 0 20 20" fill="currentColor">
                  <path fill-rule="evenodd" d="M10 18a8 8 0 100-16 8 8 0 000 16zm3.707-9.293a1 1 0 00-1.414-1.414L9 10.586 7.707 9.293a1 1 0 00-1.414 1.414l2 2a1 1 0 001.414 0l4-4z" clip-rule="evenodd"/>
                </svg>
              </label>
            </div>

            <!-- Q2 -->
            <div v-if="current === 1" class="option-group">
              <label v-for="opt in q2Options" :key="opt" class="option-label" :class="{ selected: answers.q2 === opt }">
                <input type="radio" v-model="answers.q2" :value="opt" />
                <span class="option-text">{{ opt }}</span>
                <svg v-if="answers.q2 === opt" class="option-check" viewBox="0 0 20 20" fill="currentColor">
                  <path fill-rule="evenodd" d="M10 18a8 8 0 100-16 8 8 0 000 16zm3.707-9.293a1 1 0 00-1.414-1.414L9 10.586 7.707 9.293a1 1 0 00-1.414 1.414l2 2a1 1 0 001.414 0l4-4z" clip-rule="evenodd"/>
                </svg>
              </label>
            </div>

            <!-- Q3 -->
            <div v-if="current === 2" class="option-group">
              <label v-for="opt in q3Options" :key="opt" class="option-label" :class="{ selected: answers.q3 === opt }">
                <input type="radio" v-model="answers.q3" :value="opt" />
                <span class="option-text">{{ opt }}</span>
                <svg v-if="answers.q3 === opt" class="option-check" viewBox="0 0 20 20" fill="currentColor">
                  <path fill-rule="evenodd" d="M10 18a8 8 0 100-16 8 8 0 000 16zm3.707-9.293a1 1 0 00-1.414-1.414L9 10.586 7.707 9.293a1 1 0 00-1.414 1.414l2 2a1 1 0 001.414 0l4-4z" clip-rule="evenodd"/>
                </svg>
              </label>
            </div>

            <!-- Q4 multi -->
            <div v-if="current === 3" class="option-group two-col">
              <label v-for="opt in q4Options" :key="opt" class="option-label" :class="{ selected: answers.q4.includes(opt) }">
                <input type="checkbox" :value="opt" v-model="answers.q4" />
                <span class="option-text">{{ opt }}</span>
                <svg v-if="answers.q4.includes(opt)" class="option-check" viewBox="0 0 20 20" fill="currentColor">
                  <path fill-rule="evenodd" d="M10 18a8 8 0 100-16 8 8 0 000 16zm3.707-9.293a1 1 0 00-1.414-1.414L9 10.586 7.707 9.293a1 1 0 00-1.414 1.414l2 2a1 1 0 001.414 0l4-4z" clip-rule="evenodd"/>
                </svg>
              </label>
            </div>

            <!-- Q5 multi max 3 -->
            <div v-if="current === 4" class="option-group two-col">
              <label
                v-for="opt in q5Options"
                :key="opt"
                class="option-label"
                :class="{
                  selected: answers.q5.includes(opt),
                  disabled: answers.q5.length >= 3 && !answers.q5.includes(opt)
                }"
              >
                <input
                  type="checkbox"
                  :value="opt"
                  v-model="answers.q5"
                  :disabled="answers.q5.length >= 3 && !answers.q5.includes(opt)"
                />
                <span class="option-text">{{ opt }}</span>
                <svg v-if="answers.q5.includes(opt)" class="option-check" viewBox="0 0 20 20" fill="currentColor">
                  <path fill-rule="evenodd" d="M10 18a8 8 0 100-16 8 8 0 000 16zm3.707-9.293a1 1 0 00-1.414-1.414L9 10.586 7.707 9.293a1 1 0 00-1.414 1.414l2 2a1 1 0 001.414 0l4-4z" clip-rule="evenodd"/>
                </svg>
              </label>
            </div>

            <!-- Q6 -->
            <div v-if="current === 5" class="option-group">
              <label v-for="opt in q6Options" :key="opt" class="option-label" :class="{ selected: answers.q6 === opt }">
                <input type="radio" v-model="answers.q6" :value="opt" />
                <span class="option-text">{{ opt }}</span>
                <svg v-if="answers.q6 === opt" class="option-check" viewBox="0 0 20 20" fill="currentColor">
                  <path fill-rule="evenodd" d="M10 18a8 8 0 100-16 8 8 0 000 16zm3.707-9.293a1 1 0 00-1.414-1.414L9 10.586 7.707 9.293a1 1 0 00-1.414 1.414l2 2a1 1 0 001.414 0l4-4z" clip-rule="evenodd"/>
                </svg>
              </label>
            </div>

            <!-- Q7 -->
            <div v-if="current === 6" class="option-group">
              <label v-for="opt in q7Options" :key="opt" class="option-label" :class="{ selected: answers.q7 === opt }">
                <input type="radio" v-model="answers.q7" :value="opt" />
                <span class="option-text">{{ opt }}</span>
                <svg v-if="answers.q7 === opt" class="option-check" viewBox="0 0 20 20" fill="currentColor">
                  <path fill-rule="evenodd" d="M10 18a8 8 0 100-16 8 8 0 000 16zm3.707-9.293a1 1 0 00-1.414-1.414L9 10.586 7.707 9.293a1 1 0 00-1.414 1.414l2 2a1 1 0 001.414 0l4-4z" clip-rule="evenodd"/>
                </svg>
              </label>
            </div>

          </div>
        </Transition>

        <transition name="err">
          <p v-if="error" class="error">{{ error }}</p>
        </transition>

        <div class="nav-buttons">
          <button v-if="current > 0" type="button" class="btn-back" @click="current--">
            <svg viewBox="0 0 20 20" fill="none" stroke="currentColor" stroke-width="2">
              <polyline points="12,4 6,10 12,16" stroke-linecap="round" stroke-linejoin="round"/>
            </svg>
            Back
          </button>

          <button v-if="current < questions.length - 1" type="button" class="btn-next" @click="next">
            Continue
            <svg viewBox="0 0 20 20" fill="none" stroke="currentColor" stroke-width="2">
              <polyline points="8,4 14,10 8,16" stroke-linecap="round" stroke-linejoin="round"/>
            </svg>
          </button>

          <button v-if="current === questions.length - 1" type="submit" class="btn-next" :disabled="loading">
            <span v-if="loading" class="spinner"></span>
            {{ loading ? 'Submitting…' : 'Finish' }}
          </button>
        </div>
      </form>
    </div>
  </div>
</template>

<script>
import axios from 'axios'

export default {
  data() {
    return {
      current: 0,
      loading: false,
      error: '',
      questions: [0, 1, 2, 3, 4, 5, 6],
      questionTexts: [
        'What best describes you?',
        'Have you ever worked with a lawyer before?',
        'How often do you deal with legal documents?',
        'What types of documents do you most commonly need help with?',
        'When reviewing a legal document, what is your biggest concern?',
        'How comfortable are you with understanding legal jargon?',
        'What outcome do you most want from this tool?'
      ],
      questionHints: [
        '',
        '',
        '',
        'Select all that apply',
        'Select up to 3',
        '',
        'Select one'
      ],
      q1Options: ['Individual', 'Small business owner', 'Nonprofit', 'Other'],
      q2Options: ['Yes', 'No', 'Currently do'],
      q3Options: ['Rarely', 'A few times a year', 'Monthly', 'Weekly'],
      q4Options: [
        'Contracts & agreements',
        'Leases & rental agreements',
        'Employment documents',
        'Court filings',
        'Wills & estate documents',
        'Business formation docs',
        'NDAs',
        'Other'
      ],
      q5Options: [
        'Hidden clauses or fine print',
        'Understanding my rights and obligations',
        'Deadlines and key dates',
        'Financial risks',
        'Whether the document is enforceable',
        'Unfair or one-sided terms',
        'Plain-language explanation'
      ],
      q6Options: ['Very comfortable', 'Somewhat', 'Not at all'],
      q7Options: [
        "Understand what I'm signing",
        'Identify risks before I sign',
        'Know when I need a real lawyer',
        'Build or negotiate a stronger document',
        'Settle a dispute'
      ],
      answers: { q1: '', q2: '', q3: '', q4: [], q5: [], q6: '', q7: '' }
    }
  },
  computed: {
    progressPct() {
      return Math.round(((this.current + 1) / this.questions.length) * 100)
    }
  },
  methods: {
    next() {
      this.error = ''
      if (!this.isCurrentAnswered()) {
        this.error = 'Please answer this question before continuing.'
        return
      }
      this.current++
    },
    isCurrentAnswered() {
      const q = this.current
      if (q === 0) return !!this.answers.q1
      if (q === 1) return !!this.answers.q2
      if (q === 2) return !!this.answers.q3
      if (q === 3) return this.answers.q4.length > 0
      if (q === 4) return this.answers.q5.length > 0
      if (q === 5) return !!this.answers.q6
      if (q === 6) return !!this.answers.q7
      return true
    },
    async submit() {
      this.error = ''
      if (!this.isCurrentAnswered()) {
        this.error = 'Please answer this question before submitting.'
        return
      }
      this.loading = true
      try {
        await axios.post('http://localhost:18080/survey', this.answers)
        this.$router.push('/profile')
      } catch (err) {
        this.error = err.response?.data?.message || 'Could not submit survey. Please try again.'
      } finally {
        this.loading = false
      }
    }
  }
}
</script>

<style scoped>
.survey-page {
  min-height: 100vh;
  display: flex;
  align-items: center;
  justify-content: center;
  padding: 1.5rem;
  background:
    radial-gradient(ellipse 120% 70% at 60% -5%, rgba(66, 184, 131, 0.13) 0%, transparent 65%),
    var(--color-background);
}

.survey-card {
  width: 100%;
  max-width: 540px;
  background: var(--color-background-soft);
  border-radius: 24px;
  padding: 2.5rem;
  box-shadow:
    0 4px 6px -1px rgba(0, 0, 0, 0.06),
    0 20px 50px -12px rgba(0, 0, 0, 0.12);
  animation: fadeSlideUp 0.45s cubic-bezier(0.22, 1, 0.36, 1) both;
}

@keyframes fadeSlideUp {
  from { opacity: 0; transform: translateY(24px); }
  to   { opacity: 1; transform: translateY(0); }
}

/* Brand */
.brand {
  display: flex;
  align-items: center;
  gap: 0.5rem;
  margin-bottom: 1.5rem;
}

.brand-icon {
  width: 32px;
  height: 32px;
  background: linear-gradient(135deg, #42b883, #2d9a6a);
  border-radius: 9px;
  display: flex;
  align-items: center;
  justify-content: center;
  color: #fff;
  padding: 6px;
  flex-shrink: 0;
}

.brand-icon svg { width: 100%; height: 100%; }

.brand-name {
  font-size: 1rem;
  font-weight: 700;
  color: var(--color-heading);
  letter-spacing: -0.02em;
}

.title {
  font-size: 1.75rem;
  font-weight: 800;
  color: var(--color-heading);
  letter-spacing: -0.03em;
  margin-bottom: 0.3rem;
}

.subtitle {
  color: var(--vt-c-text-light-2, #888);
  font-size: 0.9rem;
  margin-bottom: 1.5rem;
}

/* Steps */
.steps {
  display: flex;
  align-items: center;
  gap: 0.35rem;
  margin-bottom: 0.85rem;
  flex-wrap: wrap;
}

.step-dot {
  width: 28px;
  height: 28px;
  border-radius: 50%;
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 0.75rem;
  font-weight: 700;
  border: 2px solid var(--color-border);
  color: var(--vt-c-text-light-2, #aaa);
  background: var(--color-background);
  transition: all 0.25s;
  flex-shrink: 0;
}

.step-dot.done {
  background: #42b883;
  border-color: #42b883;
  color: #fff;
}

.step-dot.active {
  background: #42b883;
  border-color: #42b883;
  color: #fff;
  box-shadow: 0 0 0 4px rgba(66, 184, 131, 0.2);
}

.check { width: 12px; height: 12px; }

/* Progress bar */
.progress-track {
  height: 5px;
  background: var(--color-border);
  border-radius: 99px;
  overflow: hidden;
  margin-bottom: 0.4rem;
}

.progress-fill {
  height: 100%;
  background: linear-gradient(90deg, #42b883, #2d9a6a);
  border-radius: 99px;
  transition: width 0.4s ease;
}

.progress-label {
  font-size: 0.78rem;
  color: var(--vt-c-text-light-2, #aaa);
  margin-bottom: 0;
}

/* Question area */
.form { margin-top: 1.75rem; }

.question { min-height: 260px; }

.question-text {
  font-size: 1.1rem;
  font-weight: 700;
  color: var(--color-heading);
  line-height: 1.45;
  margin-bottom: 0.4rem;
}

.hint {
  font-size: 0.82rem;
  color: var(--vt-c-text-light-2, #888);
  margin-bottom: 0.85rem;
}

/* Options */
.option-group {
  display: grid;
  grid-template-columns: 1fr;
  gap: 0.5rem;
  margin-top: 0.85rem;
}

.option-group.two-col {
  grid-template-columns: 1fr 1fr;
}

.option-label {
  display: flex;
  align-items: center;
  gap: 0.75rem;
  padding: 0.8rem 1rem;
  min-height: 52px;
  border: 1.5px solid var(--color-border);
  border-radius: 12px;
  cursor: pointer;
  transition: border-color 0.15s, background 0.15s, box-shadow 0.15s;
  position: relative;
}

.option-label input {
  position: absolute;
  opacity: 0;
  width: 0;
  height: 0;
}

.option-label:hover {
  border-color: #42b883;
  background: rgba(66, 184, 131, 0.04);
}

.option-label.selected {
  border-color: #42b883;
  background: rgba(66, 184, 131, 0.07);
  box-shadow: 0 0 0 3px rgba(66, 184, 131, 0.12);
}

.option-label.disabled {
  opacity: 0.4;
  cursor: not-allowed;
}

.option-text {
  flex: 1;
  font-size: 0.93rem;
  color: var(--color-text);
  font-weight: 500;
}

.option-check {
  width: 20px;
  height: 20px;
  color: #42b883;
  flex-shrink: 0;
}

/* Slide transition */
.slide-enter-active,
.slide-leave-active { transition: opacity 0.2s ease, transform 0.2s ease; }
.slide-enter-from   { opacity: 0; transform: translateX(20px); }
.slide-leave-to     { opacity: 0; transform: translateX(-20px); }

/* Error */
.error {
  margin-top: 0.75rem;
  color: #ef4444;
  font-size: 0.85rem;
  background: rgba(239, 68, 68, 0.08);
  border: 1px solid rgba(239, 68, 68, 0.2);
  border-radius: 8px;
  padding: 0.5rem 0.75rem;
}

.err-enter-active, .err-leave-active { transition: opacity 0.2s; }
.err-enter-from, .err-leave-to { opacity: 0; }

/* Navigation */
.nav-buttons {
  display: flex;
  gap: 0.75rem;
  margin-top: 1.75rem;
}

.btn-back {
  display: flex;
  align-items: center;
  gap: 0.35rem;
  flex: 1;
  height: 48px;
  padding: 0 1.25rem;
  justify-content: center;
  background: transparent;
  color: var(--color-text);
  border: 1.5px solid var(--color-border-hover);
  border-radius: 12px;
  font-size: 0.95rem;
  font-weight: 600;
  cursor: pointer;
  transition: border-color 0.2s, background 0.2s;
}

.btn-back svg { width: 18px; height: 18px; }

.btn-back:hover {
  border-color: #42b883;
  background: rgba(66, 184, 131, 0.05);
}

.btn-next {
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 0.35rem;
  flex: 2;
  height: 48px;
  background: linear-gradient(135deg, #42b883 0%, #2d9a6a 100%);
  color: #fff;
  border: none;
  border-radius: 12px;
  font-size: 0.95rem;
  font-weight: 700;
  cursor: pointer;
  box-shadow: 0 4px 15px rgba(66, 184, 131, 0.35);
  transition: transform 0.15s, box-shadow 0.15s, opacity 0.15s;
}

.btn-next svg { width: 18px; height: 18px; }

.btn-next:hover:not(:disabled) {
  transform: translateY(-1px);
  box-shadow: 0 6px 20px rgba(66, 184, 131, 0.45);
}

.btn-next:disabled { opacity: 0.65; cursor: not-allowed; }

.spinner {
  width: 16px;
  height: 16px;
  border: 2px solid rgba(255, 255, 255, 0.4);
  border-top-color: #fff;
  border-radius: 50%;
  animation: spin 0.7s linear infinite;
}

@keyframes spin { to { transform: rotate(360deg); } }
</style>
