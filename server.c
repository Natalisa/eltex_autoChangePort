#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <string.h>

#define MY_PORT 3490

int main(){
    int s, ns;
    struct sockaddr_in serv_addr, clnt_addr,cl_addr;
    char buf[100];
    if((s=socket(AF_INET,SOCK_STREAM, 0)) < 0){
        perror("Ошибка создания сокета");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(MY_PORT);

    if(bind(s, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        perror("Ошибка связывания сокета с адресом");
        return -1;
    }
    int length = sizeof(serv_addr);
    if(listen(s,5) < 0){
        perror("Ошибка вызова listen()");
        return -1;
    }
    int addrlen;

    addrlen = sizeof(clnt_addr);

    while(1){
      if((ns = accept(s, (struct sockaddr *)&clnt_addr, &addrlen)) < 0){
            perror("Ошибка приема запроса от клиента\n");
            return -1;
      }
      int tmp = socket(AF_INET,SOCK_STREAM, 0);
      struct sockaddr_in addr;
      addr.sin_family = AF_INET;
      addr.sin_addr.s_addr = INADDR_ANY;
      addr.sin_port = htons (0);
      bind(tmp, (struct sockaddr *)&addr, sizeof(addr));
      struct sockaddr_in name;
      int n_l = sizeof (name);
      getsockname (tmp, (struct sockaddr* ) &name, &n_l);
      listen(tmp,1);
      short unsigned int port = name.sin_port;
      printf("%d\n",name.sin_port);
      char tmp1[10] = "";
      sprintf(tmp1,"%d",name.sin_port);
      send(ns,(void*)tmp1, strlen(tmp1), 0);
      switch(fork()) {
        case 0: {
          int cl;
          addrlen = sizeof(cl_addr);
          if((cl = accept(tmp, (struct sockaddr *)&cl_addr, &addrlen)) < 0){
                perror("Ошибка приема запроса от клиента\n");
                return -1;
          }
          memset(buf, 0, sizeof(buf));
          recv(cl, buf, sizeof(buf), 0);
          printf("%s\n",buf);
          close(cl);
        } break;
        default: {
          close(ns);
          close(tmp);
        } break;
      }
  }
    close(s);
    return 0;
}
