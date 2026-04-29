# LexAssist T&C Detector — Chrome Extension

A Chrome extension that automatically detects Terms & Conditions pages and alerts you with a badge on the extension icon.

## How It Works

When you visit any webpage, the extension scans the page for signals that it's a legal/T&C page:

- Page title (e.g. "Terms of Service - Example")
- URL path (e.g. `/terms`, `/privacy-policy`, `/eula`)
- Headings (H1, H2, H3) containing legal keywords
- Body text mentioning multiple legal terms

Each signal has a point value. If the total score reaches the threshold, the extension flags the page with a red **!** badge. Click the extension icon to see which signals matched.

The extension also uses a **MutationObserver** to catch pages that load content dynamically (single-page apps), so it works even if the T&C text appears after the initial page load.

## Installation (Developer Mode)

1. Open Chrome and go to `chrome://extensions`
2. Toggle **Developer mode** on (top-right corner)
3. Click **Load unpacked**
4. Select the `extension/` folder from this repo
5. The LexAssist icon (teal square) should appear in your toolbar

If you don't see it, click the puzzle piece icon in Chrome's toolbar and pin LexAssist.

## Usage

- Browse normally. When you land on a T&C page, a red **!** badge appears on the extension icon.
- Click the icon to open the popup — it shows whether T&C was detected and which signals matched.
- On non-T&C pages, the popup shows "No T&C found on this page."

## File Overview

```
extension/
  manifest.json   — Chrome extension config (Manifest V3)
  content.js      — Runs on every page: detection logic + MutationObserver
  background.js   — Service worker: manages badge and stores detection state per tab
  popup.html      — Popup UI structure
  popup.css       — Popup styles
  popup.js        — Popup logic: queries background for current tab status
  icons/          — Extension icons (16/48/128px)
```

## Testing

Try these pages to verify detection works:

- https://policies.google.com/terms — should trigger (URL + title + headings)
- https://www.apple.com/legal/internet-services/terms/site.html — should trigger
- https://www.google.com — should NOT trigger

## Reloading After Changes

After editing any extension file:

1. Go to `chrome://extensions`
2. Click the reload icon (circular arrow) on the LexAssist card
3. Refresh the page you're testing on
