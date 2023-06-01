#pragma once
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>

#include "../sky_structs.h"
#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#include "imgui/imgui.cpp"
#include "imgui/imgui_tables.cpp"
#include "imgui/imgui_widgets.cpp"
#include "imgui/imgui_draw.cpp"
#include "imgui/imgui_demo.cpp"
#include "imgui/imgui_impl_opengl3.cpp"
#include "imgui/imgui_impl_sdl2.cpp"

// SECTION: engine code. Not for user access

// NOTE: Called before gameloop
void InitUI(WindowState *ws)
{
    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(ws->window, ws->gl_context);
    ImGui_ImplOpenGL3_Init("#version 330 core");
}

// NOTE: Called after gameloop
void DeinitUI()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

// NOTE: Called before the game's Update/Start function is called
void UI_FrameStart(Vector2Int screen_size)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(screen_size.x, screen_size.y));
    ImGui::Begin("Overlay", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
}

// NOTE: Called after the game's Update/Start function is called
void UI_FrameRender()
{
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

// SECTION: user code. Should be called inside the update or start methods of the game.


// NOTE: Goes in order Y_X. Used for UI alignment 
enum UI_Alignment {
    TOP_LEFT,
    TOP_CENTER,
    TOP_RIGHT,
    CENTER_LEFT,
    CENTER_CENTER,
    CENTER_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_CENTER,
    BOTTOM_RIGHT
};


// NOTE: The screen_position ans scale should be passed with values between 0.0f and 1.0f.
// A scale of 1.0f, 1.0f will take up the whole screen.
// NOTE: RETURNS true if button is pressed. false otherwise

ImFont* LoadFont(const char* font_path, float font_size) {
    ImFontAtlas* font_atlas = ImGui::GetIO().Fonts;

    ImFontConfig font_config;
    font_config.FontDataOwnedByAtlas = false;

    ImFont* font = font_atlas->AddFontFromFileTTF(font_path, font_size, &font_config);

    return font;

}

bool DrawSimpleButton(const char* label, Vector2 screen_position, Vector2 scale, ImFont* font)
{
    int height = ImGui::GetWindowSize().y;
    int width = ImGui::GetWindowSize().x;
    ImVec2 button_size((int)(scale.x * width), (int)(scale.y * height));

    ImVec2 button_position((int)(screen_position.x * width), (int)(screen_position.y * height));

    ImGui::SetCursorPos(button_position);

    ImGui::PushFont(font);

    bool r = ImGui::Button(label, button_size);

    ImGui::PopFont();
    
    return r;
}

// NOTE: Currently, vertical alignment is broken. Its close enough at medium sizes.
void DrawSimpleText(const char* text, Vector2 screen_position, UI_Alignment alignment, ImFont* font = nullptr) {
    int height = SCREEN_HEIGHT;
    int width = SCREEN_WIDTH;

    if(font != nullptr) ImGui::PushFont(font);

    ImFont* defaultFont = ImGui::GetIO().Fonts->Fonts[0]; // Get the default font

    
    ImVec2 text_position((int)(screen_position.x * width), (int)(screen_position.y * height));
    ImVec2 text_size = ImGui::CalcTextSize(text);

    // Calculate the baseline offset
    float baseline_offset = defaultFont->Ascent;
    
    switch(alignment) {
        case TOP_LEFT: {
            // NOTE: Already aligned on Top left corner
        } break;
        case TOP_CENTER: {
            text_position.x -= text_size.x * 0.5f;
        } break;
        case TOP_RIGHT: {
            text_position.x -= text_size.x; 
        } break;
        
        case CENTER_LEFT: {
            text_position.y -= text_size.y * 0.5f;
        } break;
        case CENTER_CENTER: {
            text_position.x -= text_size.x * 0.5f;
            text_position.y -= text_size.y * 0.5f;
        } break;
        case CENTER_RIGHT: {
            text_position.x -= text_size.x;
            text_position.y -= text_size.y * 0.5f;
        } break;
        
        case BOTTOM_LEFT: {
            text_position.y -= text_size.y + baseline_offset;
        } break;
        case BOTTOM_CENTER: {
            text_position.x -= text_size.x * 0.5f;
            text_position.y -= text_size.y + baseline_offset;
        } break;
        case BOTTOM_RIGHT: {
            text_position.x -= text_size.x;
            text_position.y -= text_size.y + baseline_offset;
        } break;
        
        default: {
        }break;
    }

    ImGui::SetCursorPos(text_position);
    
    ImGui::Text(text);

    if (font != nullptr) ImGui::PopFont();
    return;
}
