/* din_philo.c */
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

#define N_PHILOSOPHERS 5
#define N_FORKS 5
#define DELAY 5000
#define COOKIES_ON_PLATE 10

void *philosopher(void *id);
void grab_fork(int, int, char *);
void down_forks(int, int);
int food_on_table(int philosopher_id);

pthread_mutex_t forks[N_FORKS];
pthread_t philosophers[N_PHILOSOPHERS];
pthread_mutex_t food_lock;
int sleep_seconds = 0;

// lock: down
// unlock: up

int main(int argc, char **argv)
{
	int i;

	// Serve pra tornar o ato de "pegar os garfos" atômico
	// Ou seja, ou pega os dois garfos ou não pega nenhum
	pthread_mutex_init(&food_lock, NULL);

	// Inicializa os garfos
	for (i = 0; i < N_FORKS; i++)
		pthread_mutex_init(&forks[i], NULL);

	// Inicializa os filósofos
	for (i = 0; i < N_PHILOSOPHERS; i++)
		pthread_create(&philosophers[i], NULL, philosopher, (void *) i);

	// Certifica que as threads finalizaram a rotina
	for (i = 0; i < N_PHILOSOPHERS; i++)
		pthread_join(philosophers[i], NULL);

	return 0;
}

void *philosopher(void *num)
{
	int id;
	int left_fork, right_fork;

	id = (int) num;
	printf ("Philosopher %d: is done thinking and now ready to eat.\n", id);
	right_fork = id;
	left_fork = (id + 1) % N_PHILOSOPHERS;

	int remaining_cookies;
	while (remaining_cookies = food_on_table(id))
	{
		grab_fork(id, right_fork, "right");
		grab_fork(id, left_fork, "left");

		pthread_mutex_unlock(&food_lock);

		printf("Philosopher %d: eating.\n", id);
		usleep (DELAY * (COOKIES_ON_PLATE - remaining_cookies + 1));
		down_forks (left_fork, right_fork);
	}

	printf ("Philosopher %d: is done eating.\n", id);
}

int food_on_table(int philosopher_id)
{
	static int remaining_cookies = COOKIES_ON_PLATE;

	pthread_mutex_lock(&food_lock);

	// Tá comendo
	if (remaining_cookies > 0){
		remaining_cookies--;
		printf("Philosopher %d: get your food, %d remaining cookies.\n", philosopher_id, remaining_cookies);
	}

	//Caso os biscoitos acabem eu preciso destravar o food_lock
	//Do contrário, os filósofos que estavam na fila pra tentar comer jamais entrariam
	//no while para dar unlock (em philosopher()), assim a execução entraria em deadlock
	//pois existiria threads que jamais sairiam de sua rotina esperando o unlock
	if(!remaining_cookies)
		pthread_mutex_unlock(&food_lock);
	
	return remaining_cookies;
}

void grab_fork(int philosopher_id, int fork_id, char *side)
{
	// side = "left" | "right"
	pthread_mutex_lock(&forks[fork_id]);
	printf ("Philosopher %d: got %s fork %d\n", philosopher_id, side, fork_id);
}

void down_forks(int left_fork, int right_fork)
{
	pthread_mutex_unlock(&forks[left_fork]);
	pthread_mutex_unlock(&forks[right_fork]);
}
