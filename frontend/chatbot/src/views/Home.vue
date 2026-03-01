<template>
  <div class="home-page">
    <div class="home-inner">

      <!-- Brand -->
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

      <h1 class="hero-title">Your AI Legal Document Assistant</h1>
      <p class="hero-sub">Understand what you're signing. Spot risks before they matter.</p>

      <!-- Primary actions -->
      <div class="primary-actions">
        <router-link to="/login" class="btn-primary">Sign in</router-link>
        <router-link to="/signup" class="btn-secondary">Create account</router-link>
      </div>

      <!-- Page nav cards -->
      <div class="nav-grid">
        <router-link v-for="page in pages" :key="page.to" :to="page.to" class="nav-card">
          <div class="nav-card-icon" :style="{ background: page.bg, color: page.color }">
            <svg viewBox="0 0 20 20" fill="currentColor" v-html="page.icon"></svg>
          </div>
          <div class="nav-card-body">
            <span class="nav-card-title">{{ page.title }}</span>
            <span class="nav-card-desc">{{ page.desc }}</span>
          </div>
          <svg class="nav-arrow" viewBox="0 0 20 20" fill="currentColor">
            <path fill-rule="evenodd" d="M7.293 14.707a1 1 0 010-1.414L10.586 10 7.293 6.707a1 1 0 011.414-1.414l4 4a1 1 0 010 1.414l-4 4a1 1 0 01-1.414 0z" clip-rule="evenodd"/>
          </svg>
        </router-link>
      </div>

    </div>
  </div>
</template>

<script>
export default {
  data() {
    return {
      pages: [
        {
          to: '/login',
          title: 'Login',
          desc: 'Sign in to your existing account',
          bg: 'rgba(66,184,131,0.12)',
          color: '#42b883',
          icon: '<path fill-rule="evenodd" d="M3 3a1 1 0 011 1v12a1 1 0 11-2 0V4a1 1 0 011-1zm7.707 3.293a1 1 0 010 1.414L9.414 9H17a1 1 0 110 2H9.414l1.293 1.293a1 1 0 01-1.414 1.414l-3-3a1 1 0 010-1.414l3-3a1 1 0 011.414 0z" clip-rule="evenodd"/>'
        },
        {
          to: '/signup',
          title: 'Sign Up',
          desc: 'Create a new account to get started',
          bg: 'rgba(59,130,246,0.12)',
          color: '#3b82f6',
          icon: '<path d="M8 9a3 3 0 100-6 3 3 0 000 6zM8 11a6 6 0 016 6H2a6 6 0 016-6zM16 7a1 1 0 10-2 0v1h-1a1 1 0 100 2h1v1a1 1 0 102 0v-1h1a1 1 0 100-2h-1V7z"/>'
        },
        {
          to: '/survey',
          title: 'Survey',
          desc: 'Personalize your experience in 2 min',
          bg: 'rgba(139,92,246,0.12)',
          color: '#8b5cf6',
          icon: '<path fill-rule="evenodd" d="M3 4a1 1 0 011-1h12a1 1 0 110 2H4a1 1 0 01-1-1zm0 4a1 1 0 011-1h12a1 1 0 110 2H4a1 1 0 01-1-1zm0 4a1 1 0 011-1h8a1 1 0 110 2H4a1 1 0 01-1-1zm0 4a1 1 0 011-1h4a1 1 0 110 2H4a1 1 0 01-1-1z" clip-rule="evenodd"/>'
        },
        {
          to: '/profile',
          title: 'Profile',
          desc: 'Manage your preferences and settings',
          bg: 'rgba(249,115,22,0.12)',
          color: '#f97316',
          icon: '<path fill-rule="evenodd" d="M10 9a3 3 0 100-6 3 3 0 000 6zm-7 9a7 7 0 1114 0H3z" clip-rule="evenodd"/>'
        }
      ]
    }
  }
}
</script>

<style scoped>
.home-page {
  min-height: 100vh;
  display: flex;
  align-items: center;
  justify-content: center;
  padding: 2rem 1.5rem;
  background:
    radial-gradient(ellipse 130% 70% at 50% -10%, rgba(66, 184, 131, 0.14) 0%, transparent 60%),
    var(--color-background);
}

.home-inner {
  width: 100%;
  max-width: 520px;
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
  gap: 0.6rem;
  margin-bottom: 2rem;
}

.brand-icon {
  width: 40px;
  height: 40px;
  background: linear-gradient(135deg, #42b883, #2d9a6a);
  border-radius: 12px;
  display: flex;
  align-items: center;
  justify-content: center;
  color: #fff;
  padding: 8px;
  flex-shrink: 0;
}

.brand-icon svg { width: 100%; height: 100%; }

.brand-name {
  font-size: 1.25rem;
  font-weight: 800;
  color: var(--color-heading);
  letter-spacing: -0.03em;
}

/* Hero */
.hero-title {
  font-size: 2.25rem;
  font-weight: 800;
  color: var(--color-heading);
  letter-spacing: -0.04em;
  line-height: 1.15;
  margin-bottom: 0.75rem;
}

.hero-sub {
  font-size: 1.05rem;
  color: var(--vt-c-text-light-2, #888);
  margin-bottom: 2rem;
  line-height: 1.5;
}

/* Primary actions */
.primary-actions {
  display: flex;
  gap: 0.75rem;
  margin-bottom: 2.5rem;
}

.btn-primary {
  flex: 1;
  height: 48px;
  display: flex;
  align-items: center;
  justify-content: center;
  background: linear-gradient(135deg, #42b883, #2d9a6a);
  color: #fff;
  border-radius: 12px;
  font-size: 1rem;
  font-weight: 700;
  text-decoration: none;
  box-shadow: 0 4px 15px rgba(66, 184, 131, 0.35);
  transition: transform 0.15s, box-shadow 0.15s;
}

.btn-primary:hover {
  transform: translateY(-1px);
  box-shadow: 0 6px 20px rgba(66, 184, 131, 0.45);
}

.btn-secondary {
  flex: 1;
  height: 48px;
  display: flex;
  align-items: center;
  justify-content: center;
  background: var(--color-background-soft);
  color: var(--color-text);
  border-radius: 12px;
  font-size: 1rem;
  font-weight: 600;
  text-decoration: none;
  border: 1.5px solid var(--color-border-hover);
  transition: border-color 0.15s, background 0.15s;
}

.btn-secondary:hover {
  border-color: #42b883;
  background: rgba(66, 184, 131, 0.05);
}

/* Nav cards */
.nav-grid {
  display: flex;
  flex-direction: column;
  gap: 0.65rem;
}

.nav-card {
  display: flex;
  align-items: center;
  gap: 1rem;
  padding: 1rem 1.1rem;
  background: var(--color-background-soft);
  border-radius: 14px;
  text-decoration: none;
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.05);
  border: 1.5px solid var(--color-border);
  transition: border-color 0.15s, transform 0.15s, box-shadow 0.15s;
}

.nav-card:hover {
  border-color: #42b883;
  transform: translateX(3px);
  box-shadow: 0 4px 16px rgba(0, 0, 0, 0.08);
}

.nav-card-icon {
  width: 40px;
  height: 40px;
  border-radius: 10px;
  display: flex;
  align-items: center;
  justify-content: center;
  flex-shrink: 0;
  padding: 9px;
}

.nav-card-icon svg { width: 100%; height: 100%; }

.nav-card-body {
  flex: 1;
  display: flex;
  flex-direction: column;
  gap: 0.1rem;
}

.nav-card-title {
  font-size: 0.95rem;
  font-weight: 700;
  color: var(--color-heading);
}

.nav-card-desc {
  font-size: 0.82rem;
  color: var(--vt-c-text-light-2, #888);
}

.nav-arrow {
  width: 18px;
  height: 18px;
  color: var(--vt-c-text-light-2, #aaa);
  flex-shrink: 0;
  transition: transform 0.15s, color 0.15s;
}

.nav-card:hover .nav-arrow {
  transform: translateX(3px);
  color: #42b883;
}
</style>
