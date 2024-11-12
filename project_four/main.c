#include <stdio.h>
#include <string.h>

/*
 * Name: Ethan Bielecki
 * Date: 11/11/2024
 * Description: This function will perform first
 * in first out on the processes 
*/ 
void FIFO(int numProcesses, int nameLength, char processNames[numProcesses][nameLength], int processBurst[], int processArrival[])
{
    int totalWaitTime = 0;
    int totalResponseTime = 0;
    int averageWaitTime = 0;
    int averageResponseTime = 0;
    int cycles = 0;
    int processesCompleted = 0;
    int throughput = 0;

    for (int i = 0; i < numProcesses; i++)
    {
        // Standard variable initialization
        int waitTime = 0;
        int responseTime = 0;

        printf("Process %s info:\n", processNames[i]);
        
        // Wait time
        for (int j = 0; j < i; j++)
        {
            waitTime += processBurst[j];
        }
        
        totalWaitTime += waitTime;
        printf("Wait time: %d\n", waitTime);
        
        // Response time
        responseTime = waitTime - processArrival[i];
        totalResponseTime += responseTime;
        printf("Response time: %d\n", responseTime);

        // Throughput check
        if (cycles < 10 && (cycles + processBurst[i]) <= 10)
        {
            cycles += processBurst[i];
            processesCompleted++;
        }
    }

    printf("\n"); 
    
    // Chart creation
    // We loop over each process
    for (int i = 0; i < numProcesses; i++)
    {
        printf("%-20s: ", processNames[i]);
        
        // We now loop over each process before the current one
        for (int j = 0; j <= i; j++)
        {
            // Now loop through those processes burst times
            for (int k = 0; k < processBurst[k]; k++)
            {
                // if j and i are equal, that means the process is running
                if (j == i)
                    printf("#");
                else
                    printf("_");
            }
        }
        printf("\n");
    }
    printf("\n");

    // Average information
    averageWaitTime = totalWaitTime / numProcesses;
    averageResponseTime = totalResponseTime / numProcesses;
    throughput = processesCompleted / 10; // throughput over 10 cycles
    printf("Average waiting time: %d\n", averageWaitTime);
    printf("Average response time: %d\n", averageResponseTime);
    printf("Throughput: %d\n", throughput);
}

/*
 * Name: Ethan Bielecki
 * Date: 11/11/2024
 * Description: This function will perform
 * shortest job first on the processes
*/ 
void SJF(int numProcesses, int nameLength, char processNames[numProcesses][nameLength], int processBurst[], int processArrival[])
{
    printf("Not implemented yet!");
}

/*
 * Name: Ethan Bielecki
 * Date: 11/11/2024
 * Description: This function will perform round
 * robin scheduling on the processes
*/ 
void roundRobin(int numProcesses, int nameLength, char processNames[numProcesses][nameLength], int procesBurst[], int processArrival[])
{
    printf("Not implemented yet!");
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
    int nameLength = 128;
    
    // Max 5 processes, max 128 char names
    char processNames[numProcesses][nameLength];
    int processBurst[numProcesses];
    int processArrival[numProcesses];
    
    // Input file with process info
    FILE* fileInput = fopen("input.txt", "r");

    if (fileInput == NULL)
    {
        printf("Error opening file. Does it exist?\n");
        return 1;
    }

    // Loop through all the processes in the file
    int i = 0;
    while (fscanf(fileInput, "%s %d %d", processNames[i], &processBurst[i], &processArrival[i]) == 3)
    {
        printf("Found: %s %d %d\n", processNames[i], processBurst[i], processArrival[i]);
        i++;
    }
    
    // Update numProcesses so we don't look at null data
    numProcesses = i;

    fclose(fileInput);

    char tempName[nameLength];
    int tempBurst;
    int tempArrival;
    // Sort the arrays based on arrival time
    for (int i = 0; i < numProcesses - 1; i++)
    {
        for (int j = 0; j < numProcesses - i - 1; j++)
        {
            if (processArrival[j] > processArrival[j + 1])
            {
                // Swap names
                strcpy(tempName, processNames[j]);
                strcpy(processNames[j], processNames[j + 1]);
                strcpy(processNames[j + 1], tempName);
                
                // Swap Burst
                tempBurst = processBurst[j];
                processBurst[j] = processBurst[j + 1];
                processBurst[j + 1] = tempBurst;
                
                // Swap arrival
                tempArrival = processArrival[j];
                processArrival[j] = processArrival[j + 1];
                processArrival[j + 1] = tempArrival;
            }
        }
    }

    // Start the different scheduling methods
    printf("\nStarting Scheduler simulation...\n");
    printf("FIFO variation:\n");
    FIFO(numProcesses, nameLength, processNames, processBurst, processArrival);
    printf("\n\nSJF variation\n");
    SJF(numProcesses, nameLength, processNames, processBurst, processArrival);
    printf("\n\nRound Robin variation\n");
    roundRobin(numProcesses, nameLength, processNames, processBurst, processArrival);
    printf("\n\nSimulation ended!\n");
    return 0;
}
