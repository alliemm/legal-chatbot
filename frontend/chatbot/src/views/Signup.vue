<template>
  <div class="auth-page">
    <div class="auth-card">

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

      <h1 class="title">Create an account</h1>
      <p class="subtitle">Sign up to get started</p>

      <form @submit.prevent="signup" class="form">
        <div class="field">
          <label for="name">Full name</label>
          <div class="input-wrapper">
            <svg class="input-icon" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
              <path d="M20 21v-2a4 4 0 0 0-4-4H8a4 4 0 0 0-4 4v2"/>
              <circle cx="12" cy="7" r="4"/>
            </svg>
            <input
              id="name"
              v-model="name"
              type="text"
              placeholder="Your name"
              autocomplete="name"
              required
            />
          </div>
        </div>

        <div class="field">
          <label for="email">Email</label>
          <div class="input-wrapper">
            <svg class="input-icon" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
              <path d="M4 4h16c1.1 0 2 .9 2 2v12c0 1.1-.9 2-2 2H4c-1.1 0-2-.9-2-2V6c0-1.1.9-2 2-2z"/>
              <polyline points="22,6 12,13 2,6"/>
            </svg>
            <input
              id="email"
              v-model="email"
              type="email"
              placeholder="you@example.com"
              autocomplete="email"
              required
            />
          </div>
        </div>

        <div class="field">
          <label for="password">Password</label>
          <div class="input-wrapper">
            <svg class="input-icon" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
              <rect x="3" y="11" width="18" height="11" rx="2" ry="2"/>
              <path d="M7 11V7a5 5 0 0 1 10 0v4"/>
            </svg>
            <input
              id="password"
              v-model="password"
              type="password"
              placeholder="••••••••"
              autocomplete="new-password"
              required
            />
          </div>
          <div v-if="password" class="strength-bar">
            <div
              class="strength-fill"
              :class="strengthClass"
              :style="{ width: strengthPct + '%' }"
            ></div>
          </div>
          <p v-if="password" class="strength-label" :class="strengthClass">{{ strengthText }}</p>
        </div>

        <div class="field">
          <label for="confirm">Confirm password</label>
          <div class="input-wrapper">
            <svg class="input-icon" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
              <rect x="3" y="11" width="18" height="11" rx="2" ry="2"/>
              <path d="M7 11V7a5 5 0 0 1 10 0v4"/>
            </svg>
            <input
              id="confirm"
              v-model="confirm"
              type="password"
              placeholder="••••••••"
              autocomplete="new-password"
              required
            />
          </div>
        </div>

        <transition name="err">
          <p v-if="error" class="error">{{ error }}</p>
        </transition>

        <button type="submit" :disabled="loading" class="submit-btn">
          <span v-if="loading" class="spinner"></span>
          {{ loading ? 'Creating account…' : 'Create account' }}
        </button>
      </form>

      <p class="footer-text">
        Already have an account?
        <router-link to="/login">Sign in</router-link>
      </p>
    </div>
  </div>
</template>

<script>
import axios from 'axios'

export default {
  data() {
    return {
      name: '',
      email: '',
      password: '',
      confirm: '',
      loading: false,
      error: ''
    }
  },
  computed: {
    strengthScore() {
      const p = this.password
      if (!p) return 0
      let score = 0
      if (p.length >= 8)  score++
      if (p.length >= 12) score++
      if (/[A-Z]/.test(p)) score++
      if (/[0-9]/.test(p)) score++
      if (/[^A-Za-z0-9]/.test(p)) score++
      return score
    },
    strengthPct()   { return Math.min(100, this.strengthScore * 22) },
    strengthClass() {
      const s = this.strengthScore
      if (s <= 1) return 'weak'
      if (s <= 3) return 'fair'
      return 'strong'
    },
    strengthText() {
      return { weak: 'Weak', fair: 'Fair', strong: 'Strong' }[this.strengthClass]
    }
  },
  methods: {
    async signup() {
      this.error = ''
      if (this.password !== this.confirm) {
        this.error = 'Passwords do not match.'
        return
      }
      this.loading = true
      try {
        const res = await axios.post('https://legal-chatbot-4t8e.onrender.com/signup', {
          "name": this.name,
          "email": this.email,
          "password": this.password
        })
        console.log(res.data)
        this.$router.push('/login')
      } catch (err) {
        this.error = err.response?.data?.message || 'Could not create account. Please try again.'
      } finally {
        this.loading = false
      }
    }
  }
}
</script>

<style scoped>
.auth-page {
  min-height: 100vh;
  display: flex;
  align-items: center;
  justify-content: center;
  padding: 1.5rem;
  background:
    radial-gradient(ellipse 120% 70% at 60% -5%, rgba(66, 184, 131, 0.13) 0%, transparent 65%),
    var(--color-background);
}

.auth-card {
  width: 100%;
  max-width: 420px;
  background: var(--color-background-soft);
  border-radius: 24px;
  padding: 2.75rem 2.5rem;
  box-shadow:
    0 4px 6px -1px rgba(0, 0, 0, 0.06),
    0 20px 50px -12px rgba(0, 0, 0, 0.12);
  animation: fadeSlideUp 0.45s cubic-bezier(0.22, 1, 0.36, 1) both;
}

@keyframes fadeSlideUp {
  from { opacity: 0; transform: translateY(24px); }
  to   { opacity: 1; transform: translateY(0); }
}

.brand {
  display: flex;
  align-items: center;
  gap: 0.5rem;
  margin-bottom: 2rem;
}

.brand-icon {
  width: 36px;
  height: 36px;
  background: linear-gradient(135deg, #42b883, #2d9a6a);
  border-radius: 10px;
  display: flex;
  align-items: center;
  justify-content: center;
  color: #fff;
  padding: 7px;
  flex-shrink: 0;
}

.brand-icon svg { width: 100%; height: 100%; }

.brand-name {
  font-size: 1.1rem;
  font-weight: 700;
  color: var(--color-heading);
  letter-spacing: -0.02em;
}

.title {
  font-size: 1.85rem;
  font-weight: 800;
  color: var(--color-heading);
  letter-spacing: -0.03em;
  line-height: 1.2;
  margin-bottom: 0.35rem;
}

.subtitle {
  color: var(--vt-c-text-light-2, #888);
  font-size: 0.95rem;
  margin-bottom: 2rem;
}

.form {
  display: flex;
  flex-direction: column;
  gap: 1.1rem;
}

.field {
  display: flex;
  flex-direction: column;
  gap: 0.45rem;
}

.field label {
  font-size: 0.825rem;
  font-weight: 600;
  color: var(--color-text);
  letter-spacing: 0.01em;
}

.input-wrapper { position: relative; }

.input-icon {
  position: absolute;
  left: 14px;
  top: 50%;
  transform: translateY(-50%);
  width: 16px;
  height: 16px;
  color: #aaa;
  pointer-events: none;
}

.input-wrapper input {
  width: 100%;
  height: 48px;
  padding: 0 1rem 0 2.7rem;
  border: 1.5px solid var(--color-border-hover);
  border-radius: 12px;
  background: var(--color-background);
  color: var(--color-text);
  font-size: 0.95rem;
  outline: none;
  transition: border-color 0.2s, box-shadow 0.2s;
}

.input-wrapper input:focus {
  border-color: #42b883;
  box-shadow: 0 0 0 3px rgba(66, 184, 131, 0.18);
}

.input-wrapper input::placeholder { color: #bbb; }

/* Password strength */
.strength-bar {
  height: 4px;
  background: var(--color-border);
  border-radius: 99px;
  overflow: hidden;
}

.strength-fill {
  height: 100%;
  border-radius: 99px;
  transition: width 0.3s, background 0.3s;
}

.strength-fill.weak   { background: #ef4444; }
.strength-fill.fair   { background: #f59e0b; }
.strength-fill.strong { background: #42b883; }

.strength-label {
  font-size: 0.78rem;
  font-weight: 600;
}
.strength-label.weak   { color: #ef4444; }
.strength-label.fair   { color: #f59e0b; }
.strength-label.strong { color: #42b883; }

.error {
  color: #ef4444;
  font-size: 0.85rem;
  background: rgba(239, 68, 68, 0.08);
  border: 1px solid rgba(239, 68, 68, 0.2);
  border-radius: 8px;
  padding: 0.5rem 0.75rem;
}

.err-enter-active, .err-leave-active { transition: opacity 0.2s, transform 0.2s; }
.err-enter-from, .err-leave-to { opacity: 0; transform: translateY(-4px); }

.submit-btn {
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 0.5rem;
  margin-top: 0.5rem;
  height: 48px;
  background: linear-gradient(135deg, #42b883 0%, #2d9a6a 100%);
  color: #fff;
  border: none;
  border-radius: 12px;
  font-size: 1rem;
  font-weight: 700;
  cursor: pointer;
  box-shadow: 0 4px 15px rgba(66, 184, 131, 0.35);
  transition: transform 0.15s, box-shadow 0.15s, opacity 0.15s;
}

.submit-btn:hover:not(:disabled) {
  transform: translateY(-1px);
  box-shadow: 0 6px 20px rgba(66, 184, 131, 0.45);
}

.submit-btn:active:not(:disabled) { transform: translateY(0); }

.submit-btn:disabled { opacity: 0.65; cursor: not-allowed; }

.spinner {
  width: 16px;
  height: 16px;
  border: 2px solid rgba(255, 255, 255, 0.4);
  border-top-color: #fff;
  border-radius: 50%;
  animation: spin 0.7s linear infinite;
  flex-shrink: 0;
}

@keyframes spin { to { transform: rotate(360deg); } }

.footer-text {
  margin-top: 1.75rem;
  text-align: center;
  font-size: 0.9rem;
  color: var(--vt-c-text-light-2, #888);
}

.footer-text a {
  color: #42b883;
  text-decoration: none;
  font-weight: 600;
}

.footer-text a:hover { text-decoration: underline; }
</style>
