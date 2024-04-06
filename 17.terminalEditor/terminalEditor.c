#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>

#define LINE_LEN 40
#define MAX_WORDS_N ((LINE_LEN) / 2 + 1)

//a word is counted starting from first space (' ') to delete this spaces
//a negative value is stored if only ' ' are in the word
typedef struct terminalWord{
	char word[LINE_LEN+1];
	int length;
	int is_empty;
} TermWord;

TermWord words[MAX_WORDS_N];
int words_count = 0;

TermWord* get_last_word(){
	return &(words[words_count-1]);
}

void clear_words(){
	words_count = 0;
}

void delete_word(int length){
	int fd = fileno(stdout);
	for (int i = 0; i < length; i++){
		write(fd, "\b", 1);	//delete characters of the last word
	}
	for (int i = 0; i < length; i++){
		write(fd, " ", 1);	//fill deletted spcae with ' '
	}
	for (int i = 0; i < length; i++){
		write(fd, "\b", 1);	//delete empty spaces
	}
}

void initTerminalWords(){
	for(int i = 0; i < MAX_WORDS_N; i++){
		words[i].length = 0;
		words[i].is_empty = 0;
	}
}

int main() {

	struct termios tty, safetty;
	int in_fd = fileno(stdin);
	int out_fd = fileno(stdout);

	if (!isatty(in_fd)) {
		perror("Not valid terminal");
		return EXIT_FAILURE;
	}

	tcgetattr(out_fd, &tty);
	safetty = tty;
	
	tty.c_lflag &= ~(ICANON | ECHO);
	tty.c_oflag &= ~OPOST;
	tty.c_cc[VMIN] = 1;
	tty.c_cc[VTIME] = 0;
    tcsetattr(in_fd, TCSANOW, &tty);
	
	char c;
    int current_line_len = 0;

	while (read(in_fd, &c, 1)) {

        //^H and DEL
		if (c == 8 || c == 127) {
            if(current_line_len == 0){  //if it's the end of the line
				continue;
            }

			write(out_fd, "\b \b", 3);
            current_line_len--;

			//delete one character from the last word
			if(words_count != 0){
				TermWord* last_word = get_last_word();
				if(last_word->length > 0){	//deleted normal symbol
					last_word->length--;
				}

				//deletted one word
				if(last_word->length == 0){
					words_count--;
				}
			}
		}

		//^C (KILL)
		else if (c == 21) {
			write(out_fd, "\r", 1);	//back to the start of the line
			for (int i = 0; i < current_line_len; ++i) {
				write(out_fd, " ", 1);	//fill space with emptiness
			}
			write(out_fd, "\r", 1); //back again for writing
			
			current_line_len = 0;

			clear_words(); //no words since we deletted them all
		}
		
		//^W (ERASE WORD)
		else if (c == 23) {
			if(current_line_len == 0){
				continue;
			}

			int last_word_length = get_last_word()->length;
			if(last_word_length != 0){	//there are any symbols in the word
				delete_word(last_word_length);
				current_line_len -= last_word_length;
			}
		}

		//^D		
		else if (c == 4) {
			if (current_line_len == 0) {
				break;
			}
		}

		else if (c == '\n'){
			clear_words();
			current_line_len = 0;
			write(out_fd, "\n", 1);
		}

        //Printable character
        else {			
			TermWord* last_word = get_last_word();
			if(current_line_len == LINE_LEN){
				if(last_word->length == LINE_LEN){	//word will be longer then line length
					clear_words();					//next line will contain new word (just one symbol - c)
					current_line_len = 1;
					write(out_fd, "\n\r", 2);
				}
				else{	//transfer word to next line (if it will fit there)
					delete_word(last_word->length);
					write(out_fd, "\n\r", 2);
					write(out_fd, last_word->word, last_word->length);

					/*in case of word-tranferring we need to make it first (and only) word*/
					words[0].is_empty = last_word->is_empty;
					words[0].length = last_word->length;
					strcpy(words[0].word, last_word->word);
					words_count = 1;

					current_line_len = last_word->length;
				}
            }
			
			write(out_fd, &c, 1);

			//word processing
			if(c == ' '){
				if(last_word->is_empty){
					last_word->word[last_word->length++] = ' ';
				}
				else{
					words_count++;
					get_last_word()->length = 1;
					get_last_word()->word[get_last_word-1] = ' ';

				}
			}
			else{
				if(last_word->is_empty){
					last_word->is_empty = 0;
				}
					last_word->word[last_word->length++] = c;
			}
			
        }
		/*
		else if (c == 10) {
			// return
			write(out_fd, "\n", 1);
			line_len = 0;
			last_word_pos = 0;
			last_c = ' ';

			while (word_pos_stack.num) {
				popStack(&word_pos_stack);
			}
		}
		else if (c < 32) {
			// non-printable
			write(out_fd, "\a", 1);
		}*/
	}

    tcsetattr(fileno(stdout), TCSANOW, &safetty);
	return EXIT_SUCCESS;
}
