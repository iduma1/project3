#ifndef GAME_H
#define GAME_H

using namespace std;

class Game {
public:    
    Game();
        
    void setPlayer1Name(string message);    
    string getPlayer1Name();
    
    void setPlayer2Name(string message);    
    string getPlayer2Name();
    
    void clearBoard();

    bool makeMove(string message);

    bool checkWin();

    void nextTurn();
    
    void displayBoard();
    
private:        
    vector <char> tttBoard;
    
    int currentPlayer;
    
    int coord;
    
    int player1Wins;
    
    int player2Wins;
    
    string player1Name;
    
    string player2Name;
    
};

#endif /* GAME_H */