# Integration Guide for Sania

Hey Sania, here's how the T&C detection code works and where your tasks plug in.

## What's done

The `extension/` folder has a working Chrome extension that:

- Detects T&C pages using keyword scoring (`content.js`)
- Shows a red "!" badge when detected (`background.js`)
- Has a popup showing detection status (`popup.html/js/css`)
- Uses a MutationObserver to catch dynamically loaded content

You can load it in Chrome right now — see `extension/README.md` for steps.

## Where your work connects

### 1. Clause extractor (your Apr 18-23 task)

Right now `content.js` only *detects* whether a page is T&C. It doesn't extract the actual text. Your clause extractor should:

- Pull the legal text off the page (probably `document.body.innerText` or a more targeted selector)
- Send it to the backend for analysis

The natural place to add this is in `content.js`, after detection succeeds. Look for this block around line 165-172:

```js
const initialResult = detectTermsAndConditions();
sendResult(initialResult);
```

After a positive detection, you could extract the page text and send it to the backend. You'd add a new message type like:

```js
if (initialResult.detected) {
  chrome.runtime.sendMessage({
    type: "TC_TEXT_EXTRACTED",
    text: document.body.innerText,
    url: location.href
  });
}
```

Then handle that in `background.js` — forward it to the backend server.

### 2. Highlights on the page (your Apr 18-23 task)

Once the backend returns which clauses are risky, you'll need to highlight them on the page. This also lives in `content.js` since it's the content script with DOM access.

The flow would be:
1. `content.js` sends extracted text to background
2. `background.js` forwards to backend, gets back risky clauses
3. `background.js` sends results back to `content.js`
4. `content.js` highlights the matching text on the page

To receive messages back in `content.js`, add a listener:

```js
chrome.runtime.onMessage.addListener((message) => {
  if (message.type === "HIGHLIGHT_CLAUSES") {
    // message.clauses = [{ text: "...", risk: "high" }, ...]
    // Find and highlight these in the DOM
  }
});
```

### 3. T&C page detector improvements

If you find the detection is missing pages or giving false positives, the scoring table is at the top of `content.js`:

- `TC_KEYWORDS` — the keyword list (add more if needed)
- `TC_URL_SLUGS` — URL patterns to match
- The scoring weights are in `detectTermsAndConditions()`

Feel free to tune these. The threshold is `score >= 3`.

## Message types (so we stay consistent)

| Type | From → To | Purpose |
|------|-----------|---------|
| `TC_DETECTION_RESULT` | content → background | Report whether T&C was detected |
| `GET_STATUS` | popup → background | Popup asks for current tab state |
| `TC_TEXT_EXTRACTED` | content → background | (you'd add this) Send page text for analysis |
| `HIGHLIGHT_CLAUSES` | background → content | (you'd add this) Send back clauses to highlight |

## How to work on it

1. Clone the repo, load `extension/` as unpacked in Chrome
2. Make your changes
3. Reload the extension at `chrome://extensions` (click the reload arrow)
4. Refresh the test page
5. Open DevTools → Console to see any errors (content script logs show in the page's console, background logs show in the service worker console — click "Inspect views: service worker" on the extensions page)

Let me know if anything's unclear.
