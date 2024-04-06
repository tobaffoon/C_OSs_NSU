#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {

	if (argc != 3) {
		perror("Wrong number of arguments, expected 3");
		return EXIT_FAILURE;
	}

	char* path = argv[1];
	char* pattern = argv[2];

	DIR* dirp;
	struct dirent* dp;

    if ((dirp = opendir(path)) == NULL) {
        perror("couldn't open");
        return 1;
    }

	int matches_count = 0;

	do {
        if ((dp = readdir(dirp)) != NULL) {
			char* name = dp->d_name;

			int i = 0;
			int j = 0;

			size_t name_len = strlen(name);
			size_t pat_len = strlen(pattern);

			while (i <= name_len && j <= pat_len) {
				
				if (pattern[j] == '/') {
					perror("Can't use / in pattern");
					return EXIT_FAILURE;
				}

				if (pattern[j] != '*' && pattern[j] != '?') {
					if (name[i] == pattern[j]) {
						j++;
						i++;
					}
					else {
						break;
					}
				}
				else if (pattern[j] == '?') {
					i++;
					j++;
				}
				else if (pattern[j] == '*') {
					
					if (pattern[j+1] == '\0') {
						j++;
						while (name[i] != '\0') {
							i++;
						}
					}
					else if (pattern[j+1] == name[i]) {
						j++;
						while (pattern[j+1] == pattern[j]) {
                            j++;
                        }
                        while (name[i+1] == name[i]) {
                            i++;
                        }
					}
					else {
						i++;
					}
				}
			}

			if (i == name_len + 1 && j == pat_len + 1) {
				printf("found %s\n", name);
				matches_count++;
			}
		}
    } while (dp != NULL);

	if (!matches_count) {
		printf("nothing found: %s\n", pattern);
	}

	closedir(dirp);

	return 0;
}
