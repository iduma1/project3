#MakeFile to build and deploy the tictactoe server and testclient
# For CSC3004 Software Development

# Put your user name below:
USER= wilhelmj

CC= g++

#For Optimization
#CFLAGS= -O2
#For debugging
CFLAGS= -g

RM= /bin/rm -f

#all: tictactoeserver testclient PutCGI PutHTML
all: tictactoeserver testclient 

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

		
#tictactoeclient: namefetchajax.o  fifo.h
#	$(CC) namefetchajax.o  fifo.o -o namefetchajax -L/usr/local/lib -lcgicc

#PutCGI: tictactoeclient
	#chmod 757 namefetchajaxmak
	#cp namefetchajax /usr/lib/cgi-bin/$(USER)_namefetchajax.cgi 

	#echo "Current contents of your cgi-bin directory: "
#	ls -l /usr/lib/cgi-bin/

#PutHTML:
	#cp name_stats_ajax.html /var/www/html/class/softdev/$(USER)
	#cp name_stats_ajax.css /var/www/html/class/softdev/$(USER)

	echo "Current contents of your HTML directory: "
	ls -l /var/www/html/class/softdev/$(USER)

clean:
	rm -f *.o tictactoeserver testclient
