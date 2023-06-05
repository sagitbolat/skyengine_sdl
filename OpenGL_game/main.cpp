#define INCLUDE_IMGUI
#define FPS_SHOW
#include "../Engine/SDL_sky.cpp"
#include "../Engine/skymath.h"

#include "scene.h"

// SECTION: Initialization of stuff...
void Init(int *w, int *h, float *w_in_world_space, bool *fullscreen, fColor *clear_color)
{
    //*w = 640;
    //*h = 480;
    *w_in_world_space = 15.0f;
    //*clear_color = {0.8f/2, 0.83f/2, 1.0f/2, 1.0f};
    *clear_color = {0.0f, 0.0f, 0.0f, 1.0f};
}


// SECTION: Audio
AudioSource global_audio_source = {0};
AudioSource shoot_audio_source  = {0};
AudioSource hit_audio_source    = {0};
AudioSource enemy_audio_source  = {0};
// NOTE: Sound clips
#define NUM_BACKGROUND_TRACKS 3
SoundClip laser_sound           = {0};
SoundClip enemy_death_sound     = {0};
SoundClip player_death_sound    = {0};
SoundClip game_over_sound       = {0};
SoundClip* background_tracks    = {0};

// SCENE
#define NUM_SCENES 3

#define SCENE_MAIN_MENU 0
#define SCENE_GAME 1
#define SCENE_GAME_OVER 2

SceneManager scene_manager = {0};

int high_score = 0;
int last_score = 0;


// SECTION: UI Fonts

ImFont* button_font = nullptr;
ImFont* title_font = nullptr;
fColor font_color = {(float)125/255, (float)212/255, (float)221/255, 1.0f};

ButtonStyle main_button_style = {0.0f};

// NOTE: Background
Sprite bg_sprite = {0};
Transform bg_transform = {0.0f};



#include "mainmenu_scene.cpp"
#include "game_scene.cpp"
#include "gameover_scene.cpp"

void Awake(GameMemory *gm)
{

    // NOTE: Audio Init
    global_audio_source = LoadAudioSource(false, 0.5f);
    shoot_audio_source  = LoadAudioSource();
    hit_audio_source    = LoadAudioSource();
    enemy_audio_source  = LoadAudioSource();
    
    // NOTE: Sound clips
    background_tracks = (SoundClip*)malloc(sizeof(SoundClip) * NUM_BACKGROUND_TRACKS);
    background_tracks[0]    = LoadSoundClip("music_track1.ogg");
    background_tracks[1]    = LoadSoundClip("music_track2.ogg");
    background_tracks[2]    = LoadSoundClip("music_track3.ogg");
    laser_sound             = LoadSoundClip("laser_shot.ogg");
    player_death_sound      = LoadSoundClip("hit_hurt.ogg");
    game_over_sound         = LoadSoundClip("game_over.ogg");
    enemy_death_sound       = LoadSoundClip("explosion.ogg");



    // NOTE: Load background texture
    bg_sprite = LoadSprite("background3.png");
    
    bg_transform.position = {0.0f, 0.0f, -10.0f};
    bg_transform.rotation = {0.0f, 0.0f, 0.0f};
    bg_transform.scale = {main_camera.width, main_camera.height, 1.0f};
    
    
    // SECTION: SceneManager init
    scene_manager.InitManager(NUM_SCENES);

    // Init individual scenes
    scene_manager.AwakeScene(SCENE_MAIN_MENU, MainMenuAwake, MainMenuStart, MainMenuUpdate, MainMenuClose, MainMenuFree);
    scene_manager.AwakeScene(SCENE_GAME, GameAwake, GameStart, GameUpdate, GameClose, GameFree);
    scene_manager.AwakeScene(SCENE_GAME_OVER, GameoverAwake, GameoverStart, GameoverUpdate, GameoverClose, GameoverFree);

    // TODO: Open the highscore file, or create one if needed.
    FILE *file;
    const char *filename = "save_data";

    file = fopen(filename, "r+"); // Open the file for reading and writing
    if (file == NULL)
    {
        file = fopen(filename, "w+"); // Create a new file if it doesn't exist
        fprintf(file, "%d", high_score);
        fclose(file); // Close the file when you're done
    }
    else
    {
        fscanf(file, "%d", &high_score);
        fclose(file); // Close the file when you're done
    }

    // SECTION: Load Fonts
    button_font = LoadFont("pixel.ttf", 32.0f);
    title_font = LoadFont("pixel.ttf", 64.0f);

    // SECTION: Initializing button style
    main_button_style.button_idle_color = font_color;
    main_button_style.button_hover_color = font_color * 0.8f;
    main_button_style.button_active_color = font_color * 0.9f;

    main_button_style.outline_color = font_color * 0.5f;
    main_button_style.text_color = {0.0f, 0.0f, 0.0f, 1.0f};
    
    main_button_style. button_alpha = 1.0f;
    main_button_style.text_alignment = {0.5f, 0.5f}; // NOTE: Centered
    main_button_style.corner_rounding = 0.0f;
    main_button_style.border_size = 3.0f;
    main_button_style.frame_padding = {0.0f, 0.0f};

}

void Start(GameState *gs, KeyboardState *ks)
{
    // NOTE: Start background music and then start first scene
    PlaySoundFromSource(&global_audio_source, &background_tracks[0]);
    scene_manager.scenes[0].StartScene(gs, ks, 0.0);
}

void Update(GameState *gs, KeyboardState *ks, double dt)
{
    static int track_num = 1;
    if (!global_audio_source.IsPlaying()) {
        PlaySoundFromSource(&global_audio_source, &background_tracks[track_num % NUM_BACKGROUND_TRACKS]);
        ++track_num;
    }
    scene_manager.SceneUpdate(gs, ks, dt);
}

void UserFree()
{
    scene_manager.FreeScene(0);
    scene_manager.FreeScene(1);
    scene_manager.FreeScene(2);
    scene_manager.FreeManager();

    // NOTE: Sounds freeing
    FreeSoundClip(&background_tracks[0]);
    FreeSoundClip(&background_tracks[1]);
    FreeSoundClip(&background_tracks[2]);
    free(background_tracks);
    FreeSoundClip(&laser_sound);
    FreeSoundClip(&player_death_sound);
    FreeSoundClip(&game_over_sound);
    FreeSoundClip(&enemy_death_sound);
    
    FreeAudioSource(&global_audio_source);
    FreeAudioSource(&shoot_audio_source);
    FreeAudioSource(&hit_audio_source);
    FreeAudioSource(&enemy_audio_source);

    // TODO: Save the highscore into a file
    FILE *file;
    const char *filename = "save_data";
    file = fopen(filename, "w");  // Open the file for writing
    fprintf(file, "%d", high_score);
}
