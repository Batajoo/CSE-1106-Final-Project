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
    // initializing the GPIO and port pin number
    gpioInitialise();
    gpioSetMode(PIN, PI_OUTPUT);

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
    bool reRunProgram = true;
    do {
        int classRoomTemperatureInput;
        printf("Your current acceptable temperature range is %d to %d.\n", acceptableTemperatureRange[0], acceptableTemperatureRange[1]);
        printf("Enter the classroom temperature: ");
        scanf("%d", &classRoomTemperatureInput);

        printf("Do you want to change the acceptable temperature? (Y/N)");
        char changeTemperatureInput;
        scanf(" %c", &changeTemperatureInput);

        printf("\n");

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

        printf("\n");
        
        saveTemperatureToFile(classRoomTemperatureInput);
        bool isTemperatureAcceptableResult = isTemperatureAcceptable(classRoomTemperatureInput, acceptableTemperatureRange);

        printf("------------------------------------------------\n");
        if (isTemperatureAcceptableResult) {
            printf("Normal Temperature.\n");
            gpioWrite(PIN, 0); // LED OFF
            getchar();
        } else {
            gpioWrite(PIN, 1); // LED ON
            printf("Warning Temperature!\n");
            getchar();
            
        }
        printf("------------------------------------------------\n");

        file = fopen("../data/temperature_log.csv", "r");
        
        getchar(); // waits for Enter key to end the program
        gpioWrite(PIN, 0); // LED OFF

        printf("Enter Y or y if you want to rerun the program or anything else to exit: ");
        int reRunProgramInput = getchar();
        reRunProgram = reRunProgramInput == 'Y' || reRunProgramInput == 'y';
    } while (reRunProgram);

    double newAverageTemperature = getAverageTemperature(file);
    fclose(file);


    printf("The new average temperature is %.2f.\n", newAverageTemperature);
    printf("Press enter to end the program.\n");

        gpioTerminate();
    return 0;
}
