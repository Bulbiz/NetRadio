#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

char pseudo [9];

void configuration (){
    printf("Bonjour, pouvez vous me donnez votre nom ? [exactement 8 caractère]\n");
    while(read(STDIN_FILENO,pseudo,9) != 9)
        printf("Bonjour, pouvez vous me donnez votre nom ? [exactement 8 caractère]\n");
    pseudo[8] = '\0'; //supression du \n à la fin
    printf("Votre nom est donc %s !\n",pseudo);
}

void connection_gestionnaire (){
    char * machine = malloc (sizeof(char) * 500);
    printf("Sur quel machine réside votre gestionnaire de diffuseur ? (moins de 500 caractère)\n");
    int size = read(STDIN_FILENO,machine,500);
    if (size < 0 ) {perror("Gestionnaire");} //pour les Debugs 
    machine[size - 1] = '\0';
    printf("Machine : %s", machine);
}

int main (){
   configuration();
   connection_gestionnaire ();
}