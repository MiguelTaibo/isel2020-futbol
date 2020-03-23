#include "player.h"

#include <time.h>
#include <stdio.h>
#include <pthread.h>
#include <stddef.h>

// comprobacions
static int key_up_not_top (fsm_t* this) {
 player_t* p = (player_t*) this;
 return (p->pos_y > MIN_Y) && (p->flags_key & FLAG_UP);
}

static int key_down_not_bottom (fsm_t* this) {
 player_t* p = (player_t*) this;
 return (p->pos_y < MAX_Y) && (p->flags_key & FLAG_DOWN);
}

static int key_left_not_left (fsm_t* this) {
 player_t* p = (player_t*) this;
 return (p->pos_x > MIN_X) && (p->flags_key & FLAG_LEFT);
}

static int key_right_not_right (fsm_t* this) {
 player_t* p = (player_t*) this;
 return (p->pos_x < MAX_X) && (p->flags_key & FLAG_RIGHT);
}

//transiciones
static void move_up (fsm_t* this) {
 player_t* p = (player_t*) this;
 ((p->flags_key) &= ~FLAG_UP);
 p->pos_y --;
}

static void move_down (fsm_t* this) {
 player_t* p = (player_t*) this;
 ((p->flags_key) &= ~FLAG_DOWN);
 p->pos_y ++;
}

static void move_left (fsm_t* this) {
 player_t* p = (player_t*) this;
 ((p->flags_key) &= ~FLAG_LEFT);
 p->pos_x --;
}

static void move_right (fsm_t* this) {
 player_t* p = (player_t*) this;
 ((p->flags_key) &= ~FLAG_RIGHT);
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
 this->flags_key = 0;
}

fsm_t* fsm_new_player (int id)
{
 player_t* this = (player_t*) malloc (sizeof (player_t));
 player_init (this, id);
 return (fsm_t*) this;
}
