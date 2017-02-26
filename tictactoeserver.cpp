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
			case onePlayer: current = onePlayerFn(player2Name);
			case twoPlayer: cout << "Both players connecteD!" << endl;
		}
	}
	return 0;
}

state noPlayerFn(string& player1Name) {
	cout << "No players connected";
	sendfifo.openwrite();//Blocks out others
	recfifo.openread();//Opens rec fifo
	player1Name = recfifo.recv();//stores player 1 name
	cout << "Message received--player 1's name is: " << player1Name;
	recfifo.fifoclose();
	sendfifo.fifoclose();
	return onePlayer;
}

state onePlayerFn(string& player2Name) {
	sendfifo.openwrite();
	recfifo.openread();
	player2Name = recfifo.recv();
	cout << "Message received--player 2's name is: " << player2Name;
	recfifo.fifoclose();
	sendfifo.fifoclose();
	return twoPlayer;
}