<template>
  <nav class="app-nav">
    <div class="app-nav-inner">
      <router-link to="/" class="nav-brand">
        <div class="nav-brand-icon">
          <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
            <path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z"/>
            <polyline points="14 2 14 8 20 8"/>
            <circle cx="10" cy="15" r="2.5"/>
            <line x1="12" y1="17" x2="14.5" y2="19.5"/>
          </svg>
        </div>
        <span class="nav-brand-name">LexAssist</span>
      </router-link>

      <div class="nav-links">
        <router-link to="/login" class="nav-btn nav-btn-outline">Sign in</router-link>
        <router-link to="/signup" class="nav-btn nav-btn-primary">Create account</router-link>
      </div>
    </div>
  </nav>
</template>

<script>
export default {}
</script>

<style scoped>
.app-nav {
  position: fixed;
  top: 0;
  left: 0;
  right: 0;
  height: 56px;
  z-index: 900;
  background: rgba(248, 248, 248, 0.88);
  backdrop-filter: blur(12px);
  -webkit-backdrop-filter: blur(12px);
  border-bottom: 1px solid var(--color-border);
  box-shadow: 0 1px 8px rgba(0, 0, 0, 0.05);
}

@media (prefers-color-scheme: dark) {
  .app-nav {
    background: rgba(34, 34, 34, 0.88);
  }
}

.app-nav-inner {
  max-width: 1100px;
  margin: 0 auto;
  height: 100%;
  padding: 0 1.5rem;
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 1rem;
}

/* Brand */
.nav-brand {
  display: flex;
  align-items: center;
  gap: 0.5rem;
  text-decoration: none;
  flex-shrink: 0;
}

.nav-brand-icon {
  width: 30px;
  height: 30px;
  background: linear-gradient(135deg, #42b883, #2d9a6a);
  border-radius: 8px;
  display: flex;
  align-items: center;
  justify-content: center;
  color: #fff;
  padding: 6px;
}

.nav-brand-icon svg { width: 100%; height: 100%; }

.nav-brand-name {
  font-size: 1rem;
  font-weight: 800;
  color: var(--color-heading);
  letter-spacing: -0.02em;
}

/* Links */
.nav-links {
  display: flex;
  align-items: center;
  gap: 0.5rem;
}

.nav-btn {
  height: 34px;
  padding: 0 1rem;
  display: flex;
  align-items: center;
  border-radius: 9px;
  font-size: 0.875rem;
  font-weight: 600;
  text-decoration: none;
  transition: transform 0.15s, box-shadow 0.15s, background 0.15s, border-color 0.15s;
  white-space: nowrap;
}

.nav-btn-outline {
  background: transparent;
  color: var(--color-text);
  border: 1.5px solid var(--color-border-hover);
}

.nav-btn-outline:hover {
  border-color: #42b883;
  color: #42b883;
  background: rgba(66, 184, 131, 0.05);
}

.nav-btn-outline.router-link-active {
  color: #42b883;
  border-color: rgba(66, 184, 131, 0.4);
}

.nav-btn-primary {
  background: linear-gradient(135deg, #42b883, #2d9a6a);
  color: #fff;
  border: none;
  box-shadow: 0 2px 8px rgba(66, 184, 131, 0.3);
}

.nav-btn-primary:hover {
  transform: translateY(-1px);
  box-shadow: 0 4px 14px rgba(66, 184, 131, 0.42);
}

.nav-btn-primary.router-link-active {
  box-shadow: 0 0 0 3px rgba(66, 184, 131, 0.25);
}

@media (max-width: 400px) {
  .app-nav-inner { padding: 0 1rem; }
  .nav-btn { padding: 0 0.75rem; font-size: 0.82rem; }
}
</style>
