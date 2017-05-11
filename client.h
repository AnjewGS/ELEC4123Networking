/*
  HEADER
*/

#ifndef CLIENT_H
#define CLIENT_H

#define TRUE  1
#define FALSE 0
#define CHAT_LEN 100
#define PERIOD_IN_SEC 45
#define LOCAL_PORT 12280
#define SEND 1
#define RECV -1
#define INI_SIZE 3000


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

void request();
int udp_sent_recv(pack _info, int option); // option:1->send only; -1->recv only;
int chat_serv();


#endif
