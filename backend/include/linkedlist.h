
#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "vehicle.h"

extern Vehicle *vehicleHead;

Vehicle *createVehicleNode(VehicleInfo info, int slotNumber);

int insertVehicle(VehicleInfo info, int slotNumber);

Vehicle *searchVehicleNode(const char vehicleNumber[]);

int deleteVehicleNode(const char vehicleNumber[]);

void displayVehicleList(void);
void freeVehicleList(void);

#endif
