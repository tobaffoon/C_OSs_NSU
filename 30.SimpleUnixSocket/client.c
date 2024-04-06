#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define BUFFER_SIZE 256

int main(int argc, char** argv) {
    struct sockaddr_un addr;
    int i;
    int ret;
    int data_socket;
    char buffer[BUFFER_SIZE];

    data_socket = socket(AF_UNIX, SOCK_STREAM, 0);
	
	if (data_socket == -1) {
        perror("Failed to open data socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));

    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, "./tmp/a.mazyar.client.XXXXXX", sizeof(addr.sun_path) - 1);
    mktemp(addr.sun_path);

    ret = connect (
			data_socket,
			(const struct sockaddr *) &addr,
            sizeof(struct sockaddr_un)
		);

	if (ret == -1) {
        fprintf(STDERR_FILENO, "Can't connetct to server\n");
        exit(EXIT_FAILURE);
    }

	printf("Connected to server!\n");

	char* send_string = "Connection is alive!";

	write(data_socket, send_string, strlen(send_string) + 1);

	printf("Sent data!\n");

    close(data_socket);
    unlink(addr.sun_path);

	printf("Client exit!\n");

    exit(EXIT_SUCCESS);
}
