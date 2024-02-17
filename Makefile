CC=gcc

client: buffer.c client.c helpers.c parson.c requests.c
		$(CC) -o client buffer.c client.c helpers.c parson.c requests.c -ljson-c -Wall
		
run: client
		./client

clean:
		rm -f *.o client