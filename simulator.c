//calling in the .h file 
#include "carparkSim.h"

//call the plates.txt and put in array
//genarate 30 banned plates and put in array

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

/*Will need to take car plate parameter to check if true of false
 * If car is in list call 
 * Check if there's space
 * If both conditions are turn
 * book gate to open 
 * and change display to show available spot*/
void LPREntry() {
	/*Select an empty parkign space */
	
	/*When car park ifs full put on display */

}


void LPRExit() {
	/*Check time car has been there*/
	/*Calculate */
	/*Call function to write to bill.txt */
}



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

int main(){
    printf(allowedList);
    printf(bannedList);
}



