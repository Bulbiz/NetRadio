all : 
	gcc -Wall -pthread src/client.c -o client
	gcc -Wall -pthread src/gestionnaire.c -o gestionnaire
	javac src/Diffuseur.java 
	mv src/Diffuseur.class Diffuseur.class


clean :
	rm client
	rm gestionnaire
	rm *.class
