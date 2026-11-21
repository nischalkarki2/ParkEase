
#include <stdio.h>
#include <string.h>

#include "../include/stack.h"

// Create the activity stack
Stack activityStack;

// Initialize the stack
void initializeStack(void)
{
    activityStack.top = -1;
}


// Check whether the stack is empty
int isStackEmpty(void)
{
    return activityStack.top == -1;
}

// Check whether the stack is full
int isStackFull(void)
{
    return activityStack.top == MAX_HISTORY - 1;
}


 // Add a new activity to the stack
int pushActivity(const char operation[])
{
    if (isStackFull())
    {
        printf("Activity stack is full!\n");
        return 0;
    }

    // Move top to the next position
    activityStack.top++;

    // Store the activity message safely
    strncpy(activityStack.operations[activityStack.top],
            operation,
            sizeof(activityStack.operations[0]) - 1);

    activityStack.operations[activityStack.top]
        [sizeof(activityStack.operations[0]) - 1] = '\0';

    return 1;
}


 // Remove the most recent activity from the stack
int popActivity(char operation[])
{
    if (isStackEmpty())
    {
        printf("Activity stack is empty!\n");
        return 0;
    }

    // Copy the top activity
    strcpy(operation, activityStack.operations[activityStack.top]);

    // Remove the top activity
    activityStack.top--;

    return 1;
}


 // Display recent activities
void displayRecentActivity(void)
{
    int i;

    if (isStackEmpty())
    {
        printf("\nNo recent activities found.\n");
        return;
    }

    printf("\n========== RECENT ACTIVITIES ==========\n");

    // Display activities from newest to oldest
    for (i = activityStack.top; i >= 0; i--)
    {
        printf("%d. %s\n",
               activityStack.top - i + 1,
               activityStack.operations[i]);
    }

    printf("=======================================\n");
}
