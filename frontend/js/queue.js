
const API_URL = "http://127.0.0.1:8080";

// Load waiting queues from the C backend
async function loadWaitingQueues() {
    try {
        const response = await fetch(`${API_URL}/api/queues`);
        const result = await response.json();

        if (result.status !== "success") {
            throw new Error("Unable to load waiting queues");
        }

        const twoWheeler = result.queues.twoWheeler.vehicles;
        const fourWheeler = result.queues.fourWheeler.vehicles;

        // Update queue statistics
        document.getElementById("twoWheelerWaiting").textContent =
            twoWheeler.length;

        document.getElementById("fourWheelerWaiting").textContent =
            fourWheeler.length;

        document.getElementById("totalWaiting").textContent =
            twoWheeler.length + fourWheeler.length;

        // Display both queues
        displayQueue("twoWheelerQueueBody", twoWheeler, "Two-Wheeler");
        displayQueue("fourWheelerQueueBody", fourWheeler, "Four-Wheeler");

    } catch (error) {
        console.error("Waiting queue loading error:", error);
    }
}

// Display vehicles in FIFO order
function displayQueue(tableBodyId, vehicles, vehicleType) {
    const tableBody = document.getElementById(tableBodyId);
    tableBody.replaceChildren();

    if (vehicles.length === 0) {
        const row = document.createElement("tr");
        const cell = document.createElement("td");

        cell.colSpan = 4;
        cell.textContent = "No vehicles currently waiting.";

        row.appendChild(cell);
        tableBody.appendChild(row);
        return;
    }

    vehicles.forEach((vehicle, index) => {
        const row = document.createElement("tr");

        const values = [
            index + 1,
            vehicle.vehicleNumber,
            vehicle.ownerName,
            vehicleType
        ];

        values.forEach(value => {
            const cell = document.createElement("td");
            cell.textContent = value;
            row.appendChild(cell);
        });

        tableBody.appendChild(row);
    });
}

// Load queues when the page opens
loadWaitingQueues();
