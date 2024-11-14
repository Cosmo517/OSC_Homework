#include <stdio.h>
#include <string.h>

// The below struct will be used to keep track of
// a processes information. More useful for 
// SRTF and RR
typedef struct {
    char name[128];
    int burstTime;
    int arrivalTime;
    int remainingTime;
    int startTime;
    int finishTime;
    int waitTime;
    int responseTime;
    char outputString[512];
} Process;


/*
 * Name: Ethan Bielecki
 * Date: 11/14/2024
 * Description: This function will perform first
 * in first out on the processes 
*/ 
void FIFO(int numProcesses, Process processes[], int cyclesThroughput)
{
    int totalWaitTime = 0;
    int totalResponseTime = 0;
    float averageWaitTime = 0;
    float averageResponseTime = 0;
    int cycles = 0;
    int processesCompleted = 0;

    for (int i = 0; i < numProcesses; i++)
    {
        // Standard variable initialization

        printf("Process %s info:\n", processes[i].name);
        
        // Wait time
        for (int j = 0; j < i; j++)
        {
            processes[i].waitTime += processes[j].burstTime;
        }

        processes[i].waitTime -= processes[i].arrivalTime;
        
        totalWaitTime += processes[i].waitTime;
        printf("Wait time: %d\n", processes[i].waitTime);
        
        // Response time
        processes[i].responseTime = processes[i].waitTime;
        totalResponseTime += processes[i].responseTime;
        printf("Response time: %d\n\n", processes[i].responseTime);

        cycles += processes[i].burstTime;

        // Throughput check
        if (cycles <= cyclesThroughput)
        {
            processesCompleted++;
        }
    }

    printf("\n"); 
   
    // Chart creation
    // We loop over each process
    for (int i = 0; i < numProcesses; i++)
    {
        int currentCycles = 0; // Used to track cycle count
        printf("%-20s: ", processes[i].name);
        
        // We now loop over each process before the current one
        for (int j = 0; j <= i; j++)
        {
            // Now loop through those processes burst times
            for (int k = 0; k < processes[j].burstTime; k++)
            {
                // if j and i are equal, that means the process is running
                if (j == i)
                    printf("#");
                else if (currentCycles >= processes[i].arrivalTime) // Process is waiting
                    printf("_");
                else
                    printf(" "); // Process hasn't arrived yet
                currentCycles++;
            }
        }
        printf("\n");
    }
    printf("\n");

    // Average information
    averageWaitTime = (float)totalWaitTime / numProcesses;
    averageResponseTime = (float)totalResponseTime / numProcesses;
    printf("Average waiting time: %f\n", averageWaitTime);
    printf("Average response time: %f\n", averageResponseTime);
    printf("Throughput: %d / %d \n", processesCompleted, cyclesThroughput);
}

/*
 * Name: Ethan Bielecki
 * Date: 11/13/2024
 * Description: This function will perform
 * shortest job first on the processes
*/ 
void SJF(int numProcesses, Process processes[], int cyclesThroughput)
{
    // used to track if we have completed all processes
    int finished = 0;
    int currentCycle = 0;
    int processesCompletedThroughput = 0;

    while (finished != numProcesses)
    {
        // We need to find the shortest process that also arrived
        Process* shortestProcess = NULL;
        int shortestProcessIndex = -1;
        for (int i = 0; i < numProcesses; i++)
        {
            if (currentCycle >= processes[i].arrivalTime && processes[i].remainingTime > 0 && (!shortestProcess || processes[i].remainingTime < shortestProcess->remainingTime))
            {
                shortestProcess = &processes[i];
                shortestProcessIndex = i;
            }
        }

        // Make sure we do have a shortest process (its not null)
        if (shortestProcess)
        {
            // Check if the process just started
           if (shortestProcess->remainingTime == shortestProcess->burstTime)
            {
                shortestProcess->startTime = currentCycle;
                shortestProcess->responseTime = currentCycle - shortestProcess->arrivalTime;
            }

            // Decrease the time on the process
            shortestProcess->remainingTime--;

            // We need to insert a # for the process
            int k = 0;
            while (shortestProcess->outputString[k] != '\0')
                k++;
            shortestProcess->outputString[k] = '#';
            
            // If the process is done, increase our finished count
            if (shortestProcess->remainingTime == 0)
            {
                finished++;
                // Throughput
                if (currentCycle <= cyclesThroughput)
                    processesCompletedThroughput++;
            }
        }
        
        // For any process that isnt the shortest process, we need
        // to either add a space or _ depending on if it arrived or not
        for (int i = 0; i < numProcesses; i++)
        {
            if (shortestProcessIndex != i)
            {
                int j = 0;
                while (processes[i].outputString[j] != '\0')
                    j++;

                if (processes[i].arrivalTime > currentCycle)
                {
                    processes[i].outputString[j] = ' ';
                }
                else
                {
                    processes[i].outputString[j] = '_';
                    if (processes[i].remainingTime > 0)
                        processes[i].waitTime++;
                }
            }
        }

        currentCycle++;
    }

   // Now we need to display all the information about the processes
   // We can also do calculations for average times
   int totalWaitTime = 0;
   float averageWaitTime = 0;
   int totalResponseTime = 0;
   float averageResponseTime = 0;

   for (int i = 0; i < numProcesses; i++)
   {
       printf("Process %s info:\n", processes[i].name);
       printf("Wait time: %d\n", processes[i].waitTime);
       printf("Response time: %d\n", processes[i].responseTime);
       printf("\n");

       totalWaitTime += processes[i].waitTime;
       totalResponseTime += processes[i].responseTime;
   }
   

   // Now display the chart
   for (int i = 0; i < numProcesses; i++)
   {
       printf("%-20s: %s\n", processes[i].name, processes[i].outputString); 
   }

   // Now display average information
   averageWaitTime = (float)totalWaitTime / numProcesses;
   averageResponseTime = (float)totalResponseTime / numProcesses;
   printf("\nAverage waiting time: %f\n", averageWaitTime);
   printf("Average response time: %f\n", averageResponseTime);
   printf("Throughput: %d / %d\n", processesCompletedThroughput, cyclesThroughput);
}

/*
 * Name: Ethan Bielecki
 * Date: 11/14/2024
 * Description: This function will perform round
 * robin scheduling on the processes
*/ 
void roundRobin(int numProcesses, Process processes[], int cyclesThroughput)
{
    Process* readyQueue[5];
    int processesStartedTracker[5] = {0};
    int finished = 0;
    int processesStarted = 0;
    int currentCycle = 0;
    int queueIndex = 0; // marks where we will be pulling from
    int queueEnd = 0; // marks where the end of the queue is
    int processesCompletedThroughput = 0;
    int queueCount = 0;

    Process* poppedProcess = NULL;
    while (finished != numProcesses)
    {
        // We first want to check if a process has arrived
        // If it has, add it to the end of the queue
        if (processesStarted != numProcesses)
        {
            for (int i = 0; i < numProcesses; i++)
            {
                // If the arrivalTime is <= currentCycle, that means
                // we can add the process to the ready queue
                if (processes[i].arrivalTime <= currentCycle &&
                        processesStartedTracker[i] == 0)
                {
                    // Add the process to the end of the queue
                    readyQueue[queueEnd] = &processes[i];
                    queueEnd = (queueEnd + 1) % numProcesses;
                    processesStarted++;
                    processesStartedTracker[i] = 1;
                    queueCount++;
                }
                else if (processesStartedTracker[i] == 0)
                {
                    // The process hasnt arrived yet, add a ' '
                    int j = 0;
                    while (processes[i].outputString[j] != '\0')
                        j++;
                    processes[i].outputString[j] = ' ';
                }
            }
        }

        // Check if we had a popped process, if we do, add it
        // to the back of the ready queue
        if (poppedProcess && poppedProcess->remainingTime > 0)
        {
            readyQueue[queueEnd] = poppedProcess;
            queueEnd = (queueEnd + 1) % numProcesses;
            queueCount++;
        }

        // Now that we have the processes in the ready queue
        // We need to take the first one out to begin processing
        if (queueCount > 0) // queue has items in it
        {
            poppedProcess = readyQueue[queueIndex];
            queueIndex = (queueIndex + 1) % numProcesses;
            queueCount--;
            // This handles if the process just starts
            if (poppedProcess->remainingTime == poppedProcess->burstTime)
            {
                poppedProcess->startTime = currentCycle;
                poppedProcess->responseTime = currentCycle - poppedProcess->arrivalTime;
            }

            // Now we "run" the process
            poppedProcess->remainingTime--;
            // Add a # to the end of the process
            int i = 0;
            while (poppedProcess->outputString[i] != '\0')
                i++;
            poppedProcess->outputString[i] = '#';

            // Check if the process is done
            if (poppedProcess->remainingTime == 0)
            {
                finished++;
                if (currentCycle <= cyclesThroughput)
                    processesCompletedThroughput++;
            }

            // Lastly, we add a '_' for every other process in the
            // waiting queue
            for (int k = queueIndex; k != queueEnd; k = (k + 1) % numProcesses)
            {
                int n = 0;
                while (readyQueue[k]->outputString[n] != '\0')
                    n++;
                readyQueue[k]->outputString[n] = '_';
                readyQueue[k]->waitTime++;
            }
        }

        currentCycle++;
    }

    // We can also do calculations for average times
    int totalWaitTime = 0;
    float averageWaitTime = 0;
    int totalResponseTime = 0;
    float averageResponseTime = 0;

    for (int i = 0; i < numProcesses; i++)
    {
        printf("Process %s info:\n", processes[i].name);
        printf("Wait time: %d\n", processes[i].waitTime);
        printf("Response time: %d\n", processes[i].responseTime);
        printf("\n");

        totalWaitTime += processes[i].waitTime;
        totalResponseTime += processes[i].responseTime;
    }
   

    // Now display the chart
    for (int i = 0; i < numProcesses; i++)
    {
        printf("%-20s: %s\n", processes[i].name, processes[i].outputString); 
    }

    // Now display average information
    averageWaitTime = (float)totalWaitTime / numProcesses;
    averageResponseTime = (float)totalResponseTime / numProcesses;
    printf("\nAverage waiting time: %f\n", averageWaitTime);
    printf("Average response time: %f\n", averageResponseTime);
    printf("Throughput: %d / %d\n", processesCompletedThroughput, cyclesThroughput);
}

/*
 * Name: Ethan Bielecki
 * Date: 11/14/2024
 * Description: The function resets or sets certain attributes
 * for each process in between each scheduling implementation
*/ 
void initProcesses(int numProcesses, Process processes[])
{
    for (int i = 0; i < numProcesses; i++)
    { 
        processes[i].remainingTime = processes[i].burstTime;
        processes[i].waitTime = 0;
        processes[i].startTime = 0;
        processes[i].responseTime = 0;

        memset(processes[i].outputString, '\0', sizeof(processes[i].outputString));
    }
}


/*
 * Name: Ethan Bielecki
 * Date: 11/11/2024
 * Description: This will be the main function to start the program
*/ 
int main()
{
    // Basic process information
    int numProcesses = 5;
    int cyclesThroughputCalc = 10;

    // Max 5 processes, max 128 char names
    Process processes[numProcesses];    

    // Input file with process info
    FILE* fileInput = fopen("input.txt", "r");

    if (fileInput == NULL)
    {
        printf("Error opening file. Does it exist?\n");
        return 1;
    }

    // Loop through all the processes in the file
    int i = 0;
    while (fscanf(fileInput, "%s %d %d", processes[i].name, &processes[i].burstTime, &processes[i].arrivalTime) == 3)
    {
        printf("Found: %s %d %d\n", processes[i].name, processes[i].burstTime, processes[i].arrivalTime);
        i++;
    }
    
    // Update numProcesses so we don't look at null data
    numProcesses = i;

    fclose(fileInput);

    initProcesses(numProcesses, processes);

    Process tempProcess;
    // Sort the arrays based on arrival time
    for (int i = 0; i < numProcesses - 1; i++)
    {
        for (int j = 0; j < numProcesses - i - 1; j++)
        {
            if (processes[j].arrivalTime > processes[j + 1].arrivalTime)
            {
                // Swap processes based on arrival time
                tempProcess = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = tempProcess;
            }
        }
    }

    // Start the different scheduling methods
    printf("\nStarting Scheduler simulation...\n");
    printf("\nFIFO variation:\n\n");
    FIFO(numProcesses, processes, cyclesThroughputCalc);
    initProcesses(numProcesses, processes); // Resets certain attributes
    printf("\n\nSJF variation\n\n");
    SJF(numProcesses, processes, cyclesThroughputCalc);
    initProcesses(numProcesses, processes); // Resets certain attributes
    printf("\n\nRound Robin variation\n\n");
    roundRobin(numProcesses, processes, cyclesThroughputCalc);
    printf("\n\nSimulation ended!\n");
    return 0;
}
