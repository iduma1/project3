#ifndef PLAYER_H
#define PLAYER_H

using namespace std;

class Player {

public:   
    Player();
        
    void setPlayerName(string message);    
    string getPlayerName();
    
    void setSign(char symbol);
    char getSign();
    
private:        
   char sign;
   string playerName;
    
};

#endif /* PLAYER_H */