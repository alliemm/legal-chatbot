document.addEventListener("DOMContentLoaded", () => {
  const statusArea   = document.getElementById("statusArea");
  const statusText   = document.getElementById("statusText");
  const signalsSection = document.getElementById("signalsSection");
  const signalsList  = document.getElementById("signalsList");

  function setLoading() {
    statusArea.className = "status-area loading";
    statusText.textContent = "Checking page…";
    signalsSection.hidden = true;
  }

  function setDetected(signals = []) {
    statusArea.className = "status-area detected";
    statusText.textContent = "T&C detected on this page";

    if (signals.length > 0) {
      signalsList.innerHTML = "";
      signals.forEach((signal) => {
        const li = document.createElement("li");
        li.textContent = signal;
        signalsList.appendChild(li);
      });
      signalsSection.hidden = false;
    }
  }

  function setSafe() {
    statusArea.className = "status-area safe";
    statusText.textContent = "No T&C found on this page";
    signalsSection.hidden = true;
  }

  function setError(msg) {
    statusArea.className = "status-area loading";
    statusText.textContent = msg || "Could not determine page status.";
    signalsSection.hidden = true;
  }

  setLoading();

  chrome.tabs.query({ active: true, currentWindow: true }, (tabs) => {
    const tab = tabs[0];
    if (!tab) {
      setError("No active tab found.");
      return;
    }

    chrome.runtime.sendMessage(
      { type: "GET_STATUS", tabId: tab.id },
      (response) => {
        if (chrome.runtime.lastError) {
          setError("Extension error: " + chrome.runtime.lastError.message);
          return;
        }

        if (!response) {
          setError("No response from background.");
          return;
        }

        if (response.detected) {
          setDetected(response.matchedSignals || []);
        } else {
          setSafe();
        }
      }
    );
  });
});
