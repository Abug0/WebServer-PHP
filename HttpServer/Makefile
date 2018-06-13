HttpServer.exe: HttpServer.o handleRequest.o handleResource.o parseRequest.o awei_string.o awei_conf.o error.o fastcgi.o
	gcc HttpServer.o handleRequest.o handleResource.o parseRequest.o awei_string.o awei_conf.o error.o -lwsock32 fastcgi.o -o HttpServer.exe
HttpServer.o: HttpServer.h HttpServer.c  parseRequest.h
	gcc -c HttpServer.c 
handleRequest.o: handleRequest.h handleRequest.c
	gcc -c handleRequest.c
handleResource.o: handleResource.h handleResource.c
	gcc -c handleResource.c
parseRequest.o: HttpServer.h parseRequest.c awei_string.h handleRequest.h
	gcc -c parseRequest.c
getResource.o: HttpServer.h getResource.c
	gcc -c getResource.c
awei_string.o: awei_string.h awei_string.c
	gcc -c awei_string.c
awei_conf.o: awei_conf.h awei_conf.c
	gcc -c awei_conf.c
error.o: error.h error.c
	gcc -c error.c
fastcgi.o: fastcgi.h fastcgi.c
	gcc -c fastcgi.c