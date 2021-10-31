//calling in the .h file 
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
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>

//calling in the .h file 
/*#include "car_park.h"*/
#include "simulator.h"
//defines to make the project flexible
#define LEVELS 5
#define ENTRANCES 5
#define EXITS 5
#define CARSPERLEVEL 20


static const char white_text[] = "\033[37m"; // White Text
static const char black_text[] = "\033[30m";  // Black Text
static const char white_background[] = "\033[47m"; // White Background
static const char black_background[] = "\033[40m";  // Black Background
static const char reset_colour[] = "\033[0m";  // Reset

void Entry_LPR (displayMonolith *monolith, carPark *parking);
void Exit_LPR (displayMonolith *monolith, carPark *parking);

const char *spinnerStrings[] = {"⠀⡰","⢀⡠","⢄⡀", "⢆⠀", "⠎⠀", "⠊⠁", "⠈⠑", "⠀⠱"};

void polarity(int isPositive) {
    if(isPositive) {
        printf("%s%s", white_background, black_text);
    } else {
        printf("%s%s", black_background, white_text);
    }
}

/* Drawing Functions */

void spinner(void) {
    // Spinner function - nonfunctional but aim is to have a loading icon with state.
    system("clear");
    polarity(1);
    printf("%-07s%-97s\n", spinnerStrings[0], "Loading...");
    return;
}

char* boolFormat(bool value) {
    // Formats a Boolean to a string os 'Yes' or 'No' for nice display. Also adds colour!
	char *returnString;
	if(value) {
		returnString = "\x1B[32mYes Y\x1B[0m ";
	} else {
		returnString = "\x1B[31mNo  X\x1B[0m ";
    }
	return returnString;
};

char* boomFormat(char value) {
    // Formats a boom 'char' sign to a nicer display of it's state.
	char *returnString;

    if(value == 'C') {
        returnString = "C \x1B[31m⠤⠤\x1B[0m ";
    } else if(value == 'O') {
        returnString = "O \x1B[32m⡠⠊\x1B[0m ";
    } else if(value == 'L') {
        returnString = "L \x1B[33m⠤⠒\x1B[0m ";
    } else if(value == 'R') {
        returnString = "R \x1B[33m⠤⠒\x1B[0m ";
    } else {
        returnString = "     ";
    }
    //printf("%c : %s\n", value, returnString);
	return returnString;
};

char *headerDraw(void) {
    // Draws the first line of headers.
	return("\n      level data                  entrances                           exits\n"
	"     ┌╼#╾┬─╼Temp╾─┬─╼Parking╾─┐  ┌╼#╾┬─╼Plate╾─┬─╼Boom╾─┬─╼Valid╾─┐  ┌╼#╾┬─╼Plate╾─┬─╼Boom╾─┐\n");
}

char *headerLowerDraw(void) {
    // Draws the second line of headers.
	return("\n      revenue                     entrance signs                      exit signs\n"
	"     ┌╼$╾────────────┐           ┌╼#╾╼*╾┬───╼History╾───┐            ┌╼#╾╼*╾┬───╼History╾───┐\n");
}

void shuffleSignData(entrexitSlice* entry) {
    // Moves the sign history data down by a blip.
    for (int i = 5; i >= 0; i--)
    {
        //printf("%d: %c -> %d: %c\n", i, entry->signHistory[i], i+1, entry->signHistory[i+1]); 
        entry->signHistory[i+1] = entry->signHistory[i];
    }
    entry->signHistory[0] = entry->sign;
    
}

char *signData(int entryNum, entrexitSlice* entry, int type) {
    // Displays the sign data in a bounded box.
	char *returnString = malloc(120);;
	if(type == 1) {
		snprintf(returnString, 120, "      └──────┴───────────────┘");
	} else if (type == 2) {
		snprintf(returnString, 120, "                               ");
	} else {
        snprintf(returnString, 120, "      │ %d  \x1B[37;1m%c\x1B[0m │ %c %c %c %c %c %c %c │", entryNum, entry->sign, entry->signHistory[0], 
            entry->signHistory[1], entry->signHistory[2], entry->signHistory[3], entry->signHistory[4], 
            entry->signHistory[5], entry->signHistory[6]);
        shuffleSignData(entry);
    }
	return(returnString);
}

char *levelData(displaySlice entry) {
    // Displays the level data (temperature, car num, capacity) in a bounded box.
	char *returnString = malloc(120);;
	if(entry.entry == LEVELS + 1) {
		snprintf(returnString, 120, " └───┴────────┴───────────┘ ");
	} else if (entry.entry > LEVELS + 1) {
		snprintf(returnString, 120, "                            ");
	} else {
        snprintf(returnString, 120, " │ %d │ %5.2fc │   %2d/%2d   │ ", entry.entry, entry.temp, entry.cars, PARKS);
    }
	return(returnString);
}

char *entrexitData(int entryNum, entrexitSlice entry, int type, bool isEntrance) {
    // Displays entry and exit LPR data.
	char *returnString = malloc(120);;
	if(type == 1) {
        if(isEntrance) {
            snprintf(returnString, 120, " └───┴─────────┴────────┴─────────┘");
        } else {
            snprintf(returnString, 120, " └───┴─────────┴────────┘         ");
        }
	} else if (type == 2) {
		snprintf(returnString, 120, "                                   ");
	} else {
        char *boolf = boolFormat(entry.valid);
        char *boomf = boomFormat(entry.boom);
        if(isEntrance) {
            snprintf(returnString, 120, " │ %d │ %s  │  %s │  %s │", entryNum, entry.plate, boomf, boolf);
        } else {
            snprintf(returnString, 120, " │ %d │ %s  │  %s │", entryNum, entry.plate, boomf);
        }
    }
	return(returnString);

}

char *revenueData(int dollars, int cents, int type) {
    // Displays the dollars and cents value of the system.
	char *returnString = malloc(120);;
	if(type == 1) {
		snprintf(returnString, 120, " └───────────────┘");
	} else if (type == 2) {
		snprintf(returnString, 120, "                  ");
	} else {
        snprintf(returnString, 120, " │ %10d.%02d │", dollars, cents);
    }
	return(returnString);

}

char *printSlice(displaySlice entry) {
    // Creates one line of the upper section of the display.
	char *returnString = malloc(320);;
	int entrymode;
	int exitmode;
	if(entry.entry > ENTRANCES + 1) {
		entrymode = 2;
	} else if (entry.entry == ENTRANCES + 1) {
		entrymode = 1;
	} else {
		entrymode = 0;
	};
    if(entry.entry > EXITS + 1) {
        exitmode = 2;
    } else if (entry.entry == EXITS + 1) {
        exitmode = 1;
    } else {
        exitmode = 0;
	};
	char *level = levelData(entry);
	char *entryData = entrexitData(entry.entry, entry.entrance, entrymode, true);
	char *exitData = entrexitData(entry.entry, entry.exit, exitmode, false);
	snprintf(returnString, 320, "    %s%s %s\n", level, entryData, exitData);

	return returnString;

}

char *printLowerSlice(displayMonolith* monolith, int level) {
    // Creates one line of the lower section of the display.
	char *returnString = malloc(320);;
	int entrymode;
	int exitmode;
    int revenuemode;
    //printf("%d\n", (monolith->level)[1].entry);
	if(monolith->level[level].entry > ENTRANCES + 1) {
		entrymode = 2;
	} else if (monolith->level[level].entry == ENTRANCES + 1) {
		entrymode = 1;
	} else {
		entrymode = 0;
	};
    if(monolith->level[level].entry > EXITS + 1) {
        exitmode = 2;
    } else if (monolith->level[level].entry == EXITS + 1) {
        exitmode = 1;
    } else {
        exitmode = 0;
	};
    if(level == 0) {
        revenuemode = 0;
    } else if (level == 1) {
        revenuemode = 1;
    } else {
        revenuemode = 2;
    }

	char *entryData = signData(monolith->level[level].entry, &(&(monolith->level)[level])->entrance, entrymode);
	char *exitData = signData(monolith->level[level].entry, &(&(monolith->level)[level])->exit, exitmode);
    char *moneyData = revenueData(monolith->dollars, monolith->cents, revenuemode);
	snprintf(returnString, 320, "    %s     %s      %s\n", moneyData, entryData, exitData);

	return returnString;

}

/* Monolith Functionality */

void setEvacuation (displayMonolith *monolith, bool val) {
    if(val == true && monolith->evacuation == 0) {
        monolith->evacuation = 1;
    } else {
        monolith->evacuation = 0;
    }
}

void testFunc(displayMonolith *monolith, carPark *parking) {

    if(monolith->tick < 200) {
        if(monolith->tick == 40) {
            setEvacuation(monolith, true);
        } else if (monolith->tick == 60) {
            setEvacuation(monolith, false);
        } else if (monolith->tick % 2 == 1) {
            Entry_LPR(monolith, parking);
        } if (monolith->tick % 3 == 1 && monolith->tick > 90) {
            Exit_LPR(monolith, parking);
        }
    } else {

        if (monolith->tick % 2 == 1) {
            Entry_LPR(monolith, parking);
        }
        
        if (monolith->tick % 6 == 1) {
            Exit_LPR(monolith, parking);
        }
        

        if (monolith->evacuation > 0) {
            Exit_LPR(monolith, parking);
        }
    }
}

void monolithPreTick(displayMonolith *monolith) {
    int evacuation = monolith->evacuation;
    char evacChar = ' ';
    if(evacuation > 0) {
        evacChar = "EVACUATE"[evacuation-1];
        if(evacuation < 8) {
            monolith->evacuation++;
        } else {
            monolith->evacuation = 1;
        }
        for (size_t i = 0; i < LEVELS; i++)
        {
            (&(&(monolith)->level[i])->entrance)->sign = evacChar;
            (&(&(monolith)->level[i])->exit)->sign = evacChar;
            for (size_t x = 0; x < fmax(ENTRANCES, EXITS); x++)
            {
                if((&(&(monolith)->level[x])->exit)->boom == 'L') {
                    (&(&(monolith)->level[x])->exit)->boom = 'O';
                }
                if((&(&(monolith)->level[x])->entrance)->boom == 'L') {
                    (&(&(monolith)->level[x])->entrance)->boom = 'O';
                }
                if((&(&(monolith)->level[x])->exit)->boom == 'C') {
                    (&(&(monolith)->level[x])->exit)->boom = 'R';
                }
                if((&(&(monolith)->level[x])->entrance)->boom == 'C') {
                    (&(&(monolith)->level[x])->entrance)->boom = 'R';
                }
            }
            
        }
        
    }
}

void monolithTick(displayMonolith *monolith) {
    for (size_t i = 0; i < LEVELS; i++)
    {
        char oldBoom = (&(&(monolith)->level[i])->entrance)->boom;
        if (oldBoom == 'L') {
            (&(&(monolith)->level[i])->entrance)->boom = 'C';
        } else if (oldBoom == 'O') {
            (&(&(monolith)->level[i])->entrance)->boom = 'L';
        } else if (oldBoom == 'R') {
            (&(&(monolith)->level[i])->entrance)->boom = 'O';
        }
        oldBoom = (&(&(monolith)->level[i])->exit)->boom;
        if (oldBoom == 'L') {
            (&(&(monolith)->level[i])->exit)->boom = 'C';
        } else if (oldBoom == 'O') {
            (&(&(monolith)->level[i])->exit)->boom = 'L';
        } else if (oldBoom == 'R') {
            (&(&(monolith)->level[i])->exit)->boom = 'O';
        }
        (&(&(monolith)->level[i])->entrance)->sign = ' ';
        (&(&(monolith)->level[i])->exit)->sign = ' ';
    }
    monolith->tick++;
}

void printMonolith(displayMonolith *monolith) {
    // Print an instance of a monolith struct - displaying the entire system's data.
	long ns;
	struct timespec spec;
	clock_gettime(CLOCK_MONOTONIC, &spec);
	ns = spec.tv_nsec;

    monolithPreTick(monolith);
	char *header = headerDraw();
	char *headerLower = headerLowerDraw();
    displaySlice blankSlice = {6};

	char *returnString = malloc(sizeof(char) * 4 * 100 * 24);
    memset(returnString, 0, sizeof(char) * 4 * 100 * 24);
    char *slice;


    strcat(returnString, header);

    for (size_t i = 0; i < LEVELS + 1; i++)
    {
        slice = printSlice(monolith->level[i]);
        strcat(returnString, slice);
        free(slice);
    }
    
    strcat(returnString, headerLower);
    
    for (size_t i = 0; i < fmax(ENTRANCES, EXITS) + 1; i++)
    {
        slice = printLowerSlice(monolith, i);
        strcat(returnString, slice);
        free(slice);
    }

    //system("clear");
    //
    printf("\033[2J%s\n\n\n\n\n\n", returnString); // more optimised version of system("clear");

    //printf("\n %p \n", (void *)&returnString); // print string pointer
	//free(returnString);  // not needed apparently
    //strcpy(returnString, " "); // also it breaks it so-
    monolithTick(monolith);
    
    clock_gettime(CLOCK_MONOTONIC, &spec);
	
	ns = abs(floor((spec.tv_nsec - ns) / 1.0e6));
	if (ns > 500) {
		ns = 1000-ns;
	}
	printf("\n internal time: %3ld milliseconds\n", ns);
    //sleep(1);
	nanosleep((const struct timespec[]){{0, 200000000L}}, NULL); //0
}

/* Manager / Simulator Functions */

int timespecDiff(struct timespec *time_a, struct timespec *time_b) {
    // returns the time diff between two timespecs, in milliseconds.
    return (((time_a->tv_sec * 1000000000) + time_a->tv_nsec) -
           ((time_b->tv_sec * 1000000000) + time_b->tv_nsec) ) / 1e6;
}

void *makeCar(carPark *parking, char plate[7], int level) {
    // Make a car and add it to the parking object provided.
    carHolder *p = malloc(sizeof(carHolder));
    if (p == NULL) {
        printf("Failed to allocate.\n");
    }
    car *newCar = malloc(sizeof(car));
    strcpy((newCar->plate), plate);
    newCar->level = level;
    clock_gettime(CLOCK_MONOTONIC, &(newCar->entryTime));
    p->node = newCar;
    if (parking->list == NULL) {
        p->next = NULL;
        parking->list = p;
    } else {
        p->next = parking->list;
        parking->list = p;
    }
}

void charge (car *carToCharge, displayMonolith *monolith) {
    // Finds how long a car has existed, and charge it based upon that.
    struct timespec end_test;
    char *toPrint;
    int dollars;
    int cents;
    clock_gettime(CLOCK_MONOTONIC, &end_test);
    int timeElapsed = timespecDiff(&end_test, &(carToCharge->entryTime));
    dollars = (int)((timeElapsed*0.05)/100);
    cents = (int)fmod((timeElapsed*0.05), 100);
    monolith->dollars += dollars;
    if(cents + monolith->cents > 100) {
        monolith->cents += cents - 100;
    } else {
        monolith->cents += cents;
    }
    FILE *fileBill = fopen("billing.txt", "a");
    fprintf(fileBill, "%s: $%d.%d\n", carToCharge->plate, dollars, cents);
    fclose(fileBill);
}

bool checkIfInFile (char *plateToCheck) {
    // Checks if a car is in plates.txt. (Candidate for threading?)
    char plate[8];
    FILE *filePlates = fopen("plates.txt", "r");
    //printf("File Opened.\n\n");
    if (filePlates == NULL) {
        return false;
    }

    while(fgets(plate, 8, filePlates) != NULL) {
        //printf("%s\n", plate);
        for (size_t i = 0; i < 6; i++)
        {
            //printf("%d\n", i);
            if(plateToCheck[i] != plate[i]) {
                i = 7;
            }
            if(i == 5) {
                //printf("match\n");
                fclose(filePlates);
                return true;
            }
        }
        
    }
    //printf("File Closed.\n");
    return false;

}

/*
carHolder *insertNode(carHolder *head, char plate[6], int level) {
    carHolder *p = makeCar(plate, level);
    if (p == NULL) {
        printf("Failed to allocate.");
    }
    
}*/

// char* ReadFile(char *filename)
// {
//    char *buffer = NULL;
//    int string_size, read_size;
//    FILE *handler = fopen(filename, "r");

//    if (handler)
//    {
//        // Seek the last byte of the file
//        fseek(handler, 0, SEEK_END);
//        // Offset from the first to the last byte, or in other words, filesize
//        string_size = ftell(handler);
//        // go back to the start of the file
//        rewind(handler);

//        // Allocate a string that can hold it all
//        buffer = (char*) malloc(sizeof(char) * (string_size + 1) );

//        // Read it all in one operation
//        read_size = fread(buffer, sizeof(char), string_size, handler);

//        // fread doesn't set it so put a \0 in the last position
//        // and buffer is now officially a string
//        buffer[string_size] = '\0';

//        if (string_size != read_size)
//        {
//            // Something went wrong, throw away the memory and set
//            // the buffer to NULL
//            free(buffer);
//            buffer = NULL;
//        }

//        // Always remember to close the file.
//        fclose(handler);
//     }

//     return buffer;
// }






// int allowedList(int argc, char const *argv[]) 
// { 
//     FILE* inp; 
//     inp = fopen("plates.txt","r");		//filename of your data file 
//     char arr[100][7];			//max word length 50 
//     int i = 0; 
//     while(1){ 
//         char r = (char)fgetc(inp); 
//         int k = 0; 
//         while(r!=',' && !feof(inp)){	//read till , or EOF 
//             arr[i][k++] = r;			//store in array 
//             r = (char)fgetc(inp); 
//         } 
//         arr[i][k]=0;		//make last character of string null  
//         if(feof(inp)){		//check again for EOF 
//             break; 
//         } 
//         i++; 
//     } 
//     int j; 
//     for(j = 0;j<=i;j++){ 
//         printf("%s\n",arr[j] );	//print array 
//     } 
//     return 0; 
// } 


// int bannedList(){      
//     int i=0;
//     char plates[30][7];//declaration of array       
//     for(int x; x < 30; x++) {
//         char plate[7];
//         for(int i = 0; i < 3; i++) {
//             plate[i] = (char)(48 + (rand() % 10));
//             plate[i+3] =  (char)(65 + (rand() % 26));
//             plate[6] = '\0';
//         }
//         strcpy(plates[x], plate);
// 	} 
// }

char* genarateRandomLicense(void) {

    char plateC[6];
    char *plate = malloc(7);
    for(int i = 0; i < 3; i++) {
        plateC[i] = (char)('0' + (rand() % 10));
        plateC[i+3] =  (char)('A' + (rand() % 26));
    }	
    snprintf(plate, 6, "%s", plateC);
    //printf("%s\n\n", plate);
    return plate;
}
/* Unfinished */

void deleteCar (carPark *parking, carHolder *carToChange) {
    carHolder *temp = parking->list;
    carHolder *previous;

    // if in head:
    if (temp != NULL && temp->node == carToChange->node) {
        parking->list = temp->next;
        free(temp);
        return;
    }

    while (temp != NULL && temp->node != carToChange->node) {
        previous = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        return;
    }

    previous->next = temp->next;
    free(temp);
}


int getTotalCars(displayMonolith *monolith) {
    int cars = 0;
    for (size_t i = 0; i < LEVELS; i++)
    {
        cars = cars + (&(monolith)->level[i])->cars;
        //printf("cars: %d\n\n", cars);
    }
    return cars;
}

void Exit_LPR (displayMonolith *monolith, carPark *parking) {
    //(&(&(monolith)->level[1])->exit)->sign = '-';
    int carsToPickFrom = getTotalCars(monolith);
    int exit = random () % EXITS;
    if(carsToPickFrom == 0) {
        return;
    }
    int carNum = rand() % carsToPickFrom;
    printf("Didn't crash here.");
    struct carHolder *temp;
    if(parking->list != NULL) {
        temp = parking->list;
    }
    //(&(&(monolith)->level[4])->exit)->sign = 'b' + carNum;
    printf("Didn't crash here.");
    printf("%d\n\n", carsToPickFrom);
    for (size_t i = 0; i < (carNum); i++)
    {
        if(temp->next != NULL) {
            temp = temp->next;
        } else {
            return;
        }
    }
    //printf("Didn't crash here.");
    if((&(&(monolith)->level[exit])->exit)->boom != 'O') {
        (&(&(monolith)->level[exit])->exit)->boom = 'R';
    }
    (&(monolith)->level[temp->node->level])->cars--;
    (&(&(monolith)->level[exit])->exit)->sign = '1' + temp->node->level;
    (&(&(monolith)->level[exit])->exit)->plate = temp->node->plate;

    charge(temp->node, monolith);
    deleteCar(parking, temp);
}

void Entry_LPR (displayMonolith *monolith, carPark *parking) {
    int spaceFree = -1;
    int entrance = random () % ENTRANCES;
    char plate[7] = {'a', ' ', ' ', ' ', ' ', ' ', '\0'};
    char plateF[8];
    int fileLength = 0;
    int coinFlip = random() % 2;

    if(coinFlip == 1) {
        for(int i = 0; i < 3; i++) {
                plate[i] = (char)('0' + (rand() % 10));
                plate[i+3] =  (char)('A' + (rand() % 26));
            }
    } else {
        FILE *filePlates = fopen("plates.txt", "r");
        //printf("File Opened.\n\n");
        if (filePlates == NULL) {
            return;
        }
        
        while(fgets(plateF, 8, filePlates) != NULL) {
            if(plateF != "\n") {
                fileLength++;
            }
        }/**/
        fseek(filePlates, 0, SEEK_SET);

        for (size_t i = 0; i < rand() % (fileLength-1); i++) {
            //printf("%s\n", plate);
            fgets(plateF, 8, filePlates);
            for (size_t x = 0; x < 6; x++)
            {
                plate[x] = plateF[x];
            }
            
        }
    }
    

    ////
    
    if(monolith->evacuation) {
        return;
    }
    //
    if(checkIfInFile(plate)) {
        (&(&(monolith)->level[entrance])->entrance)->valid = true;
        for (size_t i = 0; i < LEVELS; i++) {
            if ((&(monolith)->level[i])->cars < PARKS) {
                spaceFree = i;
            }
        }
        if (spaceFree > -1) {  // There is a free space in the carpark.
            printf("Space Free in %d.\n", spaceFree);
            (&(&(monolith)->level[entrance])->entrance)->sign = '1' + spaceFree;
            (&(monolith)->level[spaceFree])->cars++;
            if((&(&(monolith)->level[entrance])->entrance)->boom != 'O') {
                (&(&(monolith)->level[entrance])->entrance)->boom = 'R';
            }
            makeCar(parking, plate, spaceFree);

            carHolder *temp = parking->list;
            if(temp->next != NULL) {
                temp = temp->next;
            }
            (&(&(monolith)->level[entrance])->entrance)->plate = temp->node->plate;
        } else { // Carpark full
            printf("Parking Full.\n");

        }
        //printf("match\n");
        //testFunction(monolith);
    } else {
        (&(&(monolith)->level[entrance])->entrance)->valid = false;
        if((&(&(monolith)->level[entrance])->entrance)->boom != 'C') {
            (&(&(monolith)->level[entrance])->entrance)->boom = 'L';
        }
    };
}

void simulator (displayMonolith *monolith, carPark *parking) {
    // Here's where the simulator files would go.

    //printf("\nPlate: %s\n", parking->list->node->plate);
    //printf("\n%s\n", parking->list->next->node->plate);
    testFunc(monolith, parking);

    /*
    printf("%s\n", genarateRandomLicense());
    printf("%s\n", genarateRandomLicense());
    deleteCar(parking, parking->list->next->next);
    for (int i = 0; i < 3; i++) {
        //sleep(2);
        charge(parking->list->node);

    }
    */

}

// Entry LPR:
// Generate a random numberplate
// Check if in plates.txt with checkIfInFile
//  if so, set valid to yes, then check if there's an 'entry < parks' in monolith->level
//   if so, set the gates to o, then run makeCar to add it. set entrance->sign to the level the car's being added to.
// if not in plates.txt (set valid to false), or carpark is full, set boom gates to c

// Exit LPR:
// Lift gates
// Bill car.
// Increase capacity of the car's level by 1. (monolith->levels->cars)
// Delete car.
// Lower gates

int main() {
    // Main function
    entrexitSlice entery = {"      ", true, 'C', ' ', {' ', ' ', ' ', ' ', ' ', ' ', ' '}};
    entrexitSlice exity = {"      ", false, 'O', ' ', {' ', ' ', ' ', ' ', ' ', ' ', ' '}};
    displaySlice level1 = {1, 0.5, 0, PARKS, entery, exity};
    displaySlice level2 = {2, 0.5, 0, PARKS, entery, exity};
    displaySlice level3 = {3, 0.5, 0, PARKS, entery, exity};
    displaySlice level4 = {4, 0.5, 0, PARKS, entery, exity};
    displaySlice level5 = {5, 0.5, 0, PARKS, entery, exity};
    displaySlice blanker = {6};
    displayMonolith monolith = {0, 0, {level1, level2, level3, level4, level5, blanker}, 0, 0};
    //setEvacuation(&monolith, true);
    
    carPark *parking = malloc(sizeof(carPark));


    while(true) {
        for (size_t i = 0; i < 5; i++)
        {
            /*
            (&(&(&monolith)->level[i])->entrance)->sign = "                            ABCDEFGHIJKLMNOPQRSTUVWXYZqwertyuiopasdfghjklzxcvbnm12345678901234567890"[random () % 100];
            (&(&(&monolith)->level[i])->exit)->sign = "                            ABCDEFGHIJKLMNOPQRSTUVWXYZqwertyuiopasdfghjklzxcvbnm12345678901234567890"[random () % 100];

            (&(&(&monolith)->level[i])->entrance)->boom = "COLR"[random () % 4];
            (&(&(&monolith)->level[i])->exit)->boom = "COLR"[random () % 4];

            (&(&(&monolith)->level[i])->entrance)->valid = random() % 2;
            (&(&(&monolith)->level[i])->exit)->valid = random() % 2;
            */
        }
        //printf("%c", (&(&(&monolith)->level[0])->entrance)->boom);
        simulator(&monolith, parking);
        printMonolith(&monolith);
    }

/*
    system("clear");
	printf("%s", header);
	printf("%s", printSlice(level1));
	printf("%s", headerLower);
	printf("%s", printLowerSlice(&monolith, 0));*/
    /*
    char *string = ReadFile("plates.txt");
    if (string)
    {
        puts(string);
        free(string);
    }*/

    return 0;


}


//calls in the function to make sure the cars are allowed to enter (bool)
//if true let plate continue if false display "car is banned"

//calls in the license plates for lpr function for each entrance (needs to be interchangeable)
//stays for 2ms before lpr lets it in

// call in the function to time the boom gates
//takes 10ms to open , stays open for 10ms and takes 10ms to close 

    //calls in the function to check which levels are free to add cars to park
    //needs to have a count at all times

    //calls in the function to display if the carpark is full
    //call in all the levels and check of it is not equal to max per lvl (10/10 or 9/10)

//calls in the function to rand the time car stays at park 
//start the time as soon as the car enters (maybe add 10ms to exit)  

    //call in the function to multiply the time car stayed by the cost per min
    //mutiply rand time by dollars
        
//call in the function to write the cost of each car parking price to a txt file
//make new txt if no current and add new lines (REMEMBER TO CLOSE)

//call in the function to time the boom gate opening at the exit
//make sure it takes 30ms like at open

    //call in the function to display values depending on the returns of functions
    //use aquas 


//call in the function that gives the firalarm a rand temp every 5 seconds 





//int main(int argc, char* argv[]) {
	//Starting state for parking space
	
	//Error check terminal incase individuals hasn't call the right thing
	//Print Enterance header 
	
	//Create a semaphore
	//Error if you can't create semaphore
	
	//get inputs from plate and randomePlate
	
	//Make pthreads
	
	//Print display
    
//}

/*
 * Wait for another LPR thread to release the sempare, so plate can be called in
 * if carsParked < carCount then search for level
 * carCount++ if it's allowed in
 * else if cars park is >= 100 (100 being the limit of cars)
 * 
 * 
 *  */



