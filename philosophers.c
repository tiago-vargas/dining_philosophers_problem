#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>

#define N_PHILOSOPHERS 5
#define N_HASHIS 5
#define SUSHIS_ON_PLATE 1000

void *philosopher(void *);
void grab_hashi(int, int);
void drop_hashis(int, int);
void eat_sushi_from_boat(int);

pthread_mutex_t hashis[N_HASHIS];
pthread_t philosophers[N_PHILOSOPHERS];
pthread_mutex_t sushi_boat;
int sleep_seconds = 0;

int pieces_of_sushi_eaten_by_philosophers[N_PHILOSOPHERS];

int remaining_sushis = SUSHIS_ON_PLATE;

/**
 * There'll be a central source of sushis: the sushi boat.
 *
 * Philosophers will pull the boat close to themselves, so they can grab a piece
 * of sushi with their hashis, and then push the boat to the center of the table
 * when they're done.
 *
 * Due to this mechanic, only one philosopher can eat at a time, and also only
 * one philospher will grab their hashis at a time.
*/

void initialize_mutexes();
void create_philosopher_threads();
void wait_for_philosophers_to_finish_executing();
void print_report(int [], int);

int main(int argc, char **argv)
{
	initialize_mutexes();

	create_philosopher_threads();

	wait_for_philosophers_to_finish_executing();

	print_report(pieces_of_sushi_eaten_by_philosophers, N_PHILOSOPHERS);

	return 0;
}

void initialize_mutexes()
{
	pthread_mutex_init(&sushi_boat, NULL);

	for (int i = 0; i < N_HASHIS; i++)
		pthread_mutex_init(&hashis[i], NULL);
}

void create_philosopher_threads()
{
	for (int i = 0; i < N_PHILOSOPHERS; i++)
		pthread_create(&philosophers[i], NULL, philosopher, (void *) (__intptr_t) i);
}

void wait_for_philosophers_to_finish_executing()
{
	for (int i = 0; i < N_PHILOSOPHERS; i++)
		pthread_join(philosophers[i], NULL);
}

void *philosopher(void *num)
{
	int id = (__intptr_t) num;
	printf("Philosopher %d is done thinking and is now ready to eat.\n", id);

	int right_hashi = id;
	int left_hashi = (id + 1) % N_PHILOSOPHERS;

	bool there_are_still_sushis_on_table = (remaining_sushis > 0);
	while (there_are_still_sushis_on_table)
	{
		// Pulls the boat
		pthread_mutex_lock(&sushi_boat);

		grab_hashi(id, right_hashi);
		grab_hashi(id, left_hashi);

		eat_sushi_from_boat(id);

		printf("Philosopher %d: eating.\n", id);

		drop_hashis(left_hashi, right_hashi);

		// Pushes the boat
		pthread_mutex_unlock(&sushi_boat);

		// Prepare for next iteration
		there_are_still_sushis_on_table = (remaining_sushis > 0);
	}

	printf("Philosopher %d is done eating.\n", id);
}

void eat_sushi_from_boat(int philosopher_id)
{
	if (remaining_sushis > 0)
	{
		remaining_sushis--;
		printf("Philosopher %d ate their sushi; %d remaining sushis.\n", philosopher_id, remaining_sushis);

		// For reporting purposes
		++pieces_of_sushi_eaten_by_philosophers[philosopher_id];
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

void drop_hashis(int left_hashi, int right_hashi)
{
	pthread_mutex_unlock(&hashis[left_hashi]);
	pthread_mutex_unlock(&hashis[right_hashi]);
}

void print_report(int pieces_of_sushi_eaten_by_philosophers[], int length)
{
	printf("-- Report --" "\n");

	for (int i = 0; i < length; i++)
		printf("Philosopher %d ate %d pieces of sushi.\n", i, pieces_of_sushi_eaten_by_philosophers[i]);
}
