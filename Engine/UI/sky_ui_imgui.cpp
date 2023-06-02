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


struct ButtonStyle {
    // SECTION: Coloring
    fColor button_idle_color;
    fColor button_hover_color;
    fColor button_active_color;
    fColor outline_color;
    fColor text_color;

    // SECTION: Styling
    float button_alpha;
    Vector2 text_alignment; // NOTE: {0.5, 0.5} will center the text
    float corner_rounding;
    float border_size;
    Vector2 frame_padding; // How much padding around the button there is. Useful for imagebutton for the image to take up all the space
};

inline ImVec4 fColorToImVec4(fColor c) {
    return ImVec4(c.r, c.g, c.b, c.a);
}


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

// NOTE: Called at button function calls. Can also be called by user to default the same style on multiple buttons
// EX:
//  PushStyle(&my_style);
//  
//  DrawSimpleButton(...)       // No need to pass the my_style variable to this
//  DrawSimpleImageButton(...)  // No need to pass here either
//
//  PopStyle(&my_style);
void PushStyle(const ButtonStyle* style) {
    if (style == nullptr) return;
    ImGui::PushStyleColor(ImGuiCol_Button, fColorToImVec4(style->button_idle_color));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, fColorToImVec4(style->button_hover_color));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, fColorToImVec4(style->button_active_color));
    ImGui::PushStyleColor(ImGuiCol_Border, fColorToImVec4(style->outline_color));
    ImGui::PushStyleColor(ImGuiCol_Text, fColorToImVec4(style->text_color));

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, style->button_alpha);
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(style->text_alignment.x, style->text_alignment.y));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, style->corner_rounding);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, style->border_size);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style->frame_padding.x, style->frame_padding.y));
}

void PopStyle() {
    ImGui::PopStyleColor(5);
    ImGui::PopStyleVar(5);
}

bool DrawSimpleButton(
    const char* label, 
    Vector2 screen_position, Vector2 scale, 
    ImFont* font, 
    const ButtonStyle* style = nullptr
) {
    int height = ImGui::GetWindowSize().y;
    int width = ImGui::GetWindowSize().x;
    ImVec2 button_size((int)(scale.x * width), (int)(scale.y * height));

    ImVec2 button_position((int)(screen_position.x * width), (int)(screen_position.y * height));

    ImGui::SetCursorPos(button_position);

    ImGui::PushFont(font);

    PushStyle(style); 

    bool r = ImGui::Button(label, button_size);

    if (style != nullptr) {
        PopStyle();
    }

    ImGui::PopFont();
    
    return r;
}


// If You want the image button to preseve it's aspect ratio, 
// then pass the scale vector with one of the parameters as as negative value (eg. -1.0f).
// If scale.x is negative, then it will scale the height according to your scale.
// If scale.y is negative, then it will scale the width according to your scale.
// NOTE: The scale, just like a normal button is between 0.0 and 1.0, where 1.0f takes up the whole screen.
bool DrawSimpleImageButton(
    const char* button_id, Sprite sprite, 
    Vector2 screen_position, Vector2 scale, 
    const ButtonStyle* style = nullptr,
    fColor background_color = {0.0f, 0.0f, 0.0f, 0.0f}, 
    fColor tint_color = {1.0f, 1.0f, 1.0f, 1.0f},
    Vector2 uv0 = {0.0f, 0.0f}, 
    Vector2 uv1 = {1.0f, 1.0f}
) {
    int height = ImGui::GetWindowSize().y;
    int width = ImGui::GetWindowSize().x;

    if (scale.y < 0.0f) {
        scale.y = ((float)sprite.pixel_height/(float)sprite.pixel_width) * scale.x * ((float)width/(float)height);
    } else if (scale.x < 0.0f) {
        scale.x = (sprite.pixel_width/sprite.pixel_height) * scale.y * ((float)height/(float)width);
    }

    ImVec2 button_size((int)(scale.x * width), (int)(scale.y * height));

    ImVec2 button_position((int)(screen_position.x * width), (int)(screen_position.y * height));

    ImGui::SetCursorPos(button_position);
    
    PushStyle(style); 

    bool r = ImGui::ImageButton(
        button_id, (void*)sprite.texture_id, 
        button_size, 
        ImVec2(uv0.x, 1.0f - uv0.y), ImVec2(uv1.x, 1.0f - uv1.y), 
        fColorToImVec4(background_color),
        fColorToImVec4(tint_color)
    );

    if (style != nullptr) {
        PopStyle(); 
    }

    return r;
}

// NOTE: Currently, vertical alignment is broken. Its close enough at medium sizes.
void DrawSimpleText(
    const char* text, 
    Vector2 screen_position, 
    UI_Alignment alignment, 
    ImFont* font = nullptr,
    fColor text_color = {1.0f, 1.0f, 1.0f, 1.0f} 
) {
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

    ImGui::PushStyleColor(ImGuiCol_Text, fColorToImVec4(text_color));
    ImGui::Text(text);
    ImGui::PopStyleColor();
    if (font != nullptr) ImGui::PopFont();
    return;
}

// If You want the image to preseve it's aspect ratio, 
// then pass the scale vector with one of the parameters as as negative value (eg. -1.0f).
// If scale.x is negative, then it will scale the height according to your scale.
// If scale.y is negative, then it will scale the width according to your scale.
// NOTE: The scale, just like a normal button is between 0.0 and 1.0, where 1.0f takes up the whole screen.
void DrawSimpleImage(
    Sprite sprite, 
    Vector2 screen_position, 
    Vector2 scale, 
    fColor tint_color = {1.0f, 1.0f, 1.0f, 1.0f},
    fColor border_color = {0.0f, 0.0f, 0.0f, 0.0f},
    Vector2 uv0 = {0.0f, 0.0f}, 
    Vector2 uv1 = {1.0f, 1.0f}
) {
    int height = ImGui::GetWindowSize().y;
    int width = ImGui::GetWindowSize().x;

    if (scale.y < 0.0f) {
        scale.y = ((float)sprite.pixel_height/(float)sprite.pixel_width) * scale.x * ((float)width/(float)height);
    } else if (scale.x < 0.0f) {
        scale.x = (sprite.pixel_width/sprite.pixel_height) * scale.y * ((float)height/(float)width);
    }

    ImVec2 image_size((int)(scale.x * width), (int)(scale.y * height));

    ImVec2 image_position((int)(screen_position.x * width), (int)(screen_position.y * height));

    ImGui::SetCursorPos(image_position);

    ImGui::Image(
        (void*)sprite.texture_id,
        image_size,
        ImVec2(uv0.x, 1.0f - uv0.y),
        ImVec2(uv1.x, 1.0f - uv1.y),
        fColorToImVec4(tint_color),
        fColorToImVec4(border_color)
    );

}
