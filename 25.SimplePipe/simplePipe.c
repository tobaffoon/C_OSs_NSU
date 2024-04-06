#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>

#define MSGSIZE 29


int main(int argc, char **argv){
    int fd[2]; 
    pid_t pid;
    static char msgout[MSGSIZE] = "nOt So UpPeR cASe, oR iS IT?";
    static char msgin[MSGSIZE];

    if (pipe(fd) == -1) {
        perror(argv[0]);
        exit(1);
    }
    if ((pid=fork()) > 0) { /* parent */
        write(fd[1], msgout, MSGSIZE);
    }
    else if (pid == 0) { /* child */
        read(fd[0], msgin, MSGSIZE);
        for (int i = 0; msgin[i] != '\0'; i++)
        {
            msgin[i] = toupper(msgin[i]);
        }
        puts(msgin);
    }
    else { /* cannot fork */
        perror(argv[0]);
        exit(2);
    }
    exit(0);
}