
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

//#define FILE "plates.txt"

static const char white_text[] = "\033[37m"; // White Text
static const char black_text[] = "\033[30m";  // Black Text
static const char white_background[] = "\033[47m"; // White Background
static const char black_background[] = "\033[40m";  // Black Background
static const char reset_colour[] = "\033[0m";  // Reset

const char *spinnerStrings[] = {"⠀⡰","⢀⡠","⢄⡀", "⢆⠀", "⠎⠀", "⠊⠁", "⠈⠑", "⠀⠱"};

void polarity(int isPositive) {
    if(isPositive) {
        printf("%s%s", white_background, black_text);
    } else {
        printf("%s%s", black_background, white_text);
    }
}

void manageTest(double ns1) {
	long ns;
	struct timespec spec;
	clock_gettime(CLOCK_MONOTONIC, &spec);
	ns = spec.tv_nsec;
	
	



	clock_gettime(CLOCK_MONOTONIC, &spec);
	
	ns = abs(floor((spec.tv_nsec - ns) / 1.0e6));
	if (ns > 500) {
		ns = 1000-ns;
	}
	printf(" internal time: %3ld milliseconds\n", ns);
	printf(" external time: %3f milliseconds\n", ns1);
	
	nanosleep((const struct timespec[]){{0, 20000000L}}, NULL);
}

void spinner(void) {
    system("clear");
    polarity(1);
    printf("%-07s%-97s\n", spinnerStrings[0], "Loading...");
    return;
}

typedef struct {
	char* plate;
	bool valid;
	bool boom;
	char sign;
    char signHistory[5];
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

char* boolFormat(bool value) {
	char *returnString;
	if(value) {
		returnString = "⣀⠤⠒⠉ ";
	} else {
		returnString = "⣀⣀⣀⣀ ";
	}
	return returnString;
};

char* boomFormat(bool value) {
	char *returnString;
	if(value) {
		returnString = "Yes";
	} else {
		returnString = "No ";
	}
	return returnString;
};

char *headerDraw(void) {
	return("\n  level data                  entrances                          exits\n"
	" ┌╼#╾┬─╼Temp╾─┬─╼Parking╾─┐  ┌╼#╾┬─╼Plate╾─┬─╼Boom╾─┬─╼Valid╾─┐ ┌╼#╾┬─╼Plate╾─┬─╼Boom╾─┬─╼Valid╾─┐\n");
}

char *headerLowerDraw(void) {
	return("\n  revenue                     entrance signs                     exit signs\n"
	" ┌╼$╾────────────┐           ┌╼#╾┬───┬─╼History╾─┐              ┌╼#╾┬───┬─╼History╾─┐\n");
}

/*Prototypes */
typedef struct car {
	  char plate[6];
	  int level;
}car_t;


/*typedef struct level {
	thread_mutex_t mutexLPR; /*LPR*/
	//pthread_cond_t conditionLPR;
	//char readPlate[6]; /*LPR reads 6 characte plate */
/*} level_t;

typedef struct LPREntrance{
	pthread_mutex_t mutexLPR;
	pthread_cont_t conditionLPR;
	char readingPlate[6]; /*Reads plate*/
	//pthread_mutex_t mutexBoomGate; 
	//pthread_cont_t conditionBoomGate;
	//char boomGateStatus; 
	//pthread_mutex_t informationSign; /*Information  */
	//pthread_cont_t informationSign;
//} 

/*LPREntrance_t;
typedef struct LPRExit {
	pthread_mutex_t LPRExiting;
	pthread_cond_t LPRExiting;
	pthread_mutex_t boomgateExit;
	pthread_cond_t  boomgateExit;
	char exitBoomGateStatus;
} LPRExit_t;*/

/*Prototypes*/
void createCarList(int n);
void displayCarList();
car_t* generateCar();
char* generatePlate();
char* getCarPlate();
int getPark();
int countCars();


