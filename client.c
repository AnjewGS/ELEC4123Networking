#include<stdio.h> 
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<unistd.h>
#include<ctype.h>

#include "client.h"

static char my_name[14] = "//////////////";
static int reged = FALSE;
static Link list = NULL;
static double diff_time = PERIOD_IN_SEC + 2;
static struct reg_info handle;

int main(){
  int op, ch, hold=FALSE;  

  regi(&handle);
  
  if(reged){
    pthread_create(&(tid[0]), NULL, &reg, NULL);
    pthread_create(&(tid[1]), NULL, &chat_serv, NULL);
    ghiuhgiuehges
    absdjiwjfaoiw
    while(1){
      if(!hold)
	printf("Enter command (C,T,R,L,Q, H for Help): ");
      
      ch = getchar();
      while( !isalpha(ch) && ( ch != '\n' )) {
	ch = getchar();
      }
      op = ch;
      // skip the rest of the line until newline is encountered
      while( ch != '\n' ) {
	ch = getchar();
      }
      
      switch( op ) {
	
      case 'c': case 'C': // Chat
	chat(list);
	hold = FALSE;
	break;

      case 'l': case 'L': // List
	print_list(list);
	hold = FALSE;
	break;
	
      case 't': case 'T': // Transfer file
	transfer();//File_transfer;
	hold = FALSE;
	break;
	
      case 'r': case 'R': // Refresh
	diff_time = PERIOD_IN_SEC + 2;
	hold = FALSE;
 	break;

      case 'h': case 'H': // Help
	printf("Enter command C for chatting, T for file transfering, R for refreshing online user list, L for listing online users, Q to exit.\n ");
	hold = FALSE;
	break;
	      	
      case 'q': case 'Q': // Quit
	printf("\nGood bye\n"); 
	return 0;
      
	// defualt action
      default :
	hold = TRUE;
      }
    }
  }
  return 0;
}

void reg(){
  char server_reply[2000];
  struct package towhom;
  time_t curr_time, prev_time;

  towhom.dest_ip = ntohl(inet_addr("149.171.36.211"));
  towhom.dest_port = 31180;
  towhom.message = &handle;
  towhom.mgs_len = sizeof(struct reg_info);
  towhom.reply = &server_reply[0];
  towhom.rpl_len = sizeof(server_reply);
  
  //keep communicating with server
  while(1){
    time(&curr_time);    
    if(diff_time > PERIOD_IN_SEC){
      udp_sent_recv(&towhom, 0);
      time(&prev_time);
      
      free_list(list);
      list = decode(&server_reply[0]);
    } 
    diff_time = difftime(curr_time, prev_time);
  }
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
  //Send reg_info
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

void regi(info detail){
  char password[12]="3118project";
  int flag = FALSE;

  printf("Please enter user name:\n");
  while(strlen(my_name)>= 14){
    if(flag)
      printf("User name too long, try another one.\n");
    scanf("%s", my_name);
    flag = TRUE;
  }
  flag = FALSE;
  strcpy(detail->username, my_name);
  
  printf("Create your password:\n");
  strcpy(detail->pw, password);
  
  while(strlen(password)>= 12){
    if(flag)
      printf("Password too long, try another one.\n");
    scanf("%s", password);
    flag = TRUE;
  }
  flag = FALSE;

  detail->port = htons( SERVER_PORT );
  reged = TRUE;
}

Link decode(char *reply){
  unsigned long int *online_users;
  int index=0, offset=20;
  in_addr_t *ip;
  in_addr_t *port;
  //char *temp;
  Link new = NULL, list = NULL;

  online_users = reply;
  //printf("no of users:[%d]\n", ntohl((unsigned int)*online_users));  

  while(index<50 && *(reply+4+index*offset)!= '\0'){
    /*
    printf("user %d name:%s\n", index+1, reply+4+index*offset);
    printf("user %X port:%d\n", index+1, ntohs((in_port_t)*(reply+4+14+index*offset)));
    temp = reply+4+14+2+index*offset;
    ip = reply+4+14+2+index*offset;
    printf("user %d addr:%d.%d.%d.%d\n", index+1, (uint8_t)*temp,(uint8_t)*(temp+1),(uint8_t)*(temp+2),(uint8_t)*(temp+3));
    printf("%X.%X.%X.%X\n",(uint8_t)*temp,(uint8_t)*(temp+1),(uint8_t)*(temp+2),(uint8_t)*(temp+3));
    printf("%d\n",ntohl(*ip));
    puts("");
    */
    new = (Link)malloc(sizeof(struct peer));
    if(new == NULL){
      puts("malloc fail");
      break;
    }
    strncpy(new->username,reply+4+index*offset,14);
    port = reply+4+14+index*offset;
    new->port = ntohs(*port);
    ip = reply+4+14+2+index*offset;
    new->ip = ntohl(*ip);
    new->next = NULL;
    
    if(list != NULL){
      new->next = list;
      list = new;
    }
    else{
      list = new;
      list->next = NULL;
    }

    index++;
  }
  return list;
}

void free_list(Link list){
  if(list != NULL){
    free_list(list->next);
    free(list);    
  }
}

void print_list(Link list){
  if(list != NULL){
    print_list(list->next);
    printf("%s\n",list->username);  
    printf("ip:%d\n",(uint32_t)list->ip);
    printf("port:%d\n",list->port);
  }
}

int chat_serv(){
  int sock_chat, read_size;  
  struct sockaddr_in server_chat;
  char message[100], username[14];  
  
  //Create socket
  sock_chat = socket(AF_INET , SOCK_DGRAM, 0);
  if (sock_chat == -1){
    printf("Could not create socket");
   }
  //Prepare the sockaddr_in structure
  server_chat.sin_family = AF_INET;
  server_chat.sin_addr.s_addr = INADDR_ANY;
  server_chat.sin_port = htons( SERVER_PORT );  
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
      strncpy(username, message, 14);
      puts("********************You have mail********************");
      printf("%s sent you a message: %s\n", username, &message[14]);
      puts("*****************************************************");
    }
  }
  shutdown(sock_chat, SHUT_RDWR);
  close(sock_chat);
  return 0;
}

void chat(Link list){
  char name[14]={0}, message[CHAT_LEN]={0};
  Link curr = list;
  struct package towhom;

  puts("Who do you want to chat with?");
  scanf("%s", name);
  // found user
  while(curr != NULL && strcmp(name, curr->username) != 0 ){
      curr = curr->next;
  }
  if(curr == NULL){
    puts("Target user is offline or not found.");
    return;
  }
  // get message
  puts("Enter message:");
  scanf("%s", message);
  message[99]='\0';

  towhom.dest_ip = curr->ip;
  towhom.dest_port = curr->port;
  towhom.message = strcat(my_name, message);
  towhom.mgs_len = CHAT_LEN;
  towhom.reply = NULL;
  towhom.rpl_len = CHAT_LEN;
  
  udp_sent_recv(&towhom, 1);
}

int transfer(){
  int sock, read_size;
  struct sockaddr_in server;
  char *buff, path[1000];
  FILE *fp;
  
  //read file
  printf("Enter file path:");
  scanf("%s", path);
  fp = fopen( path, "r");
  if(fp == NULL){
    printf("Could not open file %s\n", path);
    return 1;
  }  
  buff = malloc(sizeof(char)*1000000);
  if(buff == NULL){
    puts("malloc failed\n");
    fclose(fp);
    return 1;
  }  
  read_size = fread(buff,sizeof(char),1000000,fp);
  if(read_size <= 0){
    fclose(fp);
    return 1;
  }
  fclose(fp);
  //Create socket
  sock = socket(AF_INET , SOCK_STREAM , 0);
  if (sock == -1){
    printf("Could not create socket");
  }  
  server.sin_addr.s_addr = inet_addr("127.0.0.1");
  server.sin_family = AF_INET;
  server.sin_port = htons( 15000 );
  
  //Connect to server
  if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0){
    perror("connect failed. Error");
    return 1;
  }
  //Send some data
  if( send(sock , buff , read_size , 0) < 0){
    puts("Send failed");
    return 1;
  }
  
  /*/Receive a reply from the server
  if( recv(sock , server_reply , 2000 , 0) < 0){
    puts("recv failed");
    break;
  }
  
  puts("Server reply :");
  puts(server_reply);
  */
  
  shutdown(sock, SHUT_RDWR);
  close(sock);
  return 0;
}
