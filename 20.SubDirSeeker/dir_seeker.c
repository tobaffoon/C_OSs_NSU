#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>

int match(char* pattern, char* str){
        int pattern_id = 0,
            str_id = 0;

        // check pattern for '*' because it may be empty string. Thus the end of str doesn't mean mismatch
        // check pattern for '/' because we may reached the end of directory name
        while((str_id != -1 && pattern[pattern_id] != '\0' && str[str_id] != '\0') || pattern[pattern_id] == '*' || pattern[pattern_id] == '/'){
            switch(pattern[pattern_id]){
                //разобрать случай "/*" - пустая поддиректория - фигня какая-то
                case '/':
                    return pattern_id;
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

                    if(pattern[pattern_id] == '/'){    //'/' at the end means match any directory
                        return pattern_id;
                    }

                    //look through the whole str
                    while(str[str_id] != '\0'){
                        if(pattern[pattern_id] == '?' || str[str_id] == pattern[pattern_id]){ //look for matching substr if beginnings (1st letters) match
                            int sub_match_results = match(pattern + pattern_id + 1, str + str_id + 1); //match strings tarting from next character
                            if(sub_match_results != -1){
                                return sub_match_results;
                            }
                        }
                        str_id++;
                    }
                    break;
                    
                default:
                    if(str[str_id] != pattern[pattern_id]){  //matching failed
                        str_id = -1;
                    }
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

int find_match(char* path, char* pattern){
    DIR* current_directory;
    if((current_directory = opendir(path)) == NULL){
        perror("Couldn't open working directory");
        exit(EXIT_FAILURE);
    }
    
    char *entry;                    //entry name within current directory
    char *absolute_entry_address;          //absolute entry address
    struct dirent* current_entry;   //entry structure within current directory
    struct stat stbuf;              //info about entry to check for subdirectories
    int match_results;              //return value of match function
    int old_path_length = strlen(path);

    while((current_entry = readdir(current_directory)) != NULL){
        /*get entry name and check if it's trivial (. or ..)*/
        entry = current_entry->d_name;
        if(strcmp(entry, ".") == 0 || strcmp(entry, "..") == 0){ 
            continue; //skip trivial
        }

        /*get absolute entry address*/
        absolute_entry_address = malloc(old_path_length + strlen(entry) + 2); //+2 for '/' and '\0'
        strcpy(absolute_entry_address, path);
        absolute_entry_address[old_path_length] = '/';
        strcpy(absolute_entry_address + old_path_length + 1, entry); //+1 because we added '/'

        stat(absolute_entry_address, &stbuf);

        /*match entry with pattern*/
        match_results = match(pattern, entry);
        //regular files matched
        if(match_results == 0){
            printf("%s\n", absolute_entry_address);
        }
        //directory matched => continue looking in it with altered pattern
        else if(((stbuf.st_mode & S_IFMT) == S_IFDIR) && match_results != -1){
            find_match(absolute_entry_address, pattern + match_results + 1); //match_results contains index of '/', +1 means next character
        }
    }
    
    closedir(current_directory);
}

int main(int argc, char** argv){
    if(argc < 2){
        fprintf(stderr, "No file pattern provided");
        exit(EXIT_FAILURE);
    }
    
    long size = pathconf(".", _PC_PATH_MAX);
    char* this_directory = getcwd(NULL, (size_t)size);

    find_match(this_directory, argv[1]);  
}
