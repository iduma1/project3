#ifndef PLAYER_H
#define PLAYER_H

using namespace std;

class Player {

public:   
    Player();
        
    void setPlayerName(string message);    
    string getPlayerName();
    
private:        
   
   string playerName;
    
};

#endif /* PLAYER_H */