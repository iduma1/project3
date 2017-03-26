#include <iostream>
#include <string>
#include <vector>
#include "player.h"

using namespace std;

Player::Player() {		 
    playerName = "";
}

void Player::setPlayerName(string message) {
	playerName = message;
}