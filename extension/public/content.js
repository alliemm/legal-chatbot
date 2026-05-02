// content.js — Terms & Conditions detection content script
// Runs in the context of every page the user visits.

// ---------------------------------------------------------------------------
// Keywords and URL slugs used for heuristic scoring
// ---------------------------------------------------------------------------

const TC_KEYWORDS = [
  "terms and conditions",
  "terms of service",
  "terms of use",
  "privacy policy",
  "user agreement",
  "end user license agreement",
  "eula",
  "legal notice",
  "cookie policy",
  "acceptable use policy",
  "data processing agreement",
  "service level agreement",
];

const TC_URL_SLUGS = [
  "/terms",
  "/tos",
  "/privacy-policy",
  "/privacy",
  "/eula",
  "/legal",
  "/cookie-policy",
  "/user-agreement",
  "/acceptable-use",
];

// ---------------------------------------------------------------------------
// detectTermsAndConditions()
// Scores the current page across several signals and returns a result object.
// ---------------------------------------------------------------------------

function detectTermsAndConditions() {
  let score = 0;
  const matchedSignals = [];

  const titleLower = document.title.toLowerCase();
  const urlPath = location.pathname.toLowerCase();

  // Signal: page title contains a keyword (+3 points)
  for (const keyword of TC_KEYWORDS) {
    if (titleLower.includes(keyword)) {
      score += 3;
      matchedSignals.push(`title:"${keyword}"`);
      break; // only award once regardless of how many keywords match
    }
  }

  // Signal: URL path contains a known slug (+3 points)
  for (const slug of TC_URL_SLUGS) {
    if (urlPath.includes(slug)) {
      score += 3;
      matchedSignals.push(`url:"${slug}"`);
      break; // only award once
    }
  }

  // Signal: any <h1> contains a keyword (+2 points, first match only)
  const h1Elements = document.querySelectorAll("h1");
  outerH1: for (const h1 of h1Elements) {
    const h1Text = h1.innerText.toLowerCase();
    for (const keyword of TC_KEYWORDS) {
      if (h1Text.includes(keyword)) {
        score += 2;
        matchedSignals.push(`h1:"${keyword}"`);
        break outerH1; // stop after the first matching h1
      }
    }
  }

  // Signal: <h2> or <h3> elements contain keywords (+1 each, max +2 points)
  let subheadingPoints = 0;
  const subheadings = document.querySelectorAll("h2, h3");
  for (const el of subheadings) {
    if (subheadingPoints >= 2) break;
    const elText = el.innerText.toLowerCase();
    for (const keyword of TC_KEYWORDS) {
      if (elText.includes(keyword)) {
        subheadingPoints += 1;
        matchedSignals.push(`${el.tagName.toLowerCase()}:"${keyword}"`);
        break; // one point per element, move to next element
      }
    }
  }
  score += subheadingPoints;

  // Signal: body text (first 5000 chars) contains 2+ distinct keywords (+1 point)
  const bodyText = (document.body.innerText || "").slice(0, 5000).toLowerCase();
  const bodyMatches = TC_KEYWORDS.filter((kw) => bodyText.includes(kw));
  if (bodyMatches.length >= 2) {
    score += 1;
    matchedSignals.push(`body:${bodyMatches.length} keywords`);
  }

  // Threshold: score >= 3 is considered a T&C page
  return {
    detected: score >= 3,
    score,
    matchedSignals,
  };
}

// ---------------------------------------------------------------------------
// sendResult()
// Sends the detection result to the background service worker.
// ---------------------------------------------------------------------------

function sendResult(result) {
  chrome.runtime.sendMessage({
    type: "TC_DETECTION_RESULT",
    detected: result.detected,
    score: result.score,
    url: location.href,
    matchedSignals: result.matchedSignals,
  });
}

// ---------------------------------------------------------------------------
// MutationObserver setup
// Watches for DOM changes (useful for SPAs that load content dynamically).
// Debounced by 1 second; only re-runs detection when the visible text has
// changed by more than 500 characters since the last check.
// ---------------------------------------------------------------------------

let debounceTimer = null;
let lastBodyTextLength = document.body.innerText.length;
let observerActive = true;

const observer = new MutationObserver(() => {
  // Debounce: reset the timer on every mutation batch
  clearTimeout(debounceTimer);
  debounceTimer = setTimeout(() => {
    const currentLength = document.body.innerText.length;

    // Only re-run if the text content changed significantly
    if (Math.abs(currentLength - lastBodyTextLength) > 500) {
      lastBodyTextLength = currentLength;

      const result = detectTermsAndConditions();
      sendResult(result);

      // Once detected, stop observing — no need to keep watching
      if (result.detected) {
        observer.disconnect();
        observerActive = false;
      }
    }
  }, 1000);
});

// ---------------------------------------------------------------------------
// Initialisation
// 1. Run detection immediately on script load.
// 2. Start the MutationObserver for late-loading SPA content.
// ---------------------------------------------------------------------------

// Initial detection run
const initialResult = detectTermsAndConditions();
sendResult(initialResult);

// Only set up the observer if the page wasn't already detected as T&C,
// and if document.body is available (it always should be in a content script).
if (!initialResult.detected && document.body) {
  observer.observe(document.body, { childList: true, subtree: true });
}
