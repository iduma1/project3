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
    
    int getNumberOfMoves();
    
    int getCurrentPlayer();
    
    string getBoardState();
    
    string intToString(int i);
	int stringToInt(string message);
    
    void initializeBoard();
    
    void clearBoard();

    bool makeMove(string message);

    bool checkWin();
    
    bool checkTie();

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
    
    int numberOfMoves;
    
};

#endif /* GAME_H */