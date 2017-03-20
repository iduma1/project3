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

//Indeed, myString needs to be a copy of the original string
/*
std::string StringToUpper(std::string myString)
{
  const int length = myString.length();
  for(int i=0; i!=length ; ++i)
    {
      myString[i] = std::toupper(myString[i]);
    }
  return myString;
}
*/

// fifo for communication
string receive_fifo = "status";
string send_fifo = "nextMove";

int main() {

	Cgicc cgi;// Ajax object
	char *cstr;

	// Create AJAX objects to recieve information from web page.
	form_iterator pname = cgi.getElement("player");//Gets the player who made move
	form_iterator pos = cgi.getElement("pos");//Gets the move the player made

	// create the FIFOs for communication
	Fifo recfifo(receive_fifo);
	Fifo sendfifo(send_fifo);

	string playerMove = "$"+**pname+"$"+**pos;//Creates a string with player name and the position $player$position
	
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
