#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Define constants for map dimensions
#define MAP_WIDTH 80
#define MAP_HEIGHT 21

// Define constants for terrain types
#define LONG_GRASS ':'
#define WATER '~'
#define CLEARING '.'
#define ROCK '%'
#define TREE '^'
#define ROAD '#'
//I make my Pokemon Centers and Pokemarts 2 × 2
#define POKEMON_CENTER 'C'
#define POKEMART 'M'

//Function to initialize the map
void initializeMap(char map[MAP_HEIGHT][MAP_WIDTH], int* exitIndices) {
    int i, j;
    //Initialize the map to all rocks along the edges and clearings inside the edges
    for (i = 0; i < MAP_HEIGHT; i++) {
        for (j = 0; j < MAP_WIDTH; j++) {
            if (i == 0 || i == MAP_HEIGHT - 1 || j == 0 || j == MAP_WIDTH - 1) {
                map[i][j] = ROCK;
            } else {
                map[i][j] = CLEARING;
            }
        }
    }

    //The exit is placed randomly along the top and bottom edges of the map
    srand(time(NULL));
    int exit1 = rand() % MAP_WIDTH;
    map[0][exit1] = ROAD;
    //The exit is placed randomly along the left and right edges of the map
    int exit2 = rand() % MAP_HEIGHT;
    map[exit2][0] = ROAD;
    //Store the indices of the exits in the exitIndices array
    exitIndices[0] = exit1;
    exitIndices[1] = exit2;
}


// Function to place paths
void placePaths(char map[MAP_HEIGHT][MAP_WIDTH], int* exitIndices) {

    srand(time(NULL));

    
    int nsPathStart = exitIndices[0];   
    int ewPathStart = exitIndices[1];   

    int i, j;
    // Generate N-S path
    for (i = 0; i < MAP_HEIGHT; i++) {
        // Ensure the path stays within bounds, avoiding the east and west borders
        nsPathStart = (nsPathStart < 2) ? 2 : nsPathStart;  // Avoid the left border
        nsPathStart = (nsPathStart >= MAP_WIDTH - 2) ? MAP_WIDTH - 3 : nsPathStart;  // Avoid the right border

        int moveDirection = rand() % 3 - 1;  //-1, 0, 1
        map[i][nsPathStart] = ROAD;  // Place a road at the current position
        if(moveDirection != 0){
            nsPathStart += moveDirection;

            map[i][nsPathStart] = ROAD;
        }

    }

    // Generate E-W path
    for (j = 0; j < MAP_WIDTH; j++) {
        // Ensure the path stays within bounds, avoiding the north and south borders
        ewPathStart = (ewPathStart < 2) ? 2 : ewPathStart;  // Avoid the top border
        ewPathStart = (ewPathStart >= MAP_HEIGHT - 2) ? MAP_HEIGHT - 3 : ewPathStart;  // Avoid the bottom border

        int moveDirection = rand() % 3 - 1;
        map[ewPathStart][j] = ROAD;
        if (moveDirection != 0) {
            ewPathStart += moveDirection;
            map[ewPathStart][j] = ROAD;
        }

    }
}











int main(int argc, char *argv[]) {  // Define the main function with command-line arguments
    char map[MAP_HEIGHT][MAP_WIDTH];    // Define a 2D array of characters to represent the map
    int exitIndices[2];                 // Define an array to store the indices of the exits
    initializeMap(map, exitIndices);                 // Initialize the map to all rocks
    //generateTerrain(map);               // Generate the terrain
    //print the map
    placePaths(map, exitIndices);                    // Place paths
    int i, j;
    for (i = 0; i < MAP_HEIGHT; i++) {
        for (j = 0; j < MAP_WIDTH; j++) {
            printf("%c", map[i][j]);
        }
        printf("\n");
    }
}