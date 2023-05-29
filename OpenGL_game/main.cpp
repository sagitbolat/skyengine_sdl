#include "../Engine/SDL_sky.cpp"


#include "scene.h"


// SECTION: Initialization of stuff...
void Init(int* w, int* h, float* w_in_world_space, bool* fullscreen, fColor* clear_color) {
    //*w = 640;
    //*h = 480;
    *w_in_world_space = 15.0f;
    *clear_color = {0.8f/2, 0.83f/2, 1.0f/2, 1.0f};
}





// SCENE
#define NUM_SCENES 3

#define SCENE_MAIN_MENU 0
#define SCENE_GAME 1
#define SCENE_GAME_OVER 2

SceneManager scene_manager = {0};
// SECTION: Scene method declerations:
void MainMenuAwake();
void MainMenuStart(GameState* gs, KeyboardState* ks, double dt);
void MainMenuUpdate(GameState* gs, KeyboardState* ks, double dt);
void MainMenuClose(GameState* gs, KeyboardState* ks, double dt);
void MainMenuFree();

void GameAwake();
void GameStart(GameState* gs, KeyboardState* ks, double dt);
void GameUpdate(GameState* gs, KeyboardState* ks, double dt);
void GameClose(GameState* gs, KeyboardState* ks, double dt);
void GameFree();

void GameoverAwake();
void GameoverStart(GameState* gs, KeyboardState* ks, double dt);
void GameoverUpdate(GameState* gs, KeyboardState* ks, double dt);
void GameoverClose(GameState* gs, KeyboardState* ks, double dt);
void GameoverFree();


#include "mainmenu_scene.cpp"
#include "game_scene.cpp"
#include "gameover_scene.cpp"

void Awake(GameMemory* gm) { 

    // SECTION: SceneManager init
    scene_manager.InitManager(NUM_SCENES);

    // Init individual scenes
    scene_manager.AwakeScene(SCENE_MAIN_MENU, MainMenuAwake, MainMenuStart, MainMenuUpdate, MainMenuClose, MainMenuFree);
    scene_manager.AwakeScene(SCENE_GAME, GameAwake, GameStart, GameUpdate, GameClose, GameFree);
    scene_manager.AwakeScene(SCENE_GAME_OVER, GameoverAwake, GameoverStart, GameoverUpdate, GameoverClose, GameoverFree);

}




void Start(GameState* gs, KeyboardState* ks) {
    scene_manager.scenes[0].StartScene(gs, ks, 0.0);
}

void Update(GameState* gs, KeyboardState* ks, double dt) {
    scene_manager.SceneUpdate(gs, ks, dt); 
}


void UserFree() {
    scene_manager.FreeScene(0);
    scene_manager.FreeScene(1);
    scene_manager.FreeScene(2);
    scene_manager.FreeManager();
}

