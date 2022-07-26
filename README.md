# POSIX Threads Project

## About the project
- Coded in C
- Gedit to code 
- Terminal to test and run my code

## Concepts utilized in the project 
- pthreads
- fork

## Approach
By default the code sets the array size to 10,000 this can be changed when you run the code (check the section below) I took the usual approach of assigning the threads to each process using the for-loop iteration as done in the labs, however, as for the rest I took the approach of saving the count of prime-numbers from each process to a file then summed up the numbers in the end to display the total count, then I deleted the file.

## To compile the code
write this in the terminal from the current dir: `gcc -pthread main.c`

## To run the compiled executable file:
- with the default array size (10K): `./a.out`
- with custom array size add size after: `./a.out [size]` | Example: `./a.out 100`
