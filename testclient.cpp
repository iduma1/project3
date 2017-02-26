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

	cout << "Enter player 1 name:";
	cin >> player1;
	cout << "Yeah, I got that. thanks" << endl;
	
	cout << "Opening send FIFO" << endl;
	sendfifo.openwrite();
	cout << "Opened send FIFO" << endl;
	
	cout << "Sending player 1 name...";
	sendfifo.send(player1);
	cout << "Sent player 1 name...";
	
	cout << "Enter player 2 name: ";
	cin >>  player2;
	
	sendfifo.send(player2);
	sendfifo.fifoclose();
	
	cout << "Both players connected... Waiting to start game";
	
	
	}
	}