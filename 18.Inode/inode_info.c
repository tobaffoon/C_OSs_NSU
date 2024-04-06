#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<time.h>
#include<libgen.h>

void printmode(struct stat* buf);
void printuser(struct stat* buf);

int main(int argc, char* argv[]){
    struct stat stbuf;   

    for(int i = 1; argv[i] != NULL; i++){
        if(stat(argv[i], &stbuf) == -1){
            fprintf(stderr, "%s not found\n", argv[i]);
            continue;
        }
        printmode(&stbuf);
        printuser(&stbuf);

        if(stbuf.st_mode & S_IFREG){
            printf("%d", stbuf.st_size);
        }

        struct tm *lst_mod = localtime(&stbuf.st_mtime);
        printf("\t%02d/%d/%d %02d:%02d\t",
                lst_mod->tm_mday, lst_mod->tm_mon + 1, lst_mod->tm_year + 1900,
                lst_mod->tm_hour, lst_mod->tm_min);
        printf("%s\n", basename(argv[i]));
    }
}

void printmode(struct stat* buf){
    //file type
    switch (buf->st_mode & S_IFMT){
        case S_IFDIR:
            printf("d");
            break;
        case S_IFREG:
            printf("-");
            break;
        default:
            printf("?");
    }

    //owner
        //read
    if(buf->st_mode & S_IUSRR){
        printf("r"); 
    }
    else{
        printf("-");
    }
        //write
    if(buf->st_mode & S_IWUSR){
        printf("w"); 
    }
    else{
        printf("-");
    } 
        //execute
    if(buf->st_mode & S_IXUSR){
        printf("x"); 
    }
    else{
        printf("-");
    } 

    //group
        //read
    if(buf->st_mode & S_IRGRP){
        printf("r"); 
    }
    else{
        printf("-");
    }
        //write
    if(buf->st_mode & S_IWGRP){
        printf("w"); 
    }
    else{
        printf("-");
    } 
        //execute
    if(buf->st_mode & S_IXGRP){
        printf("x"); 
    }
    else{
        printf("-");
    } 

    //other
        //read
    if(buf->st_mode & S_IROTH){
        printf("r"); 
    }
    else{
        printf("-");
    }
        //write
    if(buf->st_mode & S_IWOTH){
        printf("w"); 
    }
    else{
        printf("-");
    } 
        //execute
    if(buf->st_mode & S_IXOTH){
        printf("x"); 
    }
    else{
        printf("-");
    }

    printf("  ");
}

#include<pwd.h>
#include<grp.h>

void printuser(struct stat* buf){
    struct passwd *pw;
    struct group *grp;
    
    pw = getpwuid(buf->st_uid);
    printf("%s ", pw->pw_name);

    grp = getgrgid(buf->st_gid);
    printf("%s\t", grp->gr_name);
}
