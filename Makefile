all : 
	gcc -Wall -pthread src/client.c -o client
	gcc -Wall -pthread src/gestionnaire.c -o gestionnaire
	javac src/Diffuseur.java 


clean :
	rm client
	rm gestionnaire
	rm src/*.class
