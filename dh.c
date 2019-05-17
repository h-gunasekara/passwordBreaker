/*
* select-server.c -- a cheezy multiperson chat server
* For Q1.1
*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <strings.h>
#include <string.h>

#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 7800


int g = 15;
int p = 97;
int b;


int PowerMod(int a, int m, int n)
{
	int r;
	int y = 1;

	while (m > 0)
	{
		r = m % 2;

		// fast exponention
		if (r == 1)
			y = (y*a) % n;
		a = a*a % n;

		m = m / 2;
	}

	return y;
}

int main(int argc, char ** argv)
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent * server;

    char buffer[256];

    portno = 7800;


    /* Translate host name into peer's IP address ;
     * This is name translation service by the operating system
     */
    server = gethostbyname("172.26.37.44");

    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    /* Building data structures for socket */

    bzero((char *)&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;

    bcopy(server->h_addr_list[0], (char *)&serv_addr.sin_addr.s_addr, server->h_length);

    serv_addr.sin_port = htons(portno);

    /* Create TCP socket -- active open
    * Preliminary steps: Setup: creation of active open socket
    */

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
    {
        perror("ERROR opening socket");
        exit(0);
    }

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR connecting");
        exit(0);
    }

    /* Do processing
    */

    n = write(sockfd, "hgunasekara", strlen("hgunasekara"));

    if (n < 0)
    {
        perror("ERROR writing to socket");
        exit(0);
    }


    n = write(sockfd, "\n", strlen("\n"));

    b = strtol(argv[1], NULL, 16);
    int public_b = PowerMod(g, b, p);

    char* b_char;
    sprintf(b_char, "%d", public_b);
    n = write(sockfd, b_char, strlen(b_char));
    n = write(sockfd, "\n", strlen("\n"));


    bzero(buffer, 256);
    n = read(sockfd, buffer, 255);

    if (n < 0)
    {
        perror("ERROR reading from socket");
        exit(0);
    }

    printf("recieved public key: %s", buffer);

    int a_pub = atoi(buffer);
    int secert_key = PowerMod(a_pub, b, p);

    char* secert_char_key;
    sprintf(secert_char_key, "%d", secert_key);
    n = write(sockfd, secert_char_key, strlen(secert_char_key));
    n = write(sockfd, "\n", strlen("\n"));

    bzero(buffer, 256);
    n = read(sockfd, buffer, 255);

    printf("reply: %s", buffer);

    close(sockfd);
    return 0;
}
