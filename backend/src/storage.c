
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../include/storage.h"
#include "../include/vehicle.h"
#include "../include/parking.h"
#include "../include/queue.h"
#include "../include/linkedlist.h"
#include "../include/stack.h"

extern Stack activityStack;


// Save all parked vehicle records to a file
int saveVehicleRecords(void)
{
    FILE *file;

    // Open file in write mode
    file = fopen("data/vehicles.txt", "w");

    if (file == NULL)
    {
        printf("Error: Could not open vehicle records file!\n");
        return 0;
    }

    Vehicle *temp = vehicleHead;

    // Traverse the linked list
    while (temp != NULL)
    {
        fprintf(file, "%s|%s|%d|%d|%s\n",
                temp->info.vehicleNumber,
                temp->info.ownerName,
                temp->info.vehicleType,
                temp->slotNumber,
                temp->entryTime);

        temp = temp->next;
    }

    // Close the file
    fclose(file);

    printf("Vehicle records saved successfully!\n");

    return 1;
}


// Load previously saved vehicle records
int loadVehicleRecords(void)
{
    FILE *file;
    char line[160];

    char vehicleNumber[20];
    char ownerName[50];
    char entryTime[30];

    int vehicleType;
    int slotNumber;
    int loadedCount = 0;

    file = fopen("data/vehicles.txt", "r");

    // No saved file means there are no previous records
    if (file == NULL)
    {
        return 1;
    }

    while (fgets(line, sizeof(line), file) != NULL)
    {
        int fields = sscanf(line,
                            "%19[^|]|%49[^|]|%d|%d|%29[^\n]",
                            vehicleNumber,
                            ownerName,
                            &vehicleType,
                            &slotNumber,
                            entryTime);

        // Ignore invalid records
        if (fields != 5)
        {
            continue;
        }

        VehicleInfo vehicle;

        strcpy(vehicle.vehicleNumber, vehicleNumber);
        strcpy(vehicle.ownerName, ownerName);
        vehicle.vehicleType = vehicleType;

        // Restore the original parking slot
        if (allocateSlot(vehicle, slotNumber))
        {
            insertVehicle(vehicle, slotNumber);

            // Restore the original entry time
            Vehicle *loadedVehicle =
                searchVehicleNode(vehicleNumber);

            if (loadedVehicle != NULL)
            {
                strcpy(loadedVehicle->entryTime, entryTime);
            }

            loadedCount++;
        }
    }

    fclose(file);

    printf("%d vehicle records loaded successfully!\n", loadedCount);

    return 1;
}


// Save both waiting queues to a file
int saveWaitingQueues(void)
{
    FILE *file = fopen("data/waiting_queues.txt", "w");

    if (file == NULL)
    {
        printf("Error: Could not save waiting queues!\n");
        return 0;
    }

    int i;
    int index;

    // Save two-wheeler waiting queue
    for (i = 0; i < twoWheelerQueue.count; i++)
    {
        index = (twoWheelerQueue.front + i) % MAX_QUEUE;

        VehicleInfo vehicle = twoWheelerQueue.vehicles[index];

        fprintf(file, "%d|%s|%s\n",
                vehicle.vehicleType,
                vehicle.vehicleNumber,
                vehicle.ownerName);
    }

    // Save four-wheeler waiting queue
    for (i = 0; i < fourWheelerQueue.count; i++)
    {
        index = (fourWheelerQueue.front + i) % MAX_QUEUE;

        VehicleInfo vehicle = fourWheelerQueue.vehicles[index];

        fprintf(file, "%d|%s|%s\n",
                vehicle.vehicleType,
                vehicle.vehicleNumber,
                vehicle.ownerName);
    }

    fclose(file);

    printf("Waiting queues saved successfully!\n");

    return 1;
}


// Load previously saved waiting queues
int loadWaitingQueues(void)
{
    FILE *file = fopen("data/waiting_queues.txt", "r");

    if (file == NULL)
    {
        return 1;
    }

    char line[120];
    char vehicleNumber[20];
    char ownerName[50];

    int vehicleType;
    int loadedCount = 0;

    while (fgets(line, sizeof(line), file) != NULL)
    {
        int fields = sscanf(line,
                            "%d|%19[^|]|%49[^\n]",
                            &vehicleType,
                            vehicleNumber,
                            ownerName);

        if (fields != 3)
        {
            continue;
        }

        VehicleInfo vehicle;

        strcpy(vehicle.vehicleNumber, vehicleNumber);
        strcpy(vehicle.ownerName, ownerName);
        vehicle.vehicleType = vehicleType;

        // Restore the vehicle to its correct queue
        if (vehicleType == TWO_WHEELER)
        {
            if (enqueue(&twoWheelerQueue, vehicle))
            {
                loadedCount++;
            }
        }
        else if (vehicleType == FOUR_WHEELER)
        {
            if (enqueue(&fourWheelerQueue, vehicle))
            {
                loadedCount++;
            }
        }
    }

    fclose(file);

    printf("%d waiting vehicles loaded successfully!\n", loadedCount);

    return 1;
}


// Save recent activity records to a file
int saveActivityRecords(void)
{
    FILE *file = fopen("data/activities.txt", "w");

    if (file == NULL)
    {
        printf("Error: Could not save activity records!\n");
        return 0;
    }

    // Save activities from oldest to newest
    for (int i = 0; i <= activityStack.top; i++)
    {
        fprintf(file, "%s\n", activityStack.operations[i]);
    }

    fclose(file);

    printf("Activity records saved successfully!\n");

    return 1;
}


// Load previously saved activity records
int loadActivityRecords(void)
{
    FILE *file = fopen("data/activities.txt", "r");

    if (file == NULL)
    {
        return 1;
    }

    char operation[120];
    int loadedCount = 0;

    // Read activities one by one
    while (fgets(operation, sizeof(operation), file) != NULL)
    {
        // Remove the newline character
        operation[strcspn(operation, "\n")] = '\0';

        if (operation[0] == '\0')
        {
            continue;
        }

        // Restore activity to the stack
        if (pushActivity(operation))
        {
            loadedCount++;
        }
    }

    fclose(file);

    printf("%d activity records loaded successfully!\n", loadedCount);

    return 1;
}
