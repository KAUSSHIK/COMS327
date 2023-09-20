#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>

// Define constants for map dimensions and terrain types
#define MAP_WIDTH 80
#define MAP_HEIGHT 21

#define WORLD_SIZE 401

#define LONG_GRASS ':'
#define WATER '~'
#define CLEARING '.'
#define CLEARING_GRASS '.'
#define ROCK '%'
#define TREE '^'
#define ROAD '#'

#define POKEMON_CENTER 'C'
#define POKEMART 'M'

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define BLUE    "\x1b[34m"
#define RESET   "\x1b[0m"
#define BROWN   "\x1b[33m"
#define PINK    "\x1b[38;5;205m"

/*
                                        QUEUE
*/

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

/*
                    MAP AND WORLD INITIALIZATION
*/
int worldXCoord = 200; //world 2d array traverser
int worldYCoord = 200; //world 2d array traverser
int playerX = 0; //player position in the map think like unit circle
int playerY = 0; //player position in the map think like unit circle
int centerX;
int centerY;

struct Map
{
    char map[MAP_HEIGHT][MAP_WIDTH];
    int nGate;
    int sGate;
    int eGate;
    int wGate;
};

struct Map *world[WORLD_SIZE][WORLD_SIZE];


/*
                                        MAP GENERATION FROM PREVIOUS ASSIGNMENT
*/
//Function to initialize the map
void initializeMap(struct Map* map, int nGate, int sGate, int eGate, int wGate) {
    int i, j;
    // Initialize the map to all rocks along the edges and clearings inside the edges
    for (i = 0; i < MAP_HEIGHT; i++) {
        for (j = 0; j < MAP_WIDTH; j++) {
            if (i == 0 || i == MAP_HEIGHT - 1 || j == 0 || j == MAP_WIDTH - 1) {
                map->map[i][j] = ROCK;
            } else {
                map->map[i][j] = CLEARING;
            }
        }
    }


    // MAP TO MEET REQUIREMENTS:
    int tallGrassRegions = 7;
    int waterRegions = 4;
    int clearingRegions = 5;

    while (tallGrassRegions > 0 || waterRegions > 0 || clearingRegions > 0) {
        int startX, startY;

        // Expand the tall grass regions
        for (int tg = 0; tg < tallGrassRegions; tg++) {
            startX = rand() % (MAP_WIDTH - 2) + 1;
            startY = rand() % (MAP_HEIGHT - 2) + 1;

            while (startX > 0 && startX < MAP_WIDTH - 1 && startY > 0 && startY < MAP_HEIGHT - 1) {
                map->map[startY][startX] = LONG_GRASS;
                startX += rand() % 3 - 1;  // -1, 0, 1
                startY += rand() % 3 - 1;
            }

            tallGrassRegions--;
        }

        // Expand the water regions
        for (int wr = 0; wr < waterRegions; wr++) {
            startX = rand() % (MAP_WIDTH - 2) + 1;
            startY = rand() % (MAP_HEIGHT - 2) + 1;

            while (startX > 0 && startX < MAP_WIDTH - 1 && startY > 0 && startY < MAP_HEIGHT - 1) {
                map->map[startY][startX] = WATER;
                startX += rand() % 3 - 1;  // -1, 0, 1
                startY += rand() % 3 - 1;
            }

            waterRegions--;
        }

        // Expand the clearing regions
        for (int cr = 0; cr < clearingRegions; cr++) {
            startX = rand() % (MAP_WIDTH - 2) + 1;
            startY = rand() % (MAP_HEIGHT - 2) + 1;

            while (startX > 0 && startX < MAP_WIDTH - 1 && startY > 0 && startY < MAP_HEIGHT - 1) {
                map->map[startY][startX] = CLEARING_GRASS;
                startX += rand() % 3 - 1;  // -1, 0, 1
                startY += rand() % 3 - 1;
            }

            clearingRegions--;
        }
    }

    // Initialize the gates
    map->nGate = nGate;
    map->sGate = sGate;
    map->eGate = eGate;
    map->wGate = wGate;
}



/*
    MAP EXIT ASSIGNMENT
*/
void placeExits(struct Map* map, int nsPathStart, int nsPathEnd, int ewPathStart, int ewPathEnd) {
    if (nsPathStart == -1) {
        nsPathStart = rand() % MAP_WIDTH;
        map->nGate = nsPathStart; // Update nGate
    }
    if (nsPathEnd == -1) {
        nsPathEnd = rand() % MAP_WIDTH;
        map->sGate = nsPathEnd; // Update sGate
    }
    if (ewPathStart == -1) {
        ewPathStart = rand() % MAP_HEIGHT;
        map->wGate = ewPathStart; // Update eGate
    }
    if (ewPathEnd == -1) {
        ewPathEnd = rand() % MAP_HEIGHT;
        map->eGate = ewPathEnd; // Update wGate
    }
    map->map[0][nsPathStart] = ROAD;
    map->map[ewPathStart][0] = ROAD;
    map->map[MAP_HEIGHT - 1][nsPathEnd] = ROAD;
    map->map[ewPathEnd][MAP_WIDTH - 1] = ROAD;
}

/*
    MAP PATH PLACEMENT
*/
// Function to place paths
void placePaths(struct Map* map) {
    int nsPathStart = map->nGate;
    int nsPathEnd = map->sGate;
    int ewPathStart = map->wGate;
    int ewPathEnd = map->eGate;
    placeExits(map, nsPathStart, nsPathEnd, ewPathStart, ewPathEnd);
    int i, j;
    // Generate N-S path
    for (i = 1; i < MAP_HEIGHT - 2; i++) {
        // Ensure the path stays within bounds, avoiding the east and west borders
        nsPathStart = (nsPathStart < 2) ? 2 : nsPathStart;
        nsPathStart = (nsPathStart >= MAP_WIDTH - 2) ? MAP_WIDTH - 3 : nsPathStart;

        int moveDirection = rand() % 3 - 1;  //-1, 0, 1
        map->map[i][nsPathStart] = ROAD;
        if (moveDirection != 0) {
            nsPathStart += moveDirection;
            if (map->map[i][nsPathStart] != POKEMON_CENTER && map->map[i][nsPathStart] != POKEMART && map->map[i][nsPathStart] != ROCK) {
                map->map[i][nsPathStart] = ROAD;
            }
        }
    }
    int startCol;
    int endCol;
    if (nsPathStart < nsPathEnd) {
        startCol = nsPathStart;
        endCol = nsPathEnd;
    } else {
        startCol = nsPathEnd;
        endCol = nsPathStart;
    }

    for (int col = startCol; col <= endCol; col++) {
        map->map[i][col] = ROAD;
    }
    int nextCol = (endCol == nsPathStart) ? nsPathEnd + 1 : nsPathStart + 1;
    map->map[i][nextCol] = ROAD;

    // Generate E-W path
    for (j = 1; j < MAP_WIDTH - 2; j++) {

        ewPathStart = (ewPathStart < 2) ? 2 : ewPathStart;  // Avoid the top border
        ewPathStart = (ewPathStart >= MAP_HEIGHT - 2) ? MAP_HEIGHT - 3 : ewPathStart;  // Avoid the bottom border

        int moveDirection = rand() % 3 - 1;
        map->map[ewPathStart][j] = ROAD;
        if (moveDirection != 0) {
            ewPathStart += moveDirection;
            if (map->map[ewPathStart][j] != POKEMON_CENTER && map->map[ewPathStart][j] != POKEMART && map->map[ewPathStart][j] != ROCK) {
                map->map[ewPathStart][j] = ROAD;
            }

        }
    }
    int startRow, endRow;

    if (ewPathStart < ewPathEnd) {
        startRow = ewPathStart;
        endRow = ewPathEnd;
    } else {
        startRow = ewPathEnd;
        endRow = ewPathStart;
    }

    for (int row = startRow; row <= endRow; row++) {
        map->map[row][j] = '#';
    }
    map->map[endRow][j] = '#';
}

/*
                                FIND AND CONNECT POKE CENTERES AND POKE MARTS TO ROADS
*/


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

/*
                                                PLACE POKE STORES
*/


    // //Probability of the PokeCenter exisitng is a function of the manhatan distance from the center of the map
    // int manhattanD = (abs(playerX) + abs(playerY));
    // double probability = (-45.0 * manhattanD / 200.0) + 50.0;

    // if(probability > 100.0){
    //     probability = 100.0;
    // } else if(probability < 5.0){
    //     probability = 5.0;
    // }

    // double randomValue = (double)(rand() % 101);


    void placePokeStores(char map[MAP_HEIGHT][MAP_WIDTH]){

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

    int nearestPMRoadX, nearestPMRoadY;

    findNearestRoad(map, pmX, pmY, &nearestPMRoadX, &nearestPMRoadY);

    // Connect the PokeStore to the nearest road using a straight line
    int x = pmX;
    int y = pmY;
    while (x != nearestPMRoadX || y != nearestPMRoadY) {
        if (x < nearestPMRoadX) {
            x++;
        } else if (x > nearestPMRoadX) {
            x--;
        } else if (y < nearestPMRoadY) {
            y++;
        } else {
            y--;
        }
        if(map[y][x] != POKEMON_CENTER && map[y][x] != POKEMART){
            map[y][x] = ROAD;
        }
    }

    int nearestPCRoadX, nearestPCRoadY;

    findNearestRoad(map, pcX, pcY, &nearestPCRoadX, &nearestPCRoadY);

    // Connect the PokeStore to the nearest road using a straight line
    x = pcX; 
    y = pcY;
    while (x != nearestPCRoadX || y != nearestPCRoadY) {
        if (x < nearestPCRoadX) {
            x++;
        } else if (x > nearestPCRoadX) {
            x--;
        } else if (y < nearestPCRoadY) {
            y++;
        } else {
            y--;
        }
        if(map[y][x] != POKEMON_CENTER && map[y][x] != POKEMART){
            map[y][x] = ROAD;
        }
    }



}





/*
                                        MAP DISPLAY
*/
void printColoredMap(char map[MAP_HEIGHT][MAP_WIDTH]) {
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            char* textColor = RESET;  
            switch (map[i][j]) {
                case WATER:
                    textColor = BLUE;
                    break;
                case LONG_GRASS:
                    textColor = GREEN;
                    break;
                case ROCK:
                    textColor = BROWN;
                    break;
                case CLEARING:
                    textColor = RED;
                    break;
                case POKEMART:
                    textColor = PINK;
                    break;
                case POKEMON_CENTER:
                    textColor = PINK;
                    break;

                default:
                    
                    break;
            }

            // Print the character with the chosen color
            printf("%s%c%s", textColor, map[i][j], RESET);
        }
        printf("\n");  
    }
}










bool isValidWorldMapCoordinates(int x, int y) {
    return x >= 0 && x < WORLD_SIZE && y >= 0 && y < WORLD_SIZE;
}

void flyToWorldMap(int* worldXCoord, int* worldYCoord, int targetX, int targetY) {
    if (isValidWorldMapCoordinates(targetX, targetY)) {
        *worldXCoord = targetX;
        *worldYCoord = targetY;
    } else {
        printf("Invalid world map coordinates.\n");
    }
}


int main() {
    srand(time(NULL));

    // Initialize the world array and allocate memory for all elements
    struct Map* world[WORLD_SIZE][WORLD_SIZE];
    int worldXCoord = 200;
    int worldYCoord = 200;

    char command;
    int targetWorldX, targetWorldY;
    do {
        if (world[worldXCoord][worldYCoord] == NULL) {
            // Determine neighboring gates or set them to -1 if neighbors don't exist
            int nGate = (worldYCoord > 0 && world[worldXCoord][worldYCoord - 1] != NULL) ? world[worldXCoord][worldYCoord - 1]->sGate : -1;
            int sGate = (worldYCoord < WORLD_SIZE - 1 && world[worldXCoord][worldYCoord + 1] != NULL) ? world[worldXCoord][worldYCoord + 1]->nGate : -1;
            int eGate = (worldXCoord < WORLD_SIZE - 1 && world[worldXCoord + 1][worldYCoord] != NULL) ? world[worldXCoord + 1][worldYCoord]->wGate : -1;
            int wGate = (worldXCoord > 0 && world[worldXCoord - 1][worldYCoord] != NULL) ? world[worldXCoord - 1][worldYCoord]->eGate : -1;

            // Create a new map and initialize it with gates
            world[worldXCoord][worldYCoord] = (struct Map*)malloc(sizeof(struct Map));
            if (world[worldXCoord][worldYCoord] == NULL) {
                // Maybe log the failure and sleep for a bit.
                sleep(1); // Requires #include <unistd.h>
                // Try again.
                world[worldXCoord][worldYCoord] = (struct Map*)malloc(sizeof(struct Map));
                if (world[worldXCoord][worldYCoord] == NULL) {
                    fprintf(stderr, "Memory allocation failed again. Exiting.\n");
                    exit(EXIT_FAILURE);
                }
            }

            initializeMap(world[worldXCoord][worldYCoord], nGate, sGate, eGate, wGate);
            placePaths(world[worldXCoord][worldYCoord]);
            placePokeStores(world[worldXCoord][worldYCoord]->map);
        }

        printColoredMap(world[worldXCoord][worldYCoord]->map);
        printf("Current Coordinates: (%d, %d)\n", worldXCoord - 200, worldYCoord - 200);

        printf("Enter a command (n/s/e/w/f x y/q): ");
        scanf(" %c", &command);

        switch (command) {
            case 'n':
                if (worldYCoord > 0) {
                    worldYCoord--;
                } else {
                    printf("You cannot move north. You are at the northern edge of the world.\n");
                }
                break;
            case 's':
                if (worldYCoord < WORLD_SIZE - 1) {
                    worldYCoord++;
                } else {
                    printf("You cannot move south. You are at the southern edge of the world.\n");
                }
                break;
            case 'e':
                if (worldXCoord < WORLD_SIZE - 1) {
                    worldXCoord++;
                } else {
                    printf("You cannot move east. You are at the eastern edge of the world.\n");
                }
                break;
            case 'w':
                if (worldXCoord > 0) {
                    worldXCoord--;
                } else {
                    printf("You cannot move west. You are at the western edge of the world.\n");
                }
                break;
            case 'f':
                // Handle 'f' command (fly)
                scanf("%d %d", &targetWorldX, &targetWorldY);
                flyToWorldMap(&worldXCoord, &worldYCoord, targetWorldX, targetWorldY);
                printf("You have flown to world map coordinates (%d, %d).\n", worldXCoord, worldYCoord);
                break;
            default:
                break;
            // Handle other commands here...
        }

    } while (command != 'q');

    // Clean up and free memory
    for (int i = 0; i < WORLD_SIZE; i++) {
        for (int j = 0; j < WORLD_SIZE; j++) {
            free(world[i][j]);
        }
    }

    return 0;
}