/*******************************************************************************************
*
*   LayoutName v1.0.0 - Tool Description
*
*   LICENSE: Propietary License
*
*   Copyright (c) 2022 raylib technologies. All Rights Reserved.
*
*   Unauthorized copying of this file, via any medium is strictly prohibited
*   This project is proprietary and confidential unless the owner allows
*   usage in any other form by expresely written permission.
*
**********************************************************************************************/

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

//----------------------------------------------------------------------------------
// Controls Functions Declaration
//----------------------------------------------------------------------------------


//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main()
{
    // Initialization
    //---------------------------------------------------------------------------------------
    int screenWidth = 800;
    int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "layout_name");

    // layout_name: controls initialization
    //----------------------------------------------------------------------------------
    bool TextBox000EditMode = false;
    char TextBox000Text[128] = "Digite o Id ou Nome do produto:";
    //----------------------------------------------------------------------------------

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Implement required update logic
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR))); 

            // raygui: controls drawing
            //----------------------------------------------------------------------------------
            if (GuiTextBox((Rectangle){ 0, 40, 648, 24 }, TextBox000Text, 128, TextBox000EditMode)) TextBox000EditMode = !TextBox000EditMode;
            GuiGroupBox((Rectangle){ 0, 24, 648, 336 }, "Detalhes");
            GuiLabel((Rectangle){ 48, 64, 120, 24 }, "Id");
            GuiLabel((Rectangle){ 152, 64, 80, 24 }, "Nome");
            GuiLabel((Rectangle){ 288, 64, 80, 24 }, "Categoria");
            GuiLabel((Rectangle){ 424, 64, 64, 24 }, "        Valor");
            GuiLabel((Rectangle){ 544, 64, 80, 24 }, "Quantidade");
            GuiLabel((Rectangle){ 24, 96, 72, 40 }, "001");
            GuiLabel((Rectangle){ 128, 104, 80, 24 }, "Krai");
            GuiLabel((Rectangle){ 280, 104, 120, 24 }, "utensilios");
            GuiLabel((Rectangle){ 424, 104, 80, 24 }, "399,90");
            GuiLabel((Rectangle){ 552, 104, 88, 24 }, "100");
            //----------------------------------------------------------------------------------

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//------------------------------------------------------------------------------------
// Controls Functions Definitions (local)
//------------------------------------------------------------------------------------

