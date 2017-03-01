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

bool checkStatus(string playerName, bool& youWon, bool& youLost, bool& tie);
string makeMessage(string playerName);

int main() {
	string playerName, message, serverStatus;
 
	cout << "Opening send FIFO" << endl;
	sendfifo.openwrite();
	cout << "Opened send FIFO" << endl;

	cout << "Enter your name:" << endl;
	cin >> playerName;
	sendfifo.send(playerName);
	cout << "Sent: " << playerName << endl;
		
	while (1) {

		bool youWon = false;
		bool youLost = false;
		bool tie = false;
		
		while (checkStatus(playerName, youWon, youLost, tie) == false) {
			cout << "Waiting for my turn..." << endl;
		}
		
		if (youWon == true) {
			cout << "You won! Congratulations!" << endl;
			break;
		} else if (youLost == true) {
			cout << "You lost! Sorry :(" << endl;
			break;
		} else if (tie == true) {
			cout << "It's a tie! Lame." << endl;
			break;
		}

		message = makeMessage(playerName);
		sendfifo.send(message);
		
		cout << "Sent: " << message << endl;
	}
	sendfifo.fifoclose();
	cout << "Closing the fifo, ending the game." << endl;
}

bool checkStatus(string playerName, bool& youWon, bool& youLost, bool& tie) {

	string errorString = "$INVALID" + playerName;
	string readyToPlay = "$GO" + playerName;
	string player1String = "$WIN" + playerName;
	string winString = "$WIN";
	string tieString = "$TIE";
	
	recfifo.openread();
	string serverStatus = recfifo.recv();
	recfifo.fifoclose();
	cout << "Received: " << serverStatus << endl;
	if (serverStatus.find(player1String) != -1) {
		youWon = true;
		return true;
	} else if (serverStatus.find(winString) != -1) {
		youLost = true;
		return true;
	} else if (serverStatus.find(tieString) != -1) {
		tie = true;
		return true;
	} else if (serverStatus.find(readyToPlay) != -1 || serverStatus.find(errorString) != -1) {
		return true;
	} else {
		return false;
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