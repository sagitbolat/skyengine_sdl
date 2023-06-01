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

// NOTE: The screen_position ans scale should be passed with values between 0.0f and 1.0f.
// A scale of 1.0f, 1.0f will take up the whole screen.
// NOTE: RETURNS true if button is pressed. false otherwise
bool DrawSimpleButton(const char* label, Vector2 screen_position, Vector2 scale, float font_scale)
{
    int height = ImGui::GetWindowSize().y;
    int width = ImGui::GetWindowSize().x;
    ImVec2 button_size((int)(scale.x * width), (int)(scale.y * height));

    ImVec2 button_position((int)(screen_position.x * width), (int)(screen_position.y * height));

    ImGui::SetCursorPos(button_position);
    
    ImFont* defaultFont = ImGui::GetIO().Fonts->Fonts[0]; // Get the default font

    float originalFontSize = defaultFont->FontSize; // Store the original font size
    defaultFont->FontSize = font_scale;
    bool r = ImGui::Button(label, button_size);
    defaultFont->FontSize = originalFontSize;
    return r;
}
void DrawSimpleText(const char* text, Vector2 screen_position, float font_scale, bool centered) {
    int height = ImGui::GetWindowSize().y;
    int width = ImGui::GetWindowSize().x;

    ImFont* defaultFont = ImGui::GetIO().Fonts->Fonts[0]; // Get the default font

    float originalFontSize = defaultFont->FontSize; // Store the original font size
    defaultFont->FontSize = font_scale;
    
    
    ImVec2 text_position((int)(screen_position.x * width), (int)(screen_position.y * height));

    if (centered) {
        ImVec2 text_size = ImGui::CalcTextSize(text);
        text_position.x -= text_size.x * 0.5f;
        text_position.y -= text_size.y * 0.5f;
    }

    ImGui::SetCursorPos(text_position);
    
    ImGui::Text(text);
    defaultFont->FontSize = originalFontSize;
    return;
}
