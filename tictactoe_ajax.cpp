#include <iostream>
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include "fifo.h"

using namespace std;
using namespace cgicc; // Needed for AJAX functions.


// fifo for communication
string receive_fifo = "status";
string send_fifo = "nextMove";

int main() {

	Cgicc cgi;// Ajax object
	char *cstr;
	string playerMove;

	// Create AJAX objects to recieve information from web page.
	form_iterator command = cgi.getElement("command");//Gets the command
	form_iterator playerName = cgi.getElement("player");//Gets the player who made move
	form_iterator pos = cgi.getElement("pos");//Gets the move the player made


	// create the FIFOs for communication
	Fifo recfifo(receive_fifo);
	Fifo sendfifo(send_fifo);
	
	if(**command != "sendMove") {
		playerMove = "$"+**command+"$"+**playerName+"$"+**pos;
	} else {
		playerMove = "$"+**command+"$"+**playerName+"$"+**pos;//Creates a string with player name and the position $player$position
	}
	
	/*Send message to server*/
	sendfifo.openwrite();
	sendfifo.send(playerMove);
	
	/*Get message from server*/
	recfifo.openread();
	string board = recfifo.recv();//This string contains the board state
	
	/*Move 'em out, Close 'em up rawhide*/
	recfifo.fifoclose();
	sendfifo.fifoclose();
	
	cout << "Content-Type: text/plain\n\n";
	cout << board;//Prints out the board 

return 0;
}
