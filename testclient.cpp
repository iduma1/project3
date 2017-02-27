#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "fifo.h"

using namespace std;

string receive_fifo = "status";
string send_fifo = "nextMove";
	
// create the FIFOs for communication
Fifo recfifo(receive_fifo);
Fifo sendfifo(send_fifo);

int main() {
	string player1, player2, message, serverStatus;
	string errorString = "$INVALID";
	string player1String = "$P1WIN";
	string player2String = "$P2WIN";

 
	cout << "Opening send FIFO" << endl;
	sendfifo.openwrite();
	cout << "Opened send FIFO" << endl;

	cout << "Enter player 1 name:" << endl;
	cin >> player1;
	sendfifo.send(player1);
	cout << "Sent player 1 name..." << endl;

	cout << "Enter player 2 name: " << endl;
	cin >>  player2;
	sendfifo.send(player2);
	cout << "Sent player 2 name..." << endl;

	cout << "Enter a number between 0-8. These correspond to the positions on the tic tac toe board." << endl;
	cout << "The increase from left to right (e.g. 0 is the top left corner, 2 is the top right, 8 is the bottom right." << endl;
	while (1) {
		cin >> message;
		if (message.length() != 1) {
			cout << "Your message must be a number between 0 and 8." << endl;
			break;
		}
		sendfifo.send(message);
		recfifo.openread();
		serverStatus = recfifo.recv();
		if (serverStatus.find(errorString) != -1) {
			cout << "The server sent back an error--select a different square." << endl;
		} else if (serverStatus.find(player1String) != -1) {
			cout << player1 << " won!" << endl;
			recfifo.fifoclose();
			break;
		} else if (serverStatus.find(player2String) != -1) {
			cout << player2 << " won!" << endl;
			recfifo.fifoclose();
			break;
		}
		recfifo.fifoclose();
	}
	sendfifo.fifoclose();
	cout << "Closing the fifo, resetting the game." << endl;
	
}