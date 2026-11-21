
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../include/linkedlist.h"

// Head pointer of the linked list
Vehicle *vehicleHead = NULL;

// Create a new vehicle node
Vehicle *createVehicleNode(VehicleInfo info, int slotNumber)
{
    // Allocate memory for a new node
    Vehicle *newNode = (Vehicle *)malloc(sizeof(Vehicle));

    if (newNode == NULL)
    {
        printf("Memory allocation failed!\n");
        return NULL;
    }

    // Store vehicle information
    newNode->info = info;

    // Store allocated parking slot
    newNode->slotNumber = slotNumber;

    // Record vehicle entry time
    time_t currentTime = time(NULL);
    struct tm *localTime = localtime(&currentTime);

    if (localTime != NULL)
    {
        strftime(newNode->entryTime,
                 sizeof(newNode->entryTime),
                 "%Y-%m-%d %H:%M:%S",
                 localTime);
    }
    else
    {
        strcpy(newNode->entryTime, "Unknown");
    }

    // Initialize next pointer
    newNode->next = NULL;

    return newNode;
}


// Insert a vehicle into the linked list

int insertVehicle(VehicleInfo info, int slotNumber)
{
    // Create a new vehicle node
    Vehicle *newNode = createVehicleNode(info, slotNumber);

    // Check memory allocation
    if (newNode == NULL)
    {
        return 0;
    }

    // If the linked list is empty
    if (vehicleHead == NULL)
    {
        vehicleHead = newNode;
        printf("Vehicle added to linked list successfully!\n");
        return 1;
    }

    // Traverse to the last node
    Vehicle *temp = vehicleHead;

    while (temp->next != NULL)
    {
        temp = temp->next;
    }

    // Connect the last node to the new node
    temp->next = newNode;

    printf("Vehicle added to linked list successfully!\n");

    return 1;
}


// Search for a vehicle using its registration number
Vehicle *searchVehicleNode(const char vehicleNumber[])
{
    Vehicle *temp = vehicleHead;

    while (temp != NULL)
    {
        if (strcmp(temp->info.vehicleNumber, vehicleNumber) == 0)
        {
            return temp;
        }

        temp = temp->next;
    }

    // Vehicle not found
    return NULL;
}


// Delete a vehicle using its registration number
int deleteVehicleNode(const char vehicleNumber[])
{
    Vehicle *current = vehicleHead;
    Vehicle *previous = NULL;

    // Search for the vehicle
    while (current != NULL &&
           strcmp(current->info.vehicleNumber, vehicleNumber) != 0)
    {
        previous = current;
        current = current->next;
    }

    // Vehicle not found
    if (current == NULL)
    {
        return 0;
    }

    // If the first node is being deleted
    if (previous == NULL)
    {
        vehicleHead = current->next;
    }
    else
    {
        previous->next = current->next;
    }

    // Free allocated memory
    free(current);

    return 1;
}


// Display all currently parked vehicles
void displayVehicleList(void)
{
    Vehicle *temp = vehicleHead;
    int count = 1;

    if (vehicleHead == NULL)
    {
        printf("\nNo vehicles are currently parked.\n");
        return;
    }

    printf("\n========== PARKED VEHICLES ==========\n");

    while (temp != NULL)
    {
        printf("\nVehicle %d\n", count);

        printf("Vehicle Number : %s\n",
               temp->info.vehicleNumber);

        printf("Owner Name     : %s\n",
               temp->info.ownerName);

        if (temp->info.vehicleType == TWO_WHEELER)
        {
            printf("Vehicle Type   : Two Wheeler\n");
        }
        else
        {
            printf("Vehicle Type   : Four Wheeler\n");
        }

        printf("Parking Slot   : %d\n", temp->slotNumber);

        printf("Entry Time     : %s\n", temp->entryTime);

        temp = temp->next;
        count++;
    }

    printf("\nTotal Parked Vehicles: %d\n", count - 1);

    printf("=====================================\n");
}
void freeVehicleList(void)
{
    Vehicle *current = vehicleHead;

    while (current != NULL)
    {
        Vehicle *nextNode = current->next;

        free(current);

        current = nextNode;
    }

    vehicleHead = NULL;
}