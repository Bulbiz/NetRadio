#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define MAX_DATA 100
#define MAX_DIFFUSEUR 50
#define ID 10
#define PORT 20

//adresse ip et descripteur du client
struct client {
    int descripteur;
    u_int32_t ip;
};

typedef struct diffuseur {
    char id[ID];
    u_int32_t ip1;
    char port1[PORT];
    u_int32_t ip2;
    char port2[PORT];
}diffuseur;


// stockage des infos des diffuseurs

diffuseur list_diffuseur [MAX_DIFFUSEUR];


void*communication(void *arg){

    //recupération du client (IP + descripteur)
    struct client * utilisateur = (struct client *)arg;
    int descripteur = utilisateur -> descripteur;

    //buff pour le message 
    char buff[MAX_DATA + 4];
    memset(buff,'\0',MAX_DATA + 4);

    while (1){
        memset(buff,'\0',MAX_DATA + 4);
        //reception du message dans le buff
        recv(descripteur,buff,(MAX_DATA + 4)*sizeof(char),0);

        //condition REGI
        if(strncmp(buff, "REGI ", strlen("REGI ")) == 0){

        //condition LIST
        }else if (strncmp(buff, "LIST", strlen("LIST")) == 0){
            
        }else{
            continue;
        }    
    }
    free(arg);
    close(descripteur);
    return NULL;
}



int main(int argc, char**argv) {
    if(argc!=2){
        printf("Erreur il faut fournir un numero de port");
        return 0;
    }

    int p=atoi(argv[1]);
    int sock=socket(PF_INET,SOCK_STREAM,0);
    struct sockaddr_in address_sock;
    address_sock.sin_family=AF_INET;
    address_sock.sin_port=htons(p);
    address_sock.sin_addr.s_addr=htonl(INADDR_ANY);
    int r=bind(sock,(struct sockaddr *)&address_sock,sizeof(struct sockaddr_in));

    if(r==0){
        r=listen(sock,0);
        while(1){
            struct sockaddr_in caller;
            socklen_t size=sizeof(caller);
            int *sock2=(int *)malloc(sizeof(int));
            *sock2=accept(sock,(struct sockaddr *)&caller,&size);
            if(sock2>=0){
                //Ici on affiche les informations du client

                printf("Port de l'appelant: %d\n",ntohs(caller.sin_port));
                printf("Adresse de l'appelant: %s\n",inet_ntoa(caller.sin_addr));
                struct client utilisateur = {.descripteur = *sock2, .ip = caller.sin_addr.s_addr};

                //creation du thread pour chaque client
                pthread_t th;
                int r2=pthread_create(&th,NULL,communication, &utilisateur);
                if(r2!=0){
                    printf("Probleme de creation de thread");
                    exit(0);
                }
            }
        }
    }
    return 0;
}