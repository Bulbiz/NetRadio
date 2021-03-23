#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define MAX_DIFFUSEUR 50
#define ID 8
#define PORT 4
#define IP 15

//adresse ip et descripteur du client
struct client {
    int descripteur;
    u_int32_t ip;
};

typedef struct diffuseur {
    char id[ID];
    char ip1[IP];
    char port1[PORT];
    char ip2 [IP];
    char port2[PORT];
    int index;
}diffuseur;


// stockage des infos des diffuseurs

diffuseur list_diffuseur [MAX_DIFFUSEUR];

int diffuseurPresent (){
    int count = 0;
    for (int i = 0; i < MAX_DIFFUSEUR; i++)
        if ( strcmp(list_diffuseur[i].id, "") == 0)
            count ++;
    return count;
}

void ajoutDiffuseur (diffuseur d){
    char vide[ID];
    memset(vide,'\0', ID);
    for (int i = 0; i < MAX_DIFFUSEUR; i++){
        memcpy(vide, list_diffuseur[i].id, ID);
        if(strcmp(vide, "") == 0){
            d.index = i;
            list_diffuseur[i] = d;
            break;
        }
    }
}

void suppDiffuseur (diffuseur d){
    diffuseur vide = {.id = "", .index = -1, .ip1 = "", .ip2 = "", .port1 = "", .port2 = ""};
    list_diffuseur[d.index] = vide;
}

void*ca_va(void *arg){
    
    struct client * utilisateur = (struct client *)arg;
    int descripteur = utilisateur -> descripteur;
    char buff [4];

    char tmpId [ID];
    char tmpIp1[IP];
    char tmpIp2[IP];
    char tmpPort1[PORT];
    char tmpPort2[PORT];
    strncat (tmpId, buff + 5, ID);
    strncat (tmpIp1, buff + 5 + ID + 1, IP);
    strncat (tmpPort1, buff + 5 + ID + 2 + IP, PORT);
    strncat (tmpIp2, buff + 5 + ID + 3 + IP + PORT, IP);
    strncat (tmpPort2, buff + 5 + ID + 4 + 2*IP + PORT, PORT);
    diffuseur d;
    strcpy(d.id,tmpId);
    strcpy(d.ip1,tmpIp1);
    strcpy(d.ip2,tmpIp2);
    strcpy(d.port1,tmpPort1);
    strcpy(d.port2,tmpPort2);
    d.index = -1;
    ajoutDiffuseur(d);

    while(1){
        sleep(5);
        send(descripteur, "RUOK", strlen("RUOK"), 0);
        recv(descripteur,buff, strlen(buff),0);

        if (strncmp(buff, "IMOK", strlen("IMOK")) == 0){
            continue;
        }else{
            suppDiffuseur (d);
            break;
        }
    }

    free(arg);
    close(descripteur);
    return NULL;
}

void*communication(void *arg){

    //recupération du client (IP + descripteur)
    struct client * utilisateur = (struct client *)arg;
    int descripteur = utilisateur -> descripteur;

    //buff pour le message + REGI
    char buff[PORT*2 + ID*2 + IP*2 + 5 + 4];
    memset(buff,'\0',PORT*2 + ID*2 + IP*2 + 5 + 4);

    int nbDiffuseur = 0;

    char buffDiffuseur [PORT*2 + ID*2 + IP*2 + 5 + 4];
    memset(buffDiffuseur,'\0',PORT*2 + ID*2 + IP*2 + 5 + 4);

    char envoieNumDiff [5 + 2];
    memset(envoieNumDiff,'\0', 5 + 2);

    while (1){
        memset(buff,'\0',PORT*2 + ID*2 + IP*2 + 5 + 4);
        //reception du message dans le buff
        recv(descripteur,buff,(PORT*2 + ID*2 + IP*2 + 5 + 4)*sizeof(char),0);

        //condition REGI
        if(strncmp(buff, "REGI ", strlen("REGI ")) == 0){
            if(diffuseurPresent() < 50){
                
                pthread_t th_diffuseur;
                int r2=pthread_create(&th_diffuseur,NULL,ca_va, &utilisateur);
                if(r2!=0){
                    printf("Probleme de creation de thread");
                    exit(0);
                }
                send(descripteur, "REOK", strlen("REOK"), 0);
            }else{
                send(descripteur, "RENO", strlen("RENO"), 0);
            }

        //condition LIST
        }else if (strncmp(buff, "LIST", strlen("LIST")) == 0){

            memset(envoieNumDiff,'\0', 5 + 2);
            nbDiffuseur = diffuseurPresent();
            memcpy(envoieNumDiff, "LINB ", strlen("LINB "));
            char str[2];
            sprintf(str, "%i", nbDiffuseur);
            memcpy(envoieNumDiff + strlen("LINB "), str, 2);
            send(descripteur,envoieNumDiff, 5 + 2, 0);

            for(int i = 0; i < MAX_DIFFUSEUR; i++){
                if(strcmp(list_diffuseur[i].id, "") != 0){
                    memset(buffDiffuseur,'\0',PORT*2 + ID*2 + IP*2 + 5 + 4);

                    memcpy(buffDiffuseur, "ITEM ", strlen("ITEM "));
                    memcpy(buffDiffuseur + 5, &list_diffuseur[i].id, ID);
                    memcpy(buffDiffuseur + 5 + ID, " ", 1);
                    memcpy(buffDiffuseur + 5 + ID + 1 , &list_diffuseur[i].ip1, IP);
                    memcpy(buffDiffuseur + 5 + ID + 1 + IP, " ", 1);
                    memcpy(buffDiffuseur + 5 + ID + 2 + IP, &list_diffuseur[i].port1, PORT);
                    memcpy(buffDiffuseur + 5 + ID + 2 + IP + PORT, " ", 1);

                    memcpy(buffDiffuseur + 5 + ID + 3 + IP + PORT, &list_diffuseur[i].ip2, IP);
                    memcpy(buffDiffuseur + 5 + ID + 3 + 2*IP + PORT, " ", 1);
                    memcpy(buffDiffuseur + 5 + ID + 4 + 2*IP + PORT, &list_diffuseur[i].port2, PORT);

                    send(descripteur,buffDiffuseur, PORT*2 + ID*2 + IP*2 + 5 + 4, 0);

                }
            }
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

    diffuseur vide = {.id = "", .index = -1, .ip1 = "", .ip2 = "", .port1 = "", .port2 = ""};
    for(int i = 0; i < MAX_DIFFUSEUR; i++){
        list_diffuseur[i] = vide;
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