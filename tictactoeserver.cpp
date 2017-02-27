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
#include "game.h"

using namespace std;

/* Fifo names */
string receive_fifo = "nextMove";
string send_fifo = "status";

/* Make Fifos */
Fifo recfifo(receive_fifo);
Fifo sendfifo(send_fifo);

enum state {noPlayer, onePlayer, player1Turn, player2Turn, player1Win, player2Win, exitGame};

/* State machine functions */
state noPlayerFn(string& player1Name);
state onePlayerFn(string& player2Name, Game& game);
state player1TurnFn(Game& game);
state player2TurnFn(Game& game);
state player1WinFn();
state player2WinFn();

int main() {
	Game game;
	state current;
	current = noPlayer;
	string player1Name, player2Name;
	
	cout << "Waiting for people to connect..." << endl;
	while (1) {
		switch (current) {
			case noPlayer:	current = noPlayerFn(player1Name);
				break;
			case onePlayer: current = onePlayerFn(player2Name, game);
				break;
			case player1Turn: current = player1TurnFn(game);
				break;
			case player2Turn: current = player2TurnFn(game);
				break;
			case player1Win: current = player1WinFn();
				break;
			case player2Win: current = player2WinFn();
				break;
			case exitGame: return 0;
		}
	}
	return 0;
}

state noPlayerFn(string& player1Name) {
	cout << "No players connected" << endl;
	recfifo.openread();//Opens rec fifo
	player1Name = recfifo.recv();//stores player 1 name
	cout << "Message received--player 1's name is: " << player1Name << endl;
	recfifo.fifoclose();
	return onePlayer;
}

state onePlayerFn(string& player2Name, Game& game) {
	recfifo.openread();
	player2Name = recfifo.recv();
	cout << "Message received--player 2's name is: " << player2Name << endl;
	recfifo.fifoclose();
	cout << "Both players connected!" << endl;
	game.clearBoard();
	return player1Turn;
}

state player1TurnFn(Game& game) {
	string errorString = "$INVALID";
	string happyString = "$GOOD";
	
	bool legitMove;
	
	recfifo.openread();
	string message = recfifo.recv();
	recfifo.fifoclose();
	
	legitMove = game.makeMove(message);
	cout << "Made a move, displaying it below." << endl;
	game.displayBoard();
	
	if (legitMove == false) {
		sendfifo.openwrite();
		sendfifo.send(errorString);
		sendfifo.fifoclose();
		return player1Turn;
	} else if (game.checkWin() != true) {
		sendfifo.openwrite();
		sendfifo.send(happyString);
		sendfifo.fifoclose();
	} else {
		cout << "A win (P1) was detected!" << endl;
		return player1Win;
	}
	
	return player2Turn;
}

state player2TurnFn(Game& game) {
	string errorString = "$INVALID";
	string happyString = "$GOOD";

	bool legitMove;
	
	recfifo.openread();
	string message = recfifo.recv();
	recfifo.fifoclose();
	
	legitMove = game.makeMove(message);
	cout << "Made a move, displaying it below." << endl;
	game.displayBoard();
	
	if (legitMove == false) {
		sendfifo.openwrite();
		sendfifo.send(errorString);
		sendfifo.fifoclose();
		return player2Turn;
	} else if (game.checkWin() != true) {
		sendfifo.openwrite();
		sendfifo.send(happyString);
		sendfifo.fifoclose();
	} else {
		cout << "A win (P2) was detected!" << endl;
		return player2Win;
	}
	return player1Turn;
}

state player1WinFn() {
	string winString = "$P1WIN";

	sendfifo.openwrite();
	sendfifo.send(winString);
	sendfifo.fifoclose();

	cout << "Player 1 won!" << endl;
	cout << "Player 2 lost!" << endl;
	return exitGame;
}

state player2WinFn() {		
	string winString = "$P2WIN";

	sendfifo.openwrite();
	sendfifo.send(winString);
	sendfifo.fifoclose();

	cout << "Player 2 won!" << endl;
	cout << "Player 1 lost!" << endl;
	return exitGame;
}