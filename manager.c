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

void shuffleSignData(entrexitSlice* entry) {
    for (int i = 3; i >= 0; i--)
    {
        //printf("%d: %c -> %d: %c\n", i, entry->signHistory[i], i+1, entry->signHistory[i+1]); 
        entry->signHistory[i+1] = entry->signHistory[i];
    }
    entry->signHistory[0] = entry->sign;
    
}

char *signData(int entryNum, entrexitSlice* entry, int type) {
	char *returnString = malloc(120);;
	if(type == 1) {
		snprintf(returnString, 120, "           └───┴───┴───────────┘");
	} else if (type == 2) {
		snprintf(returnString, 120, "                                     ");
	} else {
        snprintf(returnString, 120, "           │ %d │ %c │ %c %c %c %c %c │", entryNum, entry->sign, entry->signHistory[0], 
            entry->signHistory[1], entry->signHistory[2], entry->signHistory[3], entry->signHistory[4]);
        shuffleSignData(entry);
    }
	return(returnString);
}

char *levelData(displaySlice entry) {
	char *returnString = malloc(120);;
	if(entry.entry == LEVELS + 1) {
		snprintf(returnString, 120, " └───┴────────┴───────────┘ ");
	} else if (entry.entry > LEVELS + 1) {
		snprintf(returnString, 120, "                            ");
	} else {
        snprintf(returnString, 120, " │ %d │ %5.2fc │   %2d/%2d   │ ", entry.entry, entry.temp, entry.cars, entry.capacity);
    }
	return(returnString);
}

char *entrexitData(int entryNum, entrexitSlice entry, int type) {
	char *returnString = malloc(120);;
	if(type == 1) {
		snprintf(returnString, 120, " └───┴─────────┴────────┴─────────┘");
	} else if (type == 2) {
		snprintf(returnString, 120, "                                   ");
	} else {
        char *boolf = boolFormat(entry.valid);
        char *boomf = boomFormat(entry.boom);
        snprintf(returnString, 120, " │ %d │ %s │  %s │   %s   │", entryNum, entry.plate, boolf, boomf);
    }
	return(returnString);

}

char *revenueData(int dollars, int cents, int type) {
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
	char *entryData = entrexitData(entry.entry, entry.entrance, entrymode);
	char *exitData = entrexitData(entry.entry, entry.exit, exitmode);
	snprintf(returnString, 320, "%s%s%s\n", level, entryData, exitData);

	return returnString;

}

char *printLowerSlice(displayMonolith* monolith, int level) {
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
	snprintf(returnString, 320, "%s%s   %s\n", moneyData, entryData, exitData);

	return returnString;

}




char* ReadFile(char *filename)
{
   char *buffer = NULL;
   int string_size, read_size;
   FILE *handler = fopen(filename, "r");

   if (handler)
   {
       // Seek the last byte of the file
       fseek(handler, 0, SEEK_END);
       // Offset from the first to the last byte, or in other words, filesize
       string_size = ftell(handler);
       // go back to the start of the file
       rewind(handler);

       // Allocate a string that can hold it all
       buffer = (char*) malloc(sizeof(char) * (string_size + 1) );

       // Read it all in one operation
       read_size = fread(buffer, sizeof(char), string_size, handler);

       // fread doesn't set it so put a \0 in the last position
       // and buffer is now officially a string
       buffer[string_size] = '\0';

       if (string_size != read_size)
       {
           // Something went wrong, throw away the memory and set
           // the buffer to NULL
           free(buffer);
           buffer = NULL;
       }

       // Always remember to close the file.
       fclose(handler);
    }

    return buffer;
}

void genarateRandomLicense(int argc, char **argv) {

	for(int x; x < 10; x++) {
		char plate[6];
		for(int i = 0; i < 3; i++) {
			plate[i] = (char)(48 + (rand() % 10));
			plate[i+3] =  (char)(65 + (rand() % 26));
		}		
		printf("%s\n",plate);
	}
}



void printMonolith(displayMonolith *monolith) {
	long ns;
	struct timespec spec;
	clock_gettime(CLOCK_MONOTONIC, &spec);
	ns = spec.tv_nsec;

	char *header = headerDraw();
	char *headerLower = headerLowerDraw();
    displaySlice blankSlice = {6};


    system("clear");
	printf("%s", header);
    for (size_t i = 0; i < LEVELS + 1; i++)
    {
        printf("%s", printSlice(monolith->level[i]));
    }
    
	printf("%s", headerLower);
    for (size_t i = 0; i < fmax(ENTRANCES, EXITS) + 1; i++)
    {
        printf("%s", printLowerSlice(monolith, i));
    }
	clock_gettime(CLOCK_MONOTONIC, &spec);
	
	ns = abs(floor((spec.tv_nsec - ns) / 1.0e6));
	if (ns > 500) {
		ns = 1000-ns;
	}
	printf("\n internal time: %3ld milliseconds\n", ns);
	nanosleep((const struct timespec[]){{0, 20000000L}}, NULL);
}

int allowedList(int argc, char const *argv[]) 
{ 
    FILE* inp; 
    inp = fopen("plates.txt","r");		//filename of your data file 
    char arr[100][7];			//max word length 50 
    int i = 0; 
    while(1){ 
        char r = (char)fgetc(inp); 
        int k = 0; 
        while(r!=',' && !feof(inp)){	//read till , or EOF 
            arr[i][k++] = r;			//store in array 
            r = (char)fgetc(inp); 
        } 
        arr[i][k]=0;		//make last character of string null  
        if(feof(inp)){		//check again for EOF 
            break; 
        } 
        i++; 
    } 
    int j; 
    for(j = 0;j<=i;j++){ 
        printf("%s\n",arr[j] );	//print array 
    } 
    return 0; 
} 



int bannedList(){      
    int i=0;
    char plates[30][7];//declaration of array       
    for(int x; x < 30; x++) {
        char plate[7];
        for(int i = 0; i < 3; i++) {
            plate[i] = (char)(48 + (rand() % 10));
            plate[i+3] =  (char)(65 + (rand() % 26));
            plate[6] = '\0';
        }
        strcpy(plates[x], plate);
	} 
}

int main()
{
    entrexitSlice entery = {"XXX-000", false, true, 'H', {' ', ' ', ' ', ' ', ' '}};
    entrexitSlice exity = {"000-000", true, true, 'H', {' ', ' ', ' ', ' ', ' '}};
    displaySlice level1 = {1, 0.5, 16, 20, entery, exity};
    displaySlice level2 = {2, 0.5, 16, 20, entery, exity};
    displaySlice level3 = {3, 0.5, 16, 20, entery, exity};
    displaySlice level4 = {4, 0.5, 16, 20, entery, exity};
    displaySlice level5 = {5, 0.5, 16, 20, entery, exity};
    displaySlice blanker = {6};
    displayMonolith monolith = {2765, 84, {level1, level2, level3, level4, level5, blanker}};

    while(true) {
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



