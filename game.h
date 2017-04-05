#include "player.h"

using namespace std;

class Game {
public:    
    Game();
    
    int getNumberOfMoves();
        
    int getCurrentPlayerValue();
    
    string getBoardState();
    
	int stringToInt(string message);
    
    void initializeBoard();
    
    void clearBoard();
    
    void setNumberOfMoves(int i);

    bool makeMove(string message);

    bool checkWin();
    
    bool checkTie();

    void nextTurn();
    
    void displayBoard();
            
private:        
    vector <char> tttBoard;
        
    int currentPlayer;
    
    int coord;
        
    int numberOfMoves;
};
