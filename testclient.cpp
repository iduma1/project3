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
	cout << "Sent: " << playerName << endl;
	
	string readyToPlay = "$GO" + playerName;
	
	while (1) {
	
		//check whether or not the user has received "readyToGo" with their name attached.
		
		while (readyToGo(readyToPlay) == false) {
			cout << "Waiting for my turn..." << endl;
		}

		message = makeMessage(playerName);
		sendfifo.send(message);
		
		cout << "Sent: " << message << endl;
		
		//cout << "Received: " << serverStatus << endl;
		
		/*while (error(serverStatus) == true) {
			message = makeMessage(playerName);
			sendfifo.send(message);		
			cout << "Sent: " << message << endl;	
		}*/
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
	cout << "Received: " << serverStatus << endl;
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
