#include "stdlib.h"
#include "stdio.h"
#include "sys/socket.h"
#include "sys/types.h"
#include "arpa/inet.h"
#include "error.h"
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define PORTNUMBER 50000
#define MAXLINE 200
#define MAXTHREADS 10
void *atenderCliente(void *connectionsocket);
void *printaArquivo();
sem_t semaphore;

int main(){
   int serversocket;
   int tamr;
   char buffer[MAXLINE];
   struct sockaddr_in server_addr;
   struct sockaddr_in client_addr;
   int connectionsocket;
   int connections[99];
   int connectionCounter = 0;
   int socketsize;
   int flag;
   int contador = 0;
   sem_init(&semaphore, 0, 1);
   pthread_t threads[MAXTHREADS];

   //Criacao do socket no servidor
   printf("Iniciando o servidor. \n");
   if ((serversocket = socket(AF_INET,SOCK_STREAM,0))<0){
      perror("Erro ao criar socket do servidor: \n");
   }

   //Configuracao do endereco do servidor que sera ligado ao socket
   //zera toda a estrutura
   memset(&server_addr, 0, sizeof(server_addr));

   //Tipo de endereco
   server_addr.sin_family = AF_INET;
   //especificacao do endereco
   server_addr.sin_addr.s_addr   = INADDR_ANY;

   //Querendo especificar manualmente um endereco pode-se usar
   //server_addr.sin_addr.s_addr   = inet_addr("127.0.0.1");

   //Numero da porta
   server_addr.sin_port   = htons(PORTNUMBER);
   //
   //memset(&(server_addr.sin_zero), '\0', sizeof(server_addr.sin_zero));

   //Ligando o endereco do servidor ao socket
   if (bind(serversocket, (struct sockaddr *)&server_addr, sizeof(struct sockaddr))<0){
      perror("Erro de ligacao de socket no servidor: \n");
   }

   //Indica que o socket deve "escutar"
   if (listen(serversocket, 1)<0){
      perror("Erro ao inicializar a escuta do socket no servidor: \n");
   }

   printf("Servidor escutando na porta: %d .\n", PORTNUMBER);

   //Servidor fica em loop para receber conexoes
   pthread_create(&threads[contador], NULL, printaArquivo, NULL);
   contador++;
   while(1){
      socketsize = sizeof(client_addr);

      //Servidor aguardando por conexao
      connectionsocket = accept(serversocket, (struct sockaddr *)&client_addr, &socketsize);
      connections[connectionCounter] = connectionsocket;
      if(connectionsocket < 0){
         perror("Erro ao receber pedido de conexao: \n");
      }else{
         printf("Conexao estabelecida com cliente %s. \n", inet_ntoa(client_addr.sin_addr));
         printf("Criando thread para atender a sua solicitação.\n");
         pthread_create(&threads[contador], NULL, atenderCliente, (void*) &connections[connectionCounter]);
         contador++;
         connectionCounter++;
      }
   }
   pthread_join(threads[contador], NULL);
}

void *atenderCliente(void * connectionsocket) {
  //Lendo mensagem enviada pelo cliente
  int tamr;
  int tams;
  char buffer[MAXLINE];
  FILE *file;
  label3:
  printf("Esperando mensagem do cliente...\n");
  tamr = read(*(int *)connectionsocket, buffer, MAXLINE);

  if(tamr == -1) {
    printf("Erro ao receber mensagem do cliente...\n");
    printf("Enviando 'err' para o cliente...\n");
    strcpy(buffer, "err");
    tams = write(*(int *)connectionsocket,buffer,strlen(buffer));
    if(tams == strlen(buffer)){
      printf("Mensagem enviada. \n");
    }else{
      printf("Erro no cliente.\n");
    }
    close(*(int *)connectionsocket);

  } else {

      buffer[tamr] = '\0';
      printf("Cliente enviou mensagem %s\n", buffer);
      sleep(2);
      if(strcmp(buffer, "consulta") == 0) {
        printf("Enviando os dados da tabela para o cliente...\n");

        sem_wait(&semaphore);
        file = fopen("arquivo.txt", "rb");
        fseek(file, 0, SEEK_END);
        long fsize = ftell(file);
        fseek(file, 0, SEEK_SET);

        char *string = malloc(fsize + 1);
        fread(string, fsize, 1, file);
        string[fsize] = '\0';
        fclose(file);
        sem_post(&semaphore);

        strcpy(buffer, string);
          tams = write(*(int *)connectionsocket,buffer,strlen(buffer));
          if(tams == strlen(buffer)){
             printf("Mensagem enviada. \n");
          }else{
             printf("Erro no cliente.\n");
          }

      } else {
          printf("Escrevendo no arquivo...\n");
          file = fopen("arquivo.txt", "a");
          sem_wait(&semaphore);
          fprintf(file, "%s", buffer);
          fprintf(file, "\n");
          fclose(file);
          sem_post(&semaphore);
          printf("Escrito!\n");
          sleep(2);
          printf("Enviando a mensagem 'ack'\n");
          strcpy(buffer, "ack");
          tams = write(*(int *)connectionsocket,buffer,strlen(buffer));
          if(tams == strlen(buffer)){
             printf("Mensagem enviada. \n");
          }else{
             printf("Erro no cliente.\n");
          }
          goto label3;
      }
      close(*(int *)connectionsocket);
  }
}

void *printaArquivo() {
    char str[1000];
    label2:
    sleep(10);
    FILE *file;
    sem_wait(&semaphore);
    printf("\nPrintando arquivo:\n");
    file = fopen("arquivo.txt", "r");
    while(fgets(str, 1000, file)) {
        printf("%s", str);
    }
    fclose(file);
    printf("\n");
    sem_post(&semaphore);
    goto label2;
}
