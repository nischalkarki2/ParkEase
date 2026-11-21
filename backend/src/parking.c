
#include <stdio.h>
#include <string.h>
#include "../include/parking.h"

// Parking slot arrays
ParkingSlot twoWheelerSlots[TWO_WHEELER_SLOTS];
ParkingSlot fourWheelerSlots[FOUR_WHEELER_SLOTS];

// Initialize all parking slots
void initializeParking(void)
{
    int i;

    // Initialize two-wheeler slots
    for (i = 0; i < TWO_WHEELER_SLOTS; i++)
    {
        twoWheelerSlots[i].slotNumber = i + 1;
        twoWheelerSlots[i].isOccupied = 0;
        strcpy(twoWheelerSlots[i].vehicleNumber, "");
    }

    // Initialize four-wheeler slots
    for (i = 0; i < FOUR_WHEELER_SLOTS; i++)
    {
        fourWheelerSlots[i].slotNumber = i + 1;
        fourWheelerSlots[i].isOccupied = 0;
        strcpy(fourWheelerSlots[i].vehicleNumber, "");
    }
}

// Find the first available parking slot
int findAvailableSlot(int vehicleType)
{
    int i;

    if (vehicleType == TWO_WHEELER)
    {
        for (i = 0; i < TWO_WHEELER_SLOTS; i++)
        {
            if (twoWheelerSlots[i].isOccupied == 0)
            {
                return twoWheelerSlots[i].slotNumber;
            }
        }
    }
    else if (vehicleType == FOUR_WHEELER)
    {
        for (i = 0; i < FOUR_WHEELER_SLOTS; i++)
        {
            if (fourWheelerSlots[i].isOccupied == 0)
            {
                return fourWheelerSlots[i].slotNumber;
            }
        }
    }
    
    // No available slot
    return -1;
}


// Allocate a parking slot to a vehicle
int allocateSlot(VehicleInfo vehicle, int slotNumber)
{
    int index;

    if (vehicle.vehicleType == TWO_WHEELER)
    {
        index = slotNumber - 1;

        if (index < 0 || index >= TWO_WHEELER_SLOTS)
            return 0;

        if (twoWheelerSlots[index].isOccupied == 1)
            return 0;

        twoWheelerSlots[index].isOccupied = 1;

        strcpy(twoWheelerSlots[index].vehicleNumber,
            vehicle.vehicleNumber);
            
        return 1;
    }

    else if (vehicle.vehicleType == FOUR_WHEELER)
    {
        index = slotNumber - 1;

        if (index < 0 || index >= FOUR_WHEELER_SLOTS)
            return 0;

        if (fourWheelerSlots[index].isOccupied == 1)
            return 0;

        fourWheelerSlots[index].isOccupied = 1;

        strcpy(fourWheelerSlots[index].vehicleNumber,
            vehicle.vehicleNumber);

        return 1;
    }

    return 0;
}


 // Release a parking slot
int releaseSlot(int vehicleType, int slotNumber)
{
    int index = slotNumber - 1;

    if (vehicleType == TWO_WHEELER)
    {
        if (index < 0 || index >= TWO_WHEELER_SLOTS)
            return 0;

        if (twoWheelerSlots[index].isOccupied == 0)
            return 0;

        twoWheelerSlots[index].isOccupied = 0;

        strcpy(twoWheelerSlots[index].vehicleNumber, "");

        return 1;
    }

    else if (vehicleType == FOUR_WHEELER)
    {
        if (index < 0 || index >= FOUR_WHEELER_SLOTS)
            return 0;

        if (fourWheelerSlots[index].isOccupied == 0)
            return 0;

        fourWheelerSlots[index].isOccupied = 0;

        strcpy(fourWheelerSlots[index].vehicleNumber, "");

        return 1;
    }

    return 0;
}



// Display the current parking status
void displayParkingStatus(void)
{
    int i;


    printf("\n========== PARKEASE PARKING STATUS ==========\n");

    printf("\n--- TWO-WHEELER PARKING ---\n");

    for (i = 0; i < TWO_WHEELER_SLOTS; i++)
    {
        printf("Slot %2d : ", twoWheelerSlots[i].slotNumber);

        if (twoWheelerSlots[i].isOccupied)
        {
            printf("Occupied - %s\n",
                   twoWheelerSlots[i].vehicleNumber);
        }
        else
        {
            printf("Available\n");
        }
    }

    printf("\n--- FOUR-WHEELER PARKING ---\n");

    for (i = 0; i < FOUR_WHEELER_SLOTS; i++)
    {
        printf("Slot %2d : ", fourWheelerSlots[i].slotNumber);

        if (fourWheelerSlots[i].isOccupied)
        {
            printf("Occupied - %s\n",
                   fourWheelerSlots[i].vehicleNumber);
        }
        else
        {
            printf("Available\n");
        }
    }

    printf("\n=============================================\n");
}

// Get occupied parking slot statistics
void getParkingStatistics(
    int *twoWheelerOccupied,
    int *fourWheelerOccupied
)
{
    int i;

    *twoWheelerOccupied = 0;
    *fourWheelerOccupied = 0;

    // Count occupied two-wheeler slots
    for (i = 0; i < TWO_WHEELER_SLOTS; i++)
    {
        if (twoWheelerSlots[i].isOccupied)
        {
            (*twoWheelerOccupied)++;
        }
    }

    // Count occupied four-wheeler slots
    for (i = 0; i < FOUR_WHEELER_SLOTS; i++)
    {
        if (fourWheelerSlots[i].isOccupied)
        {
            (*fourWheelerOccupied)++;
        }
    }
}