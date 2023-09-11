#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Define constants for map dimensions
#define MAP_WIDTH 80
#define MAP_HEIGHT 21

//MAKE A QUEUE
// Structure for a queue node
struct QueueNode {
    int x, y;  // Coordinates of the node
    struct QueueNode* next;  // Pointer to the next node in the queue
};

// Structure for the queue
struct Queue {
    struct QueueNode* front;
    struct QueueNode* rear;
};

// Function to create a new queue
struct Queue* createQueue() {
    struct Queue* queue = (struct Queue*)malloc(sizeof(struct Queue));
    queue->front = NULL;
    queue->rear = NULL;
    return queue;
}

// Function to enqueue a node in the queue
void enqueue(struct Queue* queue, int x, int y) {
    // Create a new node
    struct QueueNode* newNode = (struct QueueNode*)malloc(sizeof(struct QueueNode));
    newNode->x = x;
    newNode->y = y;
    newNode->next = NULL;

    // If the queue is empty, set the new node as both front and rear
    if (queue->rear == NULL) {
        queue->front = newNode;
        queue->rear = newNode;
        return;
    }

    // Otherwise, add the new node to the rear
    queue->rear->next = newNode;
    queue->rear = newNode;
}

// Function to dequeue a node from the queue
void dequeue(struct Queue* queue, int* x, int* y) {
    if (queue->front == NULL) {
        *x = -1;
        *y = -1;
        return;
    }

    // Get the front node's coordinates
    *x = queue->front->x;
    *y = queue->front->y;

    // Remove the front node
    struct QueueNode* temp = queue->front;
    queue->front = queue->front->next;

    // If the queue becomes empty, update the rear as well
    if (queue->front == NULL) {
        queue->rear = NULL;
    }

    free(temp);
}

// Define constants for terrain types
#define LONG_GRASS ':'
#define WATER '~'
#define CLEARING '.'
#define CLEARING_GRASS '.'
#define ROCK '%'
#define TREE '^'
#define ROAD '#'
//I make my Pokemon Centers and Pokemarts 2 × 2
#define POKEMON_CENTER 'C'
#define POKEMART 'M'

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_BROWN   "\x1b[33m"


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

    //MAP TO MEET REQUIREMENTS:
    int tallGrassRegions = 2;
    int waterRegions = 1;
    int clearingRegions = 2;

    int lgX1, lgY1, lgX2, lgY2;
    int wX, wY;
    int cX1, cY1, cX2, cY2;

    lgX1 = rand() % (MAP_WIDTH - 3) + 1;  
    lgY1 = rand() % (MAP_HEIGHT - 3) + 1;
    lgX2 = rand() % (MAP_WIDTH - 3) + 1;  
    lgY2 = rand() % (MAP_HEIGHT - 3) + 1;
    wX = rand() % (MAP_WIDTH - 3) + 1;  
    wY = rand() % (MAP_HEIGHT - 3) + 1;
    cX1 = rand() % (MAP_WIDTH - 3) + 1;  
    cY1 = rand() % (MAP_HEIGHT - 3) + 1;
    cX2 = rand() % (MAP_WIDTH - 3) + 1;
    cY2 = rand() % (MAP_HEIGHT - 3) + 1;

    // Expand outward from the starting points of all these regions until the map is filled
    while (tallGrassRegions > 0 || waterRegions > 0 || clearingRegions > 0) {
        // Expand the tall grass regions
        if (tallGrassRegions > 0) {
            // Expand the first tall grass region
            if (lgX1 > 0 && lgX1 < MAP_WIDTH - 1 && lgY1 > 0 && lgY1 < MAP_HEIGHT - 1) {
                map[lgY1][lgX1] = LONG_GRASS;
                lgX1 += rand() % 3 - 1;  //-1, 0, 1
                lgY1 += rand() % 3 - 1;
            } else {
                tallGrassRegions--;
            }

            // Expand the second tall grass region
            if (lgX2 > 0 && lgX2 < MAP_WIDTH - 1 && lgY2 > 0 && lgY2 < MAP_HEIGHT - 1) {
                map[lgY2][lgX2] = LONG_GRASS;
                lgX2 += rand() % 3 - 1;  //-1, 0, 1
                lgY2 += rand() % 3 - 1;
            } else {
                tallGrassRegions--;
            }
        }

        // Expand the water region
        if (waterRegions > 0) {
            if (wX > 0 && wX < MAP_WIDTH - 1 && wY > 0 && wY < MAP_HEIGHT - 1) {
                map[wY][wX] = WATER;
                wX += rand() % 3 - 1;  //-1, 0, 1
                wY += rand() % 3 - 1;
            } else {
                waterRegions--;
            }
        }

        //Expand the clearing regions
        if (clearingRegions > 0) {
            // Expand the first clearing region
            if (cX1 > 0 && cX1 < MAP_WIDTH - 1 && cY1 > 0 && cY1 < MAP_HEIGHT - 1) {
                map[cY1][cX1] = CLEARING_GRASS;
                cX1 += rand() % 3 - 1;  //-1, 0, 1
                cY1 += rand() % 3 - 1;
            } else {
                clearingRegions--;
            }

            // Expand the second clearing region
            if (cX2 > 0 && cX2 < MAP_WIDTH - 1 && cY2 > 0 && cY2 < MAP_HEIGHT - 1) {
                map[cY2][cX2] = CLEARING_GRASS;
                cX2 += rand() % 3 - 1;  //-1, 0, 1
                cY2 += rand() % 3 - 1;
            } else {
                clearingRegions--;
            }
        }
            
    }
}


// Function to place paths
void placePaths(char map[MAP_HEIGHT][MAP_WIDTH], int* exitIndices) {

    srand(time(NULL));

    
    int nsPathStart = exitIndices[0];   
    int ewPathStart = exitIndices[1];   

    int i, j;
    // Generate N-S path
    for (i = 1; i < MAP_HEIGHT; i++) {
        // Ensure the path stays within bounds, avoiding the east and west borders
        nsPathStart = (nsPathStart < 2) ? 2 : nsPathStart;  
        nsPathStart = (nsPathStart >= MAP_WIDTH - 2) ? MAP_WIDTH - 3 : nsPathStart;  

        int moveDirection = rand() % 3 - 1;  //-1, 0, 1
        map[i][nsPathStart] = ROAD;   
        if(moveDirection != 0){
            nsPathStart += moveDirection;

            map[i][nsPathStart] = ROAD;
        }

    }

    // Generate E-W path
    for (j = 1; j < MAP_WIDTH; j++) {
        
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

void placePokeStores(char map[MAP_HEIGHT][MAP_WIDTH], int* exitIndices, int* pokeCenX, int* pokeCenY, int* pokeMartX, int* pokeMartY){
    srand(time(NULL));

    int pcX, pcY;
    do {
        pcX = rand() % (MAP_WIDTH - 3) + 1;  
        pcY = rand() % (MAP_HEIGHT - 3) + 1;
    } while (!(map[pcY][pcX] == CLEARING && map[pcY][pcX + 1] == CLEARING && map[pcY + 1][pcX] == CLEARING && map[pcY + 1][pcX + 1] == CLEARING));

    // Place the PC grid
    map[pcY][pcX] = POKEMON_CENTER;
    map[pcY][pcX + 1] = POKEMON_CENTER;
    map[pcY + 1][pcX] = POKEMON_CENTER;
    map[pcY + 1][pcX + 1] = POKEMON_CENTER;
    *pokeCenX = pcX;
    *pokeCenY = pcY;
    
    int pmX, pmY;  // Coordinates for the top-left corner of the PM grid
    do {
        pmX = rand() % (MAP_WIDTH - 3) + 1;  
        pmY = rand() % (MAP_HEIGHT - 3) + 1;
    } while (!(map[pmY][pmX] == CLEARING && map[pmY][pmX + 1] == CLEARING && map[pmY + 1][pmX] == CLEARING && map[pmY + 1][pmX + 1] == CLEARING));

    // Place the PM grid
    map[pmY][pmX] = POKEMART;
    map[pmY][pmX + 1] = POKEMART;
    map[pmY + 1][pmX] = POKEMART;
    map[pmY + 1][pmX + 1] = POKEMART;
    *pokeMartX = pmX;
    *pokeMartY = pmY;

}



// Function to perform BFS to find the nearest road
void findNearestRoad(char map[MAP_HEIGHT][MAP_WIDTH], int startX, int startY, int* nearestX, int* nearestY) {
    struct Queue* queue = createQueue();
    int visited[MAP_HEIGHT][MAP_WIDTH];  
    int dx[] = {-1, 1, 0, 0};  
    int dy[] = {0, 0, -1, 1};  
    
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            visited[i][j] = 0;
        }
    }

    enqueue(queue, startX, startY);
    visited[startY][startX] = 1;  

    while (queue->front != NULL) {
        int x, y;
        dequeue(queue, &x, &y);

        if (map[y][x] == ROAD) {
            *nearestX = x;
            *nearestY = y;
            break;
        }

        for (int i = 0; i < 4; i++) {
            int newX = x + dx[i];
            int newY = y + dy[i];

            if (newX >= 0 && newX < MAP_WIDTH && newY >= 0 && newY < MAP_HEIGHT && !visited[newY][newX]) {
                enqueue(queue, newX, newY);
                visited[newY][newX] = 1;
            }
        }
    }

    free(queue);
}

// Function to connect a PokeStore (PC or PM) to the nearest road
void connectPokeStoreToRoad(char map[MAP_HEIGHT][MAP_WIDTH], int pokeStoreX, int pokeStoreY) {
    int nearestRoadX, nearestRoadY;
    findNearestRoad(map, pokeStoreX, pokeStoreY, &nearestRoadX, &nearestRoadY);

    // Connect the PokeStore to the nearest road using a straight line
    int x = pokeStoreX, y = pokeStoreY;
    while (x != nearestRoadX || y != nearestRoadY) {
        if (x < nearestRoadX) {
            x++;
        } else if (x > nearestRoadX) {
            x--;
        } else if (y < nearestRoadY) {
            y++;
        } else {
            y--;
        }
        if(map[y][x] != POKEMON_CENTER && map[y][x] != POKEMART){
            map[y][x] = ROAD;
        }
    }
}

void printColoredMap(char map[MAP_HEIGHT][MAP_WIDTH]) {
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            char* textColor = ANSI_COLOR_RESET;  
            switch (map[i][j]) {
                case WATER:
                    textColor = ANSI_COLOR_BLUE;
                    break;
                case LONG_GRASS:
                    textColor = ANSI_COLOR_GREEN;
                    break;
                case ROCK:
                    textColor = ANSI_COLOR_BROWN;
                    break;

                default:
                    
                    break;
            }

            // Print the character with the chosen color
            printf("%s%c%s", textColor, map[i][j], ANSI_COLOR_RESET);
        }
        printf("\n");  
    }
}


int main(int argc, char *argv[]) {
    char map[MAP_HEIGHT][MAP_WIDTH];  
      
    int exitIndices[2];       

    int pokemonCenX, pokemonCenY, pokeMartX, pokeMartY;  

    initializeMap(map, exitIndices);  

    placePaths(map, exitIndices);     

    placePokeStores(map, exitIndices, &pokemonCenX, &pokemonCenY, &pokeMartX, &pokeMartY);

    connectPokeStoreToRoad(map, pokemonCenX, pokemonCenY);  
    connectPokeStoreToRoad(map, pokeMartX, pokeMartY);

    int i, j;
    printColoredMap(map);
}