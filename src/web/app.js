async function fetchResults() {
  const res = await fetch("/results.json");
  const data = await res.json();
  const resultsDiv = document.getElementById("results");
  resultsDiv.innerHTML = "";

  data.forEach((item) => {
    const div = document.createElement("div");
    div.className =
      "result " + (item.status.toLowerCase() === "pass" ? "pass" : "fail");
    div.textContent = `${item.test}: ${item.status}`;
    resultsDiv.appendChild(div);
  });
}

// Fetch results on page load
fetchResults();
