// Function to load the JSON file
function loadJSON(callback) {
  fetch("/score/scoreboard")
    .then((response) => response.json())
    .then((data) => callback(data))
    .catch((error) => console.error("Error:", error));
}

// Function to display the leaderboard
function displayLeaderboard(data) {
  // Display the leaderboard
  const leaderboardContainer = document.getElementById("leaderboard");
  let leaderboardHTML =
    "<table><thead><tr><th>Ranking</th><th>Username</th><th>Score</th></tr></thead><tbody>";

  for (let i = 0; i < data.ranking.length; i++) {
    leaderboardHTML += `
      <tr>
        <td>${i + 1}</td>
        <td>${data.ranking[i].username}</td>
        <td>${data.ranking[i].score}</td>
      </tr>`;
  }

  leaderboardHTML += "</tbody></table>";
  leaderboardContainer.innerHTML = leaderboardHTML;
}

// Load the JSON file and display the leaderboard
loadJSON(displayLeaderboard);
