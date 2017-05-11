#include<stdio.h> 
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<unistd.h>
#include<ctype.h>

#include "client.h"

static char *G_buffer;
static uint32_t MAX = UINT32_MAX;
static char *F_buffer;

int main(){
	int i;  

  (char *)G_buffer =  malloc( INI_SIZE*sizeof(char));
  (char *)F_buffer =  malloc( INI_SIZE*sizeof(char));
  if(G_buffer == NULL || F_buffer == NULL){
	  perror("malloc fail");
	  return 0;
  }
  
  for(i=0,i<=INI_SIZE,i++){
	  *( F_buffer + i ) = 0;
  }
  
  pthread_create(&(tid[0]), NULL, &request, NULL);
  pthread_create(&(tid[1]), NULL, &chat_serv, NULL);
  
  return 0;
}

void request(){
  struct package towhom;
  uint32_t Location = 0;
  //time_t curr_time, prev_time;

  towhom.dest_ip = ntohl(inet_addr("149.171.36.173"));
  towhom.dest_port = 8188;
  towhom.message = &Location;
  towhom.mgs_len = sizeof(Location);
  towhom.reply = NULL;
  towhom.rpl_len = 0;
  
  //time(&curr_time);    
  //if(diff_time > PERIOD_IN_SEC){
  while(1){
	  udp_sent_recv(&towhom, SEND);
	  while( *(F_buffer + Location) ){
		  Location += 3;
	  }
	  if(Location >= MAX)
		  Location = 0;
  }
  //time(&prev_time);
  //} 
  //diff_time = difftime(curr_time, prev_time);
  
}

int udp_sent_recv(pack _info, int option){
  // option:1->send only; -1->recv only;
  int sock;
  struct sockaddr_in dest;
  socklen_t sock_len = sizeof(dest);

  if(_info == NULL)
    return 1;
  //Create socket
  sock = socket(AF_INET , SOCK_DGRAM , 0);
  if (sock == -1){
    printf("Could not create socket");
  }  
  dest.sin_addr.s_addr = htonl(_info->dest_ip);
  dest.sin_family = AF_INET;
  dest.sin_port = htons(_info->dest_port);
  //Send request
  if(option != -1){
    if( sendto(sock,_info->message, _info->mgs_len, 0, (struct sockaddr *)&dest, sock_len) == -1){
      puts("Send failed");
      return 1;
    }
  }
  //Receive a reply from the server
  if(option != 1){
    if( recvfrom(sock, _info->reply, _info->rpl_len, 0, (struct sockaddr *)&dest, &sock_len) == -1){
      puts("recv failed");
      return 1;
    }
  }

  shutdown(sock, SHUT_RDWR);
  close(sock);
  return 0;
}


int chat_serv(){
  int sock_chat, read_size;  
  struct sockaddr_in server_chat;
  char message[10];  
  
  //Create socket
  sock_chat = socket(AF_INET , SOCK_DGRAM, 0);
  if (sock_chat == -1){
    printf("Could not create socket");
   }
  //Prepare the sockaddr_in structure
  server_chat.sin_family = AF_INET;
  server_chat.sin_addr.s_addr = INADDR_ANY;
  server_chat.sin_port = htons( LOCAL_PORT );  
  //Bind
  if( bind(sock_chat,(struct sockaddr *)&server_chat , sizeof(server_chat)) != 0){
    perror("chat serv bind failed. Error");
    return 1;
  }

  while(1){
    if( (read_size = recvfrom(sock_chat, &message[0], CHAT_LEN, 0, NULL, NULL)) == -1){
      puts("recv failed");
      return 1;
    }
    if(read_size > 0){
	    printf("Char %c @ location %d get.",&message[0],&message[4]);
    }
  }
  shutdown(sock_chat, SHUT_RDWR);
  close(sock_chat);
  return 0;
}

