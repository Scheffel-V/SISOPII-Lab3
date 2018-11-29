#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <error.h>
#include <time.h>

#define MAXLINE 200

char *recursos[10] = {"127.0.0.1Recurso0", "127.0.0.1Recurso1", "127.0.0.1Recurso2", "127.0.0.1Recurso3", "127.0.0.1Recurso4", "127.0.0.1Recurso5", "127.0.0.1Recurso6", "127.0.0.1Recurso7", "127.0.0.1Recurso8", "127.0.0.1Recurso9"};

int main(){

   int clientSock;
   int conn;
   unsigned int tams;
   int tamr;
   char buffer[MAXLINE];
   struct sockaddr_in s;
   int portaServidor = 50000;
   int random = 0;
   srand( (unsigned)time(NULL) );


   //Criando socket cliente
   clientSock = socket(AF_INET, SOCK_STREAM, 0);
   if(clientSock < 0){
		perror("Erro na criacao do socket client: ");
   }

   memset(&s, 0, sizeof(s));

   //Especificacao do endereco do servidor para efetuar conexao
   s.sin_family = AF_INET;
   s.sin_addr.s_addr = inet_addr("127.0.0.1");
   s.sin_port = htons(portaServidor);

   //Efetua conexao com o sevidor
   printf("Tentando se conectar com o servidor...\n");
   conn = connect(clientSock, (struct sockaddr *) &s, sizeof(s));
   if(conn < 0){
      perror("Erro ao tentar se conectar com o servidor: ");
      exit(0);
   }
   printf("Conectado!\n");
   label1:
   strcpy(buffer, recursos[rand() % 10]);
   sleep(2);
   //Envia mensagem ao servidor

   printf("Mensagem a ser enviada: %s\n", buffer);
   tams = write(clientSock,buffer,strlen(buffer));
   if(tams == strlen(buffer)){
      printf("Mensagem enviada. \n");
   }else{
	    printf("Erro no servidor.\n");
   }
   printf("Esperando resposta do servidor...\n");
   tamr = read(clientSock, buffer, MAXLINE);
   buffer[tamr] = '\0';
   printf("Servidor enviou mensagem %s\n", buffer);
   if(strcmp(buffer, "ack") == 0) {
     printf("Servidor enviou 'ack'...\n");

     strcpy(buffer, "consulta");
     sleep(2);
     printf("Enviando mensagem 'consulta'...\n");
     printf("Mensagem a ser enviada: %s\n", buffer);
     tams = write(clientSock,buffer,strlen(buffer));
     if(tams == strlen(buffer)){
        printf("Mensagem enviada. \n");
     }else{
        printf("Erro no servidor.\n");
     }
     printf("Esperando resposta do servidor...\n");
     tamr = read(clientSock, buffer, MAXLINE);
     buffer[tamr] = '\0';
     printf("Servidor enviou mensagem %s\n", buffer);

     printf("Fechando a conexão...\n");

     conn = close(clientSock);
     printf("Cliente ecerrado. \n");
     exit(0);
   } else {
     printf("Servidor envou 'err', tentando enviar novamente a mensagem...\n");
     goto label1;
   }
}
