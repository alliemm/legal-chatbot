<template>
  <!-- Floating Action Button -->
  <button class="chat-fab" @click="open = !open" :aria-expanded="open" :aria-label="open ? 'Close chat' : 'Open chat'">
    <!-- Chat icon when closed -->
    <svg v-if="!open" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
      <path d="M21 15a2 2 0 0 1-2 2H7l-4 4V5a2 2 0 0 1 2-2h14a2 2 0 0 1 2 2z"/>
    </svg>
    <!-- Close icon when open -->
    <svg v-else viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round">
      <line x1="18" y1="6" x2="6" y2="18"/>
      <line x1="6" y1="6" x2="18" y2="18"/>
    </svg>
  </button>

  <!-- Backdrop -->
  <transition name="fade">
    <div v-if="open" class="chat-backdrop" @click="open = false" />
  </transition>

  <!-- Slide-in Panel -->
  <transition name="slide">
    <div v-if="open" class="chat-panel" role="dialog" aria-label="LexAssist Chat">

      <!-- Header -->
      <div class="chat-header">
        <div class="chat-header-brand">
          <div class="chat-header-icon">
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
              <path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z"/>
              <polyline points="14 2 14 8 20 8"/>
              <circle cx="10" cy="15" r="2.5"/>
              <line x1="12" y1="17" x2="14.5" y2="19.5"/>
            </svg>
          </div>
          <span class="chat-header-title">LexAssist Chat</span>
        </div>
        <button class="chat-close-btn" @click="open = false" aria-label="Close chat">
          <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round">
            <line x1="18" y1="6" x2="6" y2="18"/>
            <line x1="6" y1="6" x2="18" y2="18"/>
          </svg>
        </button>
      </div>

      <!-- Panel Navigation -->
      <div class="panel-nav">
        <template v-if="!isLoggedIn">
          <router-link to="/login" class="panel-nav-btn panel-nav-primary" @click="open = false">Sign in</router-link>
          <router-link to="/signup" class="panel-nav-btn panel-nav-secondary" @click="open = false">Create account</router-link>
        </template>
        <template v-else>
          <router-link to="/profile" class="panel-nav-btn panel-nav-ghost" @click="open = false">
            <svg viewBox="0 0 20 20" fill="currentColor" style="width:14px;height:14px;flex-shrink:0">
              <path fill-rule="evenodd" d="M10 9a3 3 0 100-6 3 3 0 000 6zm-7 9a7 7 0 1114 0H3z" clip-rule="evenodd"/>
            </svg>
            Profile
          </router-link>
          <router-link to="/survey" class="panel-nav-btn panel-nav-ghost" @click="open = false">
            <svg viewBox="0 0 20 20" fill="currentColor" style="width:14px;height:14px;flex-shrink:0">
              <path fill-rule="evenodd" d="M3 4a1 1 0 011-1h12a1 1 0 110 2H4a1 1 0 01-1-1zm0 4a1 1 0 011-1h12a1 1 0 110 2H4a1 1 0 01-1-1zm0 4a1 1 0 011-1h8a1 1 0 110 2H4a1 1 0 01-1-1zm0 4a1 1 0 011-1h4a1 1 0 110 2H4a1 1 0 01-1-1z" clip-rule="evenodd"/>
            </svg>
            Survey
          </router-link>
        </template>
      </div>

      <!-- Document Selector -->
      <div v-if="documents.length" class="doc-selector">
        <button class="doc-toggle" @click="showDocs = !showDocs" type="button">
          <svg viewBox="0 0 20 20" fill="currentColor">
            <path fill-rule="evenodd" d="M4 4a2 2 0 012-2h4.586A2 2 0 0112 2.586L15.414 6A2 2 0 0116 7.414V16a2 2 0 01-2 2H6a2 2 0 01-2-2V4z" clip-rule="evenodd"/>
          </svg>
          <span>Documents ({{ selectedDocs.length || 'none selected' }})</span>
          <svg class="doc-chevron" :class="{ rotated: showDocs }" viewBox="0 0 20 20" fill="currentColor">
            <path fill-rule="evenodd" d="M5.293 7.293a1 1 0 011.414 0L10 10.586l3.293-3.293a1 1 0 111.414 1.414l-4 4a1 1 0 01-1.414 0l-4-4a1 1 0 010-1.414z" clip-rule="evenodd"/>
          </svg>
        </button>
        <div v-if="showDocs" class="doc-list">
          <label v-for="doc in documents" :key="doc" class="doc-item">
            <input type="checkbox" :value="doc" v-model="selectedDocs" />
            <span class="doc-name">{{ doc }}</span>
          </label>
        </div>
      </div>

      <!-- Messages -->
      <div class="chat-messages" ref="messagesEl">
        <div v-if="messages.length === 0" class="chat-welcome">
          <div class="welcome-icon">
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round">
              <path d="M21 15a2 2 0 0 1-2 2H7l-4 4V5a2 2 0 0 1 2-2h14a2 2 0 0 1 2 2z"/>
            </svg>
          </div>
          <p class="welcome-text">Ask me anything about your legal documents or general legal questions.</p>
        </div>

        <div
          v-for="(msg, i) in messages"
          :key="i"
          :class="['msg', msg.role === 'user' ? 'msg-user' : 'msg-model']"
        >
          <div class="msg-bubble">{{ msg.text }}</div>
        </div>

        <div v-if="loading" class="msg msg-model">
          <div class="msg-bubble loading-bubble">
            <span class="dot"></span>
            <span class="dot"></span>
            <span class="dot"></span>
          </div>
        </div>

        <div v-if="error" class="msg-error">{{ error }}</div>
      </div>

      <!-- Input Bar -->
      <form class="chat-input-bar" @submit.prevent="send">
        <input
          v-model="input"
          class="chat-input"
          placeholder="Ask about your documents…"
          :disabled="loading"
          autocomplete="off"
          @keydown.enter.exact.prevent="send"
        />
        <button
          type="submit"
          class="chat-send-btn"
          :disabled="!input.trim() || loading"
          aria-label="Send message"
        >
          <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round">
            <line x1="22" y1="2" x2="11" y2="13"/>
            <polygon points="22 2 15 22 11 13 2 9 22 2"/>
          </svg>
        </button>
      </form>

    </div>
  </transition>
</template>

<script>
import axios from 'axios'

const BASE = 'http://localhost:18080'

export default {
  data() {
    return {
      open: false,
      input: '',
      messages: [],
      loading: false,
      documents: [],
      selectedDocs: [],
      showDocs: false,
      error: '',
      isLoggedIn: false
    }
  },
  async created() {
    try {
      const res = await axios.get(`${BASE}/profile`, { withCredentials: true })
      if (res.data?.documents) {
        this.documents = res.data.documents
      }
      this.isLoggedIn = true
    } catch {
      this.isLoggedIn = false
    }
  },
  methods: {
    async send() {
      const text = this.input.trim()
      if (!text || this.loading) return

      this.error = ''
      this.input = ''
      this.messages.push({ role: 'user', text })
      this.scrollToBottom()

      this.loading = true
      try {
        const res = await axios.post(`${BASE}/chat`, {
          message: text,
          history: this.messages.slice(0, -1), // history before this message
          documentNames: this.selectedDocs
        }, { withCredentials: true })

        this.messages.push({ role: 'model', text: res.data.reply })
      } catch (err) {
        const msg = err.response?.status === 401
          ? 'Please log in to use the chat.'
          : (err.response?.data?.message || 'Something went wrong. Please try again.')
        this.error = msg
      } finally {
        this.loading = false
        this.scrollToBottom()
      }
    },
    scrollToBottom() {
      this.$nextTick(() => {
        const el = this.$refs.messagesEl
        if (el) el.scrollTop = el.scrollHeight
      })
    }
  }
}
</script>

<style scoped>
/* ── Panel Nav ── */
.panel-nav {
  display: flex;
  gap: 0.5rem;
  padding: 0.75rem 1rem;
  border-bottom: 1px solid var(--color-border);
  flex-shrink: 0;
}

.panel-nav-btn {
  flex: 1;
  height: 36px;
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 0.35rem;
  border-radius: 10px;
  font-size: 0.85rem;
  font-weight: 600;
  text-decoration: none;
  transition: transform 0.15s, box-shadow 0.15s, background 0.15s, border-color 0.15s, color 0.15s;
}

.panel-nav-primary {
  background: linear-gradient(135deg, #42b883, #2d9a6a);
  color: #fff;
  box-shadow: 0 2px 8px rgba(66, 184, 131, 0.3);
}

.panel-nav-primary:hover {
  transform: translateY(-1px);
  box-shadow: 0 4px 12px rgba(66, 184, 131, 0.4);
}

.panel-nav-secondary {
  background: var(--color-background-mute);
  color: var(--color-text);
  border: 1.5px solid var(--color-border-hover);
}

.panel-nav-secondary:hover {
  border-color: #42b883;
  background: rgba(66, 184, 131, 0.05);
}

.panel-nav-ghost {
  background: var(--color-background-mute);
  color: var(--color-text);
  border: 1.5px solid var(--color-border);
  font-size: 0.82rem;
}

.panel-nav-ghost:hover {
  border-color: #42b883;
  color: #42b883;
}

/* ── FAB ── */
.chat-fab {
  position: fixed;
  bottom: 24px;
  right: 24px;
  z-index: 1001;
  width: 56px;
  height: 56px;
  border-radius: 50%;
  border: none;
  background: linear-gradient(135deg, #42b883, #2d9a6a);
  color: #fff;
  cursor: pointer;
  display: flex;
  align-items: center;
  justify-content: center;
  box-shadow: 0 4px 18px rgba(66, 184, 131, 0.38);
  transition: transform 0.15s, box-shadow 0.15s;
  padding: 14px;
}

.chat-fab:hover {
  transform: translateY(-2px);
  box-shadow: 0 6px 24px rgba(66, 184, 131, 0.48);
}

.chat-fab svg { width: 100%; height: 100%; }

/* ── Backdrop ── */
.chat-backdrop {
  position: fixed;
  inset: 0;
  z-index: 999;
  background: rgba(0, 0, 0, 0.25);
}

/* ── Panel ── */
.chat-panel {
  position: fixed;
  top: 0;
  right: 0;
  height: 100vh;
  width: 380px;
  z-index: 1000;
  background: var(--color-background-soft);
  border-left: 1px solid var(--color-border);
  box-shadow: -4px 0 32px rgba(0, 0, 0, 0.12);
  display: flex;
  flex-direction: column;
  overflow: hidden;
}

/* ── Header ── */
.chat-header {
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 1rem 1.1rem;
  border-bottom: 1px solid var(--color-border);
  background: var(--color-background-soft);
  flex-shrink: 0;
}

.chat-header-brand {
  display: flex;
  align-items: center;
  gap: 0.5rem;
}

.chat-header-icon {
  width: 30px;
  height: 30px;
  background: linear-gradient(135deg, #42b883, #2d9a6a);
  border-radius: 8px;
  display: flex;
  align-items: center;
  justify-content: center;
  color: #fff;
  padding: 6px;
  flex-shrink: 0;
}

.chat-header-icon svg { width: 100%; height: 100%; }

.chat-header-title {
  font-size: 0.95rem;
  font-weight: 700;
  color: var(--color-heading);
  letter-spacing: -0.02em;
}

.chat-close-btn {
  width: 32px;
  height: 32px;
  border: none;
  background: var(--color-background-mute);
  border-radius: 8px;
  cursor: pointer;
  display: flex;
  align-items: center;
  justify-content: center;
  color: var(--color-text);
  padding: 7px;
  transition: background 0.15s;
}

.chat-close-btn:hover { background: var(--color-border-hover); }
.chat-close-btn svg { width: 100%; height: 100%; }

/* ── Doc Selector ── */
.doc-selector {
  border-bottom: 1px solid var(--color-border);
  flex-shrink: 0;
}

.doc-toggle {
  width: 100%;
  display: flex;
  align-items: center;
  gap: 0.5rem;
  padding: 0.65rem 1.1rem;
  background: none;
  border: none;
  cursor: pointer;
  color: var(--color-text);
  font-size: 0.82rem;
  font-weight: 600;
  text-align: left;
  transition: background 0.15s;
}

.doc-toggle:hover { background: var(--color-background-mute); }

.doc-toggle svg:first-child {
  width: 16px;
  height: 16px;
  color: #42b883;
  flex-shrink: 0;
}

.doc-toggle span { flex: 1; }

.doc-chevron {
  width: 16px;
  height: 16px;
  flex-shrink: 0;
  transition: transform 0.2s;
}

.doc-chevron.rotated { transform: rotate(180deg); }

.doc-list {
  padding: 0.5rem 1.1rem 0.75rem;
  display: flex;
  flex-direction: column;
  gap: 0.4rem;
  max-height: 140px;
  overflow-y: auto;
}

.doc-item {
  display: flex;
  align-items: center;
  gap: 0.5rem;
  cursor: pointer;
  font-size: 0.82rem;
  color: var(--color-text);
}

.doc-item input[type="checkbox"] {
  accent-color: #42b883;
  width: 14px;
  height: 14px;
  flex-shrink: 0;
}

.doc-name {
  overflow: hidden;
  text-overflow: ellipsis;
  white-space: nowrap;
}

/* ── Messages ── */
.chat-messages {
  flex: 1;
  overflow-y: auto;
  padding: 1rem;
  display: flex;
  flex-direction: column;
  gap: 0.65rem;
  scroll-behavior: smooth;
}

.chat-welcome {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  gap: 0.75rem;
  padding: 2rem 1rem;
  text-align: center;
  flex: 1;
}

.welcome-icon {
  width: 48px;
  height: 48px;
  background: rgba(66, 184, 131, 0.12);
  border-radius: 14px;
  display: flex;
  align-items: center;
  justify-content: center;
  color: #42b883;
  padding: 11px;
}

.welcome-icon svg { width: 100%; height: 100%; }

.welcome-text {
  font-size: 0.875rem;
  color: var(--vt-c-text-light-2, #888);
  line-height: 1.5;
  max-width: 240px;
}

.msg {
  display: flex;
  max-width: 88%;
}

.msg-user {
  align-self: flex-end;
  justify-content: flex-end;
}

.msg-model {
  align-self: flex-start;
}

.msg-bubble {
  padding: 0.65rem 0.9rem;
  font-size: 0.875rem;
  line-height: 1.5;
  white-space: pre-wrap;
  word-break: break-word;
}

.msg-user .msg-bubble {
  background: rgba(66, 184, 131, 0.12);
  color: var(--color-text);
  border-radius: 14px 14px 4px 14px;
  border: 1px solid rgba(66, 184, 131, 0.2);
}

.msg-model .msg-bubble {
  background: var(--color-background-mute);
  color: var(--color-text);
  border-radius: 14px 14px 14px 4px;
  border: 1px solid var(--color-border);
}

/* Loading dots */
.loading-bubble {
  display: flex;
  align-items: center;
  gap: 4px;
  padding: 0.75rem 1rem;
}

.dot {
  width: 7px;
  height: 7px;
  border-radius: 50%;
  background: var(--color-border-hover);
  animation: pulse 1.2s ease-in-out infinite;
}

.dot:nth-child(2) { animation-delay: 0.2s; }
.dot:nth-child(3) { animation-delay: 0.4s; }

@keyframes pulse {
  0%, 80%, 100% { transform: scale(0.8); opacity: 0.5; }
  40% { transform: scale(1.1); opacity: 1; }
}

.msg-error {
  font-size: 0.82rem;
  color: #ef4444;
  background: rgba(239, 68, 68, 0.08);
  border: 1px solid rgba(239, 68, 68, 0.2);
  border-radius: 10px;
  padding: 0.55rem 0.8rem;
  align-self: stretch;
}

/* ── Input Bar ── */
.chat-input-bar {
  display: flex;
  gap: 0.5rem;
  padding: 0.85rem 1rem;
  border-top: 1px solid var(--color-border);
  background: var(--color-background-soft);
  flex-shrink: 0;
}

.chat-input {
  flex: 1;
  height: 40px;
  padding: 0 0.85rem;
  border: 1.5px solid var(--color-border-hover);
  border-radius: 10px;
  background: var(--color-background);
  color: var(--color-text);
  font-size: 0.875rem;
  outline: none;
  transition: border-color 0.2s, box-shadow 0.2s;
  font-family: inherit;
}

.chat-input:focus {
  border-color: #42b883;
  box-shadow: 0 0 0 3px rgba(66, 184, 131, 0.15);
}

.chat-input:disabled { opacity: 0.6; cursor: not-allowed; }

.chat-send-btn {
  width: 40px;
  height: 40px;
  border: none;
  border-radius: 10px;
  background: linear-gradient(135deg, #42b883, #2d9a6a);
  color: #fff;
  cursor: pointer;
  display: flex;
  align-items: center;
  justify-content: center;
  padding: 10px;
  flex-shrink: 0;
  transition: transform 0.15s, box-shadow 0.15s, opacity 0.15s;
  box-shadow: 0 2px 8px rgba(66, 184, 131, 0.3);
}

.chat-send-btn:hover:not(:disabled) {
  transform: translateY(-1px);
  box-shadow: 0 4px 12px rgba(66, 184, 131, 0.4);
}

.chat-send-btn:disabled { opacity: 0.45; cursor: not-allowed; }
.chat-send-btn svg { width: 100%; height: 100%; }

/* ── Transitions ── */
.slide-enter-active {
  transition: transform 0.3s cubic-bezier(0.22, 1, 0.36, 1);
}
.slide-leave-active {
  transition: transform 0.25s cubic-bezier(0.55, 0, 1, 0.45);
}
.slide-enter-from,
.slide-leave-to {
  transform: translateX(100%);
}

.fade-enter-active { transition: opacity 0.2s; }
.fade-leave-active { transition: opacity 0.2s; }
.fade-enter-from,
.fade-leave-to { opacity: 0; }

/* ── Mobile ── */
@media (max-width: 480px) {
  .chat-panel {
    width: 100vw;
  }
  .chat-fab {
    width: 48px;
    height: 48px;
    bottom: 16px;
    right: 16px;
    padding: 12px;
  }
}
</style>
