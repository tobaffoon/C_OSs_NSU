#include<stdlib.h>
int execvpe(char* name, char* argv[], char* envp[]);

int main(){
    char* args[] = {"printenvp", "mytxt", (char*)NULL};
    char* envs[] = {"WORD=WORDIK", "PATH=.", (char*)NULL};
    execvpe(args[0], args, envs);
    exit(EXIT_FAILURE);
}
