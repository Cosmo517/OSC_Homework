#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

// The constants below are used to store unchanging
// information
#define TOTAL_THREADS 4
#define TOTAL_RESOURCES 3
#define TOTAL_WORK 10

// The struct below will be used to store the thread data
typedef struct {
    int thread_id;
    int work;
} ThreadData;

sem_t semaphores[TOTAL_RESOURCES];

/*
 * Name: Ethan Bielecki
 * Date: 11/30/2024
 * Description: This function will be used to generate a random
 * number between start and end, inclusive
*/ 
int rand_num(int start, int end)
{
    return rand() % end + start;
}

/*
 * Name: Ethan Bielecki
 * Date: 11/29/2024
 * Description: This function will sleep for a random amount of time between
 * 0-10 ms, inclusive
*/ 
void random_sleep(int max_ms)
{
    struct timespec remaining, request = { 0, (rand() % (max_ms + 1)) * 1000000 };
    nanosleep(&request, &remaining);
}

/*
 * Name: Ethan Bielecki
 * Date: 11/30/2024
 * Description: This function will control the thread, choosing which resources it
 * needs to perform work on.
*/
void *thread_work(void *arg)
{
    // Grab the data from the parameter
    ThreadData *data = (ThreadData*)arg;

    // While the thread still has work to do
    while (data->work > 0)
    {

    }
}

/*
 * Name: Ethan Bielecki
 * Date: 11/29/2024
 * Description: The main function that controls the entire program
*/
int main() 
{
    // Seed the random number generation
    srand(time(NULL));
    
    // The following arrays will keep track of each thread, the thread data,
    // and contain possible thread_ids.
    pthread_t threads[TOTAL_THREADS];
    ThreadData thread_data[TOTAL_THREADS];
    char thread_ids[] = {'A', 'B', 'C', 'D'};

    // Initialize our semaphores
    for (int i = 0; i < TOTAL_RESOURCES; i++)
    {
        sem_init(&semaphores[i], 0, 1);
    }
    
    // Now we can create our threads
    for (int i = 0; i < TOTAL_THREADS; i++)
    {
        // Lets initialize the thread data
        thread_data[i].thread_id = thread_ids[i];
        thread_data[i].work = TOTAL_WORK;
        // Now we create the thread
        pthread_create(&threads[i], NULL, thread_work, &thread_data[i]);
    }

    // Now lets just our threads to prevent the program from ending
    // before all threads are finished
    for (int i = 0; i < TOTAL_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // Now lets remove our semaphores
    for (int i = 0; i < TOTAL_RESOURCES; i++)
    {
        sem_destroy(&semaphores[i]);
    }

    return 0;
}
