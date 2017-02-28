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

enum state {noPlayer, onePlayer, twoPlayer, player1Turn, player2Turn, player1Win, player2Win, exitGame};

/* State machine functions */
state noPlayerFn(Game& game);
state onePlayerFn(Game& game);
state twoPlayerFn(Game& game);
state player1TurnFn(Game& game);
state player2TurnFn(Game& game);
state player1WinFn();
state player2WinFn();

int main() {
	Game game;
	state current;
	current = noPlayer;
	
	cout << "Waiting for people to connect..." << endl;
	while (1) {
		switch (current) {
			case noPlayer:	current = noPlayerFn(game);
				break;
			case onePlayer: current = onePlayerFn(game);
				break;
			case twoPlayer: current = twoPlayerFn(game);
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

state noPlayerFn(Game& game) {
	cout << "No players connected" << endl;
	recfifo.openread();//Opens rec fifo
	string player1Name = recfifo.recv();//stores player 1 name
	//cout << "Message received--player 1's name is: " << player1Name << endl;
	game.setPlayer1Name(player1Name);
	recfifo.fifoclose();
	return onePlayer;
}

state onePlayerFn(Game& game) {
	recfifo.openread();
	string player2Name = recfifo.recv();
	game.setPlayer2Name(player2Name);
	recfifo.fifoclose();
	return twoPlayer;
}

state twoPlayerFn(Game& game) {
	string player1Name = game.getPlayer1Name();
	string readyToPlay = "$GO" + player1Name;
	
	cout << "Both players connected!" << endl;
	
	game.clearBoard();
	sendfifo.openwrite();
	sendfifo.send(readyToPlay);
	sendfifo.fifoclose();
	
	cout << "Send the readyToPlay!" << endl;
	
	return player1Turn;
}

state player1TurnFn(Game& game) {
	string player2Name = game.getPlayer2Name();
	string errorString = "$INVALID";
	string happyString = "$GO" + player2Name;
	
	bool legitMove;
		
	recfifo.openread();
	string message = recfifo.recv();
	recfifo.fifoclose();
	
	/*if (message.find(player2Name) != -1) {
		cout << "Error: " << player2Name << ", it isn't your turn!" << endl; 
		legitMove = false;
	} else {
		legitMove = game.makeMove(message);
	}*/
	
	legitMove = game.makeMove(message);
	
	game.displayBoard();
	
	if (legitMove == false) {
		sendfifo.openwrite();
		sendfifo.send(errorString);
		sendfifo.fifoclose();
		return player1Turn;
	} else if (game.checkWin() != true) {
		sendfifo.openwrite();
		sendfifo.send(happyString);
		sendfifo.send(happyString);
		sendfifo.fifoclose();
	} else {
		cout << "A win (P1) was detected!" << endl;
		return player1Win;
	}
	
	return player2Turn;
}

state player2TurnFn(Game& game) {
	string player1Name = game.getPlayer1Name();
	string errorString = "$INVALID";
	string happyString = "$GO" + player1Name;

	bool legitMove = true;
		
	recfifo.openread();
	string message = recfifo.recv(); //receive the coordinate
	recfifo.fifoclose();
	
	/*if (message.find(player1Name) != -1) {
		cout << "Error: " << player1Name << ", it isn't your turn!" << endl; 
		legitMove = false;
	} else {
		legitMove = game.makeMove(message);
	}*/
	
	legitMove = game.makeMove(message);
	
	game.displayBoard();
	
	if (legitMove == false) {
		sendfifo.openwrite();
		sendfifo.send(errorString);
		sendfifo.fifoclose();
		return player2Turn;
	} else if (game.checkWin() != true) {
		sendfifo.openwrite();
		sendfifo.send(happyString);
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