/*
PROJECT DONE BY: 
    Name:   Adam Ahsan
    ID:     6724395
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>

// Global variable
sem_t *mySem;

struct arr {
    int* arr, beg, end, size, count;
};


// Function to check if the number is prime or not
int isPrime(int num)
{
    if (num < 2 || (num % 2 == 0 && num != 2))
        return 0;
    for (int i = 3; i <= num/2; i+=2)
        if (num % i == 0)
            return 0;
    return 1;
}

void* checkPrime(void* lparams)
{
    int count = 0;
    
    // Save the structure into a variable to use it
    struct arr* var = (struct arr *)lparams;
    
    // Every element is passed to the function isPrime 
    // `count` is incremented if its a prime number
    for(int i = var->beg; i < var->end; i++)
        if (isPrime(var->arr[i]) == 1)
            count++;

    // count is saved in the structure
    var->count = count;
}

int main(int argc, char **argv)
{    
    int     numOfProcesses, numOfThreads, arraySize;
    int*    array;
    struct  arr holder;
    FILE* fileResult = fopen("result.txt", "w");
    
    // If no value passed for array size then it is set to 10,000 
    if (argc > 1 && (arraySize = atoi(argv[1])));
    else
        arraySize = 10000;
    printf("Array size set to %d\n", arraySize);
    
    // Get number of cores on the computer to create processes
    numOfProcesses = get_nprocs();
    printf("System has %d processors configured and "
                   "%d processors available.\n", get_nprocs_conf(), numOfProcesses);
                   
    // Setting the number of threads per process
    if (numOfProcesses < 2)
        numOfThreads = 1;
    else
        numOfThreads = numOfProcesses / 2;
    printf("%d thread(s) created for each process\n%d threads created in total\n\n", numOfThreads, numOfThreads * numOfProcesses);
    
    // Assigning memory and Initializing the array
    array = (int *)malloc(arraySize * sizeof(int));
    for(int i = 0; i < arraySize; i++)
        array[i] = i;
    
    // Open Semaphore 
    mySem = sem_open("/semName", O_CREAT, 0666, 1);

    // Create an array to store the process id's and set them to 0
    pid_t children[numOfProcesses];
    memset(children, 0, numOfProcesses * sizeof(pid_t));

    // Running the loop for the amount of processes and creating threads accordingly
    for(int i = 0; i < numOfProcesses; i++)
    {
        // Assigning what section of the array will each process take
        holder.beg = i * arraySize / numOfProcesses;
        holder.end = (i + 1) * arraySize / numOfProcesses;
        holder.size = holder.end - holder.beg;
        
        // Creates a new process & enters "if" condition if it's a child process
        children[i] = fork();
        if (children[i] == 0) {
            printf("Process %d | Start: %d, End: %d\n", i+1, holder.beg, holder.end);
            
            // Inside child process
            pthread_t handle[numOfThreads];
            struct arr params[numOfThreads];
            
            // Setting the threads
            for(int j = 0; j < numOfThreads; j++)
            {
                params[j].arr = array;
                params[j].beg = j * holder.size / numOfThreads + holder.beg;
                params[j].end = (j + 1) * holder.size / numOfThreads + holder.beg;
                printf("\tP%d - Thread %d:\n\t\tStart: %d, End: %d\n", i+1, j, params[j].beg, params[j].end);
                pthread_create(&handle[j], NULL, checkPrime, (void *)&params[j]);
            }
            printf("\n");
            
            int primeCount = 0;
            
            // Joining the threads and sum the count of prime numbers
            for(int j = 0; j < numOfThreads; j++)
            {
                pthread_join(handle[j], NULL);
                primeCount += params[j].count;
            }
            printf("Process %d found %d prime numbers\n", i+1, primeCount);
            
            // save the count to the file
            sem_wait(mySem); // Lock
            fprintf(fileResult, "%d\n", primeCount);
            fflush(fileResult);
            sem_post(mySem); // Unlock
            
            return 0;
        }
    }// for end
    
    // wait for the processes to end
    for(int i = 0; i < numOfProcesses; i++)
        waitpid(-1, NULL, 0);
    
    // Closing and Unlinking the semaphore
    sem_close(mySem);
    sem_unlink("/semName");
    
    // Close the file and open in reading mode
    fclose(fileResult);
    fileResult = fopen("result.txt", "r");
    
    int num, primeCount = 0;
    
    // Adding the results saved in the file to primeCount and printing it
    while(fscanf(fileResult, "%d", &num) != EOF) primeCount += num;
    
    printf("---------------------------------------\n");
    printf("Total count of prime numbers: %d\n", primeCount);
    printf("---------------------------------------\n");
    
    // Closing and Deleting the file
    fclose(fileResult);
    remove("result.txt");
    
    // Free allocated memory
    free(array);
    
    return 0;
}
