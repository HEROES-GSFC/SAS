#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SERVER_TCP_PORT 3000
#define BUFLEN 		256

int main() {
int	n, bytes_to_read;
int	sd, new_sd,client_len, port;
struct	sockaddr_in server, client;
char	*bp, buf[BUFLEN];

port = SERVER_TCP_PORT;

if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	fprintf(stderr, "Can't create a socket!\n");
	exit(1);
}

bzero((char *)&server, sizeof(struct sockaddr_in));
server.sin_family = AF_INET;
server.sin_port = htons(port);
server.sin_addr.s_addr = htonl(INADDR_ANY);
if (bind(sd, (struct sockaddr *)&server, sizeof(server)) == -1) {
	fprintf(stderr, "Can't bind name to socket\n");
	exit(1);
}

listen(sd, 5);
 while (1) {
	client_len = sizeof(client);
	if ((new_sd = accept(sd, (struct sockaddr *) &client, &client_len)) == -1) {
		fprintf(stderr, "Can't accept client\n");
		exit(1);
	}

	bp = buf;
	bytes_to_read = BUFLEN;
	while ((n = read(new_sd, bp, bytes_to_read)) > 0) {
		bp += n;
		bytes_to_read -= n;
	}

	write(new_sd, buf, BUFLEN);
	close(new_sd);
}
close(sd);

return 0;
}
