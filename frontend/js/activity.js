
const API_URL = "http://127.0.0.1:8080";

// Load activity history from the C backend
async function loadActivities() {
    const tableBody = document.getElementById("activityTableBody");

    try {
        const response = await fetch(`${API_URL}/api/activities`);
        const result = await response.json();

        if (result.status !== "success") {
            throw new Error("Unable to load activities");
        }

        tableBody.replaceChildren();

        if (result.activities.length === 0) {
            const row = document.createElement("tr");
            const cell = document.createElement("td");

            cell.colSpan = 2;
            cell.textContent = "No activities recorded yet.";

            row.appendChild(cell);
            tableBody.appendChild(row);
            return;
        }

        result.activities.forEach((activity, index) => {
            const row = document.createElement("tr");

            const numberCell = document.createElement("td");
            numberCell.textContent = index + 1;

            const activityCell = document.createElement("td");
            activityCell.textContent = activity;

            row.appendChild(numberCell);
            row.appendChild(activityCell);

            tableBody.appendChild(row);
        });

    } catch (error) {
        console.error("Activity loading error:", error);
        tableBody.textContent = "Unable to load activity history.";
    }
}

// Load activities when the page opens
loadActivities();

// Refresh activities when the button is clicked
document.getElementById("refreshActivities").addEventListener("click", async function() {
    const button = this;

    button.textContent = "Refreshing...";

    await loadActivities();

    button.textContent = "Refreshed!";

    setTimeout(() => {
        button.textContent = "Refresh Activities";
    }, 1500);
});


// Filter activity records
document.getElementById("activityFilter").addEventListener("change", function() {
    const filterValue = this.value;
    const rows = document.querySelectorAll("#activityTableBody tr");

    rows.forEach(row => {
        const activityText = row.textContent.toLowerCase();

        if (filterValue === "all") {
            row.style.display = "";
        } else if (filterValue === "registration") {
            row.style.display =
                activityText.includes("register") ? "" : "none";
        } else if (filterValue === "removal") {
            row.style.display =
                activityText.includes("remov") ? "" : "none";
        } else if (filterValue === "promotion") {
            row.style.display =
                activityText.includes("promot") ? "" : "none";
        }
    });
});
