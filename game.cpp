#include <iostream>
#include <string>
#include <vector>
#include "game.h"

Game::Game() {
    gamestate[2][2];
    currentPlayer = 1;
    xPos = 0;
    yPos = 0;
    player1Wins = 0;
    player2Wins = 0;
}

void Game::nextTurn() {
	if (currentPlayer == 1) {
		currentPlayer = 2;
	} else if (currentPlayer == 2) {
		currentPlayer = 1;
	} else {
		currentPlayer = 1;
	}
	//cout << "The value of currentPlayer is: " << currentPlayer << endl;
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

void Game::setPos(string message) {
	
	string yMessage = message.substr(1,1); //get the first character from the string
	string xMessage = message.substr(3,3); //get the last character from the string
										//ex (0,2) -- 0 is the Y coord and 2 is the X coord
	
	//cout << "xMessage is: " << xMessage << endl;

	if (xMessage == "0") {
		xPos = 0;
	} else if (xMessage == "1") {
		xPos = 1;
	} else if (xMessage == "2") {
		xPos = 2;
	} else {
		cout << "An error has occurred--xMessage is not 0 1 or 2" << endl;
	}
	
	//cout << "yMessage is: " << yMessage << endl;

	if (yMessage == "0") {
		yPos = 0;
	} else if (yMessage == "1") {
		yPos = 1;
	} else if (yMessage == "2") {
		yPos = 2;
	} else {
		cout << "An error has occurred--yMessage is not 0 1 or 2" << endl;
	}
}

int Game::getxPos() {
	return xPos;
}

int Game::getyPos() {
	return yPos;
}

void Game::clearBoard() {
	for (int i = 0; i < 3; i++) {
		for (int k = 0; k < 3; k++) {
			gamestate[i][k] = ' ';
		}
	}
}

void Game::stabilizeBoard() {
	for (int i = 0; i < 3; i++) {
		for (int k = 0; k < 3; k++) {
			if (gamestate[i][k] != 'X' || gamestate[i][k] != 'O') {
				gamestate[i][k] = ' ';
			}
		}
	}
}

void Game::displayBoard() {
	cout << "Board below:" << endl;
	for (int i = 0; i < 3; i++) {
		for (int k = 0; k < 3; k++) {
			cout << gamestate[i][k] << "|";
		}
		cout << endl;
	}
}

void Game::makeMove() {

	/* Eventually this function is going to need to be merged with setPos, because if
	someone enters a coordinate that's already occupied we need to be able to detect that
	error easily */
	
	nextTurn();
	//cout << "The value of currentPlayer is: " << currentPlayer << endl;
	//cout << "xPos is: " << xPos << endl;
	//cout << "yPos is: " << yPos << endl;
	if (currentPlayer == 1) {
		gamestate[yPos][xPos] = 'X';
		//cout << "The value of currentPlayer is: " << currentPlayer << endl;
		//cout << "If this code ran, currentPlayer is 1." << endl;
	} else if (currentPlayer == 2) {
		gamestate[yPos][xPos] = 'O';
	}
}

bool Game::checkWin() {

	/* Okay all this is doing is checking whether or not any three spots on the tic tac
	   toe board equal each other, and that they aren't equal to a ' ' char */

	bool temp = false;

	if (gamestate[0][0] == gamestate[1][1] == gamestate[2][2] && gamestate[0][0] != ' ') {
		temp = true;
	} else if (gamestate[2][0] == gamestate[1][1] == gamestate[0][2] && gamestate[2][0] != ' ') {
		temp = true;
	} else if (gamestate[0][0] == gamestate[1][0] == gamestate[2][0] && gamestate[0][0] != ' ') {
		temp = true;
	} else if (gamestate[0][1] == gamestate[1][1] == gamestate[2][1] && gamestate[0][1] != ' ') {
		temp = true;
	} else if (gamestate[0][2] == gamestate[1][2] == gamestate[2][2] && gamestate[0][2] != ' ') {
		temp = true;
	} else if (gamestate[0][0] == gamestate[0][1] == gamestate[0][2] && gamestate[0][0] != ' ') {
		temp = true;
	} else if (gamestate[1][0] == gamestate[1][1] == gamestate[1][2] && gamestate[1][0] != ' ') {
		temp = true;
	} else if (gamestate[2][0] == gamestate[2][1] == gamestate[2][2] && gamestate[2][0] != ' ') {
		temp = true;
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
