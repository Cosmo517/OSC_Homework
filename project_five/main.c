#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

// The constants below are used to store unchanging
// information
#define TOTAL_THREADS 3
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
    return rand() % (end - start + 1) + start;
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
        // Generate the amount of semaphores we need
        int numSemaphores = rand_num(1, 3);
        // This array will store the semaphores the thread needs to do work
        int semaphoresNeeded[TOTAL_RESOURCES];
        
        // Reset all resources for each run
        for (int i = 0; i < TOTAL_RESOURCES; i++)
            semaphoresNeeded[i] = 0;

        // Choose random semaphores needed
        int chosen = 0;
        while (chosen < numSemaphores)
        {
            int randSemaphore = rand_num(0, 2);
            // If the smeaphore is already chosen, choose a new one
            if (semaphoresNeeded[randSemaphore])
                continue;
            semaphoresNeeded[randSemaphore] = 1;
            chosen++;
        }

        // Now we need to try to attain all the semaphores
        int semaphoresObtained[TOTAL_RESOURCES]; // List of the semaphores we obtained
        int acquiredSemaphores = 0; // How many have we obtained
        
        // Set all obtained semaphores to 0
        for (int i = 0; i < TOTAL_RESOURCES; i++)
            semaphoresObtained[i] = 0;
        
        while(acquiredSemaphores < numSemaphores)
        {
            // Loop to find the semaphores we need
            for (int i = 0; i < TOTAL_RESOURCES; i++)
            {
                // If we need this semaphore
                if (semaphoresNeeded[i])
                {
                    if (sem_trywait(&semaphores[i]) == 0)
                    {
                        semaphoresObtained[i] = 1;
                        acquiredSemaphores++;
                        printf("%c>%d\n", data->thread_id, i);
                    }
                }
            }

            // Check to see if we need to release the semaphores
            if (acquiredSemaphores != numSemaphores)
            {
                for (int i = 0; i < TOTAL_RESOURCES; i++)
                {
                    if (semaphoresObtained[i])
                    {
                        semaphoresObtained[i] = 0;
                        sem_post(&semaphores[i]);
                        printf("%c<%d\n", data->thread_id, i);
                    }
                }
                acquiredSemaphores = 0;
                random_sleep(10); // Sleep for up to 10 ms
            }
        }

        // If we make it here, that means the thread has
        // obtained all the semaphores it needs
        printf("%c has %d left\n", data->thread_id, data->work);
        data->work--;
        acquiredSemaphores = 0;

        // Now we need to release the semaphores
        for (int i = 0; i < TOTAL_RESOURCES; i++)
        {
            if (semaphoresObtained[i])
            {
                semaphoresObtained[i] = 0;
                sem_post(&semaphores[i]);
                printf("%c<%d\n", data->thread_id, i);
            }
        }
        random_sleep(10); // Sleep for up to 10 ms
    }

    // Once here, we have no work left, so exit
    pthread_exit(NULL);
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
    
    printf("DONE\n");

    // Now lets remove our semaphores
    for (int i = 0; i < TOTAL_RESOURCES; i++)
    {
        sem_destroy(&semaphores[i]);
    }

    return 0;
}
