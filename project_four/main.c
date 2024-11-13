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
} Process;


/*
 * Name: Ethan Bielecki
 * Date: 11/11/2024
 * Description: This function will perform first
 * in first out on the processes 
*/ 
void FIFO(int numProcesses, Process processes[])
{
    int totalWaitTime = 0;
    int totalResponseTime = 0;
    float averageWaitTime = 0;
    float averageResponseTime = 0;
    int cycles = 0;
    int processesCompleted = 0;
    float throughput = 0;

    for (int i = 0; i < numProcesses; i++)
    {
        // Standard variable initialization

        printf("Process %s info:\n", processes[i].name);
        
        // Wait time
        for (int j = 0; j < i; j++)
        {
            processes[i].waitTime += processes[j].burstTime;
        }
        
        totalWaitTime += processes[i].waitTime;
        printf("Wait time: %d\n", processes[i].waitTime);
        
        // Response time
        processes[i].responseTime = processes[i].waitTime - processes[i].arrivalTime;
        totalResponseTime += processes[i].responseTime;
        printf("Response time: %d\n\n", processes[i].responseTime);

        cycles += processes[i].burstTime;

        // Throughput check
        if (cycles <= 10)
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
    averageWaitTime = totalWaitTime / numProcesses;
    averageResponseTime = totalResponseTime / numProcesses;
    throughput = (float)processesCompleted / 10; // throughput over 10 cycles
    printf("Average waiting time: %f\n", averageWaitTime);
    printf("Average response time: %f\n", averageResponseTime);
    printf("Throughput: %f\n", throughput);
}

/*
 * Name: Ethan Bielecki
 * Date: 11/11/2024
 * Description: This function will perform
 * shortest job first on the processes
*/ 
void SJF(int numProcesses, Process processes[])
{
    // used to track if we have completed all processes
    int finished = 0;
    int currentCycle = 0;
    int processesCompletedThroughput = 0;

    // The 2d array below will be used to store the output strings
    // for each process for the chart creation.
    char outputStrings[5][512] = {'\0'};
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
            while (outputStrings[shortestProcessIndex][k] != '\0')
                k++;
            outputStrings[shortestProcessIndex][k] = '#';
            
            // If the process is done, increase our finished count
            if (shortestProcess->remainingTime == 0)
            {
                finished++;
                // Throughput
                if (currentCycle <= 10)
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
                while (outputStrings[i][j] != '\0')
                    j++;

                if (processes[i].arrivalTime > currentCycle)
                {
                    outputStrings[i][j] = ' ';
                }
                else
                {
                    outputStrings[i][j] = '_';
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
   float throughput = 0;

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
       printf("%-20s: %s\n", processes[i].name, outputStrings[i]); 
   }

   // Now display average information
   averageWaitTime = totalWaitTime / numProcesses;
   averageResponseTime = totalResponseTime / numProcesses;
   throughput = (float)processesCompletedThroughput / 10;
   printf("\nAverage waiting time: %f\n", averageWaitTime);
   printf("Average response time: %f\n", averageResponseTime);
   printf("Throughput: %f\n", throughput);
}

/*
 * Name: Ethan Bielecki
 * Date: 11/11/2024
 * Description: This function will perform round
 * robin scheduling on the processes
*/ 
void roundRobin(int numProcesses, Process processes[])
{
    Process* readyQueue[5];
    int finished = 0;
    int currentCycle = 0;

    while (finished != 4)
    {
        
        currentCycle++;
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
        processes[i].remainingTime = processes[i].burstTime;
        processes[i].waitTime = 0;
        i++;
    }
    
    // Update numProcesses so we don't look at null data
    numProcesses = i;

    fclose(fileInput);

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
    //printf("FIFO variation:\n\n");
    //FIFO(numProcesses, processes);
    printf("\n\nSJF variation\n\n");
    SJF(numProcesses, processes);
    //printf("\n\nRound Robin variation\n\n");
    //roundRobin(numProcesses, processes);
    printf("\n\nSimulation ended!\n");
    return 0;
}
