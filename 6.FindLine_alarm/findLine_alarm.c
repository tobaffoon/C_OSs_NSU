#include<sys/types.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<signal.h>

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
    // while(a->arr == NULL && plus_size){
    //     plus_size /= 2;
    //     a->arr = malloc((a->cap + plus_size) * sizeof(int));
    // }
    // if(plus_size == 0){
    //     perror("Couldn't allocate enough space for array");
    //     exit(EXIT_FAILURE);
    // }
    a->cap += plus_size;
    return;
}

//try to redo with sigaction (difference between sigset)
void signal_handler(int signal_n){
    printf("%d seconds period is over\n", answer_time);

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


int main(){
    my_fd = open("mytxt", O_RDONLY);
    if(my_fd == -1){
        perror("Couldn't open file");
        exit(EXIT_FAILURE);
    }

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
    int cur_line_len;
    char* cur_line = malloc(0);
    int scan_res;

    signal(SIGALRM, &signal_handler);
    alarm(answer_time);

    for (scan_res = scanf("%d", &cur_line_num); cur_line_num != 0; scan_res = scanf("%d", &cur_line_num))
    {
        signal(SIGALRM, SIG_IGN);
        waraws;
        if(cur_line_num < 0 || cur_line_num > offs.size || scan_res == 0){
            printf("Invalid line index\n");
        }
        else{
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
        signal(SIGALRM, &signal_handler);
    }
    signal(SIGALRM, SIG_IGN);

    free(cur_line);
    // for (int i = 0; i < offs.size + 1; i++)
    // {
    //     printf("line's %d offset is %d\n", i+1, offs.arr[i]);
    // }
    
    if(close(my_fd) == -1){
        perror("Couldn't close file");
        exit(EXIT_FAILURE);
    }
    
    free(offs.arr);
    exit(EXIT_SUCCESS);
}
