#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void printUID(){
    printf("Real user ID is %d\n", getuid());
    printf("But effective user ID is %d\n", geteuid());
}

int main(){
    printUID();
    FILE* test = fopen("testFile", "r+");
    if(test != NULL){
        fclose(test);
        exit(EXIT_SUCCESS);
    }

    else{
        perror("Couldn't open file");
        if(seteuid(getuid()) != 0){
            perror("Couldn't set effective UID to the real one");
        }
        printUID();
    }
    exit(EXIT_SUCCESS);
}