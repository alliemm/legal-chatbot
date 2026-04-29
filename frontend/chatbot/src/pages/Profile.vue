<template>
  <div class="profile-page">

    <!-- Sticky top bar -->
    <div class="top-bar">
      <div class="top-bar-inner">
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
        <div class="top-bar-actions">
          <transition name="fade">
            <span v-if="saved" class="saved-badge">Saved</span>
          </transition>
          <button class="save-btn-top" :disabled="loading" @click="save">
            <span v-if="loading" class="spinner"></span>
            {{ loading ? 'Saving…' : 'Save preferences' }}
          </button>
        </div>
      </div>
    </div>

    <div class="profile-body">
      <div class="page-header">
        <h1 class="title">Profile & Preferences</h1>
        <p class="subtitle">Customize how LexAssist analyzes documents for you</p>
      </div>

      <form @submit.prevent="save" class="form">

        <!-- 0. PERSONAL INFORMATION -->
        <div class="section-card">
          <div class="section-head">
            <div class="section-icon blue">
              <svg viewBox="0 0 20 20" fill="currentColor">
                <path fill-rule="evenodd" d="M10 9a3 3 0 100-6 3 3 0 000 6zm-7 9a7 7 0 1114 0H3z" clip-rule="evenodd"/>
              </svg>
            </div>
            <div>
              <h2 class="section-title">Personal Information</h2>
              <p class="section-desc">Your display name and email address</p>
            </div>
          </div>
          <div class="context-grid">
            <div class="field">
              <label>Nickname / Display name</label>
              <input v-model="prefs.nickname" type="text" class="text-input" placeholder="How should we call you?" autocomplete="nickname" />
            </div>
            <div class="field">
              <label>Email address</label>
              <input v-model="prefs.email" type="email" class="text-input" placeholder="you@example.com" autocomplete="email" />
            </div>
          </div>
        </div>

        <!-- 0b. CHANGE PASSWORD -->
        <div class="section-card">
          <div class="section-head">
            <div class="section-icon orange">
              <svg viewBox="0 0 20 20" fill="currentColor">
                <path fill-rule="evenodd" d="M5 9V7a5 5 0 0110 0v2a2 2 0 012 2v5a2 2 0 01-2 2H5a2 2 0 01-2-2v-5a2 2 0 012-2zm8-2v2H7V7a3 3 0 016 0z" clip-rule="evenodd"/>
              </svg>
            </div>
            <div>
              <h2 class="section-title">Change Password</h2>
              <p class="section-desc">Leave blank to keep your current password</p>
            </div>
          </div>
          <div class="pw-fields">
            <div class="field">
              <label>Current password</label>
              <input v-model="pw.current" type="password" class="text-input" placeholder="••••••••" autocomplete="current-password" />
            </div>
            <div class="field">
              <label>New password</label>
              <input v-model="pw.newPwd" type="password" class="text-input" placeholder="••••••••" autocomplete="new-password" />
            </div>
            <div class="field">
              <label>Confirm new password</label>
              <input v-model="pw.confirm" type="password" class="text-input" placeholder="••••••••" autocomplete="new-password" />
            </div>
          </div>
          <transition name="err">
            <p v-if="pwError" class="error pw-feedback">{{ pwError }}</p>
          </transition>
          <transition name="fade">
            <p v-if="pwSaved" class="saved-inline">Password updated</p>
          </transition>
          <button type="button" class="pw-btn" :disabled="pwLoading" @click="changePassword">
            <span v-if="pwLoading" class="spinner"></span>
            {{ pwLoading ? 'Updating…' : 'Update password' }}
          </button>
        </div>

        <!-- 1. FOCUS AREAS -->
        <div class="section-card">
          <div class="section-head">
            <div class="section-icon green">
              <svg viewBox="0 0 20 20" fill="currentColor">
                <path d="M9 4.804A7.968 7.968 0 005.5 4c-1.255 0-2.443.29-3.5.804v10A7.969 7.969 0 015.5 14c1.396 0 2.7.37 3.8 1.016A7.966 7.966 0 0114.5 14c1.255 0 2.443.29 3.5.804v-10A7.968 7.968 0 0014.5 4c-1.255 0-2.443.29-3.5.804V12a1 1 0 11-2 0V4.804z"/>
              </svg>
            </div>
            <div>
              <h2 class="section-title">Focus Areas</h2>
              <p class="section-desc">Clause types the chatbot highlights in every analysis</p>
            </div>
          </div>
          <div class="checkbox-grid">
            <label
              v-for="area in focusAreaOptions"
              :key="area.value"
              class="check-card"
              :class="{ selected: prefs.focusAreas.includes(area.value) }"
            >
              <input type="checkbox" :value="area.value" v-model="prefs.focusAreas" />
              <div class="check-card-body">
                <span class="check-card-label">{{ area.label }}</span>
                <span class="check-card-hint">{{ area.hint }}</span>
              </div>
              <div class="check-indicator">
                <svg v-if="prefs.focusAreas.includes(area.value)" viewBox="0 0 14 14" fill="none">
                  <polyline points="2,7 5.5,10.5 12,3.5" stroke="currentColor" stroke-width="2.2" stroke-linecap="round" stroke-linejoin="round"/>
                </svg>
              </div>
            </label>
          </div>
        </div>

        <!-- 2. EXPLANATION STYLE -->
        <div class="section-card">
          <div class="section-head">
            <div class="section-icon blue">
              <svg viewBox="0 0 20 20" fill="currentColor">
                <path fill-rule="evenodd" d="M18 10a8 8 0 11-16 0 8 8 0 0116 0zm-7-4a1 1 0 11-2 0 1 1 0 012 0zM9 9a1 1 0 000 2v3a1 1 0 001 1h1a1 1 0 100-2v-3a1 1 0 00-1-1H9z" clip-rule="evenodd"/>
              </svg>
            </div>
            <div>
              <h2 class="section-title">Explanation Style</h2>
              <p class="section-desc">How the chatbot phrases its answers</p>
            </div>
          </div>
          <div class="radio-pills">
            <label
              v-for="opt in explanationStyleOptions"
              :key="opt.value"
              class="radio-pill"
              :class="{ selected: prefs.explanationStyle === opt.value }"
            >
              <input type="radio" v-model="prefs.explanationStyle" :value="opt.value" />
              <span class="pill-label">{{ opt.label }}</span>
              <span class="pill-hint">{{ opt.hint }}</span>
            </label>
          </div>
        </div>

        <!-- 3. DETAIL LEVEL -->
        <div class="section-card">
          <div class="section-head">
            <div class="section-icon purple">
              <svg viewBox="0 0 20 20" fill="currentColor">
                <path d="M3 4a1 1 0 011-1h12a1 1 0 110 2H4a1 1 0 01-1-1zm0 4a1 1 0 011-1h12a1 1 0 110 2H4a1 1 0 01-1-1zm0 4a1 1 0 011-1h8a1 1 0 110 2H4a1 1 0 01-1-1zm0 4a1 1 0 011-1h4a1 1 0 110 2H4a1 1 0 01-1-1z"/>
              </svg>
            </div>
            <div>
              <h2 class="section-title">Response Detail Level</h2>
              <p class="section-desc">How much detail per clause</p>
            </div>
          </div>
          <div class="radio-pills">
            <label
              v-for="opt in detailLevelOptions"
              :key="opt.value"
              class="radio-pill"
              :class="{ selected: prefs.detailLevel === opt.value }"
            >
              <input type="radio" v-model="prefs.detailLevel" :value="opt.value" />
              <span class="pill-label">{{ opt.label }}</span>
              <span class="pill-hint">{{ opt.hint }}</span>
            </label>
          </div>
        </div>

        <!-- 4. RISK SENSITIVITY -->
        <div class="section-card">
          <div class="section-head">
            <div class="section-icon orange">
              <svg viewBox="0 0 20 20" fill="currentColor">
                <path fill-rule="evenodd" d="M8.257 3.099c.765-1.36 2.722-1.36 3.486 0l5.58 9.92c.75 1.334-.213 2.98-1.742 2.98H4.42c-1.53 0-2.493-1.646-1.743-2.98l5.58-9.92zM11 13a1 1 0 11-2 0 1 1 0 012 0zm-1-8a1 1 0 00-1 1v3a1 1 0 002 0V6a1 1 0 00-1-1z" clip-rule="evenodd"/>
              </svg>
            </div>
            <div>
              <h2 class="section-title">Risk Sensitivity</h2>
              <p class="section-desc">Which clauses get flagged as risky</p>
            </div>
          </div>
          <div class="radio-pills">
            <label
              v-for="opt in riskOptions"
              :key="opt.value"
              class="radio-pill"
              :class="{ selected: prefs.riskSensitivity === opt.value }"
            >
              <input type="radio" v-model="prefs.riskSensitivity" :value="opt.value" />
              <span class="pill-label">{{ opt.label }}</span>
              <span class="pill-hint">{{ opt.hint }}</span>
            </label>
          </div>
        </div>

        <!-- 5. LANGUAGE -->
        <div class="section-card">
          <div class="section-head">
            <div class="section-icon teal">
              <svg viewBox="0 0 20 20" fill="currentColor">
                <path fill-rule="evenodd" d="M7 2a1 1 0 011 1v1h3a1 1 0 110 2H9.578a18.87 18.87 0 01-1.724 4.78c.29.354.596.696.914 1.026a1 1 0 11-1.44 1.389 21.034 21.034 0 01-.554-.6 19.098 19.098 0 01-3.107 3.567 1 1 0 01-1.334-1.49 17.087 17.087 0 003.13-3.733 18.992 18.992 0 01-1.487-3.754 1 1 0 111.945-.468c.348 1.443.82 2.76 1.395 3.94.573-1.18 1.045-2.497 1.393-3.94A1 1 0 017 2zm5 8a1 1 0 011 1v1h2a1 1 0 110 2h-.078a5.19 5.19 0 01-1.494 2.722 1 1 0 01-1.414-1.414A3.19 3.19 0 0013.9 14H12a1 1 0 01-1-1v-1a1 1 0 011-1h.078a3.19 3.19 0 00-.078-.002z" clip-rule="evenodd"/>
              </svg>
            </div>
            <div>
              <h2 class="section-title">Language Preference</h2>
              <p class="section-desc">Language used in chatbot responses</p>
            </div>
          </div>
          <div class="inline-fields">
            <select v-model="prefs.language" class="select-input">
              <option v-for="lang in languageOptions" :key="lang" :value="lang">{{ lang }}</option>
            </select>
            <input
              v-if="prefs.language === 'Other'"
              v-model="prefs.languageOther"
              type="text"
              class="text-input"
              placeholder="Specify language"
            />
          </div>
        </div>

        <!-- 6. DOCUMENT CONTEXT -->
        <div class="section-card">
          <div class="section-head">
            <div class="section-icon green">
              <svg viewBox="0 0 20 20" fill="currentColor">
                <path fill-rule="evenodd" d="M4 4a2 2 0 012-2h4.586A2 2 0 0112 2.586L15.414 6A2 2 0 0116 7.414V16a2 2 0 01-2 2H6a2 2 0 01-2-2V4zm2 6a1 1 0 011-1h6a1 1 0 110 2H7a1 1 0 01-1-1zm1 3a1 1 0 100 2h6a1 1 0 100-2H7z" clip-rule="evenodd"/>
              </svg>
            </div>
            <div>
              <h2 class="section-title">Document Context</h2>
              <p class="section-desc">Background info carried over from your survey — edit any time</p>
            </div>
          </div>
          <div class="context-grid">
            <div class="field">
              <label>User type</label>
              <select v-model="prefs.userType" class="select-input">
                <option v-for="opt in userTypeOptions" :key="opt" :value="opt">{{ opt }}</option>
              </select>
            </div>
            <div class="field">
              <label>Legal experience</label>
              <select v-model="prefs.legalExperience" class="select-input">
                <option v-for="opt in legalExpOptions" :key="opt" :value="opt">{{ opt }}</option>
              </select>
            </div>
            <div class="field">
              <label>Document frequency</label>
              <select v-model="prefs.docFrequency" class="select-input">
                <option v-for="opt in frequencyOptions" :key="opt" :value="opt">{{ opt }}</option>
              </select>
            </div>
            <div class="field">
              <label>Primary goal</label>
              <select v-model="prefs.primaryGoal" class="select-input">
                <option v-for="opt in goalOptions" :key="opt" :value="opt">{{ opt }}</option>
              </select>
            </div>
          </div>
        </div>

        <!-- 7. CHATBOT BEHAVIOUR -->
        <div class="section-card">
          <div class="section-head">
            <div class="section-icon purple">
              <svg viewBox="0 0 20 20" fill="currentColor">
                <path d="M2 5a2 2 0 012-2h7a2 2 0 012 2v4a2 2 0 01-2 2H9l-3 3v-3H4a2 2 0 01-2-2V5z"/>
                <path d="M15 7v2a4 4 0 01-4 4H9.828l-1.766 1.767c.28.149.599.233.938.233h2l3 3v-3h2a2 2 0 002-2V9a2 2 0 00-2-2h-1z"/>
              </svg>
            </div>
            <div>
              <h2 class="section-title">Chatbot Behaviour</h2>
              <p class="section-desc">Fine-tune how the chatbot presents information</p>
            </div>
          </div>
          <div class="toggle-list">
            <div v-for="flag in behaviourFlags" :key="flag.key" class="toggle-row">
              <span class="toggle-label">{{ flag.label }}</span>
              <button
                type="button"
                class="toggle-btn"
                :class="{ active: prefs.behaviourFlags[flag.key] }"
                @click="prefs.behaviourFlags[flag.key] = !prefs.behaviourFlags[flag.key]"
                :aria-pressed="prefs.behaviourFlags[flag.key]"
              >
                <span class="toggle-thumb"></span>
              </button>
            </div>
          </div>
        </div>

        <!-- 8. NOTIFICATIONS -->
        <div class="section-card">
          <div class="section-head">
            <div class="section-icon blue">
              <svg viewBox="0 0 20 20" fill="currentColor">
                <path d="M10 2a6 6 0 00-6 6v3.586l-.707.707A1 1 0 004 14h12a1 1 0 00.707-1.707L16 11.586V8a6 6 0 00-6-6zM10 18a3 3 0 01-3-3h6a3 3 0 01-3 3z"/>
              </svg>
            </div>
            <div>
              <h2 class="section-title">Notifications & History</h2>
              <p class="section-desc">Control what gets saved and how you're notified</p>
            </div>
          </div>
          <div class="context-grid">
            <div class="field">
              <label>Save analysis history</label>
              <select v-model="prefs.saveHistory" class="select-input">
                <option value="yes">Yes</option>
                <option value="no">No</option>
              </select>
            </div>
            <div class="field">
              <label>Email digest</label>
              <select v-model="prefs.emailDigest" class="select-input">
                <option value="never">Never</option>
                <option value="weekly">Weekly</option>
                <option value="monthly">Monthly</option>
              </select>
            </div>
            <div class="field">
              <label>Anonymous analytics</label>
              <select v-model="prefs.analytics" class="select-input">
                <option value="yes">Yes</option>
                <option value="no">No</option>
              </select>
            </div>
          </div>
        </div>

        <!-- Error & bottom save -->
        <transition name="err">
          <p v-if="error" class="error">{{ error }}</p>
        </transition>

        <button type="submit" class="save-btn-bottom" :disabled="loading">
          <span v-if="loading" class="spinner"></span>
          {{ loading ? 'Saving…' : 'Save preferences' }}
        </button>

      </form>
    </div>
  </div>
</template>

<script>
import axios from 'axios'

export default {
  data() {
    return {
      loading: false,
      error: '',
      saved: false,
      pw: { current: '', newPwd: '', confirm: '' },
      pwLoading: false,
      pwError: '',
      pwSaved: false,

      focusAreaOptions: [
        { value: 'privacy',    label: 'Privacy & data collection',    hint: 'How personal data is collected, stored, or shared' },
        { value: 'fees',       label: 'Fee & payment clauses',        hint: 'Costs, renewals, penalties, and hidden charges' },
        { value: 'termination',label: 'Termination & cancellation',   hint: 'Exit rights, notice periods, and early-exit fees' },
        { value: 'liability',  label: 'Liability & indemnification',  hint: 'Liability limits, indemnity, and legal waivers' },
        { value: 'ip',         label: 'Intellectual property',        hint: 'Content ownership, license grants, and work-for-hire' },
        { value: 'dispute',    label: 'Dispute resolution',           hint: 'Mandatory arbitration, class-action waivers' },
        { value: 'renewal',    label: 'Auto-renewal & subscriptions', hint: 'Rollover language and free-trial-to-paid conversions' },
        { value: 'noncompete', label: 'Non-compete & non-solicitation', hint: 'Employment restrictions and geographic limits' }
      ],

      explanationStyleOptions: [
        { value: 'plain',     label: 'Plain language', hint: 'Simple, jargon-free summaries' },
        { value: 'balanced',  label: 'Balanced',       hint: 'Plain language with key terms defined' },
        { value: 'technical', label: 'Technical',      hint: 'Precise legal terminology preserved' }
      ],

      detailLevelOptions: [
        { value: 'brief',    label: 'Brief',    hint: '2–3 sentence summary per clause' },
        { value: 'standard', label: 'Standard', hint: 'Paragraph-length with key points' },
        { value: 'detailed', label: 'Detailed', hint: 'Full breakdown with risk rating' }
      ],

      riskOptions: [
        { value: 'low',    label: 'Low',    hint: 'Only severe or clearly one-sided clauses' },
        { value: 'medium', label: 'Medium', hint: 'Moderately risky or unusual clauses' },
        { value: 'high',   label: 'High',   hint: 'Anything deviating from standard practice' }
      ],

      languageOptions: ['English', '中文 (Chinese)', 'Español', 'Français', 'Deutsch', 'Other'],
      userTypeOptions: ['Individual', 'Small business owner', 'Nonprofit', 'Other'],
      legalExpOptions: ['No prior lawyer', 'Past lawyer', 'Currently has lawyer'],
      frequencyOptions: ['Rarely', 'A few times a year', 'Monthly', 'Weekly'],
      goalOptions: [
        "Understand what I'm signing",
        'Identify risks before I sign',
        'Know when I need a real lawyer',
        'Build or negotiate a stronger document',
        'Settle a dispute'
      ],

      behaviourFlags: [
        { key: 'showRiskRating',  label: 'Always show a risk rating (Low / Medium / High) for each clause' },
        { key: 'recommendLawyer', label: 'Recommend consulting a lawyer when risk is High' },
        { key: 'showDeadlines',   label: 'Show relevant deadlines and key dates at the top of every summary' },
        { key: 'showTldr',        label: 'Include a one-line plain-English TL;DR at the start of each response' },
        { key: 'askClarifying',   label: 'Proactively ask clarifying questions if document context is unclear' }
      ],

      prefs: {
        nickname: '',
        email: '',
        focusAreas: [],
        explanationStyle: 'balanced',
        detailLevel: 'standard',
        riskSensitivity: 'medium',
        language: 'English',
        languageOther: '',
        userType: 'Individual',
        legalExperience: 'No prior lawyer',
        docFrequency: 'Rarely',
        primaryGoal: "Understand what I'm signing",
        behaviourFlags: {
          showRiskRating: true,
          recommendLawyer: true,
          showDeadlines: false,
          showTldr: true,
          askClarifying: false
        },
        saveHistory: 'yes',
        emailDigest: 'never',
        analytics: 'yes'
      }
    }
  },
  async created() {
    try {
      const res = await axios.get('http://localhost:18080/profile')
      if (res.data) Object.assign(this.prefs, res.data)
    } catch { /* use defaults */ }
  },
  methods: {
    async changePassword() {
      this.pwError = ''
      this.pwSaved = false
      if (!this.pw.current) {
        this.pwError = 'Please enter your current password.'
        return
      }
      if (!this.pw.newPwd) {
        this.pwError = 'Please enter a new password.'
        return
      }
      if (this.pw.newPwd !== this.pw.confirm) {
        this.pwError = 'New passwords do not match.'
        return
      }
      this.pwLoading = true
      try {
        await axios.post('http://localhost:18080/account/password', {
          currentPassword: this.pw.current,
          newPassword: this.pw.newPwd
        })
        this.pwSaved = true
        this.pw.current = ''
        this.pw.newPwd = ''
        this.pw.confirm = ''
        setTimeout(() => { this.pwSaved = false }, 3000)
      } catch (err) {
        this.pwError = err.response?.data?.message || 'Could not update password. Please try again.'
      } finally {
        this.pwLoading = false
      }
    },
    async save() {
      this.error = ''
      this.saved = false
      this.loading = true
      try {
        await axios.post('http://localhost:18080/profile', this.prefs)
        this.saved = true
        setTimeout(() => { this.saved = false }, 3000)
      } catch (err) {
        this.error = err.response?.data?.message || 'Could not save preferences. Please try again.'
      } finally {
        this.loading = false
      }
    }
  }
}
</script>

<style scoped>
/* ── Page ── */
.profile-page {
  min-height: 100vh;
  background:
    radial-gradient(ellipse 120% 50% at 70% 0%, rgba(66, 184, 131, 0.08) 0%, transparent 60%),
    var(--color-background);
}

/* ── Sticky top bar ── */
.top-bar {
  position: sticky;
  top: 0;
  z-index: 100;
  background: var(--color-background-soft);
  border-bottom: 1px solid var(--color-border);
  backdrop-filter: blur(12px);
  box-shadow: 0 1px 8px rgba(0, 0, 0, 0.06);
}

.top-bar-inner {
  max-width: 720px;
  margin: 0 auto;
  padding: 0.85rem 1.5rem;
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 1rem;
}

.brand {
  display: flex;
  align-items: center;
  gap: 0.5rem;
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

.top-bar-actions {
  display: flex;
  align-items: center;
  gap: 0.75rem;
}

.saved-badge {
  font-size: 0.82rem;
  font-weight: 600;
  color: #42b883;
  background: rgba(66, 184, 131, 0.12);
  border: 1px solid rgba(66, 184, 131, 0.25);
  border-radius: 99px;
  padding: 0.25rem 0.75rem;
}

.fade-enter-active, .fade-leave-active { transition: opacity 0.3s; }
.fade-enter-from, .fade-leave-to { opacity: 0; }

.save-btn-top {
  display: flex;
  align-items: center;
  gap: 0.4rem;
  height: 38px;
  padding: 0 1.1rem;
  background: linear-gradient(135deg, #42b883, #2d9a6a);
  color: #fff;
  border: none;
  border-radius: 10px;
  font-size: 0.875rem;
  font-weight: 700;
  cursor: pointer;
  box-shadow: 0 3px 10px rgba(66, 184, 131, 0.3);
  transition: transform 0.15s, box-shadow 0.15s, opacity 0.15s;
  white-space: nowrap;
}

.save-btn-top:hover:not(:disabled) {
  transform: translateY(-1px);
  box-shadow: 0 5px 15px rgba(66, 184, 131, 0.4);
}

.save-btn-top:disabled { opacity: 0.65; cursor: not-allowed; }

/* ── Body ── */
.profile-body {
  max-width: 720px;
  margin: 0 auto;
  padding: 2.5rem 1.5rem 4rem;
}

.page-header { margin-bottom: 2rem; }

.title {
  font-size: 1.85rem;
  font-weight: 800;
  color: var(--color-heading);
  letter-spacing: -0.03em;
  margin-bottom: 0.3rem;
}

.subtitle {
  color: var(--vt-c-text-light-2, #888);
  font-size: 0.95rem;
}

/* ── Form ── */
.form {
  display: flex;
  flex-direction: column;
  gap: 1.25rem;
}

/* ── Section cards ── */
.section-card {
  background: var(--color-background-soft);
  border-radius: 20px;
  padding: 1.75rem;
  box-shadow:
    0 2px 4px rgba(0, 0, 0, 0.04),
    0 8px 24px -4px rgba(0, 0, 0, 0.07);
  animation: fadeSlideUp 0.4s cubic-bezier(0.22, 1, 0.36, 1) both;
}

@keyframes fadeSlideUp {
  from { opacity: 0; transform: translateY(16px); }
  to   { opacity: 1; transform: translateY(0); }
}

.section-head {
  display: flex;
  align-items: flex-start;
  gap: 0.85rem;
  margin-bottom: 1.4rem;
}

.section-icon {
  width: 36px;
  height: 36px;
  border-radius: 10px;
  display: flex;
  align-items: center;
  justify-content: center;
  flex-shrink: 0;
  padding: 8px;
}

.section-icon svg { width: 100%; height: 100%; }
.section-icon.green  { background: rgba(66, 184, 131, 0.12); color: #42b883; }
.section-icon.blue   { background: rgba(59, 130, 246, 0.12); color: #3b82f6; }
.section-icon.purple { background: rgba(139, 92, 246, 0.12); color: #8b5cf6; }
.section-icon.orange { background: rgba(249, 115, 22, 0.12);  color: #f97316; }
.section-icon.teal   { background: rgba(20, 184, 166, 0.12);  color: #14b8a6; }

.section-title {
  font-size: 1rem;
  font-weight: 700;
  color: var(--color-heading);
  margin-bottom: 0.15rem;
}

.section-desc {
  font-size: 0.825rem;
  color: var(--vt-c-text-light-2, #888);
  line-height: 1.45;
}

/* ── Checkbox grid (focus areas) ── */
.checkbox-grid {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 0.65rem;
}

.check-card {
  display: flex;
  align-items: flex-start;
  gap: 0.65rem;
  padding: 0.85rem;
  border: 1.5px solid var(--color-border);
  border-radius: 12px;
  cursor: pointer;
  transition: border-color 0.15s, background 0.15s, box-shadow 0.15s;
  position: relative;
}

.check-card input { display: none; }

.check-card:hover {
  border-color: #42b883;
  background: rgba(66, 184, 131, 0.04);
}

.check-card.selected {
  border-color: #42b883;
  background: rgba(66, 184, 131, 0.07);
  box-shadow: 0 0 0 3px rgba(66, 184, 131, 0.1);
}

.check-card-body {
  flex: 1;
  display: flex;
  flex-direction: column;
  gap: 0.1rem;
}

.check-card-label {
  font-size: 0.85rem;
  font-weight: 600;
  color: var(--color-text);
}

.check-card-hint {
  font-size: 0.76rem;
  color: var(--vt-c-text-light-2, #888);
  line-height: 1.4;
}

.check-indicator {
  width: 20px;
  height: 20px;
  border-radius: 6px;
  border: 1.5px solid var(--color-border-hover);
  display: flex;
  align-items: center;
  justify-content: center;
  flex-shrink: 0;
  transition: background 0.15s, border-color 0.15s;
  margin-top: 1px;
}

.check-card.selected .check-indicator {
  background: #42b883;
  border-color: #42b883;
  color: #fff;
}

.check-indicator svg { width: 12px; height: 12px; }

/* ── Radio pills ── */
.radio-pills {
  display: flex;
  gap: 0.65rem;
  flex-wrap: wrap;
}

.radio-pill {
  flex: 1;
  min-width: 130px;
  display: flex;
  flex-direction: column;
  gap: 0.2rem;
  padding: 0.85rem 1rem;
  border: 1.5px solid var(--color-border);
  border-radius: 12px;
  cursor: pointer;
  transition: border-color 0.15s, background 0.15s, box-shadow 0.15s;
}

.radio-pill input { display: none; }

.radio-pill:hover {
  border-color: #42b883;
  background: rgba(66, 184, 131, 0.04);
}

.radio-pill.selected {
  border-color: #42b883;
  background: rgba(66, 184, 131, 0.07);
  box-shadow: 0 0 0 3px rgba(66, 184, 131, 0.1);
}

.pill-label {
  font-size: 0.875rem;
  font-weight: 700;
  color: var(--color-text);
}

.pill-hint {
  font-size: 0.775rem;
  color: var(--vt-c-text-light-2, #888);
  line-height: 1.4;
}

/* ── Inline fields (language) ── */
.inline-fields {
  display: flex;
  gap: 0.75rem;
  flex-wrap: wrap;
}

/* ── Selects & inputs ── */
.select-input {
  height: 42px;
  padding: 0 0.85rem;
  border: 1.5px solid var(--color-border-hover);
  border-radius: 10px;
  background: var(--color-background);
  color: var(--color-text);
  font-size: 0.9rem;
  outline: none;
  cursor: pointer;
  transition: border-color 0.2s, box-shadow 0.2s;
}

.select-input:focus {
  border-color: #42b883;
  box-shadow: 0 0 0 3px rgba(66, 184, 131, 0.15);
}

.text-input {
  height: 42px;
  padding: 0 0.85rem;
  border: 1.5px solid var(--color-border-hover);
  border-radius: 10px;
  background: var(--color-background);
  color: var(--color-text);
  font-size: 0.9rem;
  outline: none;
  transition: border-color 0.2s, box-shadow 0.2s;
}

.text-input:focus {
  border-color: #42b883;
  box-shadow: 0 0 0 3px rgba(66, 184, 131, 0.15);
}

/* ── Context grid ── */
.context-grid {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 1rem;
}

.field {
  display: flex;
  flex-direction: column;
  gap: 0.4rem;
}

.field label {
  font-size: 0.8rem;
  font-weight: 600;
  color: var(--color-text);
  letter-spacing: 0.01em;
}

/* ── Toggles ── */
.toggle-list {
  display: flex;
  flex-direction: column;
  gap: 0;
}

.toggle-row {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 1rem;
  padding: 0.85rem 0;
  border-bottom: 1px solid var(--color-border);
}

.toggle-row:last-child { border-bottom: none; }

.toggle-label {
  font-size: 0.9rem;
  color: var(--color-text);
  line-height: 1.4;
}

.toggle-btn {
  position: relative;
  width: 46px;
  height: 26px;
  border-radius: 99px;
  border: none;
  background: var(--color-border-hover);
  cursor: pointer;
  transition: background 0.25s;
  flex-shrink: 0;
  padding: 0;
}

.toggle-btn.active { background: #42b883; }

.toggle-thumb {
  position: absolute;
  top: 3px;
  left: 3px;
  width: 20px;
  height: 20px;
  border-radius: 50%;
  background: #fff;
  box-shadow: 0 1px 4px rgba(0, 0, 0, 0.2);
  transition: transform 0.25s cubic-bezier(0.34, 1.56, 0.64, 1);
  display: block;
}

.toggle-btn.active .toggle-thumb { transform: translateX(20px); }

/* ── Feedback ── */
.error {
  color: #ef4444;
  font-size: 0.875rem;
  background: rgba(239, 68, 68, 0.08);
  border: 1px solid rgba(239, 68, 68, 0.2);
  border-radius: 10px;
  padding: 0.65rem 0.85rem;
}

.err-enter-active, .err-leave-active { transition: opacity 0.2s; }
.err-enter-from, .err-leave-to { opacity: 0; }

/* ── Bottom save button ── */
.save-btn-bottom {
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 0.5rem;
  height: 52px;
  background: linear-gradient(135deg, #42b883 0%, #2d9a6a 100%);
  color: #fff;
  border: none;
  border-radius: 14px;
  font-size: 1rem;
  font-weight: 700;
  cursor: pointer;
  box-shadow: 0 4px 18px rgba(66, 184, 131, 0.38);
  transition: transform 0.15s, box-shadow 0.15s, opacity 0.15s;
}

.save-btn-bottom:hover:not(:disabled) {
  transform: translateY(-1px);
  box-shadow: 0 6px 24px rgba(66, 184, 131, 0.48);
}

.save-btn-bottom:disabled { opacity: 0.65; cursor: not-allowed; }

/* ── Spinner ── */
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

/* ── Responsive ── */
@media (max-width: 560px) {
  .profile-body { padding: 1.5rem 1rem 4rem; }
  .section-card { padding: 1.35rem; }
  .checkbox-grid, .context-grid { grid-template-columns: 1fr; }
  .radio-pills { flex-direction: column; }
  .top-bar-inner { padding: 0.75rem 1rem; }
}

/* ── Password section ── */
.pw-fields {
  display: flex;
  flex-direction: column;
  gap: 0.85rem;
}

.pw-feedback {
  margin-top: 0.75rem;
}

.saved-inline {
  margin-top: 0.65rem;
  font-size: 0.85rem;
  font-weight: 600;
  color: #42b883;
}

.pw-btn {
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 0.5rem;
  margin-top: 1.1rem;
  height: 44px;
  padding: 0 1.5rem;
  background: linear-gradient(135deg, #f97316, #ea580c);
  color: #fff;
  border: none;
  border-radius: 12px;
  font-size: 0.925rem;
  font-weight: 700;
  cursor: pointer;
  box-shadow: 0 4px 14px rgba(249, 115, 22, 0.3);
  transition: transform 0.15s, box-shadow 0.15s, opacity 0.15s;
}

.pw-btn:hover:not(:disabled) {
  transform: translateY(-1px);
  box-shadow: 0 6px 18px rgba(249, 115, 22, 0.4);
}

.pw-btn:disabled { opacity: 0.65; cursor: not-allowed; }

</style>
