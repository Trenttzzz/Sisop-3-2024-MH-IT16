#include <stdio.h>
#include <string.h>

// Function to handle Gap condition
const char* Gap(float distance) {
    if (distance < 3.5)
        return "Gogogo";
    else if (distance >= 3.5 && distance < 10)
        return "Push";
    else
        return "Stay out of trouble";
}

// Function to handle Fuel condition
const char* Fuel(int fuelPercentage) {
    if (fuelPercentage > 80)
        return "Push Push Push";
    else if (fuelPercentage >= 50 && fuelPercentage <= 80)
        return "You can go";
    else
        return "Conserve Fuel";
}

// Function to handle Tire condition
const char* Tire(int tireUsage) {
    if (tireUsage > 80)
        return "Go Push Go Push";
    else if (tireUsage >= 50 && tireUsage <= 80)
        return "Good Tire Wear";
    else if (tireUsage >= 30 && tireUsage < 50)
        return "Conserve Your Tire";
    else
        return "Box Box Box";
}

// Function to handle Tire Change condition
const char* TireChange(const char* tireType) {
    if (strcmp(tireType, "Soft") == 0)
        return "Mediums Ready";
    else if (strcmp(tireType, "Medium") == 0)
        return "Box for Softs";
    else
        return "Invalid tire type";
}
