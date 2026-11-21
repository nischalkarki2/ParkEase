
#ifndef VEHICLE_H
#define VEHICLE_H

#define TWO_WHEELER 1
#define FOUR_WHEELER 2

typedef struct
{
    char vehicleNumber[20];
    char ownerName[50];
    int vehicleType;

} VehicleInfo;

// Vehicle validation functions
int isValidVehicleType(int vehicleType);

int isValidVehicleNumber(const char vehicleNumber[]);

int isValidOwnerName(const char ownerName[]);

// Vehicle linked-list node
typedef struct VehicleNode Vehicle;

struct VehicleNode
{
    VehicleInfo info;

    int slotNumber;
    char entryTime[30];

    Vehicle *next;

};

#endif
