#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>

#define N_PHILOSOPHERS 5
#define N_HASHIS 5
#define SUSHIS_ON_PLATE 10000

void *philosopher(void *);
void grab_both_hashis(int);
void grab_hashi(int, int);
void drop_both_hashis(int);
void drop_hashi(int, int);
void eat_sushi_from_boat(int);

pthread_mutex_t hashis[N_HASHIS];
pthread_t philosophers[N_PHILOSOPHERS];
int sleep_seconds = 0;

int pieces_of_sushi_eaten_by_philosophers[N_PHILOSOPHERS];

int remaining_sushis = SUSHIS_ON_PLATE;

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
	printf("Philosopher %d is done thinking and is now ready to eat." "\n", id);

	bool there_are_still_sushis_on_table = (remaining_sushis > 0);
	while (there_are_still_sushis_on_table)
	{
		grab_both_hashis(id);
		eat_sushi_from_boat(id);
		drop_both_hashis(id);

		// Prepare for next iteration
		there_are_still_sushis_on_table = (remaining_sushis > 0);
	}

	printf("Philosopher %d is done eating." "\n", id);
}

void eat_sushi_from_boat(int philosopher_id)
{
	printf("Philosopher %d is eating." "\n", philosopher_id);

	if (remaining_sushis > 0)
	{
		remaining_sushis--;
		printf("Philosopher %d ate their sushi; %d remaining sushis." "\n", philosopher_id, remaining_sushis);

		// For reporting purposes
		++pieces_of_sushi_eaten_by_philosophers[philosopher_id];
	}
}

void grab_both_hashis(int philosopher_id)
{
	int right_hashi = philosopher_id;
	int left_hashi = (philosopher_id + 1) % N_PHILOSOPHERS;

	if (philosopher_id == 0)
	{
		grab_hashi(philosopher_id, right_hashi);
		grab_hashi(philosopher_id, left_hashi);
	}
	else
	{
		grab_hashi(philosopher_id, left_hashi);
		grab_hashi(philosopher_id, right_hashi);
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
	printf("Philosopher %d got %s hashi %d." "\n", philosopher_id, side, hashi_id);
}

void drop_both_hashis(int philosopher_id)
{
	int right_hashi = philosopher_id;
	int left_hashi = (philosopher_id + 1) % N_PHILOSOPHERS;
	drop_hashi(philosopher_id, left_hashi);
	drop_hashi(philosopher_id, right_hashi);
}

void drop_hashi(int philosopher_id, int hashi_id)
{
	char *side;
	if (hashi_id == philosopher_id)
		side = "right";
	else if (hashi_id == philosopher_id + 1)
		side = "left";

	pthread_mutex_unlock(&hashis[hashi_id]);
	printf("Philosopher %d dropped %s hashi %d." "\n", philosopher_id, side, hashi_id);
}

void print_report(int pieces_of_sushi_eaten_by_philosophers[], int length)
{
	printf("-- Report --" "\n");

	for (int i = 0; i < length; i++)
		printf("Philosopher %d ate %d pieces of sushi." "\n", i, pieces_of_sushi_eaten_by_philosophers[i]);
}
