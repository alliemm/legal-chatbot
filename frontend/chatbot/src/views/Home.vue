<template>
  <div class="home-page">
    <div class="home-inner">
      <h1 class="hero-title">Your AI Legal Document Assistant</h1>
      <p class="hero-sub">Understand what you're signing. Spot risks before they matter.</p>
      <p class="hero-hint">
        <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
          <path d="M21 15a2 2 0 0 1-2 2H7l-4 4V5a2 2 0 0 1 2-2h14a2 2 0 0 1 2 2z"/>
        </svg>
        Open the chat panel to get started
      </p>
    </div>
  </div>
</template>

<script>
export default {}
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
  max-width: 480px;
  text-align: center;
  animation: fadeSlideUp 0.45s cubic-bezier(0.22, 1, 0.36, 1) both;
}

@keyframes fadeSlideUp {
  from { opacity: 0; transform: translateY(24px); }
  to   { opacity: 1; transform: translateY(0); }
}

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
  line-height: 1.5;
  margin-bottom: 1.75rem;
}

.hero-hint {
  display: inline-flex;
  align-items: center;
  gap: 0.45rem;
  font-size: 0.875rem;
  color: var(--vt-c-text-light-2, #aaa);
  background: var(--color-background-soft);
  border: 1px solid var(--color-border);
  border-radius: 99px;
  padding: 0.45rem 1rem;
}

.hero-hint svg {
  width: 15px;
  height: 15px;
  color: #42b883;
  flex-shrink: 0;
}
</style>
