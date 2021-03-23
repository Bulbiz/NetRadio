#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>

char pseudo [9];// il y a un \0 à la fin !
int tout_se_passe_bien = 0;// Vérifie que tout se passe bien 

/* Permet d'obtenir l'adresse IPV4 à partir d'un nom de machine */
int conversionAdresse (char * machine_name,struct in_addr * buf){
    struct addrinfo *first_info;
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;

    int r = getaddrinfo(machine_name,NULL,&hints,&first_info);
    struct sockaddr_in *addressin = (struct sockaddr_in *) first_info -> ai_addr;
    *buf = (struct in_addr) (addressin->sin_addr);
    return r;
}

/* Connecte à une machine sur un port en TCP */
int connection (char * machine, int port){
    struct sockaddr_in socket_addr;
    socket_addr.sin_family = AF_INET; //La socket utilise du IPV4
    socket_addr.sin_port = htons(port);

    int r = conversionAdresse (machine, &socket_addr.sin_addr);

    if (r != -1){
        int descripteur = socket (PF_INET,SOCK_STREAM,0);
        connect(descripteur, (struct sockaddr *) &socket_addr, sizeof(struct sockaddr_in));
        return descripteur;
    }else{
        return -1;
    }
}

void list_diffuseur (int descripteur){
    printf("Veuillez patientez, nous recevons la liste des diffuseurs ...\n");
    char message_initial [8];
    memset(message_initial,'\0',8);
    recv(descripteur,message_initial,7,0);

    if (strncmp(message_initial,"LINB",4) != 0){
        printf("Le message reçu est mauvais ! Fermeture de la connection ... \n");
        close(descripteur);
        return;
    }

    int nombre_de_message = atoi(message_initial + 5);
    char buf [56];
    for (int i = 0; i < nombre_de_message ; i ++ ){
        memset(buf,'\0',56);
        recv(descripteur,buf,55,0);
        if (strncmp(buf,"ITEM",4) == 0){
            printf("Diffuseur : %s \n", buf + 5);
        }else{
            printf("Le message reçu est mauvais ! Fermeture de la connection ... \n");
            close(descripteur);
            return;
        }
    }
    close(descripteur);
}

/* Quelque chose s'est mal passé */
void recuperateur_erreur (int signo){
    if (signo == SIGPIPE){
        tout_se_passe_bien = -1;
    }
}

/* Demande à l'utilisateur le nom de la machine */
char * demande_nom_machine (){
    char  * machine = malloc (sizeof(char) * 500);
    memset(machine,'\0',500);
    printf("Sur quelle machine se connecter ? [<500 caractères]\n");
    scanf("%499s", machine);
    printf("La machine se trouve à %s!\n",machine);
    return machine;
}

/* Demande à l'utilisateur le port de la machine */
int demande_port (){
    char buf [5];
    int port = 0;
    while(port == 0){
        memset(buf,'\0',5);
        printf("Sur quel port se connecter ? [4 nombre]\n");
        scanf("%4s", buf);
        port = atoi(buf);
    }
    return port;
}
/* Connecte à une machine sur un port en TCP */
void list (){
    char * machine = demande_nom_machine();
    int port = demande_port();

    signal(SIGPIPE, recuperateur_erreur);
    int descripteur = connection (machine, port);
    send(descripteur,"LIST",strlen("LIST"),0);

    if(tout_se_passe_bien == 0){
        list_diffuseur(descripteur);
    }else{
        printf("Il y a eu une erreur de connection, désolé ...\n");
        tout_se_passe_bien = 0;
        close(descripteur);
    }
}

void mess (){

}

void last (){

}

void hear (){

}

void help (){
    printf("LIST -> Demande à un gestionnaire de diffuseur la liste de ces diffuseurs\n");
    printf("MESS -> Envoie un message à un diffuseur pour qu'il puisse le retransmettre\n");
    printf("LAST -> Demande à un diffuseur la liste de ces derniers messages\n");
    printf("HEAR -> Ecoute dans un port de multidiffusion\n");
    printf("HELP -> Affiche l'aide pour l'utilisateur\n");
    printf("EXIT -> Termine le programme\n");
}

/* Demande ce que veut faire l'utilisateur */
void choix_du_service (){
    char commande [5];
    while (1){
        memset(commande,'\0',5);
        printf("Que voulez vous faire entre [LIST], [MESS], [LAST], [HEAR], [HELP], [EXIT] ?\n");
        scanf("%4s", commande);

        if (strcmp(commande,"LIST") == 0){
            list();
        }
        else if (strcmp(commande,"MESS") == 0){
            mess ();
        }
        else if (strcmp(commande,"LAST") == 0){
            last();
        }
        else if (strcmp(commande,"HEAR") == 0){
            hear ();
        }
        else if (strcmp(commande,"HELP") == 0){
            help ();
        }
        else if (strcmp(commande,"EXIT") == 0){
            printf("Merci d'avoir utiliser notre service, à la prochaine !\n");
            exit (0);
        }
    }
}

void configuration (){
    char buf [100];
    memset(buf, '\0',100);
    printf("Bonjour, pouvez vous me donnez votre nom ? [exactement 8 caractère]\n");
    scanf("%99s", buf);
    while (strlen(buf) != 8){
        memset(buf, '\0',100);
        printf("Vous n'avez pas taper EXACTEMENT 8 caractère !\n");
        printf("Pouvez vous me donner votre nom en EXACTEMENT 8 caractère ? exemple : \"Traveler\" \n");
        scanf("%99s", buf);
    }
    strcat(pseudo,buf);
    printf("Votre nom est donc %s !\n",pseudo);
}

int main (){
    configuration();
    choix_du_service ();
}