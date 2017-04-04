#include <iostream>
#include <string>
#include <vector>
#include "game.h"
#include "player.h"

Game::Game() {		 
    vector <char> tttBoard;
    vector <Player> players;
    coord = 0;
    currentPlayer = 0;
    numberOfMoves = 0;
}

void Game::addPlayer(Player p) {
	players.push_back(p);
}

void Game::clearPlayers() {
	players.clear();
}

int Game::stringToInt(string message) {
	
	int convertedInt;

	if (message == "0") {
		convertedInt = 0;
	} else if (message == "1") {
		convertedInt = 1;	
	} else if (message == "2") {
		convertedInt = 2;	
	} else if (message == "3") {
		convertedInt = 3;	
	} else if (message == "4") {
		convertedInt = 4;	
	} else if (message == "5") {
		convertedInt = 5;	
	} else if (message == "6") {
		convertedInt = 6;	
	} else if (message == "7") {
		convertedInt = 7;	
	} else if (message == "8") {
		convertedInt = 8;	
	} else {
		cout << "Error--int conversion failed." << endl;
		return -1;
	}
	return convertedInt;
}

void Game::clearBoard() {
	for (int i = 0; i < 9; i++) {
		tttBoard[i] = 'Z';
	}
	numberOfMoves = 0;
}

void Game::setNumberOfMoves(int i) {
	numberOfMoves = i;
}

void Game::initializeBoard() {
	for (int i = 0; i < 9; i++) {
		tttBoard.push_back('Z');
	}
}

bool Game::makeMove(string message) {

	coord = stringToInt(message);
	
	if (currentPlayer == 0) {
		if (tttBoard[coord] == 'X' || tttBoard[coord] == 'O') {
			cout << "Error, that square is already occupied." << endl;
		} else {
			tttBoard[coord] = 'X';
			nextTurn();
			return true;
		}
	} else if (currentPlayer == 1) {
		if (tttBoard[coord] == 'X' || tttBoard[coord] == 'O') {
			cout << "Error, that square is already occupied." << endl;
		} else {
			tttBoard[coord] = 'O';
			nextTurn();
			return true;
		}
	} else {
		cout << "Error--currentPlayer: " << currentPlayer << "is not a valid value." << endl;
		return false;
	}
}

void Game::nextTurn() {
	if (currentPlayer == 0) {
		currentPlayer = 1;
	} else if (currentPlayer == 1) {
		currentPlayer = 0;
	} else {
		currentPlayer = 0;
	}
	numberOfMoves++;
	//cout << "The value of currentPlayer is: " << currentPlayer << endl;
}

Player Game::getCurrentPlayer() {
	return players[currentPlayer];
}

int Game::getCurrentPlayerValue() {
	return currentPlayer;
}

bool Game::checkWin() {

	/* Okay all this is doing is checking whether or not any three spots on the tic tac
	   toe board equal each other, and that they aren't equal to a ' ' char */

	bool temp = false;

	if ((tttBoard[0] == tttBoard[4]) && (tttBoard[0] == tttBoard[8]) && (tttBoard[0] != 'Z')) {
		temp = true;
		//cout << "wCondition 1" << endl;
	} else if ((tttBoard[6] == tttBoard[4]) && (tttBoard[6] == tttBoard[2]) && (tttBoard[6] != 'Z')) {
		temp = true;
		//cout << "wCondition 2" << endl;		
	} else if ((tttBoard[0] == tttBoard[3]) && (tttBoard[0] == tttBoard[6]) && (tttBoard[0] != 'Z')) {
		temp = true;
		//cout << "wCondition 3" << endl;
	} else if ((tttBoard[1] == tttBoard[4]) && (tttBoard[1] == tttBoard[7]) && (tttBoard[1] != 'Z')) {
		temp = true;
		//cout << "wCondition 4" << endl;
	} else if ((tttBoard[2] == tttBoard[5]) && (tttBoard[2] == tttBoard[8]) && (tttBoard[2] != 'Z')) {
		temp = true;
		//cout << "wCondition 5" << endl;
	} else if ((tttBoard[0] == tttBoard[1]) && (tttBoard[0] == tttBoard[2]) && (tttBoard[0] != 'Z')) {
		temp = true;
		//cout << "wCondition 6" << endl;
	} else if ((tttBoard[3] == tttBoard[4]) && (tttBoard[3] == tttBoard[5]) && (tttBoard[3] != 'Z')) {
		temp = true;
		//cout << "wCondition 7" << endl;
	} else if ((tttBoard[6] == tttBoard[7]) && (tttBoard[6] == tttBoard[8]) && (tttBoard[6] != 'Z')) {
		temp = true;
		//cout << "wCondition 8" << endl;
	} else {
		return false;
	}
	
	if (temp == true && currentPlayer == 0) {
		return true;
	}
	if (temp == true && currentPlayer == 1) {
		return true;
	}
}

bool Game::checkTie() {
	if (numberOfMoves > 8) {
		return true;
	} else {
		return false;
	}
}

void Game::displayBoard() {
	cout << "Board below:" << endl;
	for (int i = 0; i < 9; i++) {
		cout << tttBoard[i] << "|";
		
		//if this is the third run of the code, cout an endline
		if (i == 2 || i == 5 || i == 8) {
			cout << endl;
		}
	}
}

string Game::getBoardState() {
	string boardState;
	
	for (int i = 0; i < 9; i++) {
		if (i == 8) {
			boardState = boardState + tttBoard[i];
			break;			
		}
		boardState = boardState + tttBoard[i] + ",";
	}	
	
	return boardState;
}

int Game::getNumberOfMoves() {
	return numberOfMoves;
}