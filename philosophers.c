#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

#define N_PHILOSOPHERS 5
#define N_HASHIS 5
#define SUSHIS_ON_PLATE 1000

void *philosopher(void *);
void grab_hashi(int, int);
void down_hashis(int, int);
void eat_sushi_from_boat(int);

pthread_mutex_t hashis[N_HASHIS];
pthread_t philosophers[N_PHILOSOPHERS];
pthread_mutex_t sushi_boat;
int sleep_seconds = 0;

int remaining_sushis = SUSHIS_ON_PLATE;

// lock: down
// unlock: up

int main(int argc, char **argv)
{
	// Serve pra tornar o ato de "pegar os garfos" atômico
	// Ou seja, ou pega os dois garfos ou não pega nenhum
	pthread_mutex_init(&sushi_boat, NULL);

	// Inicializa os hashis
	for (int i = 0; i < N_HASHIS; i++)
		pthread_mutex_init(&hashis[i], NULL);

	// Inicializa os filósofos
	for (int i = 0; i < N_PHILOSOPHERS; i++)
		pthread_create(&philosophers[i], NULL, philosopher, (void *) i);

	// Certifica que as threads finalizaram a rotina
	for (int i = 0; i < N_PHILOSOPHERS; i++)
		pthread_join(philosophers[i], NULL);

	return 0;
}

void *philosopher(void *num)
{
	int id = (int) num;
	printf("Philosopher %d is done thinking and is now ready to eat.\n", id);

	int right_hashi = id;
	int left_hashi = (id + 1) % N_PHILOSOPHERS;

	bool there_are_still_sushis_on_table = (remaining_sushis > 0);
	while (there_are_still_sushis_on_table)
	{
		pthread_mutex_lock(&sushi_boat);
		eat_sushi_from_boat(id);

		grab_hashi(id, right_hashi);
		grab_hashi(id, left_hashi);

		printf("Philosopher %d: eating.\n", id);

		down_hashis(left_hashi, right_hashi);
		pthread_mutex_unlock(&sushi_boat);

		// Prepare for next iteration
		there_are_still_sushis_on_table = (remaining_sushis > 0);
	}

	printf("Philosopher %d is done eating.\n", id);
}

void eat_sushi_from_boat(int philosopher_id)
{
	// Tá comendo
	if (remaining_sushis > 0) {
		remaining_sushis--;
		printf("Philosopher %d ate their sushi; %d remaining sushis.\n", philosopher_id, remaining_sushis);
	}
}

void grab_hashi(int philosopher_id, int hashi_id)
{
	char *side;
	if (hashi_id == philosopher_id)
		side = "right";
	else if (hashi_id == philosopher_id + 1)
		side = "left";

	pthread_mutex_lock(&hashis[hashi_id]);
	printf("Philosopher %d got %s hashi %d\n", philosopher_id, side, hashi_id);
}

void down_hashis(int left_hashi, int right_hashi)
{
	pthread_mutex_unlock(&hashis[left_hashi]);
	pthread_mutex_unlock(&hashis[right_hashi]);
}
