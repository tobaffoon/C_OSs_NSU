#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <fcntl.h>

#define BUF_LEN 40

typedef struct Stack_s {
	int cap;
	int num;
	int* arr;
} Stack;

Stack initStack() {
	Stack l;

	l.cap = 1;
	l.num = 0;
	l.arr = malloc(sizeof(int));

	return l;
}

void pushStack(Stack* s, int n) {
	if (s->num == s->cap) {
		s->cap *= 2;
		s->arr = realloc(s->arr, s->cap * sizeof(int));
	}

	s->arr[s->num] = n;
	s->num++;
}

int popStack(Stack* s) {
	return s->arr[--(s->num)];
}

void destroyStack(Stack* s) {
	free(s->arr);
	s->arr = NULL;
	s->cap = 0;
	s->num = 0;
}

// Go ot start line \r one symb \b 

int main() {

	struct termios tty, savetty;
	int fin = fileno(stdin);
	int fout = fileno(stdout);

	Stack word_pos_stack = initStack();

	char last_word[BUF_LEN];
	char last_word_buf_pos = 0;

	if (!isatty(fin)) {
		perror("Not valid terminal");
		return EXIT_FAILURE;
	}

	if (tcgetattr(fout, &tty) == -1) {
		perror("Can't get tty attributes");
		return EXIT_FAILURE;
	}

	savetty = tty;
	
	tty.c_lflag &= ~(ICANON | ECHO);
	tty.c_cc[VMIN] = 1;
	tty.c_cc[VTIME] = 0;

	if (tcsetattr(fin, TCSANOW, &tty) == -1) {
		perror("Can't set tty attributes");
		return EXIT_FAILURE;
	}
	
	char c, last_c = ' ';
	int line_len = 0;
	int last_word_pos = 0;

	while (read(fin, &c, 1)) {

		if (c == 127 || c == 8) {
			// ERASE
			write(fout, "\b \b", 3);
			if (line_len) {
				line_len--;
			}

			if (last_word_buf_pos) {
				last_word_buf_pos--;
			}

			int t_word_pos = popStack(&word_pos_stack);

			if (line_len > t_word_pos) {
				pushStack(&word_pos_stack, t_word_pos);
			}
		}
		else if (c == 21) {
			// C+U KILL
			write(fout, "\r", 1);
			for (int i = 0; i < line_len; ++i) {
				write(fout, " ", 1);
			}
			write(fout, "\r", 1);
			
			line_len = 0;

			last_c = ' ';
			last_word_buf_pos = 0;
		}
		else if (c == 23) {
			// C+W
			
			if (word_pos_stack.num) {
				last_word_pos = popStack(&word_pos_stack);
			} else {
				last_word_pos = 0;
			}
			
			for (; line_len >= last_word_pos; --line_len) {
				write(fout, "\b \b", 3);
			}

			last_c = ' ';

			last_word_buf_pos = 0;
		}
		else if (c == 4) {
			// C+D
			if (line_len <= 0) {
				break;
			}
			else {
				write(fout, "\a", 1);
			}
		}
		else if (c == 10) {
			// return
			write(fout, "\n", 1);
			line_len = 0;
			last_word_pos = 0;
			last_c = ' ';

			while (word_pos_stack.num) {
				popStack(&word_pos_stack);
			}
		}
		else if (c < 32) {
			// non-printable
			write(fout, "\a", 1);
		}
		else {
			// printable
			
			if (last_c == ' ' && c != ' ') {
				pushStack(&word_pos_stack, line_len);
				last_word_buf_pos = 0;

				for (int i = 0; i < BUF_LEN; ++i) {
					last_word[i] = 0;
				}

				//printf("%d\n", word_pos_stack.arr[word_pos_stack.num-1]);
			}

			last_word[last_word_buf_pos] = c;
			last_word_buf_pos++;

			if (line_len > BUF_LEN - 1) {

				if (word_pos_stack.num) {
					last_word_pos = popStack(&word_pos_stack);
				} else {
					last_word_pos = 0;
				}
			
				for (; line_len >= last_word_pos; --line_len) {
					write(fout, "\b \b", 3);
				}

				write(fout, "\n", 1);
				line_len = 0;
				last_word_pos = 0;
				last_c = ' ';

				while (word_pos_stack.num) {
					popStack(&word_pos_stack);
				}

				write(fout, last_word, last_word_buf_pos);
				line_len = last_word_buf_pos;
			} else {
				write(fout, &c, 1);
				line_len++;
			}
		}
		//printf("%d\n", line_len);

		//printf("%d\n", c);
		//write(fd, &c, 1);
		last_c = c;
	}

	if (tcsetattr(fin, TCSAFLUSH, &savetty) == -1) {
		perror("Can't revert tty attributes");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
