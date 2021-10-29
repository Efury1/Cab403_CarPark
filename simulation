#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

/* ----------------------
* Car Park simulator 
*
*---------------------------*/



void createCarList(int n);
int countCars();
void displayCarList();

car_t* generatecar() {
    car_t* car = malloc(sizeof(car_t));
    car->plateNumber = getCarPlate();
    return car;
}

/*Load txt into memory*/
char* generatePlate() {
    char *carptr = malloc(sizeof(char) * 26);
    
    return carptr;
}


/*Get car plate from text file */ 
char* getCarPlate() {
    char *carptr = malloc(sizeof(char) * 6); /*Car had a six letter plate*/
    
    return carptr;
}

/*This is for LPR */
int getPark() {
   
    while (available < 20) {
        
    }
   
}


/*Time difference for bill*/
double getTimeDiff(time_t then) {
    
}

// Current state of parking, waits for 
// semaphore to be released before printing.
void printCar() {
    sem_wait(&mutex);
    printf("\nCar State:\n");
    for (int i=0; i<parking; ++i) {
        if (parking[i] == NULL) {
            printf("%d: No cars to exit", i);
        } else {
            printf("%d: %s (has parked for %.2f seconds)\n", i,
                    parking[i]->plateNumber, calculateCarTime
                    (parking[i]->timeExited));
        }
    }
    printf("\n");
    sem_post(&mutex);
    return;
}

void printEnterance() {
    printf("\nWelcome to the Cab403 Car Simulator.\n");
    printf("_______________________________________________\n");
    printf(" n\n");
    printf("_______________________________________________\n");
    getchar();
    return;
}

// will free all memory that was allocared using malloc
void freeMemory() {
    for (int i=0; i<parking; ++i) {
        if (parking[i] != NULL) {
            free(parking[i]->plateNumber);
            free(parking[i]);
        }
    }
    free(parking);
    return;
}

// will free a single car's allocared memory
void freecar(car_t* carptr) {
    free(carptr->plateNumber);
    free(carptr);
    return;
}




/*
 * Counts cars for level
 * Whenever a car exits of levels this function is called
 * Put in LPR
 */
int countCars()
{
    
}



