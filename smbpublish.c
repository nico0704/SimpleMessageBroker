//
// publisher.c
// Call: ./smbpublish.c brokerIP topic message
// Author: Nico Schroeder

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

// Port
const int srv_port = 8080;

// Seperator
const char *separator = "|";

// main
int main(int argc, char **argv)
{
  char *server_ip;
  char *topic;
  char *message;
  int sock_fd;
  struct sockaddr_in server_addr;
  socklen_t server_size;
  char buffer[512];
  char broker_message[512];
  int nbytes, length;

  // check command line arguments
  if (argc != 4)
  {
    fprintf(stderr, "call: %s brokerIP topic \"message\"\n", argv[0]);
    return 1;
  }
  // 1. Element -> server ip
  server_ip = argv[1];
  // 2. Element -> TOPIC
  topic = argv[2];
  // 3. Element -> MESSAGE
  message = argv[3];

  if (server_ip && topic && message)
  {
    fprintf(stderr, "TOPIC: %s\nMESSAGE: %s\n", topic, message);
  }
  else
  {
    fprintf(stderr, "call: %s brokerIP topic \"message\"\n", argv[0]);
    return 1;
  }

  strcpy(broker_message, "P");
  strcat(broker_message, separator);
  strcat(broker_message, topic);
  strcat(broker_message, separator);
  strcat(broker_message, message);
  // Socket anlegen und oeffnen
  // Familie: Internet, Typ: UDP-Socket
  sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
  fprintf(stderr, "socket: %d\n", sock_fd);
  if (sock_fd < 0)
  {
    perror("socket");
    return 1;
  }

  server_size = sizeof(server_addr);
  memset((void *)&server_addr, 0, server_size);
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(server_ip);
  server_addr.sin_port = htons(srv_port);

  // send message to server
  length = strlen(broker_message);
  fprintf(stderr, "sending... %d Bytes -> [%s]\n", length, broker_message);
  nbytes = sendto(sock_fd, broker_message, length, 0, (struct sockaddr *)&server_addr, server_size);
  if (nbytes != length)
  {
    perror("sendto");
    return 1;
  }

  // read message from server
  nbytes = recvfrom(sock_fd, buffer, sizeof(buffer) - 1, 0, NULL, NULL);
  if (nbytes < 0)
  {
    perror("recvfrom");
    return 1;
  }
  buffer[nbytes] = '\0';
  fprintf(stderr, "received message from broker: %s\n", buffer);

  // close socket
  close(sock_fd);
  return 0;
}