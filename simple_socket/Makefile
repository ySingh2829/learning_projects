.PHONY = all clean

CC = gcc

all: server.out client.out mylib/error_functions.o mylib/read_line.o database/database.o
	@echo "Done."

server.out: server_client_data/server.o mylib/error_functions.o mylib/read_line.o database/database.o
	@echo "Compiling server file..."
	@${CC} -Wall server_client_data/server.o mylib/error_functions.o mylib/read_line.o database/database.o -o server.out

server_client_data/server.o: server_client_data/server.c server_client_data/server.h
	@${CC} -Wall -c server_client_data/server.c -o server_client_data/server.o

client.out: server_client_data/client.o mylib/error_functions.o mylib/read_line.o
	@echo "Compiling client file..."
	@${CC} -Wall server_client_data/client.o mylib/error_functions.o mylib/read_line.o -o client.out

server_client_data/client.o: server_client_data/client.c server_client_data/server.h
	@${CC} -Wall -c server_client_data/client.c -o server_client_data/client.o

mylib/error_functions.o: mylib/error_functions.c mylib/error_functions.h 
	@echo "Compiling mylib error_functions file..."
	@${CC} -Wall -c mylib/error_functions.c -o mylib/error_functions.o

mylib/read_line.o: mylib/read_line.c mylib/read_line.h 
	@echo "Compiling mylib read_line file..."
	@${CC} -Wall -c mylib/read_line.c -o mylib/read_line.o

database/database.o: database/database.c database/database.h
	@echo "compiling database file..."
	@${CC} -Wall -c database/database.c -o database/database.o

clean: 
	@rm -r *.o *.out
	@echo "Removed all binary and obj files..."
