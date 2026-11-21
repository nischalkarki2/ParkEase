const API_URL = "http://127.0.0.1:8080";

// Load parked vehicles
async function loadVehicles() {

    const tableBody =
        document.getElementById("parkedVehiclesTableBody");

    try {

        const response = await fetch(`${API_URL}/api/vehicles`);
        const result = await response.json();

        if (result.status !== "success") {
            throw new Error("Unable to load vehicles");
        }

        tableBody.replaceChildren();

        if (result.vehicles.length === 0) {

            const row = document.createElement("tr");
            const cell = document.createElement("td");

            cell.colSpan = 6;
            cell.textContent = "No vehicles currently parked.";

            row.appendChild(cell);
            tableBody.appendChild(row);

            return;
        }

        result.vehicles.forEach(vehicle => {

            const row = document.createElement("tr");

            const values = [
                vehicle.vehicleNumber,
                vehicle.ownerName,
                Number(vehicle.vehicleType) === 1
                    ? "Two-Wheeler"
                    : "Four-Wheeler",
                vehicle.slotNumber,
                vehicle.entryTime
            ];

            values.forEach(value => {

                const cell = document.createElement("td");
                cell.textContent = value;

                row.appendChild(cell);

            });

            const actionCell = document.createElement("td");

            const removeButton = document.createElement("button");

            removeButton.textContent = "Remove";
            removeButton.type = "button";
            removeButton.className = "remove-table-btn";

            removeButton.addEventListener("click", async function() {

    const confirmed = confirm(
        `Are you sure you want to remove vehicle ${vehicle.vehicleNumber}?`
    );

    if (!confirmed) {
        return;
    }

    try {

        const response = await fetch(`${API_URL}/api/vehicles`, {
            method: "DELETE",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify({
                vehicleNumber: vehicle.vehicleNumber
            })
        });

        const result = await response.json();

        if (result.status === "success") {

            alert(result.message);

            loadVehicles();

        } else {

            alert(result.message);

        }

    } catch (error) {

        console.error("Vehicle removal error:", error);

        alert("Unable to connect to the backend!");

    }

});

            actionCell.appendChild(removeButton);
            row.appendChild(actionCell);
            tableBody.appendChild(row);

        });

    } catch (error) {

        console.error("Vehicle loading error:", error);

        tableBody.textContent =
            "Unable to connect to the backend.";

    }
}

// Load vehicles when page opens
loadVehicles();

// Search parked vehicles
document.getElementById("vehicleSearch").addEventListener("input", function() {

    const searchText = this.value.toLowerCase();

    const rows = document.querySelectorAll("#parkedVehiclesTableBody tr");

    rows.forEach(row => {

        const rowText = row.textContent.toLowerCase();

        row.style.display =
            rowText.includes(searchText) ? "" : "none";

    });

});