#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
int main(){
    struct termios tty;
    if (isatty(fileno(stdout)) == 0) {
        fprintf(stderr,"stdout not terminal\n");
        exit(1);
    }
    tcgetattr(fileno(stdout), &tty);
    tty.c_lflag &= ~(ISIG | ICANON);
    tty.c_cc[VMIN] = 1;
    tcsetattr(fileno(stdout), TCSAFLUSH, &tty);

    printf("What character is missing: K_TTEN?\n");
    char answer[2];
    read(fileno(stdin), answer, 2);

    printf("\n");
    if(answer[0] == 'I'){
        printf("Absoultely\n");
    }
    else{
        printf("No way\n");
    }
}