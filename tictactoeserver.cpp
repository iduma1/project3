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
#include "player.h"

using namespace std;

/* Fifo names */
string receive_fifo = "nextMove";
string send_fifo = "status";

/* Make Fifos */
Fifo recfifo(receive_fifo);
Fifo sendfifo(send_fifo);

enum state {noPlayer, onePlayer, twoPlayer, player1Turn, player2Turn, takeTurn, player1Win, player2Win, tie, exitGame};

/* State machine functions */
state noPlayerFn(Game& game, Player& player1);
state onePlayerFn(Game& game, Player& player2);
state twoPlayerFn(Game& game);
state takeTurnFn(Game& game, Player& player);
state player1TurnFn(Game& game);
state player2TurnFn(Game& game);
state player1WinFn(Game& game);
state player2WinFn(Game& game);
state tieFn(Game& game);

/* Assisting functions */
string parseName(string message);
string parseCoord(string message);

int main() {

	Game game;
	game.initializeBoard();
	
	Player player1, player2;
	
	state current = noPlayer;
	
	cout << "Waiting for people to connect..." << endl;
	while (1) {
		switch (current) {
			case noPlayer:	current = noPlayerFn(game, player1);
				break;
			case onePlayer: current = onePlayerFn(game, player2);
				break;
			case twoPlayer: current = twoPlayerFn(game);
				break;
			case takeTurn: 
				if (game.getCurrentPlayer() == 1) {
					current = takeTurnFn(game, player1);
				} else if (game.getCurrentPlayer() == 2) {
					current = takeTurnFn(game, player2);
				} else {
					cout << "Fatal error, I don't know whose turn it is." << endl;
					return 0;
				}
				break;
			case player1Turn: current = player1TurnFn(game);
				break;
			case player2Turn: current = player2TurnFn(game);
				break;
			case tie: current = tieFn(game);
				break;
			case player1Win: current = player1WinFn(game);
				break;
			case player2Win: current = player2WinFn(game);
				break;
			case exitGame: current = noPlayerFn(game, player1);
		}
	}
	return 0;
}

string parseName(string message) {
	
	//parse the message to find the player's name
	int start = message.find_first_of("$");
	int end = message.find_last_of("$");
	string name = message.substr(start + 1, end - 1);
	
	return name;
}

string parseCoord(string message) {

	//parse the message to find the player's coordinate
	int second = message.find_last_of("$");
	string coord = message.substr(second + 1);

	return coord;
}

state noPlayerFn(Game& game, Player& player1) {

	game.clearBoard(); 						//clear game board

	cout << "No players connected" << endl;
	
	recfifo.openread();						//Open rec fifo
	string message = recfifo.recv();		//receive the message
	recfifo.fifoclose();					//close rec fifo
	
	cout << "Received: " << message << endl;
	
	string boardState = game.getBoardState();
	string player1Name = parseName(message);	
	
	if (player1Name == "update") {
		sendfifo.openwrite();
		sendfifo.send(boardState);
		cout << "Sent: " << boardState << endl;
		sendfifo.fifoclose();
		return noPlayer;
	}
	
	player1.setPlayerName(player1Name);		//store player 1 name
	
	sendfifo.openwrite();					//open send fifo
	sendfifo.send(boardState);				//send the boardstate
	sendfifo.fifoclose();					//close send fifo
	cout << "Sent: " << boardState << endl;
	
	return onePlayer;
}

state onePlayerFn(Game& game, Player& player2) {

	recfifo.openread();						//Open rec fifo
	string message = recfifo.recv();		//receive the message
	recfifo.fifoclose();					//close rec fifo
	
	cout << "Received: " << message << endl;
	
	string boardState = game.getBoardState();
	string player2Name = parseName(message);
	
	if (player2Name == "update") {
		sendfifo.openwrite();
		sendfifo.send(boardState);
		cout << "Sent: " << boardState << endl;
		sendfifo.fifoclose();
		return onePlayer;
	}
	
	player2.setPlayerName(player2Name);		//store player 2 name
		
	sendfifo.openwrite();					//open send fifo
	sendfifo.send(boardState);				//send the boardstate
	sendfifo.fifoclose();					//close send fifo
	cout << "Sent: " << boardState << endl;
	
	return twoPlayer;
}

state twoPlayerFn(Game& game) {
		
	cout << "Both players connected!" << endl;
	return takeTurn;
}

state takeTurnFn(Game& game, Player& player) {
	
	//receive the message
	recfifo.openread();
	string message = recfifo.recv();
	cout << "Received: " << message << endl;
	recfifo.fifoclose();
	
	string boardState = game.getBoardState(); //get the boardstate
	string messageSig = parseName(message); //parse the players' name from the message
	string getUpdate = "$update$10";
	
	//if the player's name isn't in the message or if the server is asking for an update, 
	//we don't want to make their move, so just send them an unchanged boardstate and come back to this function again
	
	if (messageSig != player.getPlayerName() || message == getUpdate) {
		sendfifo.openwrite();
		sendfifo.send(boardState);
		cout << "Sent: " << boardState << endl;
		sendfifo.fifoclose();
		return takeTurn;
	}
	
	//otherwise, make their move
	string coord = parseCoord(message); //parse the coordinates from the message
	game.makeMove(coord); //make the move using the received coordinate
	boardState = game.getBoardState(); //get the boardstate afterwards
	game.displayBoard(); //show the boardstate
	
	//check for a tie or a win, otherwise send back the boardstate
	if (game.checkWin() != true) {
		if (game.checkTie() != true) {
			sendfifo.openwrite();
			sendfifo.send(boardState);
			cout << "Sent: " << boardState << endl;
			sendfifo.fifoclose();
			return takeTurn;
		//if there wasn't a win, but there wasn't not a tie, then there was a tie (lol)
		} else {
			cout << "A tie was detected!" << endl;
			return tie;
		}
	//if there wasn't not a win, then there was a win
	} else {
		cout << "A win was detected!" << endl;
		if (game.getCurrentPlayer() == 1) {
			return player2Win;
		} else if (game.getCurrentPlayer() == 2) {
			return player1Win;
		} else {
			cout << "Except there was an error--I'm not sure which player won." << endl;
			return exitGame;
		}
	}
	
	return takeTurn;
}

state player1TurnFn(Game& game) {

	string getUpdate = "$update$10";

	string player2Name = game.getPlayer2Name();
	string player1Name = game.getPlayer1Name();
	string boardState = game.getBoardState();
	
	bool legitMove = true;
		
	//retrieve input from player1	
	recfifo.openread();						
	string message = recfifo.recv();		
	cout << "Received: " << message << endl; 
	recfifo.fifoclose();					
	
	if (message == getUpdate) {
		sendfifo.openwrite();
		sendfifo.send(boardState);
		cout << "Sent: " << boardState << endl;
		sendfifo.fifoclose();
		return player1Turn;
	}
	
	string coord = parseCoord(message); //parse the coordinates
	string messageSig = parseName(message); //parse the name
	
	//if you find player2's name in the message, then they're trying to make a move when
	//it isn't their turn. Send them the same boardstate, and reset the function
	if (messageSig == player2Name) {
		sendfifo.openwrite();
		sendfifo.send(boardState);
		cout << "Sent: " << boardState << endl;
		sendfifo.fifoclose();
		return player1Turn;
	}
	
	legitMove = game.makeMove(coord); //makes a move, but returns false if move is invalid
	boardState = game.getBoardState(); //get the boardstate after the move is made
											  //regardless of the move's validity.
	game.displayBoard();
	
	//if the move the user made was invalid, send them the unchanged boardstate
	if (legitMove == false) {
	
		sendfifo.openwrite();
		sendfifo.send(boardState);
		cout << "Sent: " << boardState << endl;
		sendfifo.fifoclose();
		return player1Turn; //go back to player 1's turn
	} 
	
	//else, if the move was valid, and it wasn't a winning move, send the boardstate 
	else if (game.checkWin() != true) {
	
		int moves = game.getNumberOfMoves();
		if (moves > 8) { //max possible moves
			return tie;
		}
		
		sendfifo.openwrite();
		sendfifo.send(boardState);
		cout << "Sent: " << boardState << endl;
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

	string getUpdate = "$update$10";

	string player1Name = game.getPlayer1Name();
	string player2Name = game.getPlayer2Name();
	string boardState = game.getBoardState();

	bool legitMove = true;
	
	//retrieve input from player2
	recfifo.openread();						
	string message = recfifo.recv();		
	cout << "Received: " << message << endl;
	recfifo.fifoclose();	
	
	if (message == getUpdate) {
		sendfifo.openwrite();
		sendfifo.send(boardState);
		cout << "Sent: " << boardState << endl;
		sendfifo.fifoclose();
		return player2Turn;
	}				
	
	string coord = parseCoord(message); //parse the coordinate
	string messageSig = parseName(message); //parse the player's name
	
	//if you find player1's name in the message, then they're trying to make a move when
	//it isn't their turn. Send them the same boardstate, and reset the function
	if (messageSig == player1Name) {
		sendfifo.openwrite();
		sendfifo.send(boardState);
		cout << "Sent: " << boardState << endl;
		sendfifo.fifoclose();
		return player2Turn;
	}
	
	legitMove = game.makeMove(coord); //makes a move, but returns false if move is invalid
	boardState = game.getBoardState(); //get the boardstate after the move is made
											  //regardless of the move's validity.
	game.displayBoard();
	
	//if the move the user made was invalid, send them the unchanged boardstate
	if (legitMove == false) {
	
		sendfifo.openwrite();
		sendfifo.send(boardState);
		cout << "Sent: " << boardState << endl;
		sendfifo.fifoclose();		
		return player2Turn; //go back to player 2's turn
		
	//else, if the move was valid, and it wasn't a winning move, send the boardstate 
	} else if (game.checkWin() != true) {
		
		sendfifo.openwrite();
		sendfifo.send(boardState);
		cout << "Sent: " << boardState << endl;
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
			
	string winString = game.getBoardState() + ",WIN," + player1Name;

	sendfifo.openwrite();
	sendfifo.send(winString);
	cout << "Sent: " << winString << endl;
	sendfifo.fifoclose();

	return exitGame;
}

state player2WinFn(Game& game) {
	string player2Name = game.getPlayer2Name();
	string winString = game.getBoardState() + ",WIN," + player2Name;

	sendfifo.openwrite();
	sendfifo.send(winString);
	cout << "Sent: " << winString << endl;
	sendfifo.fifoclose();

	//cout << "Player 2 won!" << endl;
	//cout << "Player 1 lost!" << endl;
	return exitGame;
}

state tieFn(Game& game) {
	string tieString = game.getBoardState() + ",TIE";
	
	sendfifo.openwrite();
	sendfifo.send(tieString);		
	cout << "Sent: " << tieString << endl;
	sendfifo.fifoclose();
	
	return exitGame;
}