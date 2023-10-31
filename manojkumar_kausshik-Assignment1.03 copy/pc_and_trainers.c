#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/time.h>
#include <assert.h>
#include <unistd.h> //for usleep
#include "heap.h"

// ANSI color codes
#define ANSI_RESET       "\x1b[0m"
#define ANSI_BLUE        "\x1b[34m"
#define ANSI_GREEN       "\x1b[32m"
#define ANSI_YELLOW      "\x1b[33m"
#define ANSI_RED         "\x1b[31m"
#define ANSI_PINK        "\x1b[95m" // It's actually magenta, closest to pink
#define ANSI_DARK_GREEN "\x1b[32m"
#define ANSI_CYAN "\x1b[36m"
#define ANSI_LIGHT_MAGENTA "\x1b[95m"
#define ANSI_LIGHT_RED "\x1b[91m"
#define ANSI_DARK_GRAY "\x1b[90m"
#define ANSI_ORANGE "\x1b[38;5;214m"

#define MOUNTAIN_SYMBOL       '%'
#define BOULDER_SYMBOL        '0'
#define TREE_SYMBOL           '4'
#define FOREST_SYMBOL         '^'
#define GATE_SYMBOL           '#'
#define PATH_SYMBOL           '#'
#define POKEMART_SYMBOL       'M'
#define POKEMON_CENTER_SYMBOL 'C'
#define TALL_GRASS_SYMBOL     ':'
#define SHORT_GRASS_SYMBOL    '.'
#define WATER_SYMBOL          '~'
#define ERROR_SYMBOL          '&'

#define PC_SYMBOL       '@'
#define HIKER_SYMBOL    'h'
#define RIVAL_SYMBOL    'r'
#define EXPLORER_SYMBOL 'e'
#define SENTRY_SYMBOL   's'
#define PACER_SYMBOL    'p'
#define WANDERER_SYMBOL 'w'


/*
                                    INSTRUCTIONS TO RUN
                  1. Run make
                  2. Execute: make run
                  3. Execute make clean
*/


// Redefines the malloc function to include an assertion to ensure memory allocation is successful.
#define malloc(size) ({          \
  void *_tmp;                    \
  assert((_tmp = malloc(size))); \
  _tmp;                          \
})

//
// Defines a new data type, path_t, which is used to represent a path.
typedef struct path {
  heap_node_t *hn; // Pointer to a heap node, used for handling the priority queue.
  uint8_t pos[2];  // The position of this path node as an array of two 8-bit unsigned integers.
  uint8_t from[2]; // The position from where this path node is reached.
  int32_t cost;    // The cost associated with this path node.
} path_t;

// Defines an enumeration called dim_t, which is used to represent dimensions (x and y).
typedef enum dim {
  dim_x, // Represents the x dimension.
  dim_y, // Represents the y dimension.
  num_dims // Represents the total number of dimensions.
} dim_t;

// Defines a new data type, pair_t, as an array of two 16-bit integers.
typedef int16_t pair_t[num_dims];

#define MAP_X              80
#define MAP_Y              21
#define MIN_TREES          10
#define MIN_BOULDERS       10
#define TREE_PROB          95
#define BOULDER_PROB       95
#define WORLD_SIZE         401

// Macro to map a pair of coordinates to the map 2D array.
#define mappair(pair) (m->map[pair[dim_y]][pair[dim_x]])
#define mapxy(x, y) (m->map[y][x])
#define heightpair(pair) (m->height[pair[dim_y]][pair[dim_x]])
#define heightxy(x, y) (m->height[y][x])

// Defines an enumeration for different terrain types.
typedef enum __attribute__ ((__packed__)) terrain_type {
  //ter_debug,
  ter_boulder,
  ter_tree,
  ter_path,
  ter_mart,
  ter_center,
  ter_grass, //TGrass
  ter_clearing, //SGrass
  ter_mountain,
  ter_forest,
  ter_water,
  ter_gate,
  num_terrain_types
} terrain_type_t;

typedef enum __attribute__ ((__packed__)) movement_type {
  move_hiker,
  move_rival,
  move_pacer,
  move_wanderer,
  move_sentry, //to make life easy
  move_explorer,
  move_pc, //to make life easy
  num_movement_types 
} movement_type_t;

/*
      PC AND NPC DECLARATION
*/
typedef enum __attribute__ ((__packed__)) character_type {
  char_pc,  //required
  char_hiker, //required
  char_rival, //required
  char_other, //required: pacer, wanderer, sentry, explorer (not one of the mandatory characters that must be generated)
  num_character_types
} character_type_t;


//PC -- not moving for simplicity
typedef struct pc {
  pair_t pos;
} pc_t;

//NPC -- all things we need to define where the NPC is and how it moves
typedef struct npc {
  character_type_t char_type;
  movement_type_t move_type;
  pair_t dir;
} npc_t;

//CHARACTER -- contains both PC and NPC structs
typedef struct character {
  npc_t *npc;
  pc_t *pc;
  pair_t pos;
  char symbol;
  int next_turn;
  int priority;
} character_t;




// Define a structure to represent a map.
typedef struct map {
  terrain_type_t map[MAP_Y][MAP_X]; // 2D array representing the type of terrain at each position.
  uint8_t height[MAP_Y][MAP_X];     // 2D array representing the height at each position.
  character_t *cmap[MAP_Y][MAP_X]; // 2D array representing the characters at each position (PC and NPC characters)

  /**
 * The `whose_move` field in the `map_t` struct represents the character whose turn it currently is. 
 * This field is a pointer to a `character_t` struct that represents the character whose turn it is. 
 * During each turn, the character at the top of the turn heap is removed from the heap and its `next_turn` 
 * value is updated to the current turn plus the character's movement speed accross the new terrain. The character is then re-inserted into 
 * the turn heap based on its new `next_turn` value. The `whose_move` field is updated to point to the 
 * character whose turn it currently is. This field is used to keep track of which character is currently 
 * taking its turn and to determine when the turn is over. 
 */

  heap_t whose_move;

  int8_t n, s, e, w;                // Variables to represent north, south, east, and west directions.
} map_t;

typedef struct queue_node {
  int x, y;
  struct queue_node *next;
} queue_node_t;

// Define a structure to represent the world.
typedef struct world {
  map_t *world[WORLD_SIZE][WORLD_SIZE]; // 2D array of pointers to maps, representing the world.
  pair_t cur_idx;                     // Current index in the world as a pair of coordinates.
  map_t *cur_map;                     // Pointer to the current map.
  int hiker_dist[MAP_Y][MAP_X];       // 2D array representing the distance to the PC for the hiker from each point on the map
  int rival_dist[MAP_Y][MAP_X];       // 2D array representing the distance to the PC for the rival from each point on the map
  character_t pc;
  int priority_number;     //              -------------------------------------------WHY??????????
} world_t;


/* Even unallocated, a WORLD_SIZE x WORLD_SIZE array of pointers is a very *
 * large thing to put on the stack.  To avoid that, world is a global.     */
world_t world;

/*
A structure to represent a pair of coordinates.
direction_table array is used to represent the eight cardinal and intercardinal directions on a 2D grid, for ease of use
*/

static const pair_t direction_table[8] = {
  { -1, -1 },
  { -1,  0 },
  { -1,  1 },
  {  0, -1 },
  {  0,  1 },
  {  1, -1 },
  {  1,  0 },
  {  1,  1 },
};



/*
                        DEFINE MOVEMENT COSTS FOR THE DIFFERENT CHARACTERS
*/
static int32_t move_costs[num_character_types][num_terrain_types] = {
  //Bldr | Tree | Path | PMart | PCntr | TGras/Grass | SGras/Clearing | Mtn | Forest | Water Gate
  //PC ∞ ∞ 10 10 10 20 10 ∞ ∞ ∞ 10
  { INT_MAX, INT_MAX, 10, 10,      10,      20, 10, INT_MAX, INT_MAX, INT_MAX, 10},  //PC COST
  //∞ ∞ 10 50 50 15 10 15 15 ∞ ∞
  { INT_MAX, INT_MAX, 10, 50, 50, 15, 10, 15, 15, INT_MAX, INT_MAX },  //HIKER COST
  //∞ ∞ 10 50 50 20 10 ∞ ∞ ∞ ∞
  { INT_MAX, INT_MAX, 10, 50, 50, 20, 10, INT_MAX, INT_MAX, INT_MAX, INT_MAX },  //RIVAL COST
  //∞ ∞ 10 50 50 20 10 ∞ ∞ ∞ ∞
  { INT_MAX, INT_MAX, 10, 50, 50, 20, 10, INT_MAX, INT_MAX, INT_MAX, INT_MAX }  //PACERS, WANDERERS, SENTRIES, EXPLORERS COST
};


/**
 *                CHOOSING A RANDOM DIRECTION FOR OUR PACERS, WANDERERS, SENTRIES, AND EXPLORERS
 * Function: chooses a random number between 0 and 7 for direction from the direction_table array
 */
#define random_dir(dir){  int choice_of_drxn = rand() % 8;  dir[0] = direction_table[choice_of_drxn][0]; dir[1] = direction_table[choice_of_drxn][1];}

void random_position_generator(pair_t pos)
{
  pos[dim_x] = (rand() % (MAP_X - 2)) + 1;
  pos[dim_y] = (rand() % (MAP_Y - 2)) + 1;
}



void calculate_distance_maps(map_t *m);

static void move_H(character_t *c, pair_t destination)
{
    int min = INT_MAX;

    destination[dim_x] = c->pos[dim_x];
    destination[dim_y] = c->pos[dim_y];
    
    for (int i = 0; i < 8; i++) {
        int new_x = c->pos[dim_x] + direction_table[i][dim_x];
        int new_y = c->pos[dim_y] + direction_table[i][dim_y];

        // Check map boundaries
        if (new_x <= 0 || new_x >= MAP_X - 1 || new_y <= 0 || new_y >= MAP_Y - 1) {
            continue;
        }

        // Check if new position is not occupied and closer to the target
        if (!world.cur_map->cmap[new_y][new_x] && world.hiker_dist[new_y][new_x] <= min) 
        {
            destination[dim_x] = new_x;
            destination[dim_y] = new_y;
            min = world.hiker_dist[new_y][new_x];
        }
    }
}

static void move_R(character_t *c, pair_t destination)
{
    int min = INT_MAX;

    destination[dim_x] = c->pos[dim_x];
    destination[dim_y] = c->pos[dim_y];
    
    for (int i = 0; i < 8; i++) {
        int new_x = c->pos[dim_x] + direction_table[i][dim_x];
        int new_y = c->pos[dim_y] + direction_table[i][dim_y];

        // Check map boundaries
        if (new_x <= 0 || new_x >= MAP_X - 1 || new_y <= 0 || new_y >= MAP_Y - 1) {
            continue;
        }

        // Check if new position is not occupied and closer to the target
        if (!world.cur_map->cmap[new_y][new_x] && world.rival_dist[new_y][new_x] < min) 
        {
            destination[dim_x] = new_x;
            destination[dim_y] = new_y;
            min = world.rival_dist[new_y][new_x];
        }
    }
}

static void move_P(character_t *c, pair_t destination)
{
  //THE DIRECTION WE DEFINE WHEN WE PLACE THE PACER
  terrain_type_t t;
  
  destination[dim_x] = c->pos[dim_x];
  destination[dim_y] = c->pos[dim_y];

  //Find next terrain type that we would be going into
  t = world.cur_map->map[c->pos[dim_y] + c->npc->dir[dim_y]][c->pos[dim_x] + c->npc->dir[dim_x]];

  //Checks the character map to make sure nothing is in the way
  if ((t != ter_path && t != ter_grass && t != ter_clearing) || world.cur_map->cmap[c->pos[dim_y] + c->npc->dir[dim_y]][c->pos[dim_x] + c->npc->dir[dim_x]]) {
    c->npc->dir[dim_x] *= -1;
    c->npc->dir[dim_y] *= -1;
  }

  if ((t == ter_path || t == ter_grass || t == ter_clearing) && !world.cur_map->cmap[c->pos[dim_y] + c->npc->dir[dim_y]][c->pos[dim_x] + c->npc->dir[dim_x]]) {
    destination[dim_x] = c->pos[dim_x] + c->npc->dir[dim_x];
    destination[dim_y] = c->pos[dim_y] + c->npc->dir[dim_y];
  }
}


static void move_W(character_t *c, pair_t destination)
{
    // Initializing destination position with current character position
    destination[dim_x] = c->pos[dim_x];
    destination[dim_y] = c->pos[dim_y];

    // Calculating the next position the character might move to
    int nextPosX = c->pos[dim_x] + c->npc->dir[dim_x];
    int nextPosY = c->pos[dim_y] + c->npc->dir[dim_y];

    // Checking if next position's terrain type is different from current or if it's occupied
    int isDifferentTerrain = world.cur_map->map[nextPosY][nextPosX] != world.cur_map->map[c->pos[dim_y]][c->pos[dim_x]];
    int isPositionOccupied = (world.cur_map->cmap[nextPosY][nextPosX] != NULL);
    
    if (isDifferentTerrain || isPositionOccupied) {
        random_dir(c->npc->dir); // Changing direction if the next terrain is different or position is occupied
    }

    // Updating next position after potential direction change
    nextPosX = c->pos[dim_x] + c->npc->dir[dim_x];
    nextPosY = c->pos[dim_y] + c->npc->dir[dim_y];

    // Checking if next position's terrain type is the same as current and not occupied
    int isSameTerrain = world.cur_map->map[nextPosY][nextPosX] == world.cur_map->map[c->pos[dim_y]][c->pos[dim_x]];
    isPositionOccupied = world.cur_map->cmap[nextPosY][nextPosX] != NULL; // Updating after potential direction change
    
    if (isSameTerrain && !isPositionOccupied) {
        destination[dim_x] = nextPosX;
        destination[dim_y] = nextPosY;
    }
}

//Well this technically does not move but it makes my job easier
static void move_S(character_t *c, pair_t destination)
{
  destination[dim_x] = c->pos[dim_x];
  destination[dim_y] = c->pos[dim_y];
}


static void move_E(character_t *c, pair_t destination)
{
    // Initialize destination position with current character position
    destination[dim_x] = c->pos[dim_x];
    destination[dim_y] = c->pos[dim_y];

    // Calculate the next potential positions to move to
    int nextPosX = c->pos[dim_x] + c->npc->dir[dim_x];
    int nextPosY = c->pos[dim_y] + c->npc->dir[dim_y];

    // Retrieve the movement cost of the terrain at the next position
    int nextPosMoveCost = move_costs[char_other][world.cur_map->map[nextPosY][nextPosX]];
    
    // Check if the position is occupied on the character map
    int isPositionOccupied = world.cur_map->cmap[nextPosY][nextPosX] != NULL;

    // Decision-making based on terrain move cost and position occupancy
    if (nextPosMoveCost == INT_MAX || isPositionOccupied) {
        random_dir(c->npc->dir); // Change direction if move cost is max or position is occupied
    }
    else {
        // Update destination if terrain is passable and position is not occupied
        destination[dim_x] = nextPosX;
        destination[dim_y] = nextPosY;
    }
}

//PC DOES NOT MOVE IN MY IMPLEMENTATION, to make my life easier
static void move_PC(character_t *c, pair_t destination)
{
  destination[dim_x] = c->pos[dim_x];
  destination[dim_y] = c->pos[dim_y];
}

/*
POINTERS TO POINT TO ALL THE MOVE FUNCTIONS
*/

void (*move_func[num_movement_types])(character_t *, pair_t) = {
  move_H,
  move_R,
  move_P,
  move_W,
  move_S,
  move_E,
  move_PC
};


void rand_pos(pair_t pos)
{
  pos[dim_x] = (rand() % (MAP_X - 2)) + 1;
  pos[dim_y] = (rand() % (MAP_Y - 2)) + 1;
}

void new_hiker()
{
  pair_t pos;
  character_t *c;

  do {
    random_position_generator(pos);
  } while (world.hiker_dist[pos[dim_y]][pos[dim_x]] == INT_MAX || world.cur_map->cmap[pos[dim_y]][pos[dim_x]]);

  c = malloc(sizeof (*c));
  c->npc = malloc(sizeof (*c->npc));

  c->pos[dim_y] = pos[dim_y];
  c->pos[dim_x] = pos[dim_x];
  c->npc->char_type = char_hiker;
  c->npc->move_type = move_hiker;
  c->npc->dir[dim_x] = 0;
  c->npc->dir[dim_y] = 0;
  c->symbol = HIKER_SYMBOL;
  c->next_turn = 0;
  c->priority = world.priority_number++;
  heap_insert(&world.cur_map->whose_move, c);
  world.cur_map->cmap[pos[dim_y]][pos[dim_x]] = c;
}

void new_rival()
{
  pair_t pos;
  character_t *c;

  do {
    random_position_generator(pos);
  } while (world.rival_dist[pos[dim_y]][pos[dim_x]] == INT_MAX || world.rival_dist[pos[dim_y]][pos[dim_x]] < 0 || world.cur_map->cmap[pos[dim_y]][pos[dim_x]]);

  c = malloc(sizeof (*c));
  c->npc = malloc(sizeof (*c->npc));

  c->pos[dim_y] = pos[dim_y];
  c->pos[dim_x] = pos[dim_x];
  c->npc->char_type = char_rival; 
  c->npc->move_type = move_rival; 
  c->npc->dir[dim_x] = 0;
  c->npc->dir[dim_y] = 0;
  c->symbol = RIVAL_SYMBOL;
  c->next_turn = 0;
  c->priority = world.priority_number++; 
  heap_insert(&world.cur_map->whose_move, c); 
  world.cur_map->cmap[pos[dim_y]][pos[dim_x]] = c;
}

void new_character_other()
{
  pair_t pos;
  character_t *c;

  do {
    random_position_generator(pos);
  } while (world.rival_dist[pos[dim_y]][pos[dim_x]] == INT_MAX || world.rival_dist[pos[dim_y]][pos[dim_x]] < 0 || world.cur_map->cmap[pos[dim_y]][pos[dim_x]]);

  c = malloc(sizeof(*c));
  c->npc = malloc(sizeof(*c->npc));
  c->pos[dim_y] = pos[dim_y];
  c->pos[dim_x] = pos[dim_x];
  c->npc->char_type = char_other;
  
  switch (rand() % 4) {
    case 0: c->npc->move_type = move_pacer;     c->symbol = PACER_SYMBOL;     break;
    case 1: c->npc->move_type = move_wanderer;  c->symbol = WANDERER_SYMBOL;  break;
    case 2: c->npc->move_type = move_sentry;    c->symbol = SENTRY_SYMBOL;    break;
    case 3: c->npc->move_type = move_explorer;  c->symbol = EXPLORER_SYMBOL;  break;
  }

  random_dir(c->npc->dir);
  c->next_turn = 0;
  c->priority = world.priority_number++;
  heap_insert(&world.cur_map->whose_move, c);
  world.cur_map->cmap[pos[dim_y]][pos[dim_x]] = c;
}

int user_num_trainers = 0;
#define MAX_TRAINERS 5

void place_characters()
{
  int num_trainers = 2;

  int what_to_use = (user_num_trainers != 0) ? user_num_trainers : MAX_TRAINERS;

  //Always place a hiker and a rival, then place a random number of others
  new_hiker();
  new_rival();
  do {
    //higher probability of non- hikers and rivals
    switch(rand() % 10) {
    case 0:
      new_hiker();
      break;
    case 1:
     new_rival();
      break;
    default:
      new_character_other();
      break;
    }
  } while (++num_trainers < what_to_use);
}

//Comparator for the heap
int32_t compare_character_turns(const void *key, const void *with)
{
  return ((((character_t *)key)->next_turn == ((character_t *)with)->next_turn) ? (((character_t *)key)->priority - ((character_t *)with)->priority) :(((character_t *)key)->next_turn - ((character_t *)with)->next_turn));
}

void delete_character(void *v)
{
  if (v == &world.pc) {
    free(world.pc.pc);
  } else {
    free(((character_t *) v)->npc);
    free(v);
  }
}

/*
              INITILAISE THE PC POSITION ON THE MAP
*/
void place_pc()
{
  int x, y;

  do {
    x = rand() % (MAP_X - 2) + 1;
    y = rand() % (MAP_Y - 2) + 1;
  } while (world.cur_map->map[y][x] != ter_path || world.cur_map->map[y][x] == ter_gate);

  world.pc.pos[dim_x] = x;
  world.pc.pos[dim_y] = y;
  world.pc.symbol = PC_SYMBOL;
  world.pc.pc = malloc(sizeof (*world.pc.pc));

  world.cur_map->cmap[y][x] = &world.pc;
  world.pc.next_turn = 0;
  world.pc.priority = world.priority_number++;

  heap_insert(&world.cur_map->whose_move, &world.pc);
}








static int32_t path_cmp(const void *key, const void *with) {
  return ((path_t *) key)->cost - ((path_t *) with)->cost;
}

static int32_t edge_penalty(int8_t x, int8_t y)
{
  return (x == 1 || y == 1 || x == MAP_X - 2 || y == MAP_Y - 2) ? 2 : 1;
}

static void dijkstra_path(map_t *m, pair_t from, pair_t to)
{
  // Declare a 2D array of path_t to represent the path.
  static path_t path[MAP_Y][MAP_X], *p;
  // Declare a variable to check if the path is initialized.
  static uint32_t initialized = 0;
  // Declare a heap variable.
  heap_t h;
  // Declare variables for coordinates.
  uint32_t x, y;

  // If the path is not initialized, initialize it.
  if (!initialized) {
    for (y = 0; y < MAP_Y; y++) {
      for (x = 0; x < MAP_X; x++) {
        path[y][x].pos[dim_y] = y;
        path[y][x].pos[dim_x] = x;
      }
    }
    initialized = 1;
  }
  
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      path[y][x].cost = INT_MAX;
    }
  }

  path[from[dim_y]][from[dim_x]].cost = 0;

  heap_init(&h, path_cmp, NULL);

  for (y = 1; y < MAP_Y - 1; y++) {
    for (x = 1; x < MAP_X - 1; x++) {
      path[y][x].hn = heap_insert(&h, &path[y][x]);
    }
  }

  while ((p = heap_remove_min(&h))) {
    p->hn = NULL;

    if ((p->pos[dim_y] == to[dim_y]) && p->pos[dim_x] == to[dim_x]) {
      for (x = to[dim_x], y = to[dim_y];
           (x != from[dim_x]) || (y != from[dim_y]);
           p = &path[y][x], x = p->from[dim_x], y = p->from[dim_y]) {
        // Don't overwrite the gate
        if (x != to[dim_x] || y != to[dim_y]) {
          mapxy(x, y) = ter_path;
          heightxy(x, y) = 0;
        }
      }
      heap_delete(&h);
      return;
    }

    if ((path[p->pos[dim_y] - 1][p->pos[dim_x]    ].hn) &&
        (path[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost >
         ((p->cost + heightpair(p->pos)) *
          edge_penalty(p->pos[dim_x], p->pos[dim_y] - 1)))) {
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost =
        ((p->cost + heightpair(p->pos)) *
         edge_penalty(p->pos[dim_x], p->pos[dim_y] - 1));
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1]
                                           [p->pos[dim_x]    ].hn);
    }
    if ((path[p->pos[dim_y]    ][p->pos[dim_x] - 1].hn) &&
        (path[p->pos[dim_y]    ][p->pos[dim_x] - 1].cost >
         ((p->cost + heightpair(p->pos)) *
          edge_penalty(p->pos[dim_x] - 1, p->pos[dim_y])))) {
      path[p->pos[dim_y]][p->pos[dim_x] - 1].cost =
        ((p->cost + heightpair(p->pos)) *
         edge_penalty(p->pos[dim_x] - 1, p->pos[dim_y]));
      path[p->pos[dim_y]    ][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y]    ][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]    ]
                                           [p->pos[dim_x] - 1].hn);
    }
    if ((path[p->pos[dim_y]    ][p->pos[dim_x] + 1].hn) &&
        (path[p->pos[dim_y]    ][p->pos[dim_x] + 1].cost >
         ((p->cost + heightpair(p->pos)) *
          edge_penalty(p->pos[dim_x] + 1, p->pos[dim_y])))) {
      path[p->pos[dim_y]][p->pos[dim_x] + 1].cost =
        ((p->cost + heightpair(p->pos)) *
         edge_penalty(p->pos[dim_x] + 1, p->pos[dim_y]));
      path[p->pos[dim_y]    ][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y]    ][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]    ]
                                           [p->pos[dim_x] + 1].hn);
    }
    if ((path[p->pos[dim_y] + 1][p->pos[dim_x]    ].hn) &&
        (path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost >
         ((p->cost + heightpair(p->pos)) *
          edge_penalty(p->pos[dim_x], p->pos[dim_y] + 1)))) {
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost =
        ((p->cost + heightpair(p->pos)) *
         edge_penalty(p->pos[dim_x], p->pos[dim_y] + 1));
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1]
                                           [p->pos[dim_x]    ].hn);
    }
  }
}

static int build_paths(map_t *m)
{
  pair_t from, to;

  /*  printf("%d %d %d %d\n", m->n, m->s, m->e, m->w);*/

  if (m->e != -1 && m->w != -1) {
    from[dim_x] = 1;
    to[dim_x] = MAP_X - 2;
    from[dim_y] = m->w;
    to[dim_y] = m->e;

    dijkstra_path(m, from, to);
  }

  if (m->n != -1 && m->s != -1) {
    from[dim_y] = 1;
    to[dim_y] = MAP_Y - 2;
    from[dim_x] = m->n;
    to[dim_x] = m->s;

    dijkstra_path(m, from, to);
  }

  if (m->e == -1) {
    if (m->s == -1) {
      from[dim_x] = 1;
      from[dim_y] = m->w;
      to[dim_x] = m->n;
      to[dim_y] = 1;
    } else {
      from[dim_x] = 1;
      from[dim_y] = m->w;
      to[dim_x] = m->s;
      to[dim_y] = MAP_Y - 2;
    }

    dijkstra_path(m, from, to);
  }

  if (m->w == -1) {
    if (m->s == -1) {
      from[dim_x] = MAP_X - 2;
      from[dim_y] = m->e;
      to[dim_x] = m->n;
      to[dim_y] = 1;
    } else {
      from[dim_x] = MAP_X - 2;
      from[dim_y] = m->e;
      to[dim_x] = m->s;
      to[dim_y] = MAP_Y - 2;
    }

    dijkstra_path(m, from, to);
  }

  if (m->n == -1) {
    if (m->e == -1) {
      from[dim_x] = 1;
      from[dim_y] = m->w;
      to[dim_x] = m->s;
      to[dim_y] = MAP_Y - 2;
    } else {
      from[dim_x] = MAP_X - 2;
      from[dim_y] = m->e;
      to[dim_x] = m->s;
      to[dim_y] = MAP_Y - 2;
    }

    dijkstra_path(m, from, to);
  }

  if (m->s == -1) {
    if (m->e == -1) {
      from[dim_x] = 1;
      from[dim_y] = m->w;
      to[dim_x] = m->n;
      to[dim_y] = 1;
    } else {
      from[dim_x] = MAP_X - 2;
      from[dim_y] = m->e;
      to[dim_x] = m->n;
      to[dim_y] = 1;
    }

    dijkstra_path(m, from, to);
  }

  return 0;
}

static int gaussian[5][5] = {
  {  1,  4,  7,  4,  1 },
  {  4, 16, 26, 16,  4 },
  {  7, 26, 41, 26,  7 },
  {  4, 16, 26, 16,  4 },
  {  1,  4,  7,  4,  1 }
};

static int smooth_height(map_t *m)
{
  int32_t i, x, y;
  int32_t s, t, p, q;
  queue_node_t *head, *tail, *tmp;
  /*  FILE *out;*/
  uint8_t height[MAP_Y][MAP_X];

  memset(&height, 0, sizeof (height));

  /* Seed with some values */
  for (i = 1; i < 255; i += 20) {
    do {
      x = rand() % MAP_X;
      y = rand() % MAP_Y;
    } while (height[y][x]);
    height[y][x] = i;
    if (i == 1) {
      head = tail = malloc(sizeof (*tail));
    } else {
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
    }
    tail->next = NULL;
    tail->x = x;
    tail->y = y;
  }

  /*
  out = fopen("seeded.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&height, sizeof (height), 1, out);
  fclose(out);
  */
  
  /* Diffuse the vaules to fill the space */
  while (head) {
    x = head->x;
    y = head->y;
    i = height[y][x];

    if (x - 1 >= 0 && y - 1 >= 0 && !height[y - 1][x - 1]) {
      height[y - 1][x - 1] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y - 1;
    }
    if (x - 1 >= 0 && !height[y][x - 1]) {
      height[y][x - 1] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y;
    }
    if (x - 1 >= 0 && y + 1 < MAP_Y && !height[y + 1][x - 1]) {
      height[y + 1][x - 1] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y + 1;
    }
    if (y - 1 >= 0 && !height[y - 1][x]) {
      height[y - 1][x] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x;
      tail->y = y - 1;
    }
    if (y + 1 < MAP_Y && !height[y + 1][x]) {
      height[y + 1][x] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x;
      tail->y = y + 1;
    }
    if (x + 1 < MAP_X && y - 1 >= 0 && !height[y - 1][x + 1]) {
      height[y - 1][x + 1] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y - 1;
    }
    if (x + 1 < MAP_X && !height[y][x + 1]) {
      height[y][x + 1] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y;
    }
    if (x + 1 < MAP_X && y + 1 < MAP_Y && !height[y + 1][x + 1]) {
      height[y + 1][x + 1] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y + 1;
    }

    tmp = head;
    head = head->next;
    free(tmp);
  }

  /* And smooth it a bit with a gaussian convolution */
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      for (s = t = p = 0; p < 5; p++) {
        for (q = 0; q < 5; q++) {
          if (y + (p - 2) >= 0 && y + (p - 2) < MAP_Y &&
              x + (q - 2) >= 0 && x + (q - 2) < MAP_X) {
            s += gaussian[p][q];
            t += height[y + (p - 2)][x + (q - 2)] * gaussian[p][q];
          }
        }
      }
      m->height[y][x] = t / s;
    }
  }
  /* Let's do it again, until it's smooth like Kenny G. */
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      for (s = t = p = 0; p < 5; p++) {
        for (q = 0; q < 5; q++) {
          if (y + (p - 2) >= 0 && y + (p - 2) < MAP_Y &&
              x + (q - 2) >= 0 && x + (q - 2) < MAP_X) {
            s += gaussian[p][q];
            t += height[y + (p - 2)][x + (q - 2)] * gaussian[p][q];
          }
        }
      }
      m->height[y][x] = t / s;
    }
  }

  /*
  out = fopen("diffused.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&height, sizeof (height), 1, out);
  fclose(out);

  out = fopen("smoothed.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&m->height, sizeof (m->height), 1, out);
  fclose(out);
  */

  return 0;
}

static void find_building_location(map_t *m, pair_t p)
{
  do {
    p[dim_x] = rand() % (MAP_X - 3) + 1;
    p[dim_y] = rand() % (MAP_Y - 3) + 1;

    if ((((mapxy(p[dim_x] - 1, p[dim_y]    ) == ter_path)     &&
          (mapxy(p[dim_x] - 1, p[dim_y] + 1) == ter_path))    ||
         ((mapxy(p[dim_x] + 2, p[dim_y]    ) == ter_path)     &&
          (mapxy(p[dim_x] + 2, p[dim_y] + 1) == ter_path))    ||
         ((mapxy(p[dim_x]    , p[dim_y] - 1) == ter_path)     &&
          (mapxy(p[dim_x] + 1, p[dim_y] - 1) == ter_path))    ||
         ((mapxy(p[dim_x]    , p[dim_y] + 2) == ter_path)     &&
          (mapxy(p[dim_x] + 1, p[dim_y] + 2) == ter_path)))   &&
        (((mapxy(p[dim_x]    , p[dim_y]    ) != ter_mart)     &&
          (mapxy(p[dim_x]    , p[dim_y]    ) != ter_center)   &&
          (mapxy(p[dim_x] + 1, p[dim_y]    ) != ter_mart)     &&
          (mapxy(p[dim_x] + 1, p[dim_y]    ) != ter_center)   &&
          (mapxy(p[dim_x]    , p[dim_y] + 1) != ter_mart)     &&
          (mapxy(p[dim_x]    , p[dim_y] + 1) != ter_center)   &&
          (mapxy(p[dim_x] + 1, p[dim_y] + 1) != ter_mart)     &&
          (mapxy(p[dim_x] + 1, p[dim_y] + 1) != ter_center))) &&
        (((mapxy(p[dim_x]    , p[dim_y]    ) != ter_path)     &&
          (mapxy(p[dim_x] + 1, p[dim_y]    ) != ter_path)     &&
          (mapxy(p[dim_x]    , p[dim_y] + 1) != ter_path)     &&
          (mapxy(p[dim_x] + 1, p[dim_y] + 1) != ter_path)))) {
          break;
    }
  } while (1);
}

static int place_pokemart(map_t *m)
{
  pair_t p;

  find_building_location(m, p);

  mapxy(p[dim_x]    , p[dim_y]    ) = ter_mart;
  mapxy(p[dim_x] + 1, p[dim_y]    ) = ter_mart;
  mapxy(p[dim_x]    , p[dim_y] + 1) = ter_mart;
  mapxy(p[dim_x] + 1, p[dim_y] + 1) = ter_mart;

  return 0;
}

static int place_center(map_t *m)
{  pair_t p;

  find_building_location(m, p);

  mapxy(p[dim_x]    , p[dim_y]    ) = ter_center;
  mapxy(p[dim_x] + 1, p[dim_y]    ) = ter_center;
  mapxy(p[dim_x]    , p[dim_y] + 1) = ter_center;
  mapxy(p[dim_x] + 1, p[dim_y] + 1) = ter_center;

  return 0;
}

/* Chooses tree or boulder for border cell.  Choice is biased by dominance *
 * of neighboring cells.                                                   */
static terrain_type_t border_type(map_t *m, int32_t x, int32_t y)
{
  int32_t p, q;
  int32_t r, t;
  int32_t miny, minx, maxy, maxx;
  
  r = t = 0;
  
  miny = y - 1 >= 0 ? y - 1 : 0;
  maxy = y + 1 <= MAP_Y ? y + 1: MAP_Y;
  minx = x - 1 >= 0 ? x - 1 : 0;
  maxx = x + 1 <= MAP_X ? x + 1: MAP_X;

  for (q = miny; q < maxy; q++) {
    for (p = minx; p < maxx; p++) {
      if (q != y || p != x) {
        if (m->map[q][p] == ter_mountain ||
            m->map[q][p] == ter_boulder) {
          r++;
        } else if (m->map[q][p] == ter_forest ||
                   m->map[q][p] == ter_tree) {
          t++;
        }
      }
    }
  }
  
  if (t == r) {
    return rand() & 1 ? ter_boulder : ter_tree;
  } else if (t > r) {
    if (rand() % 10) {
      return ter_tree;
    } else {
      return ter_boulder;
    }
  } else {
    if (rand() % 10) {
      return ter_boulder;
    } else {
      return ter_tree;
    }
  }
}

static int map_terrain(map_t *m, int8_t n, int8_t s, int8_t e, int8_t w)
{
  int32_t i, x, y;
  queue_node_t *head, *tail, *tmp;
  //  FILE *out;
  int num_grass, num_clearing, num_mountain, num_forest, num_water, num_total;
  terrain_type_t type;
  int added_current = 0;
  
  num_grass = rand() % 4 + 2;
  num_clearing = rand() % 4 + 2;
  num_mountain = rand() % 2 + 1;
  num_forest = rand() % 2 + 1;
  num_water = rand() % 2 + 1;
  num_total = num_grass + num_clearing + num_mountain + num_forest + num_water;

  memset(&m->map, 0, sizeof (m->map));

  /* Seed with some values */
  for (i = 0; i < num_total; i++) {
    do {
      x = rand() % MAP_X;
      y = rand() % MAP_Y;
    } while (m->map[y][x]);
    if (i == 0) {
      type = ter_grass;
    } else if (i == num_grass) {
      type = ter_clearing;
    } else if (i == num_grass + num_clearing) {
      type = ter_mountain;
    } else if (i == num_grass + num_clearing + num_mountain) {
      type = ter_forest;
    } else if (i == num_grass + num_clearing + num_mountain + num_forest) {
      type = ter_water;
    }
    m->map[y][x] = type;
    if (i == 0) {
      head = tail = malloc(sizeof (*tail));
    } else {
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
    }
    tail->next = NULL;
    tail->x = x;
    tail->y = y;
  }

  /*
  out = fopen("seeded.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&m->map, sizeof (m->map), 1, out);
  fclose(out);
  */

  /* Diffuse the vaules to fill the space */
  while (head) {
    x = head->x;
    y = head->y;
    i = m->map[y][x];
    
    if (x - 1 >= 0 && !m->map[y][x - 1]) {
      if ((rand() % 100) < 80) {
        m->map[y][x - 1] = i;
        tail->next = malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x - 1;
        tail->y = y;
      } else if (!added_current) {
        added_current = 1;
        m->map[y][x] = i;
        tail->next = malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    if (y - 1 >= 0 && !m->map[y - 1][x]) {
      if ((rand() % 100) < 20) {
        m->map[y - 1][x] = i;
        tail->next = malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y - 1;
      } else if (!added_current) {
        added_current = 1;
        m->map[y][x] = i;
        tail->next = malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    if (y + 1 < MAP_Y && !m->map[y + 1][x]) {
      if ((rand() % 100) < 20) {
        m->map[y + 1][x] = i;
        tail->next = malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y + 1;
      } else if (!added_current) {
        added_current = 1;
        m->map[y][x] = i;
        tail->next = malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    if (x + 1 < MAP_X && !m->map[y][x + 1]) {
      if ((rand() % 100) < 80) {
        m->map[y][x + 1] = i;
        tail->next = malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x + 1;
        tail->y = y;
      } else if (!added_current) {
        added_current = 1;
        m->map[y][x] = i;
        tail->next = malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    added_current = 0;
    tmp = head;
    head = head->next;
    free(tmp);
  }

  /*
  out = fopen("diffused.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&m->map, sizeof (m->map), 1, out);
  fclose(out);
  */
  
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      if (y == 0 || y == MAP_Y - 1 ||
          x == 0 || x == MAP_X - 1) {
        mapxy(x, y) = border_type(m, x, y);
      }
    }
  }

  m->n = n;
  m->s = s;
  m->e = e;
  m->w = w;

  if (n != -1) {
    mapxy(n,         0        ) = ter_gate;
    mapxy(n,         1        ) = ter_gate;
  }
  if (s != -1) {
    mapxy(s,         MAP_Y - 1) = ter_gate;
    mapxy(s,         MAP_Y - 2) = ter_gate;
  }
  if (w != -1) {
    mapxy(0,         w        ) = ter_gate;
    mapxy(1,         w        ) = ter_gate;
  }
  if (e != -1) {
    mapxy(MAP_X - 1, e        ) = ter_gate;
    mapxy(MAP_X - 2, e        ) = ter_gate;
  }

  return 0;
}

static int place_boulders(map_t *m)
{
  int i;
  int x, y;

  for (i = 0; i < MIN_BOULDERS || rand() % 100 < BOULDER_PROB; i++) {
    y = rand() % (MAP_Y - 2) + 1;
    x = rand() % (MAP_X - 2) + 1;
    if (m->map[y][x] != ter_forest &&
        m->map[y][x] != ter_path   &&
        m->map[y][x] != ter_gate) {
      m->map[y][x] = ter_boulder;
    }
  }

  return 0;
}

static int place_trees(map_t *m)
{
  int i;
  int x, y;
  
  for (i = 0; i < MIN_TREES || rand() % 100 < TREE_PROB; i++) {
    y = rand() % (MAP_Y - 2) + 1;
    x = rand() % (MAP_X - 2) + 1;
    if (m->map[y][x] != ter_mountain &&
        m->map[y][x] != ter_path     &&
        m->map[y][x] != ter_water    &&
        m->map[y][x] != ter_gate) {
      m->map[y][x] = ter_tree;
    }
  }

  return 0;
}

// New map expects cur_idx to refer to the index to be generated.  If that
// map has already been generated then the only thing this does is set
// cur_map.

//NEW CODE ------------------------------------------------NEW CODE ADDED
static int new_map()
{
  int d, p;
  int e, w, n, s;

  //NEW DECLARATIONS
  int x, y;

  if (world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x]]) {
    world.cur_map = world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x]];
    return 0;
  }

  world.cur_map                                             =
    world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x]] =
    malloc(sizeof (*world.cur_map));

  smooth_height(world.cur_map);
  
  if (!world.cur_idx[dim_y]) {
    n = -1;
  } else if (world.world[world.cur_idx[dim_y] - 1][world.cur_idx[dim_x]]) {
    n = world.world[world.cur_idx[dim_y] - 1][world.cur_idx[dim_x]]->s;
  } else {
    n = 1 + rand() % (MAP_X - 2);
  }
  if (world.cur_idx[dim_y] == WORLD_SIZE - 1) {
    s = -1;
  } else if (world.world[world.cur_idx[dim_y] + 1][world.cur_idx[dim_x]]) {
    s = world.world[world.cur_idx[dim_y] + 1][world.cur_idx[dim_x]]->n;
  } else  {
    s = 1 + rand() % (MAP_X - 2);
  }
  if (!world.cur_idx[dim_x]) {
    w = -1;
  } else if (world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x] - 1]) {
    w = world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x] - 1]->e;
  } else {
    w = 1 + rand() % (MAP_Y - 2);
  }
  if (world.cur_idx[dim_x] == WORLD_SIZE - 1) {
    e = -1;
  } else if (world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x] + 1]) {
    e = world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x] + 1]->w;
  } else {
    e = 1 + rand() % (MAP_Y - 2);
  }
  
  map_terrain(world.cur_map, n, s, e, w);
     
  place_boulders(world.cur_map);
  place_trees(world.cur_map);
  build_paths(world.cur_map);
  d = (abs(world.cur_idx[dim_x] - (WORLD_SIZE / 2)) +
       abs(world.cur_idx[dim_y] - (WORLD_SIZE / 2)));
  p = d > 200 ? 5 : (50 - ((45 * d) / 200));
  //  printf("d=%d, p=%d\n", d, p);
  if ((rand() % 100) < p || !d) {
    place_pokemart(world.cur_map);
  }
  if ((rand() % 100) < p || !d) {
    place_center(world.cur_map);
  }



  /**
   * NEW CODE
  */
 for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      world.cur_map->cmap[y][x] = NULL;
    }
  }

  heap_init(&world.cur_map->whose_move, compare_character_turns, delete_character);

  place_pc();
  calculate_distance_maps(world.cur_map);
  place_characters();
  return 0;
}


//NEW CODE ------------------------------------------------NEW CODE ADDED: to handle printing of PC and NPC's
static void print_map()
{
    int x, y;
    int default_reached = 0;

    printf("\n\n\n");

    for (y = 0; y < MAP_Y; y++) {
        for (x = 0; x < MAP_X; x++) {
          if (world.cur_map->cmap[y][x]) {
        putchar(world.cur_map->cmap[y][x]->symbol);
      }else{
            switch (world.cur_map->map[y][x]) {
                case ter_boulder:
                    putchar(BOULDER_SYMBOL);
                    break;
                case ter_mountain:
                    printf(ANSI_YELLOW);
                    putchar(MOUNTAIN_SYMBOL);
                    printf(ANSI_RESET);
                    break;
                case ter_tree:
                    putchar(TREE_SYMBOL);
                    break;
                case ter_forest:
                    printf(ANSI_ORANGE);
                    putchar(FOREST_SYMBOL);
                    printf(ANSI_RESET);
                    break;
                case ter_path:
                    putchar(PATH_SYMBOL);
                    break;
                case ter_gate:
                    putchar(GATE_SYMBOL); // No color defined
                    break;
                case ter_mart:
                    printf(ANSI_PINK);
                    putchar(POKEMART_SYMBOL);
                    printf(ANSI_RESET);
                    break;
                case ter_center:
                    printf(ANSI_PINK);
                    putchar(POKEMON_CENTER_SYMBOL);
                    printf(ANSI_RESET);
                    break;
                case ter_grass:
                    printf(ANSI_GREEN);
                    putchar(TALL_GRASS_SYMBOL);
                    printf(ANSI_RESET);
                    break;
                case ter_clearing:
                    printf(ANSI_RED);
                    putchar(SHORT_GRASS_SYMBOL);
                    printf(ANSI_RESET);
                    break;
                case ter_water:
                    printf(ANSI_BLUE);
                    putchar(WATER_SYMBOL);
                    printf(ANSI_RESET);
                    break;
                default:
                    default_reached = 1;
                    break;
            }
        }
        }
        putchar('\n');
    }

    if (default_reached) {
        fprintf(stderr, "Default reached in %s\n", __FUNCTION__);
    }
}


// The world is global because of its size, so init_world is parameterless
void init_world()
{
  world.cur_idx[dim_x] = world.cur_idx[dim_y] = WORLD_SIZE / 2;
  world.priority_number = 0;
  new_map();
}

void delete_world()
{
  int x, y;

  for (y = 0; y < WORLD_SIZE; y++) {
    for (x = 0; x < WORLD_SIZE; x++) {
      if (world.world[y][x]) {
        free(world.world[y][x]);
        world.world[y][x] = NULL;
      }
    }
  }
  printf("World deleted\n");
}

/**
 *                                                                          GAME LOOP
*/

void handle_pc_turn(character_t* c) {
    print_map();
    usleep(250000);
    c->next_turn += move_costs[char_pc][world.cur_map->map[c->pos[dim_y]]
                                                         [c->pos[dim_x]]];
}

void handle_npc_turn(character_t* c, pair_t d) {
    move_func[c->npc->move_type](c, d);
    world.cur_map->cmap[c->pos[dim_y]][c->pos[dim_x]] = NULL;
    world.cur_map->cmap[d[dim_y]][d[dim_x]] = c;
    c->next_turn += move_costs[c->npc->char_type][world.cur_map->map[d[dim_y]]
                                                               [d[dim_x]]];
    c->pos[dim_y] = d[dim_y];
    c->pos[dim_x] = d[dim_x];
}

void game_loop()
{
    character_t* c;
    pair_t d;
    
    while (1) {
        c = heap_remove_min(&world.cur_map->whose_move);

        if (c == &world.pc) {
            handle_pc_turn(c); //map is printed here
        } else {
            handle_npc_turn(c, d);
        }
        
        heap_insert(&world.cur_map->whose_move, c);
    }
}




/*
            NEW CODE
*/



//MACRO FOR EASE OF USE WHEN NEEDING TO LOOK UP MOVE COSTS
//HOW IT WORKS: move_cost_lookup(x, y, c) is replaced with move_costs[c][m->map[y][x]]
#define move_cost_lookup(x, y, c) move_costs[c][m->map[y][x]]

/*
            HIKER COMPARATOR FOR P-QUEUE
*/

static int get_hiker_distance(const path_t *path) {
    int x = path->pos[dim_x];
    int y = path->pos[dim_y];
    return world.hiker_dist[y][x];
}

static int32_t hiker_cmp(const void *a, const void *b) {
    const path_t *path_a = (path_t *)a;
    const path_t *path_b = (path_t *)b;
    
    int a_dist = get_hiker_distance(path_a);
    int b_dist = get_hiker_distance(path_b);
    
    return a_dist - b_dist;
}

/*
            RIVAL COMPARATOR FOR P-QUEUE
*/

static int get_rival_distance(const path_t *path) {
    int x = path->pos[dim_x];
    int y = path->pos[dim_y];
    return world.rival_dist[y][x];
}

static int32_t rival_cmp(const void *a, const void *b) {
    const path_t *path_a = (path_t *)a;
    const path_t *path_b = (path_t *)b;
    
    int a_dist = get_rival_distance(path_a);
    int b_dist = get_rival_distance(path_b);
    
    return a_dist - b_dist;
}

//Function to find the path between obejcts on the map. 
void calculate_distance_maps(map_t *m)
{
  heap_t h;
  uint32_t x, y;
  static path_t p[MAP_Y][MAP_X], *c;
  static uint32_t initialized = 0;

  //Initialize the path_t array
  if (!initialized) {
    initialized = 1;
    for (y = 0; y < MAP_Y; y++) {
      for (x = 0; x < MAP_X; x++) {
        p[y][x].pos[dim_y] = y;
        p[y][x].pos[dim_x] = x;
      }
    }
  }

  //Initialize the distance arrays
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      world.hiker_dist[y][x] = INT_MAX;
      world.rival_dist[y][x] = INT_MAX;
    }
  }

  //Set the cost at the PC's position to 0, as it radially (in some sense) expands outward
  world.hiker_dist[world.pc.pos[dim_y]][world.pc.pos[dim_x]] = 0;
  world.rival_dist[world.pc.pos[dim_y]][world.pc.pos[dim_x]] = 0;

  //Initialize the heap with the HIKER COMPRATOR
  heap_init(&h, hiker_cmp, NULL);

  for (y = 1; y < MAP_Y - 1; y++) {
    for (x = 1; x < MAP_X - 1; x++) {
      if (move_cost_lookup(x, y, char_hiker) != INT_MAX) {
        p[y][x].hn = heap_insert(&h, &p[y][x]);
      } else {
        p[y][x].hn = NULL;
      }
    }
  }

  while ((c = heap_remove_min(&h)))
  {
    c->hn = NULL;
    if ((p[c->pos[dim_y] - 1][c->pos[dim_x] - 1].hn) &&
        (world.hiker_dist[c->pos[dim_y] - 1][c->pos[dim_x] - 1] >
         world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
             move_cost_lookup(c->pos[dim_x], c->pos[dim_y], char_hiker)))
    {
      world.hiker_dist[c->pos[dim_y] - 1][c->pos[dim_x] - 1] =
          world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
          move_cost_lookup(c->pos[dim_x], c->pos[dim_y], char_hiker);
      heap_decrease_key_no_replace(&h,p[c->pos[dim_y] - 1][c->pos[dim_x] - 1].hn);
    }
    if ((p[c->pos[dim_y] - 1][c->pos[dim_x]].hn) &&
        (world.hiker_dist[c->pos[dim_y] - 1][c->pos[dim_x]] >
         world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
             move_cost_lookup(c->pos[dim_x], c->pos[dim_y], char_hiker)))
    {
      world.hiker_dist[c->pos[dim_y] - 1][c->pos[dim_x]] =
          world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
          move_cost_lookup(c->pos[dim_x], c->pos[dim_y], char_hiker);
      heap_decrease_key_no_replace(&h,p[c->pos[dim_y] - 1][c->pos[dim_x]].hn);
    }
    if ((p[c->pos[dim_y] - 1][c->pos[dim_x] + 1].hn) &&
        (world.hiker_dist[c->pos[dim_y] - 1][c->pos[dim_x] + 1] >
         world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
             move_cost_lookup(c->pos[dim_x], c->pos[dim_y], char_hiker)))
    {
      world.hiker_dist[c->pos[dim_y] - 1][c->pos[dim_x] + 1] =
          world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
          move_cost_lookup(c->pos[dim_x], c->pos[dim_y], char_hiker);
      heap_decrease_key_no_replace(&h,p[c->pos[dim_y] - 1][c->pos[dim_x] + 1].hn);
    }
    if ((p[c->pos[dim_y]][c->pos[dim_x] - 1].hn) &&
        (world.hiker_dist[c->pos[dim_y]][c->pos[dim_x] - 1] >
         world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
             move_cost_lookup(c->pos[dim_x], c->pos[dim_y], char_hiker)))
    {
      world.hiker_dist[c->pos[dim_y]][c->pos[dim_x] - 1] =
          world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
          move_cost_lookup(c->pos[dim_x], c->pos[dim_y], char_hiker);
      heap_decrease_key_no_replace(&h,p[c->pos[dim_y]][c->pos[dim_x] - 1].hn);
    }
    if ((p[c->pos[dim_y]][c->pos[dim_x] + 1].hn) &&
        (world.hiker_dist[c->pos[dim_y]][c->pos[dim_x] + 1] >
         world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
             move_cost_lookup(c->pos[dim_x], c->pos[dim_y], char_hiker)))
    {
      world.hiker_dist[c->pos[dim_y]][c->pos[dim_x] + 1] =
          world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
          move_cost_lookup(c->pos[dim_x], c->pos[dim_y], char_hiker);
      heap_decrease_key_no_replace(&h,p[c->pos[dim_y]][c->pos[dim_x] + 1].hn);
    }
    if ((p[c->pos[dim_y] + 1][c->pos[dim_x] - 1].hn) &&
        (world.hiker_dist[c->pos[dim_y] + 1][c->pos[dim_x] - 1] >
         world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
             move_cost_lookup(c->pos[dim_x], c->pos[dim_y], char_hiker)))
    {
      world.hiker_dist[c->pos[dim_y] + 1][c->pos[dim_x] - 1] =
          world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
          move_cost_lookup(c->pos[dim_x], c->pos[dim_y], char_hiker);
      heap_decrease_key_no_replace(&h,p[c->pos[dim_y] + 1][c->pos[dim_x] - 1].hn);
    }
    if ((p[c->pos[dim_y] + 1][c->pos[dim_x]].hn) &&
        (world.hiker_dist[c->pos[dim_y] + 1][c->pos[dim_x]] >
         world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
             move_cost_lookup(c->pos[dim_x], c->pos[dim_y], char_hiker)))
    {
      world.hiker_dist[c->pos[dim_y] + 1][c->pos[dim_x]] =
          world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
          move_cost_lookup(c->pos[dim_x], c->pos[dim_y], char_hiker);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] + 1][c->pos[dim_x]].hn);
    }
    if ((p[c->pos[dim_y] + 1][c->pos[dim_x] + 1].hn) &&
        (world.hiker_dist[c->pos[dim_y] + 1][c->pos[dim_x] + 1] >
         world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
             move_cost_lookup(c->pos[dim_x], c->pos[dim_y], char_hiker)))
    {
      world.hiker_dist[c->pos[dim_y] + 1][c->pos[dim_x] + 1] =
          world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
          move_cost_lookup(c->pos[dim_x], c->pos[dim_y], char_hiker);
      heap_decrease_key_no_replace(&h,p[c->pos[dim_y] + 1][c->pos[dim_x] + 1].hn);
    }
  }
  heap_delete(&h);

  //Initialize the heap with the RIVAL COMPARATOR

  heap_init(&h, rival_cmp, NULL);

  for (y = 1; y < MAP_Y - 1; y++) {
    for (x = 1; x < MAP_X - 1; x++) {
      if (move_cost_lookup(x, y, char_rival) != INT_MAX) {
        p[y][x].hn = heap_insert(&h, &p[y][x]);
      } else {
        p[y][x].hn = NULL;
      }
    }
  }

  while ((c = heap_remove_min(&h)))
  {
    c->hn = NULL;
    if ((p[c->pos[dim_y] - 1][c->pos[dim_x] - 1].hn) &&
        (world.rival_dist[c->pos[dim_y] - 1][c->pos[dim_x] - 1] >
         world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
             move_cost_lookup(c->pos[dim_x], c->pos[dim_y], char_rival)))
    {
      world.rival_dist[c->pos[dim_y] - 1][c->pos[dim_x] - 1] =
          world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
          move_cost_lookup(c->pos[dim_x], c->pos[dim_y], char_rival);
      heap_decrease_key_no_replace(&h,p[c->pos[dim_y] - 1][c->pos[dim_x] - 1].hn);
    }
    if ((p[c->pos[dim_y] - 1][c->pos[dim_x]].hn) &&
        (world.rival_dist[c->pos[dim_y] - 1][c->pos[dim_x]] >
         world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
             move_cost_lookup(c->pos[dim_x], c->pos[dim_y], char_rival)))
    {
      world.rival_dist[c->pos[dim_y] - 1][c->pos[dim_x]] =
          world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
          move_cost_lookup(c->pos[dim_x], c->pos[dim_y], char_rival);
      heap_decrease_key_no_replace(&h,p[c->pos[dim_y] - 1][c->pos[dim_x]].hn);
    }
    if ((p[c->pos[dim_y] - 1][c->pos[dim_x] + 1].hn) &&
        (world.rival_dist[c->pos[dim_y] - 1][c->pos[dim_x] + 1] >
         world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
             move_cost_lookup(c->pos[dim_x], c->pos[dim_y], char_rival)))
    {
      world.rival_dist[c->pos[dim_y] - 1][c->pos[dim_x] + 1] =
          world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
          move_cost_lookup(c->pos[dim_x], c->pos[dim_y], char_rival);
      heap_decrease_key_no_replace(&h,p[c->pos[dim_y] - 1][c->pos[dim_x] + 1].hn);
    }
    if ((p[c->pos[dim_y]][c->pos[dim_x] - 1].hn) &&
        (world.rival_dist[c->pos[dim_y]][c->pos[dim_x] - 1] >
         world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
             move_cost_lookup(c->pos[dim_x], c->pos[dim_y], char_rival)))
    {
      world.rival_dist[c->pos[dim_y]][c->pos[dim_x] - 1] =
          world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
          move_cost_lookup(c->pos[dim_x], c->pos[dim_y], char_rival);
      heap_decrease_key_no_replace(&h,p[c->pos[dim_y]][c->pos[dim_x] - 1].hn);
    }
    if ((p[c->pos[dim_y]][c->pos[dim_x] + 1].hn) &&
        (world.rival_dist[c->pos[dim_y]][c->pos[dim_x] + 1] >
         world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
             move_cost_lookup(c->pos[dim_x], c->pos[dim_y], char_rival)))
    {
      world.rival_dist[c->pos[dim_y]][c->pos[dim_x] + 1] =
          world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
          move_cost_lookup(c->pos[dim_x], c->pos[dim_y], char_rival);
      heap_decrease_key_no_replace(&h,p[c->pos[dim_y]][c->pos[dim_x] + 1].hn);
    }
    if ((p[c->pos[dim_y] + 1][c->pos[dim_x] - 1].hn) &&
        (world.rival_dist[c->pos[dim_y] + 1][c->pos[dim_x] - 1] >
         world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
             move_cost_lookup(c->pos[dim_x], c->pos[dim_y], char_rival)))
    {
      world.rival_dist[c->pos[dim_y] + 1][c->pos[dim_x] - 1] =
          world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
          move_cost_lookup(c->pos[dim_x], c->pos[dim_y], char_rival);
      heap_decrease_key_no_replace(&h,p[c->pos[dim_y] + 1][c->pos[dim_x] - 1].hn);
    }
    if ((p[c->pos[dim_y] + 1][c->pos[dim_x]].hn) &&
        (world.rival_dist[c->pos[dim_y] + 1][c->pos[dim_x]] >
         world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
             move_cost_lookup(c->pos[dim_x], c->pos[dim_y], char_rival)))
    {
      world.rival_dist[c->pos[dim_y] + 1][c->pos[dim_x]] =
          world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
          move_cost_lookup(c->pos[dim_x], c->pos[dim_y], char_rival);
      heap_decrease_key_no_replace(&h,p[c->pos[dim_y] + 1][c->pos[dim_x]].hn);
    }
    if ((p[c->pos[dim_y] + 1][c->pos[dim_x] + 1].hn) &&
        (world.rival_dist[c->pos[dim_y] + 1][c->pos[dim_x] + 1] >
         world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
             move_cost_lookup(c->pos[dim_x], c->pos[dim_y], char_rival)))
    {
      world.rival_dist[c->pos[dim_y] + 1][c->pos[dim_x] + 1] =
          world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
          move_cost_lookup(c->pos[dim_x], c->pos[dim_y], char_rival);
      heap_decrease_key_no_replace(&h,p[c->pos[dim_y] + 1][c->pos[dim_x] + 1].hn);
    }
  }
  heap_delete(&h);
}



/*
                PRINT DISTANCE MAPS OF HIKER AND RIVAL
*/
// //Function to print the distance map of the hiker
// void print_hiker_dist_map()
// {
//     int x;
//     int y;
//     int pc_x = world.pc.pos[dim_x];
//     int pc_y = world.pc.pos[dim_y];
//     for (y = 0; y < MAP_Y; y++)
//     {
//         for (x = 0; x < MAP_X; x++){
//           if (world.hiker_dist[y][x] == INT_MAX || world.hiker_dist[y][x] < 0){
//             printf("   "); // basically infinity or negative so cant move out or in of here
//           }
//           else
//           {
//             if (x == pc_x && y == pc_y) {
//                 printf(ANSI_PINK " %02d" ANSI_RESET, world.hiker_dist[y][x] % 100); // print in pink
//             } else {
//                 printf(" %02d", world.hiker_dist[y][x] % 100);
//             }
//           }
//         }
//         printf("\n");
//     }
// }

// //Function to print the distance map of the rival
// void print_rival_dist_map()
// {
//     int x;
//     int y;
//     int pc_x = world.pc.pos[dim_x];
//     int pc_y = world.pc.pos[dim_y];
//     for (y = 0; y < MAP_Y; y++)
//     {
//         for (x = 0; x < MAP_X; x++){
//           if (world.rival_dist[y][x] == INT_MAX || world.rival_dist[y][x] < 0){
//             printf("   "); // basically infinity or negative so cant move out or in of here
//           }
//           else{
//             if (x == pc_x && y == pc_y) {
//                 printf(ANSI_PINK " %02d" ANSI_RESET, world.rival_dist[y][x] % 100); // print in pink
//             } else {
//                 printf(" %02d", world.rival_dist[y][x] % 100);
//             }
//           }
//         }
//         printf("\n");
//     }
// }

int main(int argc, char *argv[])
{
  //struct timeval tv;
  //uint32_t seed;
  srand(time(NULL));
  //char c;
  //int x, y;

  // if (argc == 2) {
  //   seed = atoi(argv[1]);
  // } else {
  //   gettimeofday(&tv, NULL);
  //   seed = (tv.tv_usec ^ (tv.tv_sec << 20)) & 0xffffffff;
  // }


  // Loop through command-line arguments
    for(int i = 0; i < argc; i++){
        // If we find the --numtrainers switch and there is an argument after it
        if(strcmp(argv[i], "--numtrainers") == 0 && i + 1 < argc){
            // Convert the next argument to an integer and store it in num_trainers
            int temp = atoi(argv[i+1]);

            if(temp < 2){
              user_num_trainers = 2;
            }else{
              user_num_trainers = temp;
            }
            // Skip next argument since we've processed it
            i++;
        }
    }
  

  init_world();
  game_loop();
  // printf("Using seed: %u\n", seed);
  
  // place_pc();
  // calculate_distance_maps(world.cur_map);

  // print_map();
  // printf("HIKER MAP: \n");
  // print_hiker_dist_map();
  // printf("RIVAL MAP: \n");
  // print_rival_dist_map();

  // do
  // {
  //   // place_pc();

  //   print_map();

  //   // calculate_distance_maps(world.cur_map);
  //   // printf("HIKER MAP: \n");
  //   // print_hiker_dist_map();
  //   // printf("RIVAL MAP: \n");
  //   // print_rival_dist_map();
  //   printf("Current position is %d%cx%d%c (%d,%d).  "
  //          "Enter command: ",
  //          abs(world.cur_idx[dim_x] - (WORLD_SIZE / 2)),
  //          world.cur_idx[dim_x] - (WORLD_SIZE / 2) >= 0 ? 'E' : 'W',
  //          abs(world.cur_idx[dim_y] - (WORLD_SIZE / 2)),
  //          world.cur_idx[dim_y] - (WORLD_SIZE / 2) <= 0 ? 'N' : 'S',
  //          world.cur_idx[dim_x] - (WORLD_SIZE / 2),
  //          world.cur_idx[dim_y] - (WORLD_SIZE / 2));
  //   if (scanf(" %c", &c) != 1)
  //   {
  //         /* To handle EOF */
  //         putchar('\n');
  //         break;
  //   }
  //   switch (c)
  //   {
  //   case 'n':
  //         if (world.cur_idx[dim_y])
  //         {
  //           world.cur_idx[dim_y]--;
  //           new_map();
  //         }
  //         break;
  //   case 's':
  //         if (world.cur_idx[dim_y] < WORLD_SIZE - 1)
  //         {
  //           world.cur_idx[dim_y]++;
  //           new_map();
  //         }
  //         break;
  //   case 'e':
  //         if (world.cur_idx[dim_x] < WORLD_SIZE - 1)
  //         {
  //           world.cur_idx[dim_x]++;
  //           new_map();
  //         }
  //         break;
  //   case 'w':
  //         if (world.cur_idx[dim_x])
  //         {
  //           world.cur_idx[dim_x]--;
  //           new_map();
  //         }
  //         break;
  //   case 'q':
  //         break;
  //   case 'f':
  //         scanf(" %d %d", &x, &y);
  //         if (x >= -(WORLD_SIZE / 2) && x <= WORLD_SIZE / 2 &&
  //             y >= -(WORLD_SIZE / 2) && y <= WORLD_SIZE / 2)
  //         {
  //           world.cur_idx[dim_x] = x + (WORLD_SIZE / 2);
  //           world.cur_idx[dim_y] = y + (WORLD_SIZE / 2);
  //           new_map();
  //         }
  //         break;
  //   case '?':
  //   case 'h':
  //         printf("Move with 'e'ast, 'w'est, 'n'orth, 's'outh or 'f'ly x y.\n"
  //                "Quit with 'q'.  '?' and 'h' print this help message.\n");
  //         break;
  //   default:
  //         fprintf(stderr, "%c: Invalid input.  Enter '?' for help.\n", c);
  //         break;
  //   }
  // } while (c != 'q');

  delete_world();

  printf("But how are you going to be the very best if you quit?\n");
  
  return 0;
}
