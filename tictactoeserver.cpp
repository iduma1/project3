#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "fifo.h"

using namespace std;

/* Fifo names */
string receive_fifo = "nextMove";
string send_fifo = "status";

/* Make Fifos */
Fifo recfifo(receive_fifo);
Fifo sendfifo(send_fifo);

enum state {noPlayer, onePlayer, twoPlayer, startPlay};

state noPlayerFn(string& player1Name);

state onePlayerFn(string& player2Name);

int main() {
	state current;
	current = noPlayer;
	string player1Name, player2Name;
	cout << "Waiting for people to connect..." << endl;
	while (1) {
		switch (current) {
			case noPlayer:	current = noPlayerFn(player1Name);
				break;
			case onePlayer: current = onePlayerFn(player2Name);
				break;
			case twoPlayer: //cout << "Both players connecteD!" << endl;
				break;
		}
	}
	return 0;
}

state noPlayerFn(string& player1Name) {
	cout << "No players connected" << endl;
	//sendfifo.openwrite();//Blocks out others
	//cout << "open send fifo" << endl;
	recfifo.openread();//Opens rec fifo
	cout << "waiting ... " << endl;
	player1Name = recfifo.recv();//stores player 1 name
	cout << "Message received--player 1's name is: " << player1Name << endl;
	recfifo.fifoclose();
	//sendfifo.fifoclose();
	return onePlayer;
}

state onePlayerFn(string& player2Name) {
	//sendfifo.openwrite();
	recfifo.openread();
	player2Name = recfifo.recv();
	cout << "Message received--player 2's name is: " << player2Name << endl;
	recfifo.fifoclose();
	//sendfifo.fifoclose();
	return twoPlayer;
}