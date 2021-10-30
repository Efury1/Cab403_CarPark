
/*
*CAB403 Car Park Sim
*
*
*
*/

//Calling on all libs
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h> 
#include <time.h>
#include <semaphore.h>
#include <string.h>

//defines to make the project flexible
#define ENTRANCES 5 /*Number of entrances for the carpark*/
#define EXITS 5  /*Number of exits for the carpark*/
#define NUM_OF_LEVEL 5 /*Amount of levels in carpark*/
#define PARKS 20 /*Max amount of cars that can park in each floor*/

#define BOOMGATE_OPEN  10000 /*Microseconds */
#define BOOMGATE_CLOSE 10000 /*Microseconds */

#define CAR_FINDING_PARK 10000 /*Car takes 10ms to find park */
#define MIN_TIME 100 /*Minimum time car will park*/
#define MAX_TIME 10000 /*Max time car will park */


typedef struct {
	char* plate;
	bool valid;
	char boom;
	char sign;
    char signHistory[7];
} entrexitSlice;

typedef struct {
	int entry;
	double temp;
	int cars;
	int capacity;
	entrexitSlice entrance;
	entrexitSlice exit;
} displaySlice;

typedef struct {
    int dollars;
    int cents;
    displaySlice level[6];
} displayMonolith;


/*Prototypes */
typedef struct car {
	  char plate[6];
	  int level;
}car_t;





