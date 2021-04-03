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
    
    lecture[strlen(lecture) - 1 ] = '#'; //enleve le \n
    lecture[strlen(lecture)] = '#'; //enleve le \0 original
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
    printf("Sur quelle machine se connecter ? [<500 caractères]\n");
    char * machine = lire_variable(500);
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

/* Demande à l'utilisateur le nom de la machine */
char * demande_message (){
    char  * message = malloc (sizeof(char) * 500);
    int size = 0;
    while (size != 10){
        printf("Quel est le message que vous voulez affichez ? [Exactement 10 caractères]\n");
        memset(message,'\0',500);
        size = scanf("%499s", message);
    }
    printf("Le message à envoyer est %s! Il est bien de taille : %ld\n",message,strlen(message));
    return message;
}

void mess (){
    char * machine = demande_nom_machine ();
    int port = demande_port();
    char * message = demande_message ();
    signal(SIGPIPE, recuperateur_erreur);
    int descripteur = connection (machine, port);
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
    char * pseudo = lire_diese(8);
    printf("Votre nom est donc %s !\n",pseudo);
}

int main (){
    configuration();
    choix_du_service ();
    printf("|%s|",lire_variable(8));
    printf("|%s|",lire_variable(8));
}