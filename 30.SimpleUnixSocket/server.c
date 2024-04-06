#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define BUFFER_SIZE 256
#define SOCKET_NAME "./tmp/a.mazyar.server"

int main() {
    struct sockaddr_un addr;
    int down_flag = 0;
    int ret;
    int listen_socket;
    int data_socket;
    int result;
    char buffer[BUFFER_SIZE];

    unlink(SOCKET_NAME);

    listen_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    
	if (listen_socket == -1) {
        perror("Failed to open local socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));

    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_NAME, sizeof(addr.sun_path) - 1);

    ret = bind(
			listen_socket,
			(const struct sockaddr *) &addr,
			sizeof(struct sockaddr_un)
		);

    if (ret == -1) {
        perror("Failed to bind socket");
        exit(EXIT_FAILURE);
    }

    ret = listen(listen_socket, 20);
    
	if (ret == -1) {
        perror("listen() failed");
        exit(EXIT_FAILURE);
    }

	printf("Server started!\n");

    data_socket = accept(listen_socket, NULL, NULL);

	if (data_socket == -1) {
        perror("Failed to open data socket");
        exit(EXIT_FAILURE);
    }

	printf("Client connected!\n");

    result = 0;
    
    ret = read(data_socket, buffer, BUFFER_SIZE);
        
	if (ret == -1) {
        perror("Read failed");
        exit(EXIT_FAILURE);
    }

	buffer[BUFFER_SIZE - 1] = 0;

	printf("Read data: \"%s\"\n", buffer);

    close(listen_socket);

    unlink(SOCKET_NAME);

	printf("Server is shutting down!\n");

    exit(EXIT_SUCCESS);
}
