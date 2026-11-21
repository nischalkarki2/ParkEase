
#ifndef PARKING_H
#define PARKING_H

#include "vehicle.h"

#define TWO_WHEELER_SLOTS 12
#define FOUR_WHEELER_SLOTS 8

typedef struct
{
    int slotNumber;
    int isOccupied;
    char vehicleNumber[20];

} ParkingSlot;

extern ParkingSlot twoWheelerSlots[TWO_WHEELER_SLOTS];

extern ParkingSlot fourWheelerSlots[FOUR_WHEELER_SLOTS];

void initializeParking(void);

int findAvailableSlot(int vehicleType);

void displayParkingStatus(void);


void getParkingStatistics(
    int *twoWheelerOccupied,
    int *fourWheelerOccupied
);

int allocateSlot(VehicleInfo vehicle, int slotNumber);

int releaseSlot(int vehicleType, int slotNumber);

#endif
