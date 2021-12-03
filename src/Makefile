CFLAGS = -Wall -lpthread -llog4c
LIB = -L /usr/local/log4c/lib
INCLUDE = -I /usr/log4c/include ./include

ALL: serv

serv: main.o http_server.o socket.o epoll.o logger.o thread_pool.o
    gcc -o serv ${CFLAGS} ${LIB} ${INCLUDE} http_server.o socket.o epoll.o logger.o thread_pool.o

main.o: main.c
    gcc -c main.c

http_server.o: http_server/http_server.c
    gcc -c http_server/http_server.c

socket.o: http_server/socket.c
    gcc -c http_server/socket.c

epoll.o: http_server/epoll.c
    gcc -c http_server/epoll.c

logger.o: log/logger.c
    gcc -c log/logger.c

thread_pool.o: thread_pool/thread_pool.c
    gcc -c thread_pool/thread_pool.c



clean:
    rm -rf serv *.o

.PHONY: clean ALL