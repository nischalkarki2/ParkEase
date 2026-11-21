
#ifndef STACK_H
#define STACK_H

#define MAX_HISTORY 50

typedef struct
{
    char operations[MAX_HISTORY][120];
    int top;

} Stack;
extern Stack activityStack;

void initializeStack(void);

int isStackEmpty(void);

int isStackFull(void);

int pushActivity(const char operation[]);

int popActivity(char operation[]);

void displayRecentActivity(void);

#endif
