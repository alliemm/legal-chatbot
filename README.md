# LegalEye 
**The "Pocket Lawyer" for Personalized Risk Assessment & Contract Analysis**

LegalEye is an AI-powered legal technology agent designed to democratize access to legal counsel. Unlike traditional legal chatbots that merely summarize text, LegalEye utilizes a **Personalized Risk Engine** to cross-reference document clauses against a user’s specific profile, values, and jurisdiction to flag unique privacy violations and potential lawsuit risks.

---

## Key Features

- **Personalized Risk Engine:** Leverages Google Gemini API to analyze Terms & Conditions and contracts through the lens of individual user profiles.
- **Smart Contract Editing:** Beyond identification, the platform allows users to proactively modify high-risk terms via a managed AI workflow.
- **Risk Scoring:** Assigns a "Violation Probability" score based on current data privacy laws (GDPR, CCPA) and user-specific liability concerns.
- **Enterprise-Grade Security:** Built with a focus on data integrity for sensitive legal information.

---

## Secure Software Development Lifecycle (SSDS)

LegalEye was architected with a security-first mindset to ensure the integrity of private legal data:
- **Role-Based Access Control (RBAC):** Granular permission levels to protect sensitive user documents.
- **Data Integrity:** Implementation of state machine logic for document status tracking to prevent unauthorized modifications.
- **API Security:** Secure orchestration of LLM requests with environment-level credential management and sanitization.

---

## Tech Stack

- **Frontend:** Vue.js (Reactive dashboard for document review)
- **Backend:** Flask / Python (Handling core business logic and API orchestration)
- **AI/NLP:** Google Gemini API (Custom prompt chains for risk assessment)
- **Containerization:** Docker & Docker Compose (For reproducible development environments)
- **Languages:** JavaScript, C++, Vue, Python

---

## Project Structure

- `/frontend`: Vue.js application for the user interface.
- `/backend`: Python API handling PDF parsing and AI logic.
- `/extension`: Prototype for a browser extension to analyze T&Cs in real-time.
- `/fake-server`: Mock environment for rapid testing of state machine transitions.

---

## Roadmap

- [ ] **Automated Redlining:** Integrating programmatic contract suggestions.
- [ ] **Jurisdiction Expansion:** Fine-tuning risk models for international legal standards.
- [ ] **Tiered Monetization:** Implementation of the fee-for-service model for professional contract reviews.

---

##  Project Status: Pre-Deployment 
**Current Phase:** Finalizing cloud infrastructure for Beta Launch.

- [x] Core AI Risk Engine (Gemini Integration)
- [x] Secure SSDS Backend (Flask/Python)
- [x] Responsive Dashboard (Vue.js)
- [ ] **In Progress:** CI/CD Pipeline & AWS/GCP Deployment
- [ ] **In Progress:** Finalizing SSL/TLS encryption for production traffic

*Targeting a live MVP release by [May 4th] to initial beta testers.*

---

