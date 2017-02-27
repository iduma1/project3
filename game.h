#ifndef GAME_H
#define GAME_H

using namespace std;

class Game {
public:    
    Game();
    
    void setPos(string message);    
    int getPos();
    
    void setPlayer1Name(string message);    
    string getPlayer1Name();
    
    void setPlayer2Name(string message);    
    string getPlayer2Name();
    
    void makeMove();
    bool checkWin();
    void clearBoard();
    
    //increment "wins" and tell the player what happened
    void win();

    //increment "losses" and tell the player what happened    
    void lose();
    
private:    
    char gamestate[2][2];
    
    int pos;
    
    int player1Wins;
    
    int player2Wins;
    
    string player1Name;
    
    string player2Name;
    
};

#endif /* GAME_H */