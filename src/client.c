#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <ctype.h>

char * pseudo;
int tout_se_passe_bien = 0;// Vérifie que tout se passe bien 

//Code trouvé sur internet qui permet de flush stdin après une lecture
void flush_stdin()
{
    int c = 0;
    while (c != '\n' && c != EOF)
    {
        c = getchar();
    }
}

char * lire_diese (int size){
    char * lecture = malloc (sizeof(char) * (size + 3));
    memset(lecture,'#',size);
    lecture[size] = '\0';
    if (fgets(lecture,size + 1,stdin) == NULL){
        printf("Erreur sur la lecture au clavier !");
    }

    if(strchr(lecture,'\n') == NULL)
        flush_stdin();
    else{
        lecture[strlen(lecture) - 1 ] = '#'; //enleve le \n
        lecture[strlen(lecture)] = '#'; //enleve le \0 original
    }
    lecture[size] = '\0';

    return lecture;
}

char * lire (int size){
    char * lecture = malloc (sizeof(char) * (size + 3));
    memset(lecture,'\0',size);
    while(strlen(lecture) != size){
        memset(lecture,'\0',size);
        if (fgets(lecture,size + 1,stdin) == NULL)
            printf("Erreur sur la lecture au clavier !");

        if(strchr(lecture,'\n') == NULL)
            flush_stdin();
        
        if (lecture[strlen(lecture) - 1] == '\n')
            lecture[strlen(lecture) - 1] = '\0';
    }
    return lecture;
}

char * lire_variable (int size){
    char * lecture = malloc (sizeof(char) * (size + 3));
    memset(lecture,'\0',size);
    if (fgets(lecture,size + 1,stdin) == NULL)
        printf("Erreur sur la lecture au clavier !");

    if(strchr(lecture,'\n') == NULL)
        flush_stdin();
    
    if (lecture[strlen(lecture) - 1] == '\n')
        lecture[strlen(lecture) - 1] = '\0';
    return lecture;
}





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
        printf("Le message reçu est mauvais !Il s'agissait de  : %s \nFermeture de la connection ... \n",message_initial);
        close(descripteur);
        return;
    }

    int nombre_de_message = atoi(message_initial + 5);
    char buf [56];
    for (int i = 0; i < nombre_de_message ; i ++ ){
        memset(buf,'\0',56);
        recv(descripteur,buf,55,0);
        printf("J'ai reçu : %s" , buf);
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
    printf("Sur quelle machine se connecter ? [<500 caractères]\n");
    char * machine = lire_variable(500);
    printf("La machine se trouve à %s!\n",machine);
    return machine;
}

int est_un_nombre (char * str){
    for (int i = 0; i < strlen(str) ; i ++)
        if(isdigit(str[i]) == 0)
            return -1;
    return 0;
    
}

/* Demande à l'utilisateur le port de la machine */
int demande_port (){
    int port = 0;
    while(port == 0){
        printf("Sur quel port se connecter ? [Exactement 4 chiffres]\n");
        char * buf = lire(4);
        if(est_un_nombre(buf) == 0)
            port = atoi(buf);
    }
    printf("La connection se fera sur le port %d\n",port);
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

/* Demande à l'utilisateur le nom de la machine */
char * demande_message (){
    printf("Quel est le message que vous voulez affichez ? [<= 140 caractères]\n");
    char  * message = lire_diese(140);
    printf("Le message à envoyer est %s!\nIl est bien de taille : %ld\n",message,strlen(message));
    return message;
}

void mess (){
    // Demande à l'utilisateur toute les informations utiles
    char * machine = demande_nom_machine ();
    int port = demande_port();
    char * message = demande_message ();
    signal(SIGPIPE, recuperateur_erreur);
    int descripteur = connection (machine, port);

    //Créer les buffers
    char * colis = malloc (sizeof(char) * (4 + 1 + 8 + 1 + 140 + 1));
    char * receveur = malloc (sizeof(char) * 5);

    //Envoie le message
    sprintf(colis,"MESS %s %s",pseudo,message);
    int size = send(descripteur,colis,4 + 1 + 8 + 1 + 140,0);
    if (size < 0){
        printf("Il y a eu une erreur dans l'envoi du message!\n");
        close(descripteur);
        return;
    }

    //Reçoit le message
    size = recv(descripteur,receveur,4,0);
    if (size < 0){
        printf("Il y a eu une erreur dans la reception du message!\n");
        close(descripteur);
        return;
    }

    //Vérifie que le message à bien été reçu
    receveur[size] = '\0';
    if(strcmp(receveur,"ACKM") == 0)
        printf("Le message à bien été reçu par le diffuseur!\n");
    else
        printf("Le message n'a pas bien été reçu par le diffuseur, nous avons reçu '%s'\n",receveur);
    close(descripteur);

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
    while (1){
        printf("Que voulez vous faire entre [LIST], [MESS], [LAST], [HEAR], [HELP], [EXIT] ?\n");
        char * commande = lire(4);

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
    printf("Bonjour, pouvez vous me donnez votre nom ? [<= 8 caractère]\n");
    pseudo = lire_diese(8);
    printf("Votre nom est donc %s !\n",pseudo);
}

int main (){
    configuration();
    choix_du_service ();
}