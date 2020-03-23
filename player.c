#include "player.h"

#include <stddef.h>

static int key_up = 0;
static int key_down = 0;
static int key_left = 0;
static int key_right = 0;

#define MIN_Y  0
#define MAX_Y  20
#define MIN_X  0
#define MAX_X  20

struct player_t {
 fsm_t fsm;
 int id;
 int pos_x;
 int pos_y;
};

static int key_up_not_top (fsm_t* this) {
 player_t* p = (player_t*) this;
 return (p->pos_y > MIN_Y) && key_up;
}

static int key_down_not_bottom (fsm_t* this) {
 player_t* p = (player_t*) this;
 return (p->pos_y < MAX_Y) && key_down;
}

static int key_left_not_left (fsm_t* this) {
 player_t* p = (player_t*) this;
 return (p->pos_x > MIN_X) && key_left;
}

static int key_right_not_right (fsm_t* this) {
 player_t* p = (player_t*) this;
 return (p->pos_x < MAX_X) && key_right;
}

static void move_up (fsm_t* this) {
 player_t* p = (player_t*) this;
 p->pos_y --;
}

static void move_down (fsm_t* this) {
 player_t* p = (player_t*) this;
 p->pos_y ++;
}

static void move_left (fsm_t* this) {
 player_t* p = (player_t*) this;
 p->pos_x --;
}

static void move_right (fsm_t* this) {
 player_t* p = (player_t*) this;
 p->pos_x ++;
}

void player_init (player_t* this, int id) {
 static fsm_trans_t tt[] = {
  { 0, key_up_not_top, 0, move_up },
  { 0, key_down_not_bottom, 0, move_down },
  { 0, key_left_not_left, 0, move_left },
  { 0, key_right_not_right, 0, move_right },
  { -1, NULL, -1, NULL }
 };

 fsm_init ((fsm_t*) this, tt);
 this->id = id;
 this->pos_x = 0;
 this->pos_y = 0;
}

fsm_t* fsm_new_player (int id) {
 player_t* this = (player_t*) malloc (sizeof (player_t));
 player_init (this, id);
 return (fsm_t*) this;
}
