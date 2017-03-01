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

enum state {noPlayer, onePlayer, twoPlayer, player1Turn, player2Turn, player1Win, player2Win, tie, exitGame};

/* State machine functions */
state noPlayerFn(Game& game);
state onePlayerFn(Game& game);
state twoPlayerFn(Game& game);
state player1TurnFn(Game& game);
state player2TurnFn(Game& game);
state player1WinFn(Game& game);
state player2WinFn(Game& game);
state tieFn();

string parseName(string message);
string parseCoord(string message);

int main() {

	Game game;
	state current = noPlayer;
	
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
			case tie: current = tieFn();
				break;
			case player1Win: current = player1WinFn(game);
				break;
			case player2Win: current = player2WinFn(game);
				break;
			case exitGame: return 0;
		}
	}
	return 0;
}

string parseName(string message) {
	
	//find first $
	int start = message.find_first_of("$");
	//cout << start << endl;
	//find second $
	int end = message.find_last_of("$");
	//cout << end << endl;
	
	string name = message.substr(start + 1, end - 1);
	cout << "Name is: " << name << endl;
	return name;
	
}

string parseCoord(string message) {
	
	//find last $
	int second = message.find_last_of("$");
	//cout << second << endl;
	
	string coord = message.substr(second + 1);
	cout << "Coord is: " << coord << endl;
	return coord;
}

state noPlayerFn(Game& game) {
	cout << "No players connected" << endl;
	recfifo.openread();						//Open rec fifo
	string message = recfifo.recv();
	cout << "Received: " << message << endl;
	
	string player1Name = parseName(message);
	
	game.setPlayer1Name(player1Name);		//store player 1 name
	recfifo.fifoclose();					//close rec fifo
	return onePlayer;
}

state onePlayerFn(Game& game) {
	recfifo.openread();						//Open rec fifo
	string message = recfifo.recv();
	cout << "Received: " << message << endl;
	
	string player2Name = parseName(message);
	
	game.setPlayer2Name(player2Name);		//store player 2 name
	recfifo.fifoclose();					//close rec fifo
	return twoPlayer;
}

state twoPlayerFn(Game& game) {

	string player1Name = game.getPlayer1Name();
	string readyToPlay = "$GO" + player1Name;
	
	cout << "Both players connected!" << endl;
	
	game.clearBoard(); 			//clear game board
	sendfifo.openwrite();		//open send fifo
	sendfifo.send(readyToPlay);	//send "ready" with player1's name attached
	cout << "Sent: " << readyToPlay << endl;
	sendfifo.fifoclose();		//close send fifo
	
	sendfifo.openwrite();		//open send fifo
	sendfifo.send(readyToPlay);	//send "ready" with player1's name attached
	cout << "Sent: " << readyToPlay << endl;
	sendfifo.fifoclose();		//close send fifo
		
	return player1Turn;
}

state player1TurnFn(Game& game) {
	string player2Name = game.getPlayer2Name();
	string player1Name = game.getPlayer1Name();
	
	string errorString = "$INVALID" + player1Name;
	string readyToGo = "$GO" + player2Name;
	
	bool legitMove = true;
		
	recfifo.openread();						//open rec fifo
	string message = recfifo.recv();		
	cout << "Received: " << message << endl;//retrieve input from player1
	recfifo.fifoclose();					//close rec fifo
	
	legitMove = game.makeMove(message); //makes a move, but returns false if move is invalid
	
	game.displayBoard();
	
	//if the move the user made was invalid, 
	//send them an error and run through their turn again
	if (legitMove == false) {
	
		sendfifo.openwrite();
		sendfifo.send(errorString);
		cout << "Sent: " << errorString << endl;
		sendfifo.fifoclose();
		
		sendfifo.openwrite();
		sendfifo.send(errorString);
		cout << "Sent: " << errorString << endl;
		sendfifo.fifoclose();
		return player1Turn; //go back to player 1's turn
	} 
	
	//if the move was valid, and it wasn't a winning move
	//send "ready to go" with player 2's name attached
	else if (game.checkWin() != true) {
	
		int moves = game.getNumberOfMoves();
		if (moves > 8) { //max possible moves
			return tie;
		}
		
		sendfifo.openwrite();
		sendfifo.send(readyToGo);
		cout << "Sent: " << readyToGo << endl;
		sendfifo.fifoclose();
		
		sendfifo.openwrite();
		sendfifo.send(readyToGo);
		cout << "Sent: " << readyToGo << endl;
		sendfifo.fifoclose();
		
		return player2Turn; //proceed to player 2's turn
		
	//if a win was detected, return a win for player 1.
	} else {
		cout << "A win (P1) was detected!" << endl;
		return player1Win;
	}
	
	return player2Turn;
}

state player2TurnFn(Game& game) {
	string player1Name = game.getPlayer1Name();
	string player2Name = game.getPlayer2Name();
	
	string errorString = "$INVALID" + player2Name;
	string readyToGo = "$GO" + player1Name;

	bool legitMove = true;
		
	recfifo.openread();
	string message = recfifo.recv(); 		//open rec fifo
	cout << "Received: " << message << endl;//receive input from player2
	recfifo.fifoclose();					//close rec fifo
	
	legitMove = game.makeMove(message); //makes a move, but returns false if move is invalid
	
	game.displayBoard();
	
	//if the move the user made was invalid, 
	//send them an error and run through their turn again
	if (legitMove == false) {
	
		sendfifo.openwrite();
		sendfifo.send(errorString);
		cout << "Sent: " << errorString << endl;
		sendfifo.fifoclose();
		
		sendfifo.openwrite();
		sendfifo.send(errorString);
		cout << "Sent: " << errorString << endl;
		sendfifo.fifoclose();
		
		return player2Turn; //go back to player 2's turn
		
	//if the move was valid, and it wasn't a winning move
	//send "ready to go" with player 1's name attached	
	} else if (game.checkWin() != true) {
		
		sendfifo.openwrite();
		sendfifo.send(readyToGo);
		cout << "Sent: " << readyToGo << endl;
		sendfifo.fifoclose();

		sendfifo.openwrite();
		sendfifo.send(readyToGo);
		cout << "Sent: " << readyToGo << endl;
		sendfifo.fifoclose();

		return player1Turn; //proceed to player 1's turn
		
	//if a win was detected, return a win for player 2.	
	} else {
		cout << "A win (P2) was detected!" << endl;
		return player2Win;
	}
	return player1Turn;
}

state player1WinFn(Game& game) {
	string player1Name = game.getPlayer1Name();
			
	string winString = "$WIN" + player1Name;

	sendfifo.openwrite();
	sendfifo.send(winString);
	cout << "Sent: " << winString << endl;
	sendfifo.fifoclose();
	
	sendfifo.openwrite();
	sendfifo.send(winString);
	cout << "Sent: " << winString << endl;
	sendfifo.fifoclose();

	//cout << "Player 1 won!" << endl;
	//cout << "Player 2 lost!" << endl;
	return exitGame;
}

state player2WinFn(Game& game) {
	string player2Name = game.getPlayer2Name();
	string winString = "$WIN" + player2Name;

	sendfifo.openwrite();
	sendfifo.send(winString);
	cout << "Sent: " << winString << endl;
	sendfifo.fifoclose();
	
	sendfifo.openwrite();
	sendfifo.send(winString);
	cout << "Sent: " << winString << endl;
	sendfifo.fifoclose();

	//cout << "Player 2 won!" << endl;
	//cout << "Player 1 lost!" << endl;
	return exitGame;
}

state tieFn() {
	string tieString = "$TIE";
	
	sendfifo.openwrite();
	sendfifo.send(tieString);		
	cout << "Sent: " << tieString << endl;
	sendfifo.fifoclose();
	
	sendfifo.openwrite();
	sendfifo.send(tieString);
	cout << "Sent: " << tieString << endl;
	sendfifo.fifoclose();
	
	return exitGame;
}