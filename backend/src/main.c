
#include <string.h>
#include <ctype.h>
#include "../include/vehicle.h"
#include <stdio.h>
#include <stdlib.h>
#include "../include/parking.h"
#include "../include/queue.h"
#include "../include/stack.h"
#include "../include/linkedlist.h"
#include "../include/storage.h"
#include "../include/server.h"

// Display the main menu

// Initialize the complete ParkEase application
void initializeApplication(void)
{
    printf("\nInitializing ParkEase...\n");

    // Initialize parking slots
    initializeParking();

    // Initialize waiting queues
    initializeQueue(&twoWheelerQueue);
    initializeQueue(&fourWheelerQueue);

    // Initialize activity stack
    initializeStack();

    // Restore previously saved records
    loadVehicleRecords();
    loadWaitingQueues();
    loadActivityRecords();

    printf("ParkEase initialized successfully!\n\n");
    fflush(stdout);
}


// Collect vehicle information from the user
int getVehicleDetails(VehicleInfo *vehicle)
{
    int ch;

    // Clear the leftover newline from menu input
    while ((ch = getchar()) != '\n' && ch != EOF)
    {
        // Clear input buffer
    }

    // Get vehicle registration number
    while (1)
    {
        printf("\nEnter vehicle number: ");

        if (fgets(vehicle->vehicleNumber,
                  sizeof(vehicle->vehicleNumber), stdin) == NULL)
        {
            return 0;
        }

        vehicle->vehicleNumber[
            strcspn(vehicle->vehicleNumber, "\n")] = '\0';
            // Convert vehicle number to uppercase
        for (int i = 0; vehicle->vehicleNumber[i] != '\0'; i++)
        {
            vehicle->vehicleNumber[i] =
                (char)toupper((unsigned char)vehicle->vehicleNumber[i]);
        }

        if (isValidVehicleNumber(vehicle->vehicleNumber))
        {
            break;
        }

        printf("Invalid vehicle number! Please try again.\n");
    }

    // Get owner name
    while (1)
    {
        printf("Enter owner name: ");

        if (fgets(vehicle->ownerName,
                sizeof(vehicle->ownerName), stdin) == NULL)
        {
            return 0;
        }

        vehicle->ownerName[
            strcspn(vehicle->ownerName, "\n")] = '\0';

        if (isValidOwnerName(vehicle->ownerName))
        {
            break;
        }

        printf("Invalid owner name! Please try again.\n");
    }

        // Get vehicle type
        while (1)
        {
            char input[30];
            char *endPointer;
            long choice;

            printf("\nSelect vehicle type:\n");
            printf("1. Two-Wheeler\n");
            printf("2. Four-Wheeler\n");
            printf("Enter choice: ");

            if (fgets(input, sizeof(input), stdin) == NULL)
            {
                return 0;
            }

            // Check whether the entire input line was read
            if (strchr(input, '\n') == NULL && !feof(stdin))
            {
                int ch;
                while ((ch = getchar()) != '\n' && ch != EOF)
                {
                }

                printf("Input too long! Please try again.\n");
                continue;
            }

            choice = strtol(input, &endPointer, 10);

            // Skip any spaces after the number
            while (isspace((unsigned char)*endPointer))
            {
                endPointer++;
            }

            if (endPointer == input || *endPointer != '\0')
            {
                printf("Invalid input! Please enter 1 or 2.\n");
                continue;
            }

            vehicle->vehicleType = (int)choice;

            if (isValidVehicleType(vehicle->vehicleType))
            {
                break;
            }

            printf("Invalid vehicle type! Please select 1 or 2.\n");
        }

        return 1;

}


int vehicleAlreadyExists(const char vehicleNumber[])
{
    // Check vehicles currently parked
    if (searchVehicleNode(vehicleNumber) != NULL)
    {
        return 1;
    }

    // Check the two-wheeler waiting queue
    for (int i = 0; i < twoWheelerQueue.count; i++)
    {
        int index = (twoWheelerQueue.front + i) % MAX_QUEUE;

        if (strcmp(twoWheelerQueue.vehicles[index].vehicleNumber,
                   vehicleNumber) == 0)
        {
            return 1;
        }
    }

    // Check the four-wheeler waiting queue
    for (int i = 0; i < fourWheelerQueue.count; i++)
    {
        int index = (fourWheelerQueue.front + i) % MAX_QUEUE;

        if (strcmp(fourWheelerQueue.vehicles[index].vehicleNumber,
                   vehicleNumber) == 0)
        {
            return 1;
        }
    }

    return 0;
}


// Register a vehicle and assign parking or waiting queue
void registerVehicle(void)
{
    VehicleInfo vehicle;
    int slotNumber;

    // Collect vehicle information
    if (!getVehicleDetails(&vehicle))
    {
        printf("\nUnable to read vehicle information.\n");
        return;
    }

    if (vehicleAlreadyExists(vehicle.vehicleNumber))
    {
        printf("\nVehicle already exists in ParkEase!\n");
        printf("Registration unsuccessful.\n");
        return;
    }

    // Find the first available compatible parking slot
    slotNumber = findAvailableSlot(vehicle.vehicleType);

    
    if (slotNumber != -1)
    {
        // Add vehicle to the linked list first
        if (insertVehicle(vehicle, slotNumber))
        {
            // Allocate the parking slot
            if (allocateSlot(vehicle, slotNumber))
            {
                printf("\nVehicle parked successfully!\n");
                printf("Assigned Parking Slot: %d\n", slotNumber);

                // Record the activity
                char activity[120];

                snprintf(activity, sizeof(activity),
                        "Vehicle %s parked in slot %d",
                        vehicle.vehicleNumber, slotNumber);

                pushActivity(activity);

                // Save updated records
                saveVehicleRecords();
                saveActivityRecords();
            }
            else
            {
                // Remove the linked-list node if slot allocation fails
                deleteVehicleNode(vehicle.vehicleNumber);

                printf("\nUnable to allocate the parking slot.\n");
            }
        }
        else
        {
            printf("\nUnable to register vehicle due to memory allocation failure.\n");
        }
    }
    else
    {
        // No available slot: add vehicle to the correct waiting queue
        Queue *waitingQueue;

        if (vehicle.vehicleType == TWO_WHEELER)
        {
            waitingQueue = &twoWheelerQueue;
        }
        else
        {
            waitingQueue = &fourWheelerQueue;
        }

        if (enqueue(waitingQueue, vehicle))
        {
            printf("\nParking is full for this vehicle category.\n");
            printf("Vehicle added to the waiting queue.\n");

            char activity[120];

            snprintf(activity, sizeof(activity),
                     "Vehicle %s added to waiting queue",
                     vehicle.vehicleNumber);

            pushActivity(activity);

            // Save updated records
            saveWaitingQueues();
            saveActivityRecords();
        }
        else
        {
            printf("\nWaiting queue is also full!\n");
            printf("Vehicle registration unsuccessful.\n");
        }
    }
}

void removeParkedVehicle(void)
{
    char vehicleNumber[20];

    printf("\nEnter vehicle number to remove: ");

    if (scanf("%19s", vehicleNumber) != 1)
    {
        printf("\nInvalid vehicle number input!\n");
        return;
    }

    // Convert vehicle number to uppercase
    for (int i = 0; vehicleNumber[i] != '\0'; i++)
    {
        vehicleNumber[i] =
            (char)toupper((unsigned char)vehicleNumber[i]);
    }

    Vehicle *vehicle = searchVehicleNode(vehicleNumber);

    if (vehicle == NULL)
    {
        printf("\nVehicle not found in parking records!\n");
        return;
    }

    int vehicleType = vehicle->info.vehicleType;
    int slotNumber = vehicle->slotNumber;

    if (releaseSlot(vehicleType, slotNumber))
    {
        deleteVehicleNode(vehicleNumber);

        printf("\nVehicle removed successfully!\n");
        printf("Freed Parking Slot: %d\n", slotNumber);

        char activity[120];

        snprintf(activity, sizeof(activity),
                "Vehicle %s removed from slot %d",
                vehicleNumber, slotNumber);

        pushActivity(activity);

        // Select the waiting queue for this vehicle category
        Queue *waitingQueue;

        if (vehicleType == TWO_WHEELER)
        {
            waitingQueue = &twoWheelerQueue;
        }
        else
        {
            waitingQueue = &fourWheelerQueue;
        }
        
        // Check whether another vehicle is waiting
        if (!isQueueEmpty(waitingQueue))
        {
            VehicleInfo waitingVehicle;

            if (dequeue(waitingQueue, &waitingVehicle))
            {
                // Insert the waiting vehicle into the linked list first
                if (insertVehicle(waitingVehicle, slotNumber))
                {
                    // Allocate the freed parking slot
                    if (allocateSlot(waitingVehicle, slotNumber))
                    {
                        printf("\nWaiting vehicle automatically parked!\n");
                        printf("Vehicle Number: %s\n",
                               waitingVehicle.vehicleNumber);
                        printf("Assigned Parking Slot: %d\n",
                               slotNumber);

                        snprintf(activity, sizeof(activity),
                                 "Vehicle %s promoted from waiting queue to slot %d",
                                 waitingVehicle.vehicleNumber, slotNumber);

                        pushActivity(activity);
                    }
                    else
                    {
                        // Remove the linked-list node if allocation fails
                        deleteVehicleNode(waitingVehicle.vehicleNumber);

                        // Return the vehicle to the waiting queue
                        enqueueFront(waitingQueue, waitingVehicle);

                        printf("\nUnable to allocate slot to waiting vehicle.\n");
                    }
                }
                else
                {
                    // Return the vehicle to the waiting queue
                    enqueueFront(waitingQueue, waitingVehicle);

                    printf("\nMemory allocation failed!\n");
                    printf("Vehicle returned to the waiting queue.\n");
                }
            }
        }

    // Save all updated records
    saveVehicleRecords();
    saveWaitingQueues();
    saveActivityRecords();
    }
}

void displayMenu(void)
{
printf("\n");
printf("========================================\n");
printf("              PARK EASE                 \n");
printf("   SMART COLLEGE PARKING MANAGEMENT     \n");
printf("========================================\n");

printf("\n----------- MAIN MENU -----------------\n");

printf("1. Register Vehicle & Park\n");
printf("2. Remove Parked Vehicle\n");
printf("3. View Parking Status\n");
printf("4. View Parked Vehicles\n");
printf("5. View Waiting Queues\n");
printf("6. View Recent Activity\n");
printf("7. Exit\n");

printf("----------------------------------------\n");
printf("Enter your choice: ");
}

int registerVehicleAPI(
    const char *vehicleNumber,
    const char *ownerName,
    int vehicleType,
    int *assignedSlot,
    int *isWaiting
)
{
    VehicleInfo vehicle;

    // Initialize output values
    *assignedSlot = -1;
    *isWaiting = 0;

    // Validate vehicle number
    if (!isValidVehicleNumber(vehicleNumber))
    {
        return 0;
    }

    // Validate owner name
    if (!isValidOwnerName(ownerName))
    {
        return 0;
    }

    // Validate vehicle type
    if (!isValidVehicleType(vehicleType))
    {
        return 0;
    }

    // Copy vehicle information
    snprintf(
        vehicle.vehicleNumber,
        sizeof(vehicle.vehicleNumber),
        "%s",
        vehicleNumber
    );

    snprintf(
        vehicle.ownerName,
        sizeof(vehicle.ownerName),
        "%s",
        ownerName
    );

    vehicle.vehicleType = vehicleType;

    // Check for duplicate registration
    if (vehicleAlreadyExists(vehicle.vehicleNumber))
    {
        return 0;
    }

    // Find available parking slot
    int slotNumber = findAvailableSlot(vehicleType);

    if (slotNumber != -1)
    {
        // Insert vehicle into linked list
        if (!insertVehicle(vehicle, slotNumber))
        {
            return 0;
        }

        // Allocate parking slot
        if (!allocateSlot(vehicle, slotNumber))
        {
            deleteVehicleNode(vehicle.vehicleNumber);
            return 0;
        }

        *assignedSlot = slotNumber;
    }
    else
    {
        // Add vehicle to the appropriate waiting queue
        Queue *waitingQueue;

        if (vehicleType == TWO_WHEELER)
        {
            waitingQueue = &twoWheelerQueue;
        }
        else
        {
            waitingQueue = &fourWheelerQueue;
        }

        if (!enqueue(waitingQueue, vehicle))
        {
            return 0;
        }

        *isWaiting = 1;
    }

    // Record activity
    char activity[120];

    if (*isWaiting)
    {
        snprintf(
            activity,
            sizeof(activity),
            "Vehicle %s added to waiting queue",
            vehicle.vehicleNumber
        );
    }
    else
    {
        snprintf(
            activity,
            sizeof(activity),
            "Vehicle %s parked in slot %d",
            vehicle.vehicleNumber,
            *assignedSlot
        );
    }

    pushActivity(activity);

    // Persist updated records
    saveVehicleRecords();
    saveWaitingQueues();
    saveActivityRecords();

    return 1;
}


// ==================================
// REMOVE PARKED VEHICLE API
// ==================================

int removeParkedVehicleAPI(
    const char *vehicleNumber,
    int *freedSlot,
    int *wasPromoted
)
{
    *freedSlot = -1;
    *wasPromoted = 0;

    // Find the parked vehicle
    Vehicle *vehicle = searchVehicleNode(vehicleNumber);

    if (vehicle == NULL)
    {
        return 0;
    }

    int vehicleType = vehicle->info.vehicleType;
    int slotNumber = vehicle->slotNumber;

    // Release the parking slot
    if (!releaseSlot(vehicleType, slotNumber))
    {
        return 0;
    }

    // Remove vehicle from linked list
    if (!deleteVehicleNode(vehicleNumber))
    {
        return 0;
    }

    *freedSlot = slotNumber;

    // Record removal activity
    char activity[120];

    snprintf(
        activity,
        sizeof(activity),
        "Vehicle %s removed from slot %d",
        vehicleNumber,
        slotNumber
    );

    pushActivity(activity);

    // Select the appropriate waiting queue
    Queue *waitingQueue;

    if (vehicleType == TWO_WHEELER)
    {
        waitingQueue = &twoWheelerQueue;
    }
    else
    {
        waitingQueue = &fourWheelerQueue;
    }

    // Promote the first waiting vehicle
    if (!isQueueEmpty(waitingQueue))
    {
        VehicleInfo waitingVehicle;

        if (dequeue(waitingQueue, &waitingVehicle))
        {
            if (insertVehicle(waitingVehicle, slotNumber))
            {
                if (allocateSlot(waitingVehicle, slotNumber))
                {
                    *wasPromoted = 1;

                    snprintf(
                        activity,
                        sizeof(activity),
                        "Vehicle %s promoted from waiting queue to slot %d",
                        waitingVehicle.vehicleNumber,
                        slotNumber
                    );

                    pushActivity(activity);
                }
                else
                {
                    deleteVehicleNode(waitingVehicle.vehicleNumber);
                    enqueueFront(waitingQueue, waitingVehicle);
                }
            }
            else
            {
                enqueueFront(waitingQueue, waitingVehicle);
            }
        }
    }

    // Save updated records
    saveVehicleRecords();
    saveWaitingQueues();
    saveActivityRecords();

    return 1;
}


// Main function

int main(int argc, char *argv[])
{
    if (argc > 1 && strcmp(argv[1], "--server") == 0)
    {
        initializeApplication();
        startServer();
        freeVehicleList();
        return 0;
    }

int choice;
initializeApplication();

while (1)
{
    displayMenu();

    if (scanf("%d", &choice) != 1)
    {
        printf("\nInvalid input! Please enter a number.\n");

        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF)
        {
            // Clear invalid input
        }

        continue;
    }

    switch (choice)
    {
        case 1:
            registerVehicle();
            break;

        case 2:
            removeParkedVehicle();
            break;

        case 3:
            displayParkingStatus();
            break;

        case 4:
            displayVehicleList();
            break;

        case 5:
            printf("\n========== WAITING QUEUES ==========\n");

            printf("\n--- TWO-WHEELER WAITING QUEUE ---\n");
            displayQueue(&twoWheelerQueue);

            printf("\n--- FOUR-WHEELER WAITING QUEUE ---\n");
            displayQueue(&fourWheelerQueue);

            printf("====================================\n");
            break;

        case 6:
            displayRecentActivity();
            break;

        case 7:
            printf("\nSaving ParkEase data...\n");

            saveVehicleRecords();
            saveWaitingQueues();
            saveActivityRecords();

            freeVehicleList();

            printf("\nThank you for using ParkEase!\n");
            return 0;

        default:
            printf("\nInvalid choice! Please select 1-7.\n");
    }
}

return 0;
}
