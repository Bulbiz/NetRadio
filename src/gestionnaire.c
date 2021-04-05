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
// test : REGI 48484848 123.123.123.123 1234 123.123.123.124 1523
typedef struct diffuseur {
    char  * id;     //Taille 8
    char * ip1;     //Taille 15
    char * port1;   //Taille 4
    char * ip2 ;    //Taille 15
    char * port2;   //Taille 4
    int index;
}diffuseur;


// stockage des infos des diffuseurs
diffuseur * list_diffuseur;

void afficheDiffuseur (){
    for (int i = 0; i < MAX_DIFFUSEUR; i++){
        if(list_diffuseur[i].index != -1){
            printf("INDEX : %d\n",list_diffuseur[i].index);
            printf("ID : %s\n",list_diffuseur[i].id);
            printf("IP1 : %s\n",list_diffuseur[i].ip1);
            printf("IP2 : %s\n",list_diffuseur[i].ip2);
            printf("PORT1 : %s\n",list_diffuseur[i].port1);
            printf("PORT2 : %s\n\n",list_diffuseur[i].port2);
        }
    }
}

int diffuseurPresent (){
    int count = 0;
    for (int i = 0; i < MAX_DIFFUSEUR; i++)
        if ( strncmp(list_diffuseur[i].id,"\0", strlen("\0")) != 0)
            count ++;
    return count;
}

int ajoutDiffuseur (diffuseur d){
    char * vide = malloc (sizeof(char) * ID);
    char * buff = malloc (sizeof(char) * ID);
    memset(vide,'\0', ID);
    memset(buff,'\0', ID);
    for (int i = 0; i < MAX_DIFFUSEUR; i++){
        memcpy(buff, list_diffuseur[i].id, ID);
        if(strncmp(vide, buff, strlen(buff)) == 0){
            free(list_diffuseur[i].id);
            free(list_diffuseur[i].ip1);
            free(list_diffuseur[i].ip2);
            free(list_diffuseur[i].port1);
            free(list_diffuseur[i].port2);
            d.index = i;
            list_diffuseur[i] = d;
            free (vide);
            free (buff);
            return i;
        }
    }
    free (vide);
    free (buff);
    printf("bug ajout diffusseur");
    return -1;
}

void suppDiffuseur (int index){
    free(list_diffuseur[index].id);
    free(list_diffuseur[index].ip1);
    free(list_diffuseur[index].ip2);
    free(list_diffuseur[index].port1);
    free(list_diffuseur[index].port2);

    diffuseur vide = {.id = malloc(ID), .index = -1, .ip1 = malloc(IP), .ip2 = malloc(IP), .port1 = malloc(PORT), .port2 = malloc(PORT)};
    
    memset(vide.id,'\0',ID);
    memset(vide.ip1,'\0',IP);
    memset(vide.ip2,'\0',IP);
    memset(vide.port1,'\0',PORT);
    memset(vide.port2,'\0',PORT);
    
    list_diffuseur[index] = vide;
}

void ca_va(int descripteur,char * buff){
    
    //struct client * utilisateur = (struct client *)arg;
    //int descripteur = utilisateur -> descripteur;

    char * tmpId  = malloc(sizeof(char) * ID);
    char * tmpIp1 = malloc(sizeof(char) * IP);
    char * tmpIp2 = malloc(sizeof(char) * IP);
    char * tmpPort1 = malloc(sizeof(char) * PORT);
    char * tmpPort2 = malloc(sizeof(char) * PORT);

    memset(tmpId,'\0',ID);
    memset(tmpIp1,'\0',IP);
    memset(tmpIp2,'\0',IP);
    memset(tmpPort1,'\0',PORT);
    memset(tmpPort2,'\0',PORT);

    strncat (tmpId, buff + 5, ID);
    strncat (tmpIp1, buff + 5 + ID + 1, IP);
    strncat (tmpPort1, buff + 5 + ID + 2 + IP, PORT);
    strncat (tmpIp2, buff + 5 + ID + 3 + IP + PORT, IP);
    strncat (tmpPort2, buff + 5 + ID + 4 + 2*IP + PORT, PORT);
    
    /*printf("a : %s\n",buff);
    printf("a : %s\n",buff + 5);
    printf("a : %s\n",tmpId);
    printf("a : %s\n",tmpIp1);
    printf("a : %s\n",tmpIp2);
    printf("a : %s\n",tmpPort1);
    printf("a : %s\n",tmpPort2);*/

    diffuseur d = {.id = malloc(ID), .index = -1, .ip1 = malloc(IP), .ip2 = malloc(IP), .port1 = malloc(PORT), .port2 = malloc(PORT)};
    strcpy(d.id,tmpId);
    strcpy(d.ip1,tmpIp1);
    strcpy(d.ip2,tmpIp2);
    strcpy(d.port1,tmpPort1);
    strcpy(d.port2,tmpPort2);
    d.index = -1;
    int index = ajoutDiffuseur(d);
    send(descripteur, "REOK", strlen("REOK"), 0);


    /*printf("ID : %s\n",d.id);
    printf("IP1 : %s\n",d.ip1);
    printf("IP2 : %s\n",d.ip2);
    printf("PORT1 : %s\n",d.port1);
    printf("PORT2 : %s\n",d.port2);
    printf("INDEX : %d\n",d.index);*/


    //afficheDiffuseur ();

    free (tmpId);
    free (tmpIp1);
    free (tmpIp2);
    free (tmpPort1);
    free (tmpPort2);

    while(1){
        sleep(5);
        send(descripteur, "RUOK", strlen("RUOK"), 0);
        recv(descripteur,buff, strlen(buff),0);

        if (strncmp(buff, "IMOK", strlen("IMOK")) == 0){
            continue;
        }else{
            suppDiffuseur (index);
            break;
        }
    }
}

void*communication(void *arg){

    //recupération du client (IP + descripteur)
    struct client * utilisateur = (struct client *)arg;
    int descripteur = utilisateur -> descripteur;

    //buff pour le message + REGI
    char buff[PORT*2 + ID + IP*2 + 5 + 4];
    memset(buff,'\0',PORT*2 + ID + IP*2 + 5 + 4);

    int nbDiffuseur = 0;

    char buffDiffuseur [PORT*2 + ID + IP*2 + 5 + 4];
    memset(buffDiffuseur,'\0',PORT*2 + ID + IP*2 + 5 + 4);

    char envoieNumDiff [5 + 2];
    memset(envoieNumDiff,'\0', 5 + 2);

    while (1){
        memset(buff,'\0',PORT*2 + ID + IP*2 + 5 + 4);
        //reception du message dans le buff
        int r = recv(descripteur,buff,(PORT*2 + ID + IP*2 + 5 + 4)*sizeof(char),0);
        if (r <= 0){
            printf("Message vide, Fin de la connection ...\n");
            break;
        }

        //condition REGI
        if(strncmp(buff, "REGI ", strlen("REGI ")) == 0){
            if(diffuseurPresent() < 50){
                
                /*pthread_t th_diffuseur;
                int r2=pthread_create(&th_diffuseur,NULL,ca_va, &utilisateur);
                if(r2!=0){
                    printf("Probleme de creation de thread");
                    exit(0);
                }*/
                ca_va(descripteur,buff);
                break;
            }else{
                send(descripteur, "RENO", strlen("RENO"), 0);
                break;
            }

        //condition LIST
        }else if (strncmp(buff, "LIST", 4) == 0){
            nbDiffuseur = diffuseurPresent();
            memset(envoieNumDiff,'\0', 7);
            sprintf(envoieNumDiff,"LINB %d",nbDiffuseur);
            send(descripteur,envoieNumDiff, 7, 0);

            for(int i = 0; i < MAX_DIFFUSEUR; i++){
                if(strcmp(list_diffuseur[i].id, "") != 0){
                    memset(buffDiffuseur,'\0',PORT*2 + ID + IP*2 + 5 + 4);
                    memcpy(buffDiffuseur, "ITEM ", strlen("ITEM "));
                    memcpy(buffDiffuseur + 5, list_diffuseur[i].id, ID);
                    memcpy(buffDiffuseur + 5 + ID, " ", 1);
                    memcpy(buffDiffuseur + 5 + ID + 1 , list_diffuseur[i].ip1, IP);
                    memcpy(buffDiffuseur + 5 + ID + 1 + IP, " ", 1);
                    memcpy(buffDiffuseur + 5 + ID + 1 + IP + 1, list_diffuseur[i].port1, PORT);
                    memcpy(buffDiffuseur + 5 + ID + 1 + IP + 1 + PORT, " ", 1);
                    memcpy(buffDiffuseur + 5 + ID + 1 + IP + 1 + PORT + 1, list_diffuseur[i].ip2, IP);
                    memcpy(buffDiffuseur + 5 + ID + 1 + IP + 1 + PORT + 1 + IP, " ", 1);
                    memcpy(buffDiffuseur + 5 + ID + 1 + IP + 1 + PORT + 1 + IP + 1, list_diffuseur[i].port2, PORT);

                    send(descripteur,buffDiffuseur, PORT*2 + ID + IP*2 + 5 + 4, 0);

                }
            }
        }
    }
    //free(arg);
    close(descripteur);
    return NULL;
}



int main(int argc, char**argv) {
    if(argc!=2){
        printf("Erreur il faut fournir un numero de port");
        return 0;
    }
    list_diffuseur = malloc (sizeof(diffuseur) * MAX_DIFFUSEUR);
    for(int i = 0; i < MAX_DIFFUSEUR; i++){
        diffuseur vide = {.id = malloc(ID), .index = -1, .ip1 = malloc(IP), .ip2 = malloc(IP), .port1 = malloc(PORT), .port2 = malloc(PORT)};;
        memset(vide.id,'\0',ID);
        memset(vide.ip1,'\0',IP);
        memset(vide.ip2,'\0',IP);
        memset(vide.port1,'\0',PORT);
        memset(vide.port2,'\0',PORT);

        list_diffuseur[i] = vide;
    }
    /*memset(list_diffuseur[0].id,'\0',ID);
    strcpy(list_diffuseur[0].id , "HELP####");
    list_diffuseur[0].index = 0;
    memset(list_diffuseur[0].ip1,'\0',IP);
    strcpy(list_diffuseur[0].ip1 , "125.125.125.125");
    memset(list_diffuseur[0].ip2,'\0',IP);
    strcpy(list_diffuseur[0].ip2 , "125.125.125.124");
    memset(list_diffuseur[0].port1,'\0',PORT);
    strcpy(list_diffuseur[0].port1 , "1251");
    memset(list_diffuseur[0].port2,'\0',PORT);
    strcpy(list_diffuseur[0].port2 , "1251");*/
    

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