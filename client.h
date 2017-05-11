/*
  HEADER
*/

#ifndef CLIENT_H
#define CLIENT_H

#define TRUE  1
#define FALSE 0
#define CHAT_LEN 100
#define PERIOD_IN_SEC 45
#define SERVER_PORT  12280

typedef struct reg_info *info;
struct reg_info{
  char pw[12];
  char username[14];
  unsigned short port;
};

typedef struct peer *Link;
struct peer{
  char username[14];
  in_port_t port;
  in_addr_t ip;
  Link next;
}; 

typedef struct package *pack;
struct package{
  in_addr_t dest_ip;
  in_port_t dest_port;
  void *message;
  int mgs_len;
  void *reply;
  int rpl_len;
};

pthread_t tid[2];

void reg();
void regi(info detail);
void free_list(Link list);
void print_list(Link list);
Link decode(char *reply);
int udp_sent_recv(pack _info, int option); // option:1->send only; -1->recv only;
int chat_serv();
void chat(Link list);
int transfer();

#endif
