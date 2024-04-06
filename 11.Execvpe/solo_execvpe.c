#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>

extern char** environ;

int execvpe(char* name, char* argv[], char* envp[]){
    const char* path_env = getenv("PATH");
    if(clearenv() != 0){
        perror("Couldn't set envvironment");
        exit(EXIT_FAILURE);
    }

    while(*envp != NULL){
        putenv(*envp);
        envp++;
    }
    setenv("PATH", path_env, 0);
    
    execvp(name, argv);
    perror("Exec failed");
    exit(EXIT_FAILURE);
}
