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

enum state {noPlayer, onePlayer, twoPlayer, takeTurn, win, tie, exitGame};

/* State machine functions */
state noPlayerFn(Game& game, Player& player1);
state onePlayerFn(Game& game, Player player1, Player& player2);
state twoPlayerFn(Game& game);
state takeTurnFn(Game& game, Player player);
state player1TurnFn(Game& game);
state player2TurnFn(Game& game);
state player1WinFn(Game& game);
state player2WinFn(Game& game);
state winFn(Game& game, Player player);
state tieFn(Game& game);
state exitGameFn(Game& game, Player player);

/* Assisting functions */
string parseName(string message);
string parseCoord(string message);

int main() {

	Game game;
	game.initializeBoard();
	
	Player player1, player2;
	game.addPlayer(player1);
	game.addPlayer(player2);
	
	state current = noPlayer;
	
	cout << "Waiting for people to connect..." << endl;
	while (1) {
		switch (current) {
			case noPlayer:	current = noPlayerFn(game, player1);
				break;
			case onePlayer: current = onePlayerFn(game, player1, player2);
				break;
			case twoPlayer: current = twoPlayerFn(game);
				break;
			case takeTurn: 
				if (game.getCurrentPlayerValue() == 0) {
					current = takeTurnFn(game, player2);
				} else if (game.getCurrentPlayerValue() == 1) {
					current = takeTurnFn(game, player1);
				}
				break;
			case win: 
				if (game.getCurrentPlayerValue() == 0) {
					current = winFn(game, player2);
				} else if (game.getCurrentPlayerValue() == 1) {
					current = winFn(game, player1);
				}
				break;
			case tie: current = tieFn(game);
				break;
			case exitGame: 
				if (game.getCurrentPlayerValue() == 0) {
					current = exitGameFn(game, player2);
				} else if (game.getCurrentPlayerValue() == 1) {
					current = exitGameFn(game, player1);
				}
				break;
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
	
	string player1Name = parseName(message);
	string boardState = game.getBoardState() + ",onePlayer";
	
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

state onePlayerFn(Game& game, Player player1, Player& player2) {

	recfifo.openread();						//Open rec fifo
	string message = recfifo.recv();		//receive the message
	recfifo.fifoclose();					//close rec fifo
	
	cout << "Received: " << message << endl;
	
	string player1Name = player1.getPlayerName();
	string player2Name = parseName(message);
	string boardState = game.getBoardState() + "," + player1.getPlayerName();
	
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

state takeTurnFn(Game& game, Player player) {
	
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
	if (messageSig == player.getPlayerName() || message == getUpdate) {
		cout << "Sending an unchanged boardstate." << endl;
		sendfifo.openwrite();
		sendfifo.send(boardState);
		cout << "Sent: " << boardState << endl;
		sendfifo.fifoclose();
		return takeTurn;
	}
	
	//otherwise, make their move
	string coord = parseCoord(message); //parse the coordinates from the message
	game.makeMove(coord); //make the move using the received coordinate
	boardState = game.getBoardState() + ',' + player.getPlayerName(); //get the boardstate afterwards, prep the message to send to server
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
		return win;
	}
	
	return takeTurn;
}

state winFn(Game& game, Player player) {
	string playerName = player.getPlayerName();
	string winString = game.getBoardState() + ",WIN," + playerName;
	
	sendfifo.openwrite();
	sendfifo.send(winString);
	cout << "Sent: " << winString << endl;
	sendfifo.fifoclose();

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

state exitGameFn(Game& game, Player player) {
	
	recfifo.openread();
	string message = recfifo.recv();
	recfifo.fifoclose();
	
	string winString = game.getBoardState() + ",WIN," + player.getPlayerName();
	sendfifo.openwrite();
	sendfifo.send(winString);
	sendfifo.fifoclose();
	cout << "Sent: " << winString << endl;
	
	return noPlayer;
	
	/*if (message == playAgain!!) {
		return noPlayer;
	} else {
		return exitGame;
	}*/
	
}