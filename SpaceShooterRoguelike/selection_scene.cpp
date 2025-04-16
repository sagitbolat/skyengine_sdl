#include "../Engine/SDL_sky.cpp"

enum CannonType {
    MachineGun,
    LaserCannon,
    RailGun,
    PlasmaCannon
};

enum HullType {
    StandardHull
};
enum ThursterType {
    ScoonerThrusters
};

struct ShipSelection {
    CannonType cannon_selection_1;
    CannonType cannon_selection_2;
    HullType hull_selection;
    ThursterType thruster_selection;
}; 
    
void SelectionAwake() {
}

void SelectionStart(GameState* gs, KeyboardState* ks, double dt) {
   
}

void SelectionUpdate(GameState* gs, KeyboardState* ks, double dt) {

    static ShipSelection ship_selection = {0};

    // TODO: Render selection switching buttons (arrows to left and right), and and the current ship sprite

}
void SelectionClose(GameState* gs, KeyboardState* ks, double dt) {
    
}

void SelectionFree() {

}

