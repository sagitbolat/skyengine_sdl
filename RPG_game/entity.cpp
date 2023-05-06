#pragma once
#include "../Engine/skyengine.h"


struct Player {
    Vector2Int position;
    int speed;
     
    int current_health;
    int max_health;
    
    int current_stamina;
    int max_stamina;
    
    int current_mana;
    int max_mana;
      
    int base_attack;
    int base_defense;    
};

struct Enemy {
    Vector2Int position;
    int speed;

    int current_health;
    int max_health;
    
    int base_attack;
    int base_defense;    
};
