#ifndef GAME_H
#define GAME_H

using namespace std;

class Game {
public:    
    Game();
    
    void setPos(string message);  
      
    int getxPos();
    int getyPos();
    
    void setPlayer1Name(string message);    
    string getPlayer1Name();
    
    void setPlayer2Name(string message);    
    string getPlayer2Name();
    
    void nextTurn();
    
    void makeMove();
    
    bool checkWin();
    
    void clearBoard();
    void stabilizeBoard();
    
    void displayBoard();
    
private:    
    char gamestate[2][2];
    
    int currentPlayer;
    
    int xPos;
    
    int yPos;
    
    int player1Wins;
    
    int player2Wins;
    
    string player1Name;
    
    string player2Name;
    
};

#endif /* GAME_H */