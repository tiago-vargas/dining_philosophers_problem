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
int food_on_table();

pthread_mutex_t forks[N_FORKS];
pthread_t philosophers[N_PHILOSOPHERS];
pthread_mutex_t food_lock;
int sleep_seconds = 0;

// lock: down
// unlock: up

int main(int argc, char **argv)
{
	int i;

	if (argc == 2)
		sleep_seconds = atoi(argv[1]);

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
	while (remaining_cookies = food_on_table())
	{

		/* Thanks to philosophers #1 who would like to take a nap
		 * before picking up the chopsticks, the other philosophers
		 * may be able to eat their dishes and not deadlock.
		 */

		/*
			Não influencia na execução das rotinas de cada thread ou no
			impedimento de deadlocks, apenas escolhe uma thread e simplesmente
			atrasa o seu acesso às regiões criticas.
			if (id == 1)
				sleep(sleep_seconds);
		*/

		grab_fork(id, right_fork, "right");
		grab_fork(id, left_fork, "left");

		pthread_mutex_unlock(&food_lock);

		printf("Philosopher %d: eating.\n", id);
		usleep (DELAY * (COOKIES_ON_PLATE - remaining_cookies + 1));
		down_forks (left_fork, right_fork);
	}

	printf ("Philosopher %d: is done eating.\n", id);
	return (NULL);
}

int food_on_table()
{
	static int remaining_cookies = COOKIES_ON_PLATE;

	pthread_mutex_lock(&food_lock);

	// Tá comendo
	if (remaining_cookies > 0)
		remaining_cookies--;

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
