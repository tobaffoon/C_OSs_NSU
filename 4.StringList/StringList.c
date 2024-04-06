#include <stdlib.h>
#include <stdio.h> 
#include <string.h> 
#define LEN_MAX 1001

typedef struct __Node
{
    char *value;
    struct __Node* next;
} Node;

int main(){
    char* buf = malloc(LEN_MAX * sizeof(char));
    if(buf == NULL){
        perror("Buffer too large");
        exit(EXIT_FAILURE);
    }

    Node* head = malloc(sizeof(Node));
    Node* last = head;

    int cur_len;
    while(1){
        ( void )fgets(buf, LEN_MAX, stdin);
        if(ferror(stdin)){
            fprintf(stderr, "Couldn't read from file");
            exit(EXIT_FAILURE);
        }

        // if(feof(stdin)){
        //     fprintf(stderr, "File unexpectedly ended");
        //     exit(1);
        // }

        cur_len = strlen(buf);
        if(strcmp(".\n", buf) == 0){
            break;
        }

        if(cur_len + 1 > LEN_MAX){
            printf("String too long");
            continue;
        }

        last->next = calloc(1, sizeof(Node));
        if(last->next == NULL){
            perror("Not enough space to add an element");
            exit(EXIT_FAILURE);
        }
		
        last = last->next;
        last->value = malloc(cur_len + 1);
        if(last->value == NULL){
            perror("Not enough space to add a string");
            exit(EXIT_FAILURE);
        }
        (void) strcpy(last->value, buf);
    }

    Node *to_free = head;
    while(head->next != NULL){
        printf("%s", head->next->value);
        head = head->next;
        free(to_free);
        to_free = head;
    }
	free(to_free);
	
    free(buf);
    exit(EXIT_SUCCESS);
}