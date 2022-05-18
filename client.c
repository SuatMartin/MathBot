#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#define MAXLINE 600

int open_clientfd(char *hostname, int port) {
  int clientfd;
  struct hostent *hp;
  struct sockaddr_in serveraddr;
  if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    return -1;
  }
  if ((hp = gethostbyname(hostname)) == NULL){
    return -2;
  }
  bzero((char *)&serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  bcopy((char *)hp->h_addr_list[0], (char *)&serveraddr.sin_addr.s_addr,
        hp->h_length);
  serveraddr.sin_port = htons(port);
  if (connect(clientfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0){
    return -1;
  }
  return clientfd;
}

int main(int argc, char **argv) {
  int clientfd;
  int port;
  char *host;
  char *id;
  char buf[MAXLINE];
  int a;
  char b;
  int c;
  int answer;
  char flag[MAXLINE];
  char greeting[MAXLINE];
  char src[200];
  ssize_t nr;
  if (argc != 4) {
    fprintf(stderr, "usage: %s <id> <port> <host>\n", argv[0]);
    exit(0);
  }
  host = argv[3];
  port = atoi(argv[2]);
  id = argv[1];
  clientfd = open_clientfd(host, port);
  sprintf(src, "cs230 HELLO %s\n", id);
  strcpy(greeting, src);
  ssize_t ns = send(clientfd, greeting, strlen(greeting), 0);
  if (ns >= 0){
    printf("sent hello\n");
  }
  while (nr = recv(clientfd, buf, MAXLINE, 0) >= 0) {
    char ans[200];

    if (nr >= 0){
      printf("recieved %s\n", buf);
    }


    if (sscanf(buf, "cs230 STATUS %d %c %d\n", &a,&b,&c) != -1){
      if (b == '*'){
        answer = a * c;
      } else if (b == '+') {
        answer = a + c;
      } else if (b == '-') {
        answer = a - c;
      } else if (b == '/') {
        answer = a / c;
      } else {
        answer = a % c;
      }
    } else if (sscanf(buf, "cs230 %s BYE\n", flag) != -1){
      printf("%s", flag);
      return 1;
    }
    sprintf(ans, "cs230 %d\n", answer);
    strcpy(buf, ans);
    ns = send(clientfd, buf, strlen(buf), 0);
    if (ns >= 0){
      printf("sent %d\n", answer);
    }
  }
  close(clientfd);
  exit(0);
}
