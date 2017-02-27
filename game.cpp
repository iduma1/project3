#include <iostream>
#include <string>
#include <vector>
#include "game.h"

Game::Game() {
    gamestate[2][2];
    pos = 0;
    player1Wins = 0;
    player2Wins = 0;
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
	//pos = stoi(message); //not sure whether or not this will work, but we want
								 //to convert the string to an integer value.
}

int Game::getPos() {
	return pos;
}

void Game::clearBoard() {
	for (int i = 0; i < 3; i++) {
		for (int k = 0; k < 3; k++) {
			gamestate[i][k] = '*';
		}
	}
}

void Game::makeMove() {
	//must convert pos to something that makes sense to the array...
	//then just set gamestate[pos] to X or O, by checking player1Name/player2Name
}

bool Game::checkWin() {
	//scan through gamestate, return true or false
	//also this should be able to tell who won, probably? oh...hmm that's a problem
}
