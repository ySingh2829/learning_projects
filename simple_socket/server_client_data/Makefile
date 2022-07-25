.PHONY = all clean

CC = gcc

all: server client ../mylib/error_functions.o ../mylib/read_line.o ../database/database.o
	@echo "Done."

server: server.o ../mylib/error_functions.o ../mylib/read_line.o ../database/database.o
	@echo "Compiling server file..."
	@${CC} -Wall server.o ../mylib/error_functions.o ../mylib/read_line.o ../database/database.o -o server

server.o: server.c server.h
	@${CC} -Wall -c server.c

client: client.o ../mylib/error_functions.o ../mylib/read_line.o
	@echo "Compiling client file..."
	@${CC} -Wall client.o ../mylib/error_functions.o ../mylib/read_line.o -o client

client.o: client.c server.h
	@${CC} -Wall -c client.c

../mylib/error_functions.o: ../mylib/error_functions.c ../mylib/error_functions.h 
	@echo "Compiling mylib error_functions file..."
	@${CC} -Wall -c ../mylib/error_functions.c -o ../mylib/error_functions.o

../mylib/read_line.o: ../mylib/read_line.c ../mylib/read_line.h 
	@echo "Compiling mylib read_line file..."
	@${CC} -Wall -c ../mylib/read_line.c -o ../mylib/read_line.o

../database/database.o: ../database/database.c ../database/database.h
	@echo "compiling database file..."
	@${CC} -Wall -c ../database/database.c -o ../database/database.o

clean: 
	@rm *.o server client
	@echo "Removed all binary and obj files..."
