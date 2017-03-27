#include "player.h"

using namespace std;

class Game {
public:    
    Game();
        
    int getNumberOfMoves();
    
    Player getCurrentPlayer();
    
    int getCurrentPlayerValue();
    
    string getBoardState();
    
	int stringToInt(string message);
    
    void initializeBoard();
    
    void clearBoard();

    bool makeMove(string message);

    bool checkWin();
    
    bool checkTie();

    void nextTurn();
    
    void displayBoard();
    
    void addPlayer(Player p);
    
    void clearPlayers();
    
private:        
    vector <char> tttBoard;
    
    vector <Player> players;
    
    int currentPlayer;
    
    int coord;
        
    int numberOfMoves;
};