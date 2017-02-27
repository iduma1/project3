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
	string player1, player2, message;
	while (1) {
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

		cout << "Enter 0,0...0,1...0,2...1,2...etc" << endl;
		while (1) {
			cin >> message;
			if (message.length() > 4) {
				break;
			}
			sendfifo.send(message);
		}
	
		sendfifo.fifoclose();
		cout << "Closing the fifo, resetting the game." << endl;
	}
}
