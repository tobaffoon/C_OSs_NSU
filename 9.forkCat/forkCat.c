#include<sys/types.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>

int main(){
    pid_t pid;

    if((pid = fork()) > 0){     /*parent*/
        printf("Parent is here with id %ld\n", getpid());
    }

    else if(pid == 0){   /*child*/
        // int large_file = open("mytxt", O_RDONLY);
        // char cur_char;
        // while(read(large_file, &cur_char, 1) != 0){
        //     printf("%c", cur_char);
        // }
        // close(large_file);
        if(fork() == 0){
            execl("/bin/cat", "cat", "mytxt", (char *) 0);  //child program ends here
            perror("couldn't execute cat");
            exit(EXIT_FAILURE);
        }
    }

    else{       /*couldn't fork*/
        perror("couldn't fork");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
