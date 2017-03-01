#include <iostream>
#include <string>
#include <vector>
#include "game.h"

Game::Game() {		 
    vector <char> tttBoard; 
    coord = 0;
    currentPlayer = 1;
    player1Wins = 0;
    player2Wins = 0;
    numberOfMoves = 0;
}


void Game::clearBoard() {
	for (int i = 0; i < 9; i++) {
		tttBoard.push_back(' ');
	}
}

bool Game::makeMove(string message) {

	string strCoord = message.substr(1,1);

	if (strCoord == "0") {
		coord = 0;
	} else if (strCoord == "1") {
		coord = 1;	
	} else if (strCoord == "2") {
		coord = 2;	
	} else if (strCoord == "3") {
		coord = 3;	
	} else if (strCoord == "4") {
		coord = 4;	
	} else if (strCoord == "5") {
		coord = 5;	
	} else if (strCoord == "6") {
		coord = 6;	
	} else if (strCoord == "7") {
		coord = 7;	
	} else if (strCoord == "8") {
		coord = 8;	
	} else {
		cout << "Error--message: " << message << " is not a valid value." << endl;
		return false;
	}
	if (currentPlayer == 1) {
		if (tttBoard[coord] == 'X' || tttBoard[coord] == 'O') {
			cout << "Error, that square is already occupied." << endl;
		} else {
			tttBoard[coord] = 'X';
			nextTurn();
			return true;
		}
	} else if (currentPlayer == 2) {
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

bool Game::checkWin() {

	/* Okay all this is doing is checking whether or not any three spots on the tic tac
	   toe board equal each other, and that they aren't equal to a ' ' char */

	bool temp = false;

	if ((tttBoard[0] == tttBoard[4]) && (tttBoard[0] == tttBoard[8]) && (tttBoard[0] != ' ')) {
		temp = true;
		//cout << "wCondition 1" << endl;
	} else if ((tttBoard[6] == tttBoard[4]) && (tttBoard[6] == tttBoard[2]) && (tttBoard[6] != ' ')) {
		temp = true;
		//cout << "wCondition 2" << endl;		
	} else if ((tttBoard[0] == tttBoard[3]) && (tttBoard[0] == tttBoard[6]) && (tttBoard[0] != ' ')) {
		temp = true;
		//cout << "wCondition 3" << endl;
	} else if ((tttBoard[1] == tttBoard[4]) && (tttBoard[1] == tttBoard[7]) && (tttBoard[1] != ' ')) {
		temp = true;
		//cout << "wCondition 4" << endl;
	} else if ((tttBoard[2] == tttBoard[5]) && (tttBoard[2] == tttBoard[8]) && (tttBoard[2] != ' ')) {
		temp = true;
		//cout << "wCondition 5" << endl;
	} else if ((tttBoard[0] == tttBoard[1]) && (tttBoard[0] == tttBoard[2]) && (tttBoard[0] != ' ')) {
		temp = true;
		//cout << "wCondition 6" << endl;
	} else if ((tttBoard[3] == tttBoard[4]) && (tttBoard[3] == tttBoard[5]) && (tttBoard[3] != ' ')) {
		temp = true;
		//cout << "wCondition 7" << endl;
	} else if ((tttBoard[6] == tttBoard[7]) && (tttBoard[6] == tttBoard[8]) && (tttBoard[6] != ' ')) {
		temp = true;
		//cout << "wCondition 8" << endl;
	} else {
		return false;
	}
	
	if (temp == true && currentPlayer == 1) {
		player1Wins++;
		return true;
	} else if (temp == true && currentPlayer == 2) {
		player2Wins++;
		return true;
	}
}

void Game::nextTurn() {
	if (currentPlayer == 1) {
		currentPlayer = 2;
	} else if (currentPlayer == 2) {
		currentPlayer = 1;
	} else {
		currentPlayer = 1;
	}
	numberOfMoves++;
	//cout << "The value of currentPlayer is: " << currentPlayer << endl;
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

int Game::getNumberOfMoves() {
	return numberOfMoves;
}

void Game::setPlayer1Name(string message) {
    player1Name = message;
    return;
}

string Game::getPlayer1Name() {
	return player1Name;
}

void Game::setPlayer2Name(string message) {
    player2Name = message;
    return;
}

string Game::getPlayer2Name() {
	return player2Name;
}