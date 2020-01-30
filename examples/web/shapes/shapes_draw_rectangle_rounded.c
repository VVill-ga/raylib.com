/*******************************************************************************************
*
*   raylib [shapes] example - draw rectangle rounded (with gui options)
*
*   This example has been created using raylib 2.6 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Example contributed by Vlad Adrian (@demizdor) and reviewed by Ramon Santamaria (@raysan5)
*
*   Copyright (c) 2018-2020 Vlad Adrian (@demizdor) and Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include <raylib.h>

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#include "raygui.h"                 // Required for GUI controls

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
const int screenWidth = 800;
const int screenHeight = 450;

// NOTE: Textures MUST be loaded after Window initialization (OpenGL context is required)

float roundness = 0.2f;
int width = 200;
int height = 100;
int segments = 0;
int lineThick = 1;

bool drawRect = false;
bool drawRoundedRect = true;
bool drawRoundedLines = false;

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
void UpdateDrawFrame(void);     // Update and Draw one frame

//----------------------------------------------------------------------------------
// Program Main Entry Point
//----------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "raylib [shapes] example - draw rectangle rounded");

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        UpdateDrawFrame();
    }
#endif

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
void UpdateDrawFrame(void)
{
    // Update
    //----------------------------------------------------------------------------------
    Rectangle rec = { (GetScreenWidth() - width - 250)/2, (GetScreenHeight() - height)/2, width, height };
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawLine(560, 0, 560, GetScreenHeight(), Fade(LIGHTGRAY, 0.6f));
        DrawRectangle(560, 0, GetScreenWidth() - 500, GetScreenHeight(), Fade(LIGHTGRAY, 0.3f));

        if (drawRect) DrawRectangleRec(rec, Fade(GOLD, 0.6));
        if (drawRoundedRect) DrawRectangleRounded(rec, roundness, segments, Fade(MAROON, 0.2));
        if (drawRoundedLines) DrawRectangleRoundedLines(rec,roundness, segments, lineThick, Fade(MAROON, 0.4));

        // Draw GUI controls
        //------------------------------------------------------------------------------
        width = GuiSliderBar((Rectangle){ 640, 40, 105, 20 }, "Width", NULL, width, 0, GetScreenWidth() - 300);
        height = GuiSliderBar((Rectangle){ 640, 70, 105, 20 }, "Height", NULL, height, 0, GetScreenHeight() - 50);
        roundness = GuiSliderBar((Rectangle){ 640, 140, 105, 20 }, "Roundness", NULL, roundness, 0.0f, 1.0f);
        lineThick = GuiSliderBar((Rectangle){ 640, 170, 105, 20 }, "Thickness", NULL, lineThick, 0, 20);
        segments = GuiSliderBar((Rectangle){ 640, 240, 105, 20}, "Segments", NULL, segments, 0, 60);

        drawRoundedRect = GuiCheckBox((Rectangle){ 640, 320, 20, 20 }, "DrawRoundedRect", drawRoundedRect);
        drawRoundedLines = GuiCheckBox((Rectangle){ 640, 350, 20, 20 }, "DrawRoundedLines", drawRoundedLines);
        drawRect = GuiCheckBox((Rectangle){ 640, 380, 20, 20}, "DrawRect", drawRect);
        //------------------------------------------------------------------------------

        DrawText(FormatText("MODE: %s", (segments >= 4)? "MANUAL" : "AUTO"), 640, 280, 10, (segments >= 4)? MAROON : DARKGRAY);

        DrawFPS(10, 10);

    EndDrawing();
    //----------------------------------------------------------------------------------
}
