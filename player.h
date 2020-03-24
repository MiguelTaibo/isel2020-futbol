#include "fsm.h"
#include <stdlib.h>

struct player_t;
typedef struct player_t player_t;
struct player_t {
 fsm_t fsm;
 int id;
 int pos_x;
 int pos_y;
 int flags_key;
};

void player_init (player_t* p, int id);
player_t* fsm_new_player (int id);

#define FLAG_UP 0x01
#define FLAG_DOWN 0x02
#define FLAG_LEFT 0x04
#define FLAG_RIGHT 0x08

#define MIN_Y  0
#define MAX_Y  24
#define MIN_X  0
#define MAX_X  79

#define UMBRAL 5
