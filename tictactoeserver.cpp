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
state takeTurnFn(Game& game, Player inactivePlayer, Player activePlayer);
state player1TurnFn(Game& game);
state player2TurnFn(Game& game);
state player1WinFn(Game& game);
state player2WinFn(Game& game);
state winFn(Game& game, Player player);
state tieFn(Game& game);
state exitGameFn(Game& game, Player player);

/* Assisting functions */
string parseCommand(string message);
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
					current = takeTurnFn(game, player2, player1);
				} else if (game.getCurrentPlayerValue() == 1) {
					current = takeTurnFn(game, player1, player2);
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

string parseCommand(string message) {

	//parse the message to find the player's name
	int first = message.find_first_of("$");
	int second = message.find("$", first + 1);
	string command = message.substr(first + 1, second - 1);
	
	return command;
}

string parseName(string message) {

	//parse the message to find the player's name
	int first = message.find_first_of("$");
	int second = message.find("$", first + 1);
	int third = message.find("$", second + 1);
	string name = message.erase(third);
	name = name.substr(second + 1);
		
	return name;
}

string parseCoord(string message) {

	//parse the message to find the player's coordinate
	int third = message.find_last_of("$");
	string coord = message.substr(third + 1);

	return coord;
}

state noPlayerFn(Game& game, Player& player1) {

	game.clearBoard(); 						//clear game board

	cout << "No players connected" << endl;	
	recfifo.openread();						//Open rec fifo
	string message = recfifo.recv();		//receive the message
	recfifo.fifoclose();					//close rec fifo
	cout << "Received: " << message << endl;
	sendfifo.openwrite();					//open send fifo
	
	if (parseCommand(message) != "playerRegister") {
		string boardState = game.getBoardState();
		sendfifo.send(boardState);				//send the boardstate
		sendfifo.fifoclose();					//close send fifo
		cout << "Sent: " << boardState << endl;
		return noPlayer;
	} else {
		string player1Name = parseName(message);	
		player1.setPlayerName(player1Name);		//store player 1 name
		string boardState = game.getBoardState() + ",onePlayer";
		sendfifo.send(boardState);				//send the boardstate
		sendfifo.fifoclose();					//close send fifo
		cout << "Sent: " << boardState << endl;
		return onePlayer;
	}
}

state onePlayerFn(Game& game, Player player1, Player& player2) {

	recfifo.openread();						//Open rec fifo
	string message = recfifo.recv();		//receive the message
	recfifo.fifoclose();					//close rec fifo	
	cout << "Received: " << message << endl;
	sendfifo.openwrite();					//open send fifo
	
	string boardState = game.getBoardState() + "," + player1.getPlayerName();
	if (parseCommand(message) == "playerRegister" /*|| parseCommand(message) == "restartServer"*/) {
		string player2Name = parseName(message);	
		player2.setPlayerName(player2Name);		//store player 1 name
		sendfifo.send(boardState);				//send the boardstate
		sendfifo.fifoclose();					//close send fifo
		cout << "Sent: " << boardState << endl;
		return twoPlayer;
	} else {
		boardState = game.getBoardState() + ",onePlayer";
	 	sendfifo.send(boardState);				//send the boardstate
		sendfifo.fifoclose();					//close send fifo
		cout << "Sent: " << boardState << endl;
		return onePlayer;
	}	
}


state twoPlayerFn(Game& game) {
		
	cout << "Both players connected!" << endl;
	return takeTurn;
}

state takeTurnFn(Game& game, Player inactivePlayer, Player activePlayer) {
	
	//receive the message
	recfifo.openread();
	string message = recfifo.recv();
	cout << "Received: " << message << endl;
	recfifo.fifoclose();
	
	string boardState = game.getBoardState(); //get the boardstate
	string name = parseName(message); //parse the players' name from the message
	string command = parseCommand(message); //parse command
	string coord = parseCoord(message); //parse coordinate
	
	cout << "The command I received was: " << command << endl;
	
	if (command == "makeMove" && name == activePlayer.getPlayerName()) {
		game.makeMove(coord);
		boardState = game.getBoardState() + ',' + inactivePlayer.getPlayerName(); //get the boardstate afterwards, prep the message to send to server
		game.displayBoard();
		//if there is no win
		if (game.checkWin() != true) {
			//if there is no tie
			if (game.checkTie() != true) {
				sendfifo.openwrite();
				sendfifo.send(boardState);
				cout << "Sent: " << boardState << endl;
				sendfifo.fifoclose();
				return takeTurn;	
			} else {
				cout << "A tie was detected!" << endl;
				return tie;
			}
		} else {
			cout << "A win was detected!" << endl;
			return win;
		}
		return takeTurn;
	//if the command wasn't makemove
	} else {
		boardState = game.getBoardState() + ',' + activePlayer.getPlayerName();
		sendfifo.openwrite();
		sendfifo.send(boardState);
		cout << "Sent: " << boardState << endl;
		sendfifo.fifoclose();
	}
	if (command == "restartServer") {
		return noPlayer;
	} else {
		return takeTurn;
	}
}

state winFn(Game& game, Player player) {
	string playerName = player.getPlayerName();
	string winString = game.getBoardState() + "," + playerName + ",WIN";
	
	sendfifo.openwrite();
	sendfifo.send(winString);
	cout << "Sent: " << winString << endl;
	sendfifo.fifoclose();

	return exitGame;
}

state tieFn(Game& game) {
	string tieString = game.getBoardState() + ",dummyname,TIE";
	
	sendfifo.openwrite();
	sendfifo.send(tieString);		
	cout << "Sent: " << tieString << endl;
	sendfifo.fifoclose();
	
	//wait to receive restartServer and go to no player
	//if not restartServer, just return tieFn again
	
	recfifo.openread();
	string message = recfifo.recv();
	recfifo.fifoclose();
	cout << "Received: " << message << endl;
	
	string command = parseCommand(message);
	if (command == "restartServer") {
		sendfifo.openwrite();
		sendfifo.send(tieString);
		cout << "Sent: " << tieString << endl;
		sendfifo.fifoclose();
		return noPlayer;
	} else {
		sendfifo.openwrite();
		sendfifo.send(tieString);
		cout << "Sent: " << tieString << endl;
		sendfifo.fifoclose();
		return tie;
	}
}

state exitGameFn(Game& game, Player player) {
	
	recfifo.openread();
	string message = recfifo.recv();
	recfifo.fifoclose();
	cout << "Received: " << message << endl;
	
	string winString = game.getBoardState() + "," + player.getPlayerName() + ",WIN";
	sendfifo.openwrite();
	sendfifo.send(winString);
	sendfifo.fifoclose();
	cout << "Sent: " << winString << endl;
		
	string command = parseCommand(message);
	if (command == "restartServer") {
		return noPlayer;
	} else {
		return exitGame;
	}
}
