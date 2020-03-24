#include "player.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h>
#include <sys/select.h>


/* RAW MODE */
static struct termios orig_termios;
void disable_raw_mode()
{
  if (tcsetattr(0, TCSAFLUSH, &orig_termios) == -1)
    perror ("tcsetattr");
}
void enable_raw_mode()
{
  if (tcgetattr(0, &orig_termios) == -1) perror("tcgetattr");
  atexit(disable_raw_mode);
  struct termios raw = orig_termios;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;
  if (tcsetattr(0, TCSAFLUSH, &raw) == -1) perror("tcsetattr");
}

/* TIME MANAGEMENT */
int timespec_less (struct timespec* a, struct timespec* b);
void timespec_add (struct timespec* res, struct timespec* a, struct timespec* b);
void timespec_sub (struct timespec* res, struct timespec* a, struct timespec* b);

int timespec_less (struct timespec *a, struct timespec *b)
{
  return (a->tv_sec < b->tv_sec) ||
    ((a->tv_sec == b->tv_sec) && (a->tv_nsec < b->tv_nsec));
}

void timespec_sub (struct timespec *res, struct timespec *a, struct timespec *b)
{
	res->tv_sec = a->tv_sec - b->tv_sec;
	res->tv_nsec = a->tv_nsec - b->tv_nsec;
	if (res->tv_nsec < 0) {
		--res->tv_sec;
		res->tv_nsec += 1000000000;
	}
}
void timespec_add (struct timespec *res, struct timespec *a, struct timespec *b)
{
	res->tv_sec = a->tv_sec + b->tv_sec;
	res->tv_nsec = a->tv_nsec + b->tv_nsec;
	if (res->tv_nsec > 1000000000) {
		++res->tv_sec;
		res->tv_nsec -= 1000000000;
	}
}
void delay_until (struct timespec* next_activation)
{
	struct timespec resto, now;
	clock_gettime(CLOCK_REALTIME, &now);
	timespec_sub(&resto, next_activation, &now);
	while (nanosleep(&resto,&resto)>0);
}


player_t* p1 = NULL;
player_t* p2 = NULL;
int pre1_x = UMBRAL;
int pre1_y = MAX_Y+UMBRAL;
int pre2_x = UMBRAL;
int pre2_y = MAX_Y+UMBRAL;

/* Toma de Datos */
struct timespec max = {0, 0};


int key_pressed (void)
{
  struct timeval timeout = { 0, 0 };
  fd_set rd_fdset;
  FD_ZERO(&rd_fdset);
  FD_SET(0, &rd_fdset);
  return select(1, &rd_fdset, NULL, NULL, &timeout) > 0;
}
void key_process (int ch)
{
  switch (ch) {
  case 'w': ((p1->flags_key) |= FLAG_UP); break;
  case 's': ((p1->flags_key) |= FLAG_DOWN); break;
  case 'a': ((p1->flags_key) |= FLAG_LEFT); break;
  case 'd': ((p1->flags_key) |= FLAG_RIGHT); break;

	case 'i': ((p2->flags_key) |= FLAG_UP); break;
  case 'k': ((p2->flags_key) |= FLAG_DOWN); break;
  case 'j': ((p2->flags_key) |= FLAG_LEFT); break;
  case 'l': ((p2->flags_key) |= FLAG_RIGHT); break;

	case 'q':
		printf("\e[%d;0f",MAX_Y+2*UMBRAL+3);
		printf("tiempo de ejecucion maximo del refresco de pantalla: %ld.%.9ld (s)\n",max.tv_sec,max.tv_nsec);
		printf ("\e0\e[?25h");
		printf("\e[%d;0f",MAX_Y+2*UMBRAL+5);
		exit(0); break;
  }
}

void setup_screen()
{
	printf ("\e7\e[?25l");
	printf("\e[2J\e[%d;0f",UMBRAL-1);
	for (int i=-1; i<MAX_X+2;i++) {
		printf("\e[%d;%df%c",UMBRAL-1,i+UMBRAL,'X');
		printf("\e[%d;%df%c",UMBRAL+MAX_Y+1,i+UMBRAL,'X');
	}
	for (int j=0; j<MAX_Y+1; j++) {
		printf("\e[%d;%df%c",j+UMBRAL,UMBRAL-1,'X');
		printf("\e[%d;%df%c",j+UMBRAL,UMBRAL+MAX_X+1,'X');
		for (int i=0; i<MAX_X+1; i++) {
			printf("\e[%d;%df%c",j+UMBRAL,i+UMBRAL,' ');
		}
	}
}

void screen_refresh(player_t* p1,player_t* p2)
{
		if (pre1_x != p1->pos_x || pre1_y != p1->pos_y)  {
			printf("\e[%d;%df%c",pre1_y+UMBRAL,pre1_x+UMBRAL,' ');
			pre1_x = p1->pos_x;
			pre1_y = p1->pos_y;
			printf("\e[%d;%df%c",pre1_y+UMBRAL,pre1_x+UMBRAL,'1');
			printf("\e[%d;%df%c",pre2_y+UMBRAL,pre2_x+UMBRAL,'2');
		}
		if (pre2_x != p2->pos_x || pre2_y != p2->pos_y) {
			printf("\e[%d;%df%c",pre2_y+UMBRAL,pre2_x+UMBRAL,' ');
			pre2_x = p2->pos_x;
			pre2_y = p2->pos_y;
			printf("\e[%d;%df%c",pre2_y+UMBRAL,pre2_x+UMBRAL,'2');
			printf("\e[%d;%df%c",pre1_y+UMBRAL,pre1_x+UMBRAL,'1');
		}
		printf("\e[%d;%dfp1:(%.2d,%.2d)", MAX_Y+2*UMBRAL, UMBRAL, p1->pos_x, p1->pos_y);
		printf("\e[%d;%dfp2:(%.2d,%.2d)", MAX_Y+2*UMBRAL+1, UMBRAL, p2->pos_x, p2->pos_y);

}

int main (void)
{

  p1 = fsm_new_player(1);
  p2 = fsm_new_player(2);

  struct timespec next;
  clock_gettime(CLOCK_REALTIME, &next);
	struct timespec T = {0, 800000};


	/* TOMA de datos */
	struct timespec t_inicial, t_final, temp;

	enable_raw_mode();
	setup_screen();

	int frame = 0;

  while (1) {

		switch (frame) {
			case 0:
				if (key_pressed()) {
					key_process(getchar());
				}
				fsm_fire ((fsm_t*)p1);
				fsm_fire ((fsm_t*)p2);
				break;
			case 1:
				if (key_pressed()) {
					key_process(getchar());
				}
				fsm_fire ((fsm_t*)p1);
				fsm_fire ((fsm_t*)p2);
				break;
			case 2:
				if (key_pressed()) {
					key_process(getchar());
				}
				fsm_fire ((fsm_t*)p1);
				fsm_fire ((fsm_t*)p2);
				break;
			case 3:
				if (key_pressed())
					key_process(getchar());
				fsm_fire ((fsm_t*)p1);
				fsm_fire ((fsm_t*)p2);
				break;
			case 4:

				if (key_pressed())
					key_process(getchar());
				fsm_fire ((fsm_t*)p1);
				fsm_fire ((fsm_t*)p2);

				/* Toma de datos */
				clock_gettime(CLOCK_REALTIME, &t_inicial);
				screen_refresh(p1,p2);
				clock_gettime(CLOCK_REALTIME, &t_final);
				timespec_sub(&temp,&t_final,&t_inicial);
				if (timespec_less(&max,&temp)) {
					max = temp;
				}
				break;
		}

    timespec_add(&next,&next,&T);
    delay_until(&next);
		//printf("p1:(%d,%d);;p2:(%d,%d)\r\n", p1->pos_x, p1->pos_y, p2->pos_x, p2->pos_y);
		frame = (frame+1)%5;
	}
}
