
// Include required libraries
#include <string.h>
#include <ctype.h>

#include "../include/vehicle.h"

// Check whether vehicle type is valid
int isValidVehicleType(int vehicleType)
{
    if (vehicleType == TWO_WHEELER ||
        vehicleType == FOUR_WHEELER)
    {
        return 1;
    }

    return 0;
}


// Validate Indian vehicle registration numbers
int isValidVehicleNumber(const char vehicleNumber[])
{
    if (vehicleNumber == NULL)
    {
        return 0;
    }

    int length = strlen(vehicleNumber);

    // Indian registration numbers must be 9 to 10 characters
    if (length < 9 || length > 10)
    {
        return 0;
    }

    // First two characters: State code
    if (!isalpha((unsigned char)vehicleNumber[0]) ||
        !isalpha((unsigned char)vehicleNumber[1]))
    {
        return 0;
    }

    // Find the first digit after the state code
    int i = 2;

    // RTO code: one or two digits
    int rtoDigits = 0;

    while (i < length && isdigit((unsigned char)vehicleNumber[i]))
    {
        rtoDigits++;
        i++;
    }

    if (rtoDigits < 1 || rtoDigits > 2)
    {
        return 0;
    }

    // Registration series: one or two letters
    // Registration series: one to three letters
    int seriesLetters = 0;

    while (i < length && isalpha((unsigned char)vehicleNumber[i]))
    {
        seriesLetters++;
        i++;
    }

    if (seriesLetters < 1 || seriesLetters > 3)
    {
        return 0;
    }

    // Remaining characters: one to four digits
    int numberDigits = 0;

    while (i < length && isdigit((unsigned char)vehicleNumber[i]))
    {
        numberDigits++;
        i++;
    }

    if (numberDigits < 1 || numberDigits > 4)
    {
        return 0;
    }

    // No extra characters allowed
    return (i == length);
}

// Check whether owner name is provided

int isValidOwnerName(const char ownerName[])
{
    if (ownerName == NULL || ownerName[0] == '\0')
    {
        return 0;
    }

    int hasLetter = 0;

    for (int i = 0; ownerName[i] != '\0'; i++)
    {
        unsigned char ch = (unsigned char)ownerName[i];

        if (isalpha(ch))
        {
            hasLetter = 1;
        }
        else if (isspace(ch) || ch == '-' || ch == '\'' || ch == '.')
        {
            continue;
        }
        else
        {
            return 0;
        }
    }

    return hasLetter;
}
