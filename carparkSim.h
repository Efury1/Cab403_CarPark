#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_OF_LEVEL 5
#define PARKS 20
#define BOOMGATE_OPEN  10000
#define BOOMGATE_CLOSE 10000 /*Microseconds */
#define CAR_FINDING_PARK 10000 /*Car takes 10ms to find park */

#define MIN_TIME 100 /*Minimum time car will park */
#define MAX_TIME 10000 /*Max time car will park */
#define FILE "plates.txt"


/*Prototypes */
typedef struct car {
	  char plate[6];
	  int level;
}car_t;


typedef struct level {
	thread_mutex_t mutexLPR; /*LPR*/
	pthread_cond_t conditionLPR;
	char readPlate[6]; /*LPR reads 6 characte plate */
} level_t;

typedef struct LPREntrance{
	pthread_mutex_t mutexLPR;
	pthread_cont_t conditionLPR;
	char readingPlate[6]; /*Reads plate*/
	pthread_mutex_t mutexBoomGate; 
	pthread_cont_t conditionBoomGate;
	char boomGateStatus; 
	pthread_mutex_t informationSign; /*Information  */
	pthread_cont_t informationSign;
} 

LPREntrance_t;
typedef struct LPRExit {
	pthread_mutex_t LPRExiting;
	pthread_cond_t LPRExiting;
	pthread_mutex_t boomgateExit;
	pthread_cond_t  boomgateExit;
	char exitBoomGateStatus;
} LPRExit_t;

/*Prototypes*/
void createCarList(int n);
void displayCarList();
car_t* generateCar();
char* generatePlate();
char* getCarPlate();
int getPark();
int countCars();


