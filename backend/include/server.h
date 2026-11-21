
#ifndef SERVER_H
#define SERVER_H

#define SERVER_PORT 8080
#define BUFFER_SIZE 8192

void startServer(void);
int registerVehicleAPI(
    const char *vehicleNumber,
    const char *ownerName,
    int vehicleType,
    int *assignedSlot,
    int *isWaiting
);

int removeParkedVehicleAPI(
    const char *vehicleNumber,
    int *freedSlot,
    int *wasPromoted
);

#endif
