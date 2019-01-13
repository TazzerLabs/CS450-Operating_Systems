// =============================================================================
//
//  Programmers: Alissa Greaney & Johnathan Soto
//  Program:     p1.c
//  Language:    C
//
//  Description: 
//
//
//
//
// =============================================================================

// Included Libraries
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int staticVar = 0;

int main(void)
{
    
    staticVar = getpid();
    
    // sleep and printf in infinite loop
    while (1)
    {
        // sleep causes the program to wait x seconds
        sleep(10);
        printf("Address: %x; Value: %d\n", &staticVar, staticVar);
    }
    
    return 0;
    
} // end of main
