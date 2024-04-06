#include<sys/types.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<unistd.h>
#include<wait.h>

int main(int argc, char* argv[]){
    if(argc < 2){
        fprintf(stderr, "No command passed as an argument\n");
        exit(EXIT_FAILURE);
    }
    /*normal routine starts here*/
    pid_t pid;
    int status;

    if((pid = fork()) > 0){    /*parent*/
        while(wait(&status) == -1){
            printf("Forked program was interrupted\n");
        }
        
        if(WIFEXITED(status)){
            printf("\nChild exited with exit status: %d\n", WEXITSTATUS(status));
        }

        else{
            printf("\nChild program didn't exit\n");
        }
    } 

    else if(pid == 0){      /*child*/
        execvp(argv[1], argv + 1);
    }

    else{               /*couldn't fork*/
        perror("couldn't fork");
        exit(EXIT_FAILURE);
    }
    
    exit(EXIT_SUCCESS);
}
