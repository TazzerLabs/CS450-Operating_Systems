#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "constants.h"

typedef struct args_cmds
{

    char* name;
    int input_flag;
    int output_flag;
    char* input_file;
    char* output_file;
    char** arguments;
    int total;
    
} command;

/* Enumerated list
 
    LESS    -> '<'
    GREATER -> '>'
    PIPER    -> '|'
    CMD    -> It's a command
 
 */
typedef enum Operator { LESS, GREATER, PIPER, CMD } Operator;

void syserror( const char* s); // done
Operator Operator_checker(const char* line_words); // done
int Pipe_counter(char* line_words[], int numWords); //done
void Get_cmds(char* line_words[], int num_words);
void Execute_cmds(command* cmds, int NumCmds);
void Quote_remover(char* words[], int num_words); // done

int main()
{
    // Buffer for reading one line of input
    char line[MAX_LINE_CHARS];
    char* line_words[MAX_LINE_WORDS + 1];

    // Loop until user hits Ctrl-D (end of input)
    // or some other input error occurs
    while(fgets(line, MAX_LINE_CHARS, stdin))
    {
        // Get the number of words user inserted
        int num_words = split_cmd_line(line, line_words);
    
        // Get the commands from user input and execute
        Get_cmds(line_words, num_words);
    
    } // end of while

    return 0;

} // end of main

// ==== syserror ===============================================================
//
//  Input:              const char* s -> the string to display before exiting
//
//  Output:             VOID
//
//  Description:        Caller sends and error message to be displayed to the
//                      function, and outputs and errno string with it. Exit with
//                      with error 1.
//
// ==============================================================================

void syserror(const char* s)
{
    extern int errno;

    fprintf( stderr, "%s\n", s );
    fprintf( stderr, " (%s)\n", strerror(errno) );
    exit( 1 );
    
} // end of syserror

// ==== Operator_checker =======================================================
//
//  Input:              char* line_words -> The words to be checked
//
//  Output:             Operator -> The enumerated defined list from Operator
//                                  that has {<, >, |} defined as
//                                  {less, greater, pipe} respectively.
//
//  Description:        Checks what operator is being used, and based on that
//                      returns the enum equivalent to caller.
//
//
// ==============================================================================

Operator Operator_checker(const char* line_words)
{
    if (strcmp(line_words, "<") == 0)
        return LESS;
    else if (strcmp(line_words, ">") == 0)
        return GREATER;
    else if (strcmp(line_words, "|") == 0)
        return PIPER;
    else
        return CMD;
    
} // end of Operator_checker

// ==== Pipe_counter ============================================================
//
//  Input:              char* line_words[] -> Array of char* with the words
//
//                      int numWords      -> Has the number of words in the array
//
//  Output:             int -> The number of pipes the user input has inserted
//
//  Description:        The caller sends over the array with the user input and
//                      the number of words inside.  With this the function
//                      checks through the whole array, counting out the number
//                      pipes within the array.  When finished, send the result
//                      number of pipes back to the caller
//
// ==============================================================================

int Pipe_counter(char* line_words[], int numWords)
 {
    
    int numPipes = 0;
    
    for (int i = 0; i < numWords; i++)
        if (Operator_checker(line_words[i]) == PIPER)
            numPipes++;
     
    return numPipes;
    
} // end of Pipe_counter

// ==== Quote_remover ===========================================================
//
//  Input:              char* words[]        ->  An array of char* that houses
//                                                  each command
//
//                      int num_words     -> The number of arguments passed
//
//  Output:             VOID
//
//  Description:        Removes the quotes from the commands
//
// ==============================================================================

void Quote_remover(char* words[], int num_words)
{
    
    // Loop through the words in the array and remove my quotes
    for (int i = 0; i < num_words; i++)
    {
        if (words[i][0] == '\"' || words[i][0] == '\'')
        {
        
            words[i]++; // incr word
            words[i][(strlen(words[i]-1)) - 2] = 0; // @ char of word = 0
 
        }
        
    } // end of for loop

} // end of Quote_remover

// ==== Get_cmds ================================================================
//
//  Input:               char* line_words[]   -> an array of char* that houses the
//                                              words from each line
//
//                       int num_words        -> the number of words
//
//                       command* cmds       -> empty array of command struct
//                                              data, needs to be filled
//
//                      int NumCmds         ->  the number of commands
//
//  Output:             int -> the number of commands needed to be executed
//
//  Description:        Function will parse through text given by user to be
//                      seperated and is to be utilized by the execution function
//                      for the commands.  This makes things easier for the
//                      Execute_cmds function via filling a struct with the data.
//                      Call Execute_cmds at end.
//
//
// ==============================================================================

void Get_cmds(char* line_words[], int num_words)
{

    // Variables
    int is_file = 0;
    int is_input_or_output = 0;
    int newest = 1;
    int argCount = 1;
    int CmdCount = 0;
    
    // Get the number of commands and allocate space for it in
    // the structure
    int NumCmds = (Pipe_counter(line_words, num_words)) + 1;
    command* cmds = malloc( (NumCmds) * sizeof(command));

    // allocate dynamic space for new arguments
    cmds[CmdCount].arguments = malloc(num_words * sizeof(char*));
    
    // loop through all the words
    for (int i = 0; i < num_words; i++)
     {
        
        // Variable that is used constantly for len of line_words
        int len_line = (strlen(line_words[i]) + 1);
        
        // if this is the first command at beginning/after pipe
        if (newest)
        {
        
            switch (Operator_checker(line_words[i]))
             {
             
                case PIPER: // a new pipe
                {
                    
                    // increment the Cmd count and allocate more space for the arguments
                    // also set newest to 0 for new info coming in
                    cmds[++CmdCount].arguments = malloc(sizeof(char*));
                    newest = 1;
                    break;
                }
                case CMD: // this is a command
                {
                    
                    // Add the command to the struct while dynamicilly getting space for it
                    strcpy((cmds[CmdCount].name = malloc(len_line)), line_words[i]);
                    strcpy((cmds[CmdCount].arguments[0] = malloc(len_line)), line_words[i]);
                    
                    // Update the total and flags for struct @ location
                    cmds[CmdCount].total = argCount = 1;
                    cmds[CmdCount].input_flag = 0;
                    cmds[CmdCount].output_flag = 0;
                    
                    // No file to report
                    is_file = 0;
                    
                    newest = 0;
                    
                    break;
                }
                    
            } // end of switch
        } // end of if
        else // not new
        {
            switch (Operator_checker(line_words[i]))
            {
                case GREATER: // Ouput >, set flags
                {
                    
                    is_file = 1;
                    is_input_or_output = 0;
                    cmds[CmdCount].output_flag = 1;
                    
                    break;
                }
                case LESS: //input <, set flags
                {
                
                    is_file = 1;
                    is_input_or_output = 1;
                    cmds[CmdCount].input_flag = 1;
 
                    break;
                }
                case PIPER: // create space and set flag for incoming
                {
                    cmds[++CmdCount].arguments = malloc(num_words * sizeof(char*));
                    newest = 1; // Pipe before = cmd coming in
                    break;
                }
                case CMD: // this is an argument
                {
                    // check to see if file flag has been set
                    if (is_file)
                    {
                        // check to see if input or output,
                        // if output, create new space for the output_file name &
                        // copy over name from the user input
                        // else input, and do the same for input
                        if (is_input_or_output)
                            strcpy((cmds[CmdCount].input_file = malloc(len_line)), line_words[i]);
                        else
                            strcpy((cmds[CmdCount].output_file = malloc(len_line)), line_words[i]);

                    } // end of is_file
                    else // there is no file
                    {
                        
                        // create more space for my arguments @ location and store in
                        // user input @ i location
                        strcpy((cmds[CmdCount].arguments[argCount++] = malloc(len_line)), line_words[i]);
                        
                        // increment number of commands for local and @ Cmd location
                        cmds[CmdCount].total++;
                        
                    }
                    
                    break;
                    
                } // end of CMD case

            } // end of switch
            
        } // end of else
    
    } // end of for loop
    
    
    // Execute the new commmands in cmd struct
    Execute_cmds(cmds, NumCmds);

} // end of Get_cmds

// ==== Execute_cmds ============================================================
//
//  Input:              command* cmds -> an array of command data type that houses
//                                      the commands needing to be executed
//
//                      int NumCmds  -> the number of commands needing to be
//                                      executed. Also has how many pipes -1.
//
//
// output:              VOID
//
// Description:         The function will emulate a UNIX shell via execution of
//                      commands given by the user.  Pipes will be created
//                      first for set up.
//
//
// ==============================================================================

void Execute_cmds(command* cmds, int NumCmds)
{

    int pfd[(NumCmds-1) * 2];

    // Create (the number of commands -1) * 2 pipes
    for (int i = 0; i < (NumCmds-1); i++)
        if (pipe(pfd + i * 2) < 0)
            syserror( "Error creating pipes" );

    // Loop through all the cmds to execute
    for (int i = 0; i < NumCmds; i++)
     {
        
        // Set the @ location arguments @ location of the command count to NULL
        cmds[i].arguments[cmds[i].total] = NULL;
        
        Quote_remover(cmds[i].arguments, cmds[i].total);

        switch (fork())
        {
        
            case -1:
                syserror( "Error creating fork" );
            case 0: // Fork worked!
            {
                // Open output file if necessary
                if (cmds[i].output_flag)
                {
                    
                    int fd;
                    
                    if ((fd = open(cmds[i].output_file, O_WRONLY | O_CREAT | O_TRUNC, 0777)) < 0)
                        syserror("Error opening output file");
            
                    if (dup2(fd, STDOUT_FILENO) < 0)
                        syserror("Error connecting STDOUT to file descriptor");
                }
                
                // Open input file if necessary
                if (cmds[i].input_flag)
                 {
                     
                    int fd;
                    
                    if ((fd = open(cmds[i].input_file, O_RDONLY)) < 0)
                        syserror("Error opening input file");

                    if (dup2(fd, STDIN_FILENO) < 0)
                        syserror("Error connecting STDIN to file descriptor");
                }

                // if not first input from user, then need to connect to input pipe
                if (i != 0)
                    if (dup2(pfd[(i-1)*2], STDIN_FILENO) < 0)
                        syserror( "Error connecting STDIN to pipe read pipe" );
                

                // if we haven't reached the end & can be first input, connect to output pipe
                if (i != NumCmds-1)
                    if (dup2(pfd[(i*2)+1], STDOUT_FILENO) < 0)
                        syserror( "Error connecting STDOUT to write pipe" );
                

                // Close all pipes
                for (int i = 0; i < (NumCmds-1) * 2; i ++)
                    if (close(pfd[i]) < 0)
                        syserror( "Error closing pipes" );
    

                // Perform execution and error if exevp fails
                execvp(cmds[i].name, cmds[i].arguments);
            
                syserror( "Error could not execvp" );
                
            } // end of successful fork

        } // end of switch

    } // end of for loop


    // Close remainder pipes
    for (int i = 0; i < (NumCmds-1) * 2; i ++)
        if (close(pfd[i]) < 0)
            syserror( "Error closing remainder pipes" );
    
    
    //Wait for all children to die
    while ( wait(NULL) != -1)
        ;

} // end of Exec_cmds
