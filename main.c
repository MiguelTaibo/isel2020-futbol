#include "player.h"
#include <time.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
//TIME MANAGEMENT
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

//THREAD TO READ KEYBOARD
struct compute_prime_struct;
typedef struct compute_prime_struct compute_prime_struct;
struct compute_prime_struct {
  player_t *p1;
  player_t *p2;
};

void printMap(player_t* p1, player_t* p2)
{
    int x1=p1->pos_x;
    int y1=p1->pos_y;
    int x2=p2->pos_x;
    int y2=p2->pos_y;

    printf("XXXXXXXXXXXXXXXXXXXXXXXX\n");
    char A[20];
    for (int i=0; i<22;i++) {
      for (int j=0; j<22; j++) {
          if (i==y1 && j==x1) {
            A[j]='1';
          } else if (i==y2 && j==x2) {
            A[j]='2';
          } else {
            A[j]='_';
          }
      }
      printf("X%sX\n",A);
    }
    printf("XXXXXXXXXXXXXXXXXXXXXXXX\n");
}

void *checkInputsThread (void *args)
{

	compute_prime_struct *actual_args = args;
  player_t* p1 = (player_t*) actual_args->p1;
  player_t* p2 = (player_t*) actual_args->p2;

  // Codigo de la funcion
	char ss;
	//struct timespec segundo = {0,0};

	while (scanf("%c",&ss)) {
		if (ss=='w') {
			((p1->flags_key) |= FLAG_UP);
		}	else if (ss=='a') {
      ((p1->flags_key) |= FLAG_LEFT);
    }	else if (ss=='s') {
      ((p1->flags_key) |= FLAG_DOWN);
    }	else if (ss=='d') {
      ((p1->flags_key) |= FLAG_RIGHT);
    }	else if (ss=='i') {
      ((p2->flags_key) |= FLAG_UP);
    }	else if (ss=='j') {
      ((p2->flags_key) |= FLAG_LEFT);
    }	else if (ss=='k') {
      ((p2->flags_key) |= FLAG_DOWN);
    }	else if (ss=='l') {
      ((p2->flags_key) |= FLAG_RIGHT);
    } else if (ss=='q') {
			exit(-1);
    } else if (ss==' ') {
      printMap(p1, p2);
		}	else if (ss=='\n'){
			continue;
		}	else {
      printf("%d\n",p1->flags_key );
			printf("Ver Instrucciones \n");
		}
		//nanosleep(&segundo,&segundo);
	}
	pthread_exit(NULL);
}


int main (void)
{

  player_t* p1 = fsm_new_player(1);
  player_t* p2 = fsm_new_player(2);

  compute_prime_struct *args = malloc(sizeof *args);
  args->p1=p1;
  args->p2=p2;

  pthread_t thInputs;
	if ( 0 != pthread_create(&thInputs, NULL, checkInputsThread, args) ) {
		printf("no empezamos thread\n");
	}

  struct timespec next;
  clock_gettime(CLOCK_REALTIME, &next);
	struct timespec T = {0, 50000000}; //1/20 segundos

  while (1) {
    fsm_fire ((fsm_t*)p1);
    fsm_fire ((fsm_t*)p2);

    timespec_add(&next,&next,&T);
    delay_until(&next);
  }
}
