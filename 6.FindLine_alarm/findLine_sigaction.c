#include<sys/types.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<signal.h>
#include<errno.h>

extern int errno ;
const int answer_time = 5;

typedef struct _dynArr{
    int *arr;
    int size;
    int cap;
} dArr;

dArr offs;
int my_fd;

void enlarge(dArr* a){
    int plus_size = a->cap;
    a->arr = reallocf(a->arr, (a->cap + plus_size) * sizeof(int));
    if(a->arr == NULL){
        perror("Couldn't allocate enough space for array");
        exit(EXIT_FAILURE);
    }
    a->cap += plus_size;
    return;
}

void signal_handler(){
    printf("\n%d seconds period is over\n", answer_time);

    char* cur_line = malloc(0);
    for(int i = 0; i < offs.size; i++){
        lseek(my_fd, offs.arr[i], SEEK_SET);

        int cur_line_len = offs.arr[i + 1] - offs.arr[i];
        cur_line = realloc(cur_line, cur_line_len);
        if(cur_line == NULL){
            perror("Couldn't allocate enough space for the line");
            exit(EXIT_FAILURE);
        }

        if(read(my_fd, cur_line, cur_line_len - 1) == -1){              //-1 argument to not read last \n and replace it with \0
            perror("Couldn't read from file");
            exit(EXIT_FAILURE);
        }
        cur_line[cur_line_len-1] = '\0';
        printf("line %d is: %s\n", i+1, cur_line);
    }
    free(cur_line);
    exit(EXIT_SUCCESS);
}

int main(int argc, char** argv){
    my_fd = open(argv[1], O_RDONLY);
    if(my_fd == -1){
        perror("Couldn't open file");
        exit(EXIT_FAILURE);
    }

    struct sigaction s_handle;
    s_handle.sa_handler = signal_handler;
    sigemptyset(&s_handle.sa_mask);
    s_handle.sa_flags = 0;
 
    struct sigaction ign_handle;
    ign_handle.sa_handler = SIG_IGN;
    sigemptyset(&ign_handle.sa_mask);
    ign_handle.sa_flags = 0;

    offs.cap = 5;
    offs.arr = malloc(5 * sizeof(int));
    offs.arr[0] = 0;
    offs.size = 0;

    char cur_symb;
    ssize_t read_res;
    
    while(read_res = read(my_fd, &cur_symb, 1)){
        if(read_res == -1){
            perror("Couldn't read from file");
            exit(EXIT_FAILURE);
        }
        offs.arr[offs.size+1]++;
        if(cur_symb == '\n'){
            offs.size++;
            if(offs.size+1 == offs.cap){
                enlarge(&offs);
            }
            offs.arr[offs.size+1] = offs.arr[offs.size];
        }
    }

    int cur_line_num;
    long cur_line_len;
    char* cur_line = malloc(0);
    char cur_c;

    sigaction(SIGALRM, &s_handle, NULL);
    alarm(answer_time);
    while(1)
    {
        cur_line_num = 0;
        while((cur_c = getchar()) != EOF && cur_c != '\n'){
            cur_line_num *= 10;
            if(cur_c >= '0' && cur_c <= '9'){
                cur_line_num += cur_c - '0';
            }
            else{
                cur_line_num = -1;
                while((cur_c = getchar()) != EOF && cur_c != '\n') ;    //read wrong line to the end
                break;
            }
        }

        if(cur_line_num < 0 || cur_line_num > offs.size){
            printf("Invalid line index\n");
        }

        else{
            if(cur_line_num == 0){
                break;
            }
            sigaction(SIGALRM, &ign_handle, NULL);
            lseek(my_fd, offs.arr[cur_line_num-1], SEEK_SET);

            cur_line_len = offs.arr[cur_line_num] - offs.arr[cur_line_num-1];
            cur_line = realloc(cur_line, cur_line_len);
            if(cur_line == NULL){
                perror("Couldn't allocate enough space for the line");
                exit(EXIT_FAILURE);
            }

            if(read(my_fd, cur_line, cur_line_len - 1) == -1){              //-1 argument to not read last \n and replace it with \0
                perror("Couldn't read from file");
                exit(EXIT_FAILURE);
            }
            cur_line[cur_line_len-1] = '\0';
            printf("line %d is: %s\n", cur_line_num, cur_line);
        }
        alarm(answer_time);
        sigaction(SIGALRM, &s_handle, NULL);
    }
    sigaction(SIGALRM, &ign_handle, NULL);

    free(cur_line);
    
    if(close(my_fd) == -1){
        perror("Couldn't close file");
        exit(EXIT_FAILURE);
    }

    free(offs.arr);
    exit(EXIT_SUCCESS);
}
