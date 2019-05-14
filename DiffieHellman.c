/*
** Title: image_tagger.c
** Author: Hamish Gunasekara
** Adapted from http-server.c
*/

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <strings.h>
#include <sys/select.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// constants
static char const * const HTTP_200_FORMAT = "HTTP/1.1 200 OK\r\n\
Content-Type: text/html\r\n\
%s\
Content-Length: %ld\r\n\r\n";
static char const * const HTTP_400 = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n";
static int const HTTP_400_LENGTH = 47;
static char const * const HTTP_404 = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
static int const HTTP_404_LENGTH = 45;


// represents the types of method.
typedef enum
{
    GET,
    POST,
    UNKNOWN
} METHOD;


// Starting image.
int img = 1;

static bool send_page(int sockfd, int n, char* buff, char* page, player_t* players);

int main(int argc, char * argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "usage: %s ip port\n", argv[0]);
        return 0;
    }

    // create TCP socket which only accept IPv4
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // reuse the socket if possible
    int const reuse = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // create and initialise address we will listen on
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    // if ip parameter is not specified
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    // bind address to socket
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // listen on the socket
    listen(sockfd, 5);

    // initialise an active file descriptors set
    fd_set masterfds;
    FD_ZERO(&masterfds);
    FD_SET(sockfd, &masterfds);
    // record the maximum socket number
    int maxfd = sockfd;
    fprintf(stderr, "image_tagger server is now running at IP: %s on port %s\n", argv[1], argv[2]);
    while (1)
    {
        // monitor file descriptors
        fd_set readfds = masterfds;
        if (select(FD_SETSIZE, &readfds, NULL, NULL, NULL) < 0)
        {
            perror("select");
            exit(EXIT_FAILURE);
        }

        // loop all possible descriptor
        for (int i = 0; i <= maxfd; ++i)
            // determine if the current file descriptor is active
            if (FD_ISSET(i, &readfds))
            {
                // create new socket if there is new incoming connection request
                if (i == sockfd)
                {
                    struct sockaddr_in cliaddr;
                    socklen_t clilen = sizeof(cliaddr);
                    int newsockfd = accept(sockfd, (struct sockaddr *)&cliaddr, &clilen);
                    if (newsockfd < 0)
                        perror("accept");
                    else
                    {
                        // add the socket to the set
                        FD_SET(newsockfd, &masterfds);
                        // update the maximum tracker
                        if (newsockfd > maxfd)
                            maxfd = newsockfd;
                        // print out the IP and the socket number
                        char ip[INET_ADDRSTRLEN];
                        printf(
                            "new connection from %s on socket %d\n",
                            // convert to human readable string
                            inet_ntop(cliaddr.sin_family, &cliaddr.sin_addr, ip, INET_ADDRSTRLEN),
                            newsockfd
                        );
                    }
                }
                // a request is sent from the client
                else if (!handle_http_request(i, players))
                {
                    close(i);
                    FD_CLR(i, &masterfds);
                }
            }
    }

    return 0;
}

static bool send_page(int sockfd, int n, char* buff, char* page, player_t* players) {
  struct stat st;
  stat(page, &st);
  // increase file size to accommodate the username
  long size = st.st_size;
  int curr_play_num;
  char temp[2048];

  for (int i = 0; i < 2; ++i)
  {
      if (players[i].sockfd == sockfd)
      {
          curr_play_num = i;
      }
  }

  if (strcmp(page, ACCEPTED) == 0)
  {
      size = st.st_size - 2;
      if (players[curr_play_num].num_guesses != 0)
      {
          size += 11;
          for (int guess = 0; guess < players[curr_play_num].num_guesses; ++guess)
          {
              size += strlen(players[curr_play_num].guesses[guess]);
          }
      }

  }

  n = sprintf(buff, HTTP_200_FORMAT, "", size);

  if (strcmp(page, START) == 0)
  {
      char temp[strlen("Set-Cookie: username=%s\r\n") + MAXKEYLENGTH];
      sprintf(temp, "Set-Cookie: username=%s\r\n", players[curr_play_num].name);
      size = st.st_size + players[curr_play_num].name_len  - 1;
      n = sprintf(buff, HTTP_200_FORMAT, temp, size);
  }



  // send the header first
  if (write(sockfd, buff, n) < 0)
  {
      perror("write");
      return false;
  }

  // read the content of the HTML file
  int filefd = open(page, O_RDONLY);
  n = read(filefd, buff, 2048);
  if (n < 0)
  {
      perror("read");
      close(filefd);
      return false;
  }
  close(filefd);

  // Add keywords
  if (strcmp(page, ACCEPTED) == 0)
  {
    char guesslist[1000000];
    strcpy(guesslist, "Keywords: ");
    for (int i = 0; i < players[curr_play_num].num_guesses; ++i)
    {
      if (i == 0){
        strcat(guesslist, players[curr_play_num].guesses[i]);
      } else {
        strcat(guesslist, ", ");
        strcat(guesslist, players[curr_play_num].guesses[i]);
      }
    }
    n = sprintf(temp, buff, img, guesslist);
    temp[n] = 0;
  }

  //changed the picture
  else if (strcmp(page, INTRO) == 0 || strcmp(page, TURN) == 0 || strcmp(page, DISCARDED) == 0)
  {
    n = sprintf(temp, buff, img);
    temp[n] = 0;
  }

  // Show Username
  else if (strcmp(page, START) == 0) {
    for (int i = 0; i < 2; ++i)
    {
      if (players[i].sockfd == sockfd)
      {
        n = sprintf(temp, buff, img, players[i].name);
        temp[n] = 0;
      }
    }
  }
  else
  {
      strcpy(temp, buff);
  }


  if (write(sockfd, temp, size) < 0)
  {
      perror("write");
      return false;
  }

  return true;

}
