CFLAGS = -Wall -lpthread -llog4c
LIB = -L /usr/log4c/lib
INCLUDE = -I ./include -I /usr/log4c/include
STD = -std=gnu99

ALL: serv

serv: main.o util.o url_reg.o http_parse.o http_server.o socket.o epoll.o logger.o thread_pool.o
	gcc -o serv ${CFLAGS} ${LIB} ${STD} main.o util.o url_reg.o http_parse.o http_server.o socket.o epoll.o logger.o thread_pool.o

main.o: main.c
	gcc -c -g ${INCLUDE} ${STD} main.c

http_parse.o: http_server/http_parse.c
	gcc -c -g ${INCLUDE} ${STD} http_server/http_parse.c

http_server.o: http_server/http_server.c
	gcc -c -g ${INCLUDE} ${STD} http_server/http_server.c

socket.o: http_server/socket.c
	gcc -c -g ${INCLUDE} ${STD} http_server/socket.c

url_reg.o: http_server/url_reg.c
	gcc -c -g ${INCLUDE} ${STD} http_server/url_reg.c

util.o: http_server/util.c
	gcc -c -g ${INCLUDE} ${STD} http_server/util.c

epoll.o: http_server/epoll.c
	gcc -c -g ${INCLUDE} ${STD} http_server/epoll.c

logger.o: log/logger.c
	gcc -c -g ${INCLUDE} ${STD} log/logger.c

thread_pool.o: thread_pool/thread_pool.c
	gcc -c -g ${INCLUDE} ${STD} thread_pool/thread_pool.c

clean:
	rm -rf serv *.o

.PHONY: clean ALL