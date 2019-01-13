#include <iostream>
#include <chrono>
#include <sys/types.h>
#include <unistd.h>
using namespace std;

int GLOBAL = 1;

/*
void foo()
{}

void foo()
{
    GLOBAL++;
}*/
void foo()
{
    
    int* dum = new int;
    
}


int main()
{
 
    // For the function
    auto start = chrono::high_resolution_clock::now();
    
    // Function code here
    foo();
    
    auto diff = chrono::high_resolution_clock::now() - start;
    auto time = chrono::duration_cast<std::chrono::nanoseconds>(diff);
    
    cout << "\nelapsed time for my function: " << time.count();
    
    // For the syscall
    start = chrono::high_resolution_clock::now();
    
    // Syscall code here
    getpid();
    // The sysycall is not generated in the assembly code generated by the compiler, since 
    // it is most likely located in the unistd library files.

    diff = chrono::high_resolution_clock::now() - start;
    time = chrono::duration_cast<std::chrono::nanoseconds>(diff);
    
    cout << "\nelapsed time for my syscall: " << time.count();
    cout << endl;

    return 0;
    
}

