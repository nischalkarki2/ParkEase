
const API_URL = "http://127.0.0.1:8080";

// Load all parked vehicles from the backend
async function loadParkingSlots() {
    try {
        const response = await fetch(`${API_URL}/api/vehicles`);
        const result = await response.json();

        if (result.status !== "success") {
            throw new Error("Unable to load vehicles");
        }

        const vehicles = result.vehicles;

        // Update parking statistics
        const occupied = vehicles.length;
        const available = 20 - occupied;

        document.getElementById("totalSlots").textContent = 20;
        document.getElementById("occupiedSlots").textContent = occupied;
        document.getElementById("availableSlots").textContent = available;

        displaySlots("twoWheelerSlots", 12, 1, vehicles);
        displaySlots("fourWheelerSlots", 8, 2, vehicles);

    } catch (error) {
        console.error("Parking slots loading error:", error);
    }
}

// Display individual parking slots
function displaySlots(containerId, totalSlots, vehicleType, vehicles) {
    const container = document.getElementById(containerId);
    container.replaceChildren();

    for (let i = 1; i <= totalSlots; i++) {

        const vehicle = vehicles.find(v =>
            Number(v.vehicleType) === vehicleType &&
            Number(v.slotNumber) === i
        );

        const slot = document.createElement("div");
        slot.className = vehicle ? "slot-card occupied" : "slot-card available";

        if (vehicle) {
            slot.textContent = `Slot ${i} - ${vehicle.vehicleNumber}`;
        } else {
            slot.textContent = `Slot ${i} - Available`;
        }

        container.appendChild(slot);
    }
}

// Load slots when the page opens
loadParkingSlots();
