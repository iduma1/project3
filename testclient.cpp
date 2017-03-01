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

//Just testing things 
	
// create the FIFOs for communication
Fifo recfifo(receive_fifo);
Fifo sendfifo(send_fifo);

void checkStatus(string status, string playerName, bool& youWon, bool& youLost, bool& tie);
string makeMessage(string playerName);

int main() {

	string playerName, message, serverStatus, position;
 
	cout << "Opening send FIFO" << endl;
	sendfifo.openwrite();
	cout << "Opened send FIFO" << endl;

	cout << "Enter your name:" << endl;
	cin >> playerName;
	cout << "Enter a position:" << endl;
	cin >> position;
	
	message = "$" + playerName + "$" + position;
	
	sendfifo.send(message);
	cout << "Sent: " << message << endl;
	
	recfifo.openread();
	serverStatus = recfifo.recv();
	recfifo.fifoclose();
	cout << "Received: " << serverStatus << endl;
	
	bool youWon, youLost, tie;
	checkStatus(serverStatus, playerName, youWon, youLost, tie);
	
	if (youWon == true) {
		cout << "You won! :)" << endl;
	} else if (youLost == true) {
		cout << "You lost! :(" << endl;
	} else if (tie == true) {
		cout << "It's a tie. Lame." << endl;
	}
	return 0;
}

void checkStatus(string status, string playerName, bool& youWon, bool& youLost, bool& tie) {

	string player1String = "$WIN" + playerName;
	string winString = "$WIN";
	string tieString = "$TIE";
	
	if (status.find(player1String) != -1) {
		youWon = true;
		return;
	} else if (status.find(winString) != -1) {
		youLost = true;
		return;
	} else if (status.find(tieString) != -1) {
		tie = true;
		return;
	}
}

string makeMessage(string playerName) {
	string message, finalMessage;
	bool error = false;
	do {
		cout << "Enter coordinates." << endl;
		cin >> message;
		if (message.length() != 1) {
			cout << "Your message must be a number between 0 and 8." << endl;
			error = true;
		}
	} while (error == true);
	
	finalMessage = "$" + message + playerName;
	return finalMessage;
}