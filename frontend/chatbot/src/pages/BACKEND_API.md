# LexAssist — Backend API Reference

This document describes every HTTP request the frontend sends so that the C++ backend (listening on `localhost:18080`) knows exactly what to parse.

All requests use `Content-Type: application/json`.
All error responses must include `{ "message": "<human-readable string>" }` so the UI can display it.

---

## 1. `POST /login`

**Triggered by:** `Login.vue` on form submit.

### Request body

```json
{
  "email":    "user@example.com",
  "password": "s3cr3t"
}
```

| Field      | Type   | Notes                  |
|------------|--------|------------------------|
| `email`    | string | Valid e-mail address   |
| `password` | string | Raw plaintext password |

### Success response

HTTP `2xx`. Body content is currently ignored by the frontend (`console.log` only).
After success the user is navigated to `/profile`.

### Error response

HTTP `4xx` / `5xx`:

```json
{ "message": "Invalid email or password." }
```

---

## 2. `POST /signup`

**Triggered by:** `Signup.vue` on form submit.

### Request body

```json
{
  "name":     "Jane Doe",
  "email":    "jane@example.com",
  "password": "s3cr3t"
}
```

| Field      | Type   | Notes                                              |
|------------|--------|----------------------------------------------------|
| `name`     | string | Full name, non-empty                               |
| `email`    | string | Valid e-mail address                               |
| `password` | string | Raw plaintext password (strength validated client-side only) |

### Success response

HTTP `2xx`. Body content is ignored.
After success the user is navigated to `/login`.

### Error response

```json
{ "message": "Could not create account. Please try again." }
```

---

## 3. `GET /profile`

**Triggered by:** `Profile.vue` on page load (component `created` hook).

### Request

No body.

### Success response

Return the saved profile and preferences object for the authenticated user.
The frontend merges the response into its local state with `Object.assign`.
If the user has no saved data yet, returning an empty body or `{}` is fine — the frontend falls back to its defaults.

**Full schema:**

```json
{
  "nickname":          "Jane",
  "email":             "jane@example.com",
  "focusAreas":        ["privacy", "fees"],
  "explanationStyle":  "balanced",
  "detailLevel":       "standard",
  "riskSensitivity":   "medium",
  "language":          "English",
  "languageOther":     "",
  "userType":          "Individual",
  "legalExperience":   "No prior lawyer",
  "docFrequency":      "Rarely",
  "primaryGoal":       "Understand what I'm signing",
  "behaviourFlags": {
    "showRiskRating":  true,
    "recommendLawyer": true,
    "showDeadlines":   false,
    "showTldr":        true,
    "askClarifying":   false
  },
  "saveHistory":  "yes",
  "emailDigest":  "never",
  "analytics":    "yes"
}
```

See Section 4 for the full enumeration of valid values.

---

## 4. `POST /profile`

**Triggered by:** `Profile.vue` when the user clicks "Save preferences".

### Request body

Exactly the same schema as the `GET /profile` response above.

#### Field reference

| Field              | Type            | Allowed values |
|--------------------|-----------------|----------------|
| `nickname`         | `string`        | Free text display name, may be empty |
| `email`            | `string`        | Valid e-mail address |
| `focusAreas`       | `string[]`      | Any subset of: `"privacy"`, `"fees"`, `"termination"`, `"liability"`, `"ip"`, `"dispute"`, `"renewal"`, `"noncompete"` |
| `explanationStyle` | `string`        | `"plain"`, `"balanced"`, `"technical"` |
| `detailLevel`      | `string`        | `"brief"`, `"standard"`, `"detailed"` |
| `riskSensitivity`  | `string`        | `"low"`, `"medium"`, `"high"` |
| `language`         | `string`        | `"English"`, `"中文 (Chinese)"`, `"Español"`, `"Français"`, `"Deutsch"`, `"Other"` |
| `languageOther`    | `string`        | Free text; only meaningful when `language == "Other"` |
| `userType`         | `string`        | `"Individual"`, `"Small business owner"`, `"Nonprofit"`, `"Other"` |
| `legalExperience`  | `string`        | `"No prior lawyer"`, `"Past lawyer"`, `"Currently has lawyer"` |
| `docFrequency`     | `string`        | `"Rarely"`, `"A few times a year"`, `"Monthly"`, `"Weekly"` |
| `primaryGoal`      | `string`        | `"Understand what I'm signing"`, `"Identify risks before I sign"`, `"Know when I need a real lawyer"`, `"Build or negotiate a stronger document"`, `"Settle a dispute"` |
| `behaviourFlags`   | `object`        | See sub-table below |
| `saveHistory`      | `string`        | `"yes"`, `"no"` |
| `emailDigest`      | `string`        | `"never"`, `"weekly"`, `"monthly"` |
| `analytics`        | `string`        | `"yes"`, `"no"` |

**`behaviourFlags` fields** (all `bool`):

| Key               | Default | Meaning |
|-------------------|---------|---------|
| `showRiskRating`  | `true`  | Show Low/Medium/High risk tag per clause |
| `recommendLawyer` | `true`  | Suggest a lawyer when risk is High |
| `showDeadlines`   | `false` | Highlight key dates at top of summary |
| `showTldr`        | `true`  | Prepend a one-line plain-English TL;DR |
| `askClarifying`   | `false` | Ask follow-up questions when context is unclear |

### Success response

HTTP `2xx`. Body content is ignored; the frontend shows a "Saved" badge.

### Error response

```json
{ "message": "Could not save preferences. Please try again." }
```

---

## 5. `POST /account/password`

**Triggered by:** `Profile.vue` when the user clicks "Update password".

This is a **separate** endpoint from `/profile` so that password changes can be handled with dedicated validation (e.g. rate-limiting, re-authentication) without touching other preferences.

### Request body

```json
{
  "currentPassword": "old-secret",
  "newPassword":     "new-secret"
}
```

| Field             | Type   | Notes |
|-------------------|--------|-------|
| `currentPassword` | string | The user's existing password for verification |
| `newPassword`     | string | The replacement password (no strength rules enforced server-side by this spec, but recommended) |

> **Note:** `confirmPassword` is validated client-side only and is **not** sent to the backend.

### Success response

HTTP `2xx`. Body content is ignored; the frontend clears all three password fields and briefly shows "Password updated".

### Error response

```json
{ "message": "Current password is incorrect." }
```

Common error cases to handle:
- Wrong `currentPassword` → `400` or `401`
- `newPassword` too short / fails policy → `400`
- User not authenticated → `401`

---

## 6. `POST /survey`

**Triggered by:** `Survey.vue` on final step submit.

### Request body

```json
{
  "q1": "Individual",
  "q2": "Yes",
  "q3": "Monthly",
  "q4": ["Contracts & agreements", "NDAs"],
  "q5": ["Hidden clauses or fine print", "Financial risks"],
  "q6": "Somewhat",
  "q7": "Identify risks before I sign"
}
```

#### Field reference

| Field | Type       | Question                                              | Allowed values |
|-------|------------|-------------------------------------------------------|----------------|
| `q1`  | `string`   | What best describes you?                              | `"Individual"`, `"Small business owner"`, `"Nonprofit"`, `"Other"` |
| `q2`  | `string`   | Have you ever worked with a lawyer before?            | `"Yes"`, `"No"`, `"Currently do"` |
| `q3`  | `string`   | How often do you deal with legal documents?           | `"Rarely"`, `"A few times a year"`, `"Monthly"`, `"Weekly"` |
| `q4`  | `string[]` | What types of documents do you most need help with? *(select all)* | `"Contracts & agreements"`, `"Leases & rental agreements"`, `"Employment documents"`, `"Court filings"`, `"Wills & estate documents"`, `"Business formation docs"`, `"NDAs"`, `"Other"` |
| `q5`  | `string[]` | What is your biggest concern? *(select up to 3)*      | `"Hidden clauses or fine print"`, `"Understanding my rights and obligations"`, `"Deadlines and key dates"`, `"Financial risks"`, `"Whether the document is enforceable"`, `"Unfair or one-sided terms"`, `"Plain-language explanation"` |
| `q6`  | `string`   | How comfortable are you with legal jargon?            | `"Very comfortable"`, `"Somewhat"`, `"Not at all"` |
| `q7`  | `string`   | What outcome do you most want from this tool?         | `"Understand what I'm signing"`, `"Identify risks before I sign"`, `"Know when I need a real lawyer"`, `"Build or negotiate a stronger document"`, `"Settle a dispute"` |

Notes:
- `q4` is an array with no upper limit.
- `q5` is an array with a client-enforced maximum of **3** items.
- All single-answer fields (`q1`, `q2`, `q3`, `q6`, `q7`) will always be a non-empty string when submitted; the frontend requires them before allowing the user to advance.

### Success response

HTTP `2xx`. Body content is ignored.
After success the user is navigated to `/profile`.

### Error response

```json
{ "message": "Could not submit survey. Please try again." }
```

---

## Quick C++ parsing sketch (nlohmann/json)

```cpp
#include <nlohmann/json.hpp>
using json = nlohmann::json;

// POST /login
void handle_login(const std::string& body) {
    auto j = json::parse(body);
    std::string email    = j.at("email").get<std::string>();
    std::string password = j.at("password").get<std::string>();
}

// POST /signup
void handle_signup(const std::string& body) {
    auto j = json::parse(body);
    std::string name     = j.at("name").get<std::string>();
    std::string email    = j.at("email").get<std::string>();
    std::string password = j.at("password").get<std::string>();
}

// POST /profile  (save preferences + personal info)
void handle_profile_save(const std::string& body) {
    auto j = json::parse(body);

    // Personal information
    std::string nickname = j.value("nickname", "");
    std::string email    = j.value("email", "");

    // Preferences
    auto focus_areas     = j.at("focusAreas").get<std::vector<std::string>>();
    std::string style    = j.at("explanationStyle").get<std::string>();
    std::string detail   = j.at("detailLevel").get<std::string>();
    std::string risk     = j.at("riskSensitivity").get<std::string>();
    std::string language = j.at("language").get<std::string>();
    std::string lang_other = j.value("languageOther", "");

    std::string user_type    = j.at("userType").get<std::string>();
    std::string legal_exp    = j.at("legalExperience").get<std::string>();
    std::string doc_freq     = j.at("docFrequency").get<std::string>();
    std::string primary_goal = j.at("primaryGoal").get<std::string>();

    auto& flags = j.at("behaviourFlags");
    bool show_risk      = flags.at("showRiskRating").get<bool>();
    bool recommend_law  = flags.at("recommendLawyer").get<bool>();
    bool show_deadlines = flags.at("showDeadlines").get<bool>();
    bool show_tldr      = flags.at("showTldr").get<bool>();
    bool ask_clarify    = flags.at("askClarifying").get<bool>();

    std::string save_history = j.at("saveHistory").get<std::string>();
    std::string email_digest = j.at("emailDigest").get<std::string>();
    std::string analytics    = j.at("analytics").get<std::string>();
}

// POST /account/password
void handle_password_change(const std::string& body) {
    auto j = json::parse(body);
    std::string current_password = j.at("currentPassword").get<std::string>();
    std::string new_password     = j.at("newPassword").get<std::string>();
    // 1. Verify current_password against stored hash for the authenticated user
    // 2. Hash new_password and store it
}

// POST /survey
void handle_survey(const std::string& body) {
    auto j = json::parse(body);

    std::string q1 = j.at("q1").get<std::string>();
    std::string q2 = j.at("q2").get<std::string>();
    std::string q3 = j.at("q3").get<std::string>();
    auto        q4 = j.at("q4").get<std::vector<std::string>>();  // multi
    auto        q5 = j.at("q5").get<std::vector<std::string>>();  // multi, max 3
    std::string q6 = j.at("q6").get<std::string>();
    std::string q7 = j.at("q7").get<std::string>();
}
```

---

## Error response helper

```cpp
// Return this JSON body with an appropriate 4xx/5xx status code.
json error_body(const std::string& msg) {
    return json{ {"message", msg} };
}
```
