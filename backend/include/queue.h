
#ifndef QUEUE_H
#define QUEUE_H

#include "vehicle.h"

#define MAX_QUEUE 10

typedef struct
{
    VehicleInfo vehicles[MAX_QUEUE];

    int front;
    int rear;
    int count;

} Queue;
extern Queue twoWheelerQueue;
extern Queue fourWheelerQueue;

void initializeQueue(Queue *q);

int isQueueEmpty(Queue *q);

int isQueueFull(Queue *q);

int enqueue(Queue *q, VehicleInfo vehicle);

int enqueueFront(Queue *q, VehicleInfo vehicle);

int dequeue(Queue *q, VehicleInfo *vehicle);

void displayQueue(Queue *q);

#endif
