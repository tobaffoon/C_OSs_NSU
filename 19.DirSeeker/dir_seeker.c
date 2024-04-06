#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int match(char* pattern, char* str){
        int pattern_id = 0,
            str_id = 0;

        // check pattern for '*' because it may be empty string. Thus the end of str doesn't mean mismatch
        while((str_id != -1 && pattern[pattern_id] != '\0' && str[str_id] != '\0') || pattern[pattern_id] == '*'){
            switch(pattern[pattern_id]){
                case '?':           //skip one symbol as it can be any symbol
                    pattern_id++;
                    str_id++;
                    break;
                case '*':
                    while(pattern[pattern_id] == '*'){  //skip all *
                        pattern_id++;
                    }

                    if(pattern[pattern_id] == '\0'){    //* at the end means match any
                        return 0;
                    }

                    //look through the whole str
                    while(str[str_id] != '\0'){
                        if(pattern[pattern_id] == '?' || str[str_id] == pattern[pattern_id]){ //look for matching substr if beginnings (1st letters) match
                            if(match(pattern + pattern_id + 1, str + str_id + 1) == 0){ //if strings starting from next character match
                                return 0;
                            }
                        }
                        str_id++;
                    }
                    break;
                    
                default:
                    if(str[str_id] != pattern[pattern_id])  //matching failed
                        str_id = -1;
                    else{
                        pattern_id++;
                        str_id++;
                    }
            }
        }

        //if mismatch didn't happen and we exhausted both strings
        if(str_id != -1 && pattern[pattern_id] == '\0' && str[str_id] == '\0'){
            return 0;
        }

    return -1;
}

int main(int argc, char** argv){
    if(argc < 2){
        fprintf(stderr, "No file pattern provided");
        exit(EXIT_FAILURE);
    }
    
    long size = pathconf(".", _PC_PATH_MAX);
    char* current_directory = getcwd(NULL, (size_t)size);

    DIR* this_directory;
    if((this_directory = opendir(current_directory)) == NULL){
        perror("Couldn't open working directory");
        exit(EXIT_FAILURE);
    }

    char *pattern = argv[1];
    char *entry;
    struct dirent* current_entry;

    while((current_entry = readdir(this_directory)) != NULL){
        entry = current_entry->d_name;
        if(match(pattern, entry) == 0){
            printf("%s\n", entry);
        }
    }
    
    closedir(this_directory);
}
