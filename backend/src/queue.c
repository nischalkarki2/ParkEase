
#include <stdio.h>
#include "../include/queue.h"
// Global waiting queues
Queue twoWheelerQueue;
Queue fourWheelerQueue;

// Initialize the circular queue
void initializeQueue(Queue *q)
{
    q->front = 0;
    q->rear = -1;
    q->count = 0;
}



 // Check whether the queue is empty
int isQueueEmpty(Queue *q)
{
    return q->count == 0;
}

// Check whether the queue is full
int isQueueFull(Queue *q)
{
    return q->count == MAX_QUEUE;
}


 // Add a vehicle to the circular queue
int enqueue(Queue *q, VehicleInfo vehicle)
{
    if (isQueueFull(q))
    {
        printf("Waiting queue is full!\n");
        return 0;
    }

    // Move rear circularly
    q->rear = (q->rear + 1) % MAX_QUEUE;

    // Store vehicle information
    q->vehicles[q->rear] = vehicle;

    // Increase vehicle count
    q->count++;

    printf("Vehicle added to waiting queue successfully!\n");

    return 1;
}


int enqueueFront(Queue *q, VehicleInfo vehicle)
{
    // Check whether the queue is full
    if (isQueueFull(q))
    {
        return 0;
    }

    // Move front backward circularly
    q->front = (q->front - 1 + MAX_QUEUE) % MAX_QUEUE;

    // Insert vehicle at the front
    q->vehicles[q->front] = vehicle;

    // Increase queue count
    q->count++;

    return 1;
}



 // Remove the first vehicle from the queue
int dequeue(Queue *q, VehicleInfo *vehicle)
{
    if (isQueueEmpty(q))
    {
        printf("Waiting queue is empty!\n");
        return 0;
    }

    // Copy the first vehicle's information
    *vehicle = q->vehicles[q->front];

    // Move front circularly
    q->front = (q->front + 1) % MAX_QUEUE;

    // Decrease vehicle count
    q->count--;

    printf("Vehicle removed from waiting queue successfully!\n");

    return 1;
}


 // Display all vehicles in the waiting queue
void displayQueue(Queue *q)
{
    int i;
    int index;

    if (isQueueEmpty(q))
    {
        printf("\nWaiting queue is empty!\n");
        return;
    }

    printf("\n========== WAITING VEHICLES ==========\n");

    for (i = 0; i < q->count; i++)
    {
        index = (q->front + i) % MAX_QUEUE;

        printf("\nVehicle %d\n", i + 1);
        printf("Vehicle Number : %s\n",
               q->vehicles[index].vehicleNumber);
        printf("Owner Name     : %s\n",
               q->vehicles[index].ownerName);

        if (q->vehicles[index].vehicleType == TWO_WHEELER)
            printf("Vehicle Type   : Two Wheeler\n");
        else
            printf("Vehicle Type   : Four Wheeler\n");
    }

    printf("\n======================================\n");
}
