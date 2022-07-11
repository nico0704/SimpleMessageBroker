//
// broker.c
// Call: ./smbbroker.c
// Author: Nico Schroeder

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#define PUBLISHER_MESSAGE "Hello Publisher!"
#define SUBSCRIBER_MESSAGE "Hello Subscriber!"

// Port
const int srv_port = 8080;

// Seperator
const char separator[] = "|";

// Hashtag
const char hashtag[] = "#";

// linked list to save subscriber addresses and their subscribed topics
typedef struct node
{
  struct sockaddr_in subscriber_addr;
  socklen_t subscriber_size;
  char topic[512];
  struct node *next;
} node_t;

// head
node_t *head = NULL;

// tmp
node_t *tmp = NULL;

// linked list functions
node_t *createNewNode(struct sockaddr_in subscriber_addr, socklen_t subscriber_size, char *top);
void insertAtHead(node_t *node_to_insert, node_t **head);
void printListIterativeFromTop(node_t *head); // print linked list

// main
int main(int argc, char **argv)
{
  int server_fd;
  struct sockaddr_in server_addr, client_addr, subscriber_addr;
  socklen_t server_size, client_size, subscriber_size;
  char buffer[512];
  char returnString[512];
  char *topic, *message;
  char s_topic[512];
  int nbytes, length;

  // Server Socket anlegen und oeffnen
  // Familie: Internet, Typ: UDP-Socket
  server_fd = socket(AF_INET, SOCK_DGRAM, 0);
  fprintf(stderr, "socket: %d\n", server_fd);
  if (server_fd < 0)
  {
    perror("socket");
    return 1;
  }

  // Serverstruktur einrichten
  // Datenstruktur auf 0 setzen
  // Familie: Internetserver
  // Adresse: beliebige Clientadressen zulassen
  // Port: wie in srv_port festgelegt
  server_size = sizeof(server_addr);
  memset((void *)&server_addr, 0, server_size);
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(srv_port);
  bind(server_fd, (struct sockaddr *)&server_addr, server_size);

  // wait for messages from clients (publisher or subscriber)
  while (1)
  {
    // read incoming messages
    client_size = sizeof(client_addr);
    fprintf(stderr, "waiting... \n");
    nbytes = recvfrom(server_fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &client_size);
    buffer[nbytes] = '\0';

    // get first char to check if client is publisher ('P') or subscriber ('S')
    char *ptr;
    ptr = strtok(buffer, separator);

    // Publisher
    if (strcmp(ptr, "P") == 0)
    {
      ptr = strtok(NULL, separator);
      topic = ptr;
      ptr = strtok(NULL, separator);
      message = ptr;

      fprintf(stderr, "publish:\n\tTOPIC: %s\n\tMESSAGE: %s\n", topic, message);
      sprintf(returnString, PUBLISHER_MESSAGE);

      // Bestätigung an publisher senden:
      length = strlen(returnString);
      nbytes = sendto(server_fd, returnString, length, 0, (struct sockaddr *)&client_addr, client_size);
      if (nbytes != length)
      {
        perror("sendto");
        continue;
      }

      // An Subscriber weiterleiten
      strcpy(returnString, "TOPIC: ");
      strcat(returnString, topic);
      strcat(returnString, " - ");
      strcat(returnString, "MESSAGE: ");
      strcat(returnString, message);

      length = strlen(returnString);

      tmp = head;
      while (tmp)
      {
        if (strcmp(tmp->topic, topic) != 0 && strcmp(tmp->topic, hashtag) != 0)
        {
          // traversing...
          tmp = tmp->next;
          continue;
        }
        // send message to subscriber
        nbytes = sendto(server_fd, returnString, length, 0, (struct sockaddr *)&tmp->subscriber_addr, tmp->subscriber_size);
        fprintf(stderr, "sending message to subscriber... -> %s\n", returnString);
        if (nbytes != length)
        {
          perror("sending message to subscriber...");
        }
        tmp = tmp->next;
      }
    }

    // Subscriber
    if (strcmp(ptr, "S") == 0)
    {
      // get topic
      ptr = strtok(NULL, separator);
      topic = ptr;
      fprintf(stderr, "request for topic: %s\n", topic);
      sprintf(s_topic, topic);

      // send confirm to subscriber
      sprintf(buffer, SUBSCRIBER_MESSAGE);
      length = strlen(buffer);
      nbytes = sendto(server_fd, buffer, length, 0, (struct sockaddr *)&client_addr, client_size);
      if (nbytes != length)
      {
        perror("sendto");
        continue;
      }

      // add subscriber to linked list
      tmp = createNewNode(client_addr, client_size, s_topic);
      insertAtHead(tmp, &head);
    }
  }
  return 0;
}

// linked list functions:

node_t *createNewNode(struct sockaddr_in subscriber_addr, socklen_t subscriber_size, char *topic)
{
  node_t *newNode = (node_t *)malloc(sizeof(node_t));
  if (newNode)
  {
    newNode->next = NULL;
    newNode->subscriber_addr = subscriber_addr;
    newNode->subscriber_size = subscriber_size;
    strcpy(newNode->topic, topic);
    return newNode;
  }
  return NULL;
}

void insertAtHead(node_t *node_to_insert, node_t **head)
{
  node_to_insert->next = *head;
  *head = node_to_insert;
  return;
}

void printListIterativeFromTop(node_t *head)
{
  node_t *tmp = head;
  while (tmp)
  {
    fprintf(stderr, "%s - ", tmp->topic);
    tmp = tmp->next;
  }
  printf("\n");
}