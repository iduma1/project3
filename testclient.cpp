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

bool error(string serverStatus);
bool win(string playerName, string serverStatus);
bool readyToGo(string readyToPlay);
string makeMessage(string playerName);

int main() {
	string playerName, message, serverStatus;
 
	cout << "Opening send FIFO" << endl;
	sendfifo.openwrite();
	cout << "Opened send FIFO" << endl;

	cout << "Enter your name:" << endl;
	cin >> playerName;
	sendfifo.send(playerName);
	
	string readyToPlay = "$GO" + playerName;
	
	//cout << "Enter a number between 0-8. These correspond to the positions on the tic tac toe board." << endl;
	//cout << "The increase from left to right (e.g. 0 is the top left corner, 2 is the top right, 8 is the bottom right." << endl;
	while (1) {
		while (readyToGo(readyToPlay) == false) {
			cout << "Waiting for my turn..." << endl;
		}

		message = makeMessage(playerName);
		sendfifo.send(message);
		
		recfifo.openread();
		serverStatus = recfifo.recv();
		recfifo.fifoclose();
		
		cout << "Secondary server status: " << serverStatus << endl;
		while (error(serverStatus) == true) {
			message = makeMessage(playerName);
			sendfifo.send(message);			
		} 
		if (win(playerName, serverStatus) == true) {
			break;
		}
	}
	sendfifo.fifoclose();
	cout << "Closing the fifo, ending the game." << endl;
}

bool error(string serverStatus) {
	string errorString = "$INVALID";
	if (serverStatus.find(errorString) != -1) {
		cout << "Error. String that you sent was invalid." << endl;
		return true;
	} else {
		return false;
	}
}

bool win(string playerName, string serverStatus) {
	string player1String = "$WIN" + playerName;
	string winString = "$WIN";
	if (serverStatus.find(player1String) != -1) {
		cout << "You," << playerName << ", won!" << endl;
		return true;
	} else if (serverStatus.find(winString) != -1) {
		cout << "The other player won :(" << endl;
		return true;
	} else {
		return false;
	}
}

bool readyToGo(string readyToPlay) {
	recfifo.openread();
	string serverStatus = recfifo.recv();
	recfifo.fifoclose();
	if (serverStatus.find(readyToPlay) != -1) {
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
