#ifndef PLAYER_H
#define PLAYER_H

using namespace std;

class Player {

public:   
    Player();
        
    void setPlayerName(string message);    
    string getPlayerName();
    
    void setPlayer2Name(string message);    
    string getPlayer2Name();
    
private:        
   
   string playerName;
    
};

#endif /* PLAYER_H */