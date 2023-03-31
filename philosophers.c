/* din_philo.c */
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

#define N_PHILOSOPHERS 5
#define N_hashiS 5
#define DELAY 5000
#define SUSHIS_ON_PLATE 10

void *philosopher(void *id);
void grab_hashi(int, int, char *);
void down_hashis(int, int);
int food_on_table(int philosopher_id);

pthread_mutex_t hashis[N_hashiS];
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
	for (i = 0; i < N_hashiS; i++)
		pthread_mutex_init(&hashis[i], NULL);

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
	int left_hashi, right_hashi;

	id = (int) num;
	printf ("Philosopher %d: is done thinking and now ready to eat.\n", id);
	right_hashi = id;
	left_hashi = (id + 1) % N_PHILOSOPHERS;

	int remaining_sushis;
	while (remaining_sushis = food_on_table(id))
	{
		grab_hashi(id, right_hashi, "right");
		grab_hashi(id, left_hashi, "left");

		pthread_mutex_unlock(&food_lock);

		printf("Philosopher %d: eating.\n", id);
		usleep (DELAY * (SUSHIS_ON_PLATE - remaining_sushis + 1));
		down_hashis (left_hashi, right_hashi);
	}

	printf ("Philosopher %d: is done eating.\n", id);
}

int food_on_table(int philosopher_id)
{
	static int remaining_sushis = SUSHIS_ON_PLATE;

	pthread_mutex_lock(&food_lock);

	// Tá comendo
	if (remaining_sushis > 0){
		remaining_sushis--;
		printf("Philosopher %d: get your food, %d remaining sushis.\n", philosopher_id, remaining_sushis);
	}

	//Caso os biscoitos acabem eu preciso destravar o food_lock
	//Do contrário, os filósofos que estavam na fila pra tentar comer jamais entrariam
	//no while para dar unlock (em philosopher()), assim a execução entraria em deadlock
	//pois existiria threads que jamais sairiam de sua rotina esperando o unlock
	if(!remaining_sushis)
		pthread_mutex_unlock(&food_lock);

	return remaining_sushis;
}

void grab_hashi(int philosopher_id, int hashi_id, char *side)
{
	// side = "left" | "right"
	pthread_mutex_lock(&hashis[hashi_id]);
	printf ("Philosopher %d: got %s hashi %d\n", philosopher_id, side, hashi_id);
}

void down_hashis(int left_hashi, int right_hashi)
{
	pthread_mutex_unlock(&hashis[left_hashi]);
	pthread_mutex_unlock(&hashis[right_hashi]);
}
