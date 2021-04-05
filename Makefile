all : 
	gcc -Wall -pthread src/client.c -o client
	gcc -Wall -pthread src/gestionnaire.c -o gestionnaire

clean :
	rm client
	rm gestionnaire
