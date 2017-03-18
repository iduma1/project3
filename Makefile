#MakeFile to build and deploy the tictactoe server and testclient
# For CSC3004 Software Development

# Put your user name below:
USER= solorioc

CC= g++

#For Optimization
#CFLAGS= -O2
#For debugging
CFLAGS= -g

RM= /bin/rm -f

all: tictactoeserver testclient PutCGI PutHTML

testclient.o: testclient.cpp fifo.h
	$(CC) -c $(CFLAGS) testclient.cpp

tictactoeserver.o: tictactoeserver.cpp fifo.h
	$(CC) -c $(CFLAGS) tictactoeserver.cpp


testclient: testclient.o fifo.o
	$(CC) testclient.o fifo.o -o testclient

tictactoeserver: tictactoeserver.o fifo.o game.o
	$(CC) tictactoeserver.o  fifo.o game.o -o tictactoeserver 

fifo.o:	fifo.cpp fifo.h
	g++ -c fifo.cpp
game.o: game.cpp game.h
	g++ -c game.cpp

PutCGI: tictactoe_ajax.cpp
	chmod 757 tictactoe_ajax
	cp tictactoe_ajax /usr/lib/cgi-bin/$(USER)_tictactoe_ajax.cgi 

	#echo "Current contents of your cgi-bin directory: "
	#ls -l /usr/lib/cgi-bin/

PutHTML:
	cp tictactoe_ajax.html /var/www/html/class/softdev/$(USER)
	cp tictactoe_ajax.css /var/www/html/class/softdev/$(USER)
	cp tictactoe_java.js /var/www/html/class/softdev/$(USER)
	#echo "Current contents of your HTML directory: "
	#ls -l /var/www/html/class/softdev/$(USER)

clean:
	rm -f *.o tictactoeserver testclient 
