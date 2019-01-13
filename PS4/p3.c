// =================================================================
//
//  Programmers: Alissa Greaney & Johnathan Soto
//
//  Language: C
//
//  Date: December 4, 2018
//
//  Description: Write a C program that does the following, in order:
//               1) Opens the file named as the command-line argument
//               for reading
//
//               2) Reads a value from the file
//
//               3) Forks a child process that calls unlink() to
//                  delete the file
//
//               4) Waits on the child process
//
//               5) Tries to read the now-deleted file
//
//               In an initial comment, explain what happens when
//               you run your program, and why.
//                  A: When the program runs, it reads in the file
//                      and outputs the contents of the file till EOF.
//                      Upon doing so, it then creates a child process
//                      that unlinks a file: which deletes it.  However
//                      I believe it is placed in a queue to be perminately
//                      deleted.  Since this is the case, when trying to
//                      read the now deleted file, both the parent and
//                      child process are able to read from the file
//                      without fail, due to the file still being
//                      accessible.  When the program ends, the now
//                      deleted file should no longer be in the directory.
//                      Also potenetially perminately deleted by that time.
//
// =================================================================


//Libraries
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Main
int main ( int argc, char *argv[] )
{

    char x;

    // We assume argv[1] is a filename to open based on ./p3.x file
    FILE* file = fopen( argv[1], "r" );

    // Could not open the file
    if ( file == 0 )
    {
        printf( "Could not open file for reading\n" );
        return -1;
    }
    
    
    printf("Contents of file:\n ");
    while (( x = (fgetc( file ) )) != EOF)
    {
        printf("%c", x);
    }
    
    // Create a child proc
    int pid = fork();
    
    // if pid is 0 then we have the child process
    if (!pid)
    {
        printf("In child\n");
        pthread_mutex_lock(&mutex);
        // Since child has been successfully forked, delete file via
        // unlink()
        if (unlink(argv[1]) != 0) // Unsuccessful unlink
        {
            printf("Could not unlink file!\n");
            return -1;
        }
        else
            printf("Successfully unlinked file!\n");
        
        pthread_mutex_unlock(&mutex);
    }
    
    // Wait on child process here
    int status;
    waitpid(pid, &status, 0);
    
    // Now try to read the file that is now deleted
    fseek(file, 0, SEEK_SET);
    printf("Contents of file after unlink:\n");
    while(( x = (fgetc( file ) )) != EOF)
    {
        printf("%c", x);
    }

    return 0;
    
}
