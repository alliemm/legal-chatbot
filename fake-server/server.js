const express = require("express");
const app = express();
const PORT = 18080;

// --- Middleware ---
app.use(express.json());

// CORS: allow the Vue dev server and any origin during development
app.use((req, res, next) => {
  const origin = req.headers.origin || "http://localhost:5173";
  res.header("Access-Control-Allow-Origin", origin);
  res.header("Access-Control-Allow-Credentials", "true");
  res.header("Access-Control-Allow-Methods", "GET,POST,DELETE,OPTIONS");
  res.header("Access-Control-Allow-Headers", "Content-Type");
  if (req.method === "OPTIONS") return res.sendStatus(204);
  next();
});

// --- Fake state ---
let loggedInUser = null;

// --- Routes ---

app.get("/", (req, res) => {
  res.send("Fake server is running!");
});

// Signup
app.post("/signup", (req, res) => {
  const { email, password } = req.body || {};
  if (!email || !password) return res.status(400).send("Missing signup information");
  loggedInUser = email;
  res.status(201).send("Created new account");
});

// Login
app.post("/login", (req, res) => {
  const { email, password } = req.body || {};
  if (!email || !password) return res.status(400).send("Missing login information");
  // Accept any credentials
  loggedInUser = email;
  res.status(202).send("User logged in");
});

// Logout
app.get("/logout", (req, res) => {
  if (!loggedInUser) return res.status(400).send("Already logged out");
  loggedInUser = null;
  res.status(200).send("User logged out");
});

// Deactivate
app.delete("/deactivate", (req, res) => {
  if (!loggedInUser) return res.status(401).send("Unauthorized");
  loggedInUser = null;
  res.status(200).send("Account Deleted");
});

// Survey
app.post("/survey", (req, res) => {
  if (!loggedInUser) return res.status(401).send("Unauthorized");
  const required = [
    "userType", "workedWithLawyer", "howOften",
    "typesOfDocuments", "biggestConcerns", "jargonUnderstanding", "outcome",
  ];
  for (const field of required) {
    if (!req.body?.[field]) return res.status(400).send("Missing survey information");
  }
  res.status(200).send("Saved survey results");
});

// Upload (accepts anything, ignores the file)
app.post("/upload", (req, res) => {
  if (!loggedInUser) return res.status(401).send("Unauthorized");
  res.status(202).send("Successfully uploaded file");
});

// Profile
app.get("/profile", (req, res) => {
  if (!loggedInUser) return res.status(401).send("Unauthorized");
  res.json({
    email: loggedInUser,
    documents: ["sample-contract.pdf", "nda-draft.docx", "lease-agreement.pdf"],
    survey: {
      userType: "Individual",
      workedWithLawyer: "No",
      howOften: "A few times a year",
      typesOfDocuments: "Contracts & agreements",
      biggestConcerns: "Hidden clauses or fine print",
      jargonUnderstanding: "Somewhat",
      outcome: "Understand what I'm signing",
    },
  });
});

// Chat — returns a hardcoded AI-style response
app.post("/chat", (req, res) => {
  if (!loggedInUser) return res.status(401).send("Unauthorized");
  const { message, documentNames } = req.body || {};
  if (!message) return res.status(400).send("Missing chat message");

  // Simulate a short delay like a real AI call
  setTimeout(() => {
    const docs = documentNames?.length ? documentNames : [];
    const reply = docs.length
      ? `[Fake response] Based on your documents (${docs.join(", ")}), here is a summary of the key clauses: This agreement contains standard liability limitations, a 30-day termination notice requirement, and a non-compete clause valid for 12 months. Let me know if you'd like me to explain any section in detail.`
      : `[Fake response] That's a great question about "${message.slice(0, 50)}". In general, terms and conditions outline the rights and obligations of both parties. Key things to watch for include liability caps, dispute resolution clauses, and data usage policies. Would you like me to go deeper on any of these?`;

    res.json({
      reply,
      usedDocuments: docs,
    });
  }, 800);
});

// --- Start ---
app.listen(PORT, () => {
  console.log(`Fake server running on http://localhost:${PORT}`);
  console.log("Endpoints: /signup, /login, /logout, /deactivate, /survey, /upload, /profile, /chat");
  console.log("Login with any email/password to start using the API.");
});
