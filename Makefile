all : 
	gcc -Wall -pthread src/client.c -o client

clean :
	rm client
