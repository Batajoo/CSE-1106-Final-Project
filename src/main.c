#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <pigpio.h>
#define PIN 17

int isTemperatureAcceptable(int temperature, int acceptableTemperatureRange[]) {
    return temperature >= acceptableTemperatureRange[0] && temperature <= acceptableTemperatureRange[1];
}


double getAverageTemperature(FILE *file) {
    int temp, sum = 0, count = 0;

    while (fscanf(file, "%d", &temp) == 1) {
        sum += temp;
        count++;
    }

    if (count > 0)
        return (double)sum / count;
    return 0.0;
}


bool saveTemperatureToFile(int temperature) {
    FILE *file = fopen("../data/temperature_log.csv", "a");
    if (file == NULL) {
        return false;
    }
    fprintf(file, "%d\n", temperature);
    fclose(file);
    return true;
}

int main(int argc, char *argv[]){
    int acceptableTemperatureRange[] = {68, 76};
    
    FILE *file = fopen("../data/temperature_log.csv", "r");
    if (file == NULL) {
        printf("Failed to open temperature log file.\n");
        return 1;
    }
    // Read the current temperature from the file and read average
    double averageTemperature = getAverageTemperature(file);
    fclose(file);

    printf("The average temperature is %.2f.\n", averageTemperature);

    int classRoomTemperatureInput;
    printf("Your current acceptable temperature range is %d to %d.\n", acceptableTemperatureRange[0], acceptableTemperatureRange[1]);
    printf("Enter the classroom temperature: ");
    scanf("%d", &classRoomTemperatureInput);

    printf("Do you want to change the acceptable temperature? (Y/N)");
    char changeTemperatureInput;
    scanf(" %c", &changeTemperatureInput);

    while (changeTemperatureInput != 'Y' && changeTemperatureInput != 'y' && changeTemperatureInput != 'N' && changeTemperatureInput != 'n') {
        printf("Invalid input. Please enter Y or N: ");
        scanf(" %c", &changeTemperatureInput);
    }

    if (changeTemperatureInput == 'Y' || changeTemperatureInput == 'y') {
        int newMinimumTemperature;
        int newMaximumTemperature;
        printf("Enter the new minimum temperature: ");
        scanf("%d", &newMinimumTemperature);
        printf("Enter the new maximum temperature: ");
        scanf("%d", &newMaximumTemperature);
        acceptableTemperatureRange[0] = newMinimumTemperature;
        acceptableTemperatureRange[1] = newMaximumTemperature;
    }
    
    saveTemperatureToFile(classRoomTemperatureInput);
    bool isTemperatureAcceptableResult = isTemperatureAcceptable(classRoomTemperatureInput, acceptableTemperatureRange);

    if (isTemperatureAcceptableResult) {
        printf("The temperature is within the acceptable range.\n");
        gpioWrite(PIN, 0); // LED OFF
    } else {
        gpioWrite(PIN, 1); // LED ON
        printf("The temperature is outside the acceptable range.\n");
    }

    file = fopen("../data/temperature_log.csv", "r");
    double newAverageTemperature = getAverageTemperature(file);
    fclose(file);



    printf("The new average temperature is %.2f.\n", newAverageTemperature);
    gpioInitialise();
    gpioSetMode(PIN, PI_OUTPUT);
    gpioWrite(PIN, 1); // LED ON
    getchar();  // waits for Enter key
    gpioWrite(PIN, 0); // LED OFF
    getchar(); // waits for Enter key
    gpioTerminate();
    return 0;
}
