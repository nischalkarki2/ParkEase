
// ==================================
// PARKEASE DASHBOARD
// ==================================

const API_URL = "http://127.0.0.1:8080";

// ==================================
// LOAD PARKING STATISTICS
// ==================================

async function loadParkingStatus() {

    try {

        const response = await fetch(`${API_URL}/api/status`);
        const result = await response.json();

        if (result.status === "success") {

            const twoWheeler = result.parking.twoWheeler;
            const fourWheeler = result.parking.fourWheeler;

            document.getElementById("totalSlots").textContent =
                twoWheeler.totalSlots + fourWheeler.totalSlots;

            document.getElementById("occupiedSlots").textContent =
                twoWheeler.occupiedSlots + fourWheeler.occupiedSlots;

            document.getElementById("availableSlots").textContent =
                twoWheeler.availableSlots + fourWheeler.availableSlots;
        }

    } catch (error) {

        console.error("Parking status error:", error);

    }
}

// ==================================
// LOAD AND DISPLAY PARKING SLOTS
// ==================================

async function loadParkingSlots() {

    try {

        const response = await fetch(`${API_URL}/api/vehicles`);
        const result = await response.json();

        if (result.status !== "success") {
            throw new Error("Unable to load vehicles");
        }

        const vehicles = result.vehicles;

        // Generate slots for each vehicle category
        displaySlots("twoWheelerSlots", 12, 1, vehicles);
        displaySlots("fourWheelerSlots", 8, 2, vehicles);

    } catch (error) {

        console.error("Parking slots error:", error);

    }
}

// ==================================
// DISPLAY INDIVIDUAL PARKING SLOTS
// ==================================

function displaySlots(containerId, totalSlots, vehicleType, vehicles) {

    const container = document.getElementById(containerId);

    container.replaceChildren();

    for (let i = 1; i <= totalSlots; i++) {

        const vehicle = vehicles.find(v =>
            v.vehicleType === vehicleType && v.slotNumber === i
        );

        const slot = document.createElement("div");

        if (vehicle) {

            slot.className = "parking-slot occupied";

            slot.textContent = `Slot ${i} - ${vehicle.vehicleNumber}`;

            slot.title = `Owner: ${vehicle.ownerName}`;

        } else {

            slot.className = "parking-slot available";

            slot.textContent = `Slot ${i} - Available`;

        }

        container.appendChild(slot);
    }
}

// ==================================
// INITIALIZE DASHBOARD
// ==================================

loadParkingStatus();
loadParkingSlots();



// ==================================
// LOAD WAITING QUEUES
// ==================================

async function loadWaitingQueue() {

    try {

        const response = await fetch(`${API_URL}/api/queues`);
        const result = await response.json();

        if (result.status !== "success") {
            throw new Error("Unable to load waiting queues");
        }

        const twoWheeler = result.queues.twoWheeler;
        const fourWheeler = result.queues.fourWheeler;

        const totalWaiting =
            twoWheeler.count + fourWheeler.count;

        document.getElementById("waitingVehicles").textContent =
            totalWaiting;

        const queueList = document.getElementById("queueList");

        queueList.replaceChildren();

        // Update Waiting Queue Table
const tableBody =
    document.getElementById("waitingQueueTableBody");

tableBody.replaceChildren();

function addQueueToTable(vehicles, vehicleType) {

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

addQueueToTable(twoWheeler.vehicles, "Two-Wheeler");

addQueueToTable(fourWheeler.vehicles, "Four-Wheeler");

if (totalWaiting === 0) {

    const row = document.createElement("tr");
    const cell = document.createElement("td");

    cell.colSpan = 4;
    cell.textContent = "No vehicles currently waiting.";

    row.appendChild(cell);
    tableBody.appendChild(row);

}

        if (totalWaiting === 0) {

            queueList.textContent = "No vehicles waitiloadActivitiesng!";

            return;
        }


        function displayQueue(title, vehicles) {

            const heading = document.createElement("h3");
            heading.textContent = title;
            heading.style.margin = "15px 0 10px";

            queueList.appendChild(heading);

            if (vehicles.length === 0) {

                const empty = document.createElement("p");
                empty.textContent = "No vehicles in this queue.";

                queueList.appendChild(empty);

                return;
            }

            vehicles.forEach((vehicle, index) => {

                const item = document.createElement("div");

                item.style.padding = "12px";
                item.style.marginBottom = "10px";
                item.style.background = "#1e293b";
                item.style.borderRadius = "8px";

                const vehicleNumber = document.createElement("strong");
                vehicleNumber.textContent =
                    `${index + 1}. ${vehicle.vehicleNumber}`;

                const details = document.createElement("p");
                details.textContent =
                    `${vehicle.ownerName} | Waiting`;

                details.style.color = "#94a3b8";
                details.style.fontSize = "13px";
                details.style.marginTop = "5px";

                item.appendChild(vehicleNumber);
                item.appendChild(details);

                queueList.appendChild(item);
            });
        }

        displayQueue("Two-Wheeler Queue", twoWheeler.vehicles);

        displayQueue("Four-Wheeler Queue", fourWheeler.vehicles);

    } catch (error) {

        console.error("Waiting queue error:", error);

    }
}

// Load waiting queue
loadWaitingQueue();

// Load recent activities
loadActivities();
loadParkedVehiclesTable();


// ==================================
// LOAD RECENT ACTIVITIES
// ==================================

async function loadActivities() {

    try {

        const response = await fetch(`${API_URL}/api/activities`);
        const result = await response.json();

        if (result.status !== "success") {
            throw new Error("Unable to load activities");
        }

        const activityList = document.getElementById("activityList");

        activityList.replaceChildren();

        if (result.activities.length === 0) {

            activityList.textContent = "No recent activities!";

            return;
        }

        result.activities.forEach((activity, index) => {

            const item = document.createElement("div");

            item.style.padding = "12px";
            item.style.marginBottom = "10px";
            item.style.background = "#1e293b";
            item.style.borderRadius = "8px";
            item.style.fontSize = "13px";

            item.textContent = `${index + 1}. ${activity}`;

            activityList.appendChild(item);

        });

    } catch (error) {

        console.error("Activities error:", error);

    }
}



// ==================================
// REGISTER VEHICLE
// ==================================

document.getElementById("registrationForm").addEventListener("submit", async function(event) {

    event.preventDefault();

    const vehicleNumber =
        document.getElementById("vehicleNumber").value.trim().toUpperCase();

    const ownerName =
        document.getElementById("ownerName").value.trim();

    const vehicleType =
        Number(document.getElementById("vehicleType").value);

    const message =
        document.getElementById("registrationMessage");

    try {

        const response = await fetch(`${API_URL}/api/vehicles`, {

            method: "POST",

            headers: {
                "Content-Type": "application/json"
            },

            body: JSON.stringify({
                vehicleNumber: vehicleNumber,
                ownerName: ownerName,
                vehicleType: vehicleType
            })

        });

        const result = await response.json();

        if (result.status === "success") {

            message.textContent = result.message;
            message.style.color = "#4ade80";

            document.getElementById("registrationForm").reset();

            loadParkingStatus();
            loadParkingSlots();
            loadWaitingQueue();
            loadActivities();
            loadParkedVehiclesTable();

        } else {

            message.textContent = result.message;
            message.style.color = "#fb7185";

        }

    } catch (error) {

        console.error("Registration error:", error);

        message.textContent = "Unable to connect to the backend!";
        message.style.color = "#fb7185";

    }

});



// ==================================
// REMOVE PARKED VEHICLE
// ==================================

document.getElementById("removeVehicleForm").addEventListener("submit", async function(event) {

    event.preventDefault();

    const vehicleNumber =
        document.getElementById("removeVehicleNumber").value.trim().toUpperCase();

    const message =
        document.getElementById("removalMessage");

    try {

        const response = await fetch(`${API_URL}/api/vehicles`, {

            method: "DELETE",

            headers: {
                "Content-Type": "application/json"
            },

            body: JSON.stringify({
                vehicleNumber: vehicleNumber
            })

        });

        const result = await response.json();

        if (result.status === "success") {

            if (result.wasPromoted) {
                message.textContent =
                    "Vehicle removed successfully! A waiting vehicle has been promoted.";
            } else {
                message.textContent = result.message;
            }

            message.style.color = "#4ade80";

            document.getElementById("removeVehicleForm").reset();

            loadParkingStatus();
            loadParkingSlots();
            loadWaitingQueue();
            loadActivities();

        } else {

            message.textContent = result.message;
            message.style.color = "#fb7185";

        }

    } catch (error) {

        console.error("Vehicle removal error:", error);

        message.textContent = "Unable to connect to the backend!";
        message.style.color = "#fb7185";

    }

});



// ==================================
// LOAD PARKED VEHICLES TABLE
// ==================================

let lastParkedVehiclesData = "";

async function loadParkedVehiclesTable() {

    const tableBody =
        document.getElementById("parkedVehiclesTableBody");

    try {

        const response = await fetch(`${API_URL}/api/vehicles`);

        const result = await response.json();

        if (result.status !== "success") {
            tableBody.textContent = "Unable to load vehicles.";
            return;
        }

        // Check whether vehicle data has changed
        const currentVehiclesData = JSON.stringify(result.vehicles);

        if (currentVehiclesData === lastParkedVehiclesData) {
            return;
        }

        lastParkedVehiclesData = currentVehiclesData;

        tableBody.innerHTML = "";

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

            // Create Remove button
            const actionCell = document.createElement("td");
            const removeButton = document.createElement("button");

            removeButton.textContent = "Remove";
            removeButton.type = "button";
            removeButton.className = "remove-table-btn";

            removeButton.addEventListener("click", function() {

    const confirmed = confirm(
        `Are you sure you want to remove vehicle ${vehicle.vehicleNumber}?`
    );

    if (!confirmed) {
        return;
    }

    document.getElementById("removeVehicleNumber").value =
        vehicle.vehicleNumber;

    document.getElementById("removeVehicleForm").requestSubmit();

});

            actionCell.appendChild(removeButton);
            row.appendChild(actionCell);

            tableBody.appendChild(row);

        });

    } catch (error) {

        console.error("Vehicle table error:", error);

        tableBody.textContent = "Unable to connect to the backend.";

    }

}

// Load table when dashboard opens
loadParkedVehiclesTable();

document.getElementById("vehicleSearch").addEventListener("input", function() {
    const searchText = this.value.toLowerCase();

    const rows = document.querySelectorAll("#parkedVehiclesTableBody tr");

    rows.forEach(row => {
        const rowText = row.textContent.toLowerCase();

        row.style.display = rowText.includes(searchText) ? "" : "none";
    });
});



// Automatically refresh dashboard data every 10 seconds
setInterval(() => {
    loadParkingStatus();
    loadParkingSlots();
    loadWaitingQueue();
    loadActivities();
    loadParkedVehiclesTable();
}, 10000);
