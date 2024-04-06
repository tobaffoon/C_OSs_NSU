#include<sys/types.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<wait.h>

int main(){
    pid_t pid;
    int status;

    if((pid = fork()) > 0){     /*parent*/
        while(wait(&status) == -1){
            printf("Forked program was interrupted\n");
        }

        if(WIFEXITED(status)){
            if(WEXITSTATUS(status) == 0)    /*cat(1) in child completed*/
                printf("\nParent is here with id %ld\n", getpid());
            else
                printf("\nChild program failed, with exit status: %d\n", WEXITSTATUS(status));
        }
    }

    else if(pid == 0){   /*child*/
        // int large_file = open("mytxt", O_RDONLY);
        // char cur_char;
        // while(read(large_file, &cur_char, 1) != 0){
        //     printf("%c", cur_char);
        // }
        // close(large_file);
        
        execl("/bin/cat", "cat", "mytxt", (char *) 0);  //child program ends here
    }

    else{       /*couldn't fork*/
        perror("couldn't fork");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
