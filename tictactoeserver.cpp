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
#include <string>
#include "fifo.h"

using namespace std;

/* Fifo names */
string receive_fifo = "nextMove";
string send_fifo = "status";

/* Make Fifos */
Fifo recfifo(receive_fifo);
Fifo sendfifo(send_fifo);

enum state {noPlayer, onePlayer, player1Turn, player2Turn, player1Win, player2Win};

/* State machine functions */
state noPlayerFn(string& player1Name);
state onePlayerFn(string& player2Name);
state player1TurnFn(string& pos);
state player2TurnFn(string& pos);
state player1WinFn(int& player1Wins);
state player2WinFn(int& player2Wins);

int main() {
		
	state current;
	current = noPlayer;
	string player1Name, player2Name;
	string pos;
	int player1Wins, player2Wins;
	
	cout << "Waiting for people to connect..." << endl;
	while (1) {
		switch (current) {
			case noPlayer:	current = noPlayerFn(player1Name);
				break;
			case onePlayer: current = onePlayerFn(player2Name);
							cout << "Both players connected!" << endl;
				break;
			case player1Turn: current = player1TurnFn(pos);
				break;
			case player2Turn: current = player2TurnFn(pos);
				break;
			case player1Win: current = player1WinFn(player1Wins);
				break;
			case player2Win: current = player2WinFn(player2Wins);
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
	return player1Turn;
}

state player1TurnFn(string& pos) {
	recfifo.openread();
	pos = recfifo.recv();
	cout << "Pos received: " << pos << endl;
	recfifo.fifoclose();
	
	//check win condition here
	//if wincondition, return loss
	
	return player2Turn;
}

state player2TurnFn(string& pos) {
	recfifo.openread();
	pos = recfifo.recv();
	cout << "Pos received: " << pos << endl;
	recfifo.fifoclose();
	
	//check win condition here
	//if wincondition, return loss
	
	return player1Win;
}

state player1WinFn(int& player1Wins) {
	cout << "Player 1 won!" << endl;
	cout << "Player 2 lost!" << endl;
	player1Wins++;
	return noPlayer;
}

state player2WinFn(int& player2Wins) {
	cout << "Player 2 won!" << endl;
	cout << "Player 1 lost!" << endl;
	player2Wins++;
	return noPlayer;
}