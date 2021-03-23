#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

char pseudo [9];// il y a un \0 à la fin !

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

void list (){

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

int main (){
    configuration();
    choix_du_service ();
}