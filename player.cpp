#include <iostream>
#include <string>
#include <vector>
#include "player.h"

using namespace std;

Player::Player() {		 
    playerName = "";
    sign = 'X';
}

void Player::setPlayerName(string message) {
	playerName = message;
}

string Player::getPlayerName() {
	return playerName;
}

void Player::setSign(char symbol) {
	sign = symbol;
}

char Player::getSign() {
	return sign;
}