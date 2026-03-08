#include "raylib.h"

#include <cstdlib>
#include <string>
#include <fstream>
#include <iostream>
#include <exception>

#include "Canvas.h"
#include "Pipeline.h"
#include "CColor.h"

#include "test_bresenham_lines.h"
#include "test_wu_blended_lines.h"

int main(int argc, char *argv[])
{
    int screenWidth = 800;
    int screenHeight = 450;
    int savedX = -1;
    int savedY = -1;

    std::string configPath = "window.state";
    if (FileExists(configPath.c_str()))
    {
        std::cout << "State file found in current directory." << std::endl;
    }
    else if (FileExists("../SoftRenderer/window.state"))
    {
        std::cout << "State file found in SoftRenderer folder." << std::endl;
        configPath = "../SoftRenderer/window.state";
    }
    else
    {
        std::cout << "State file not found. Using defaults." << std::endl;
    }

    std::cout << "Config path: " << configPath << std::endl;

    std::ifstream loadFile(configPath);
    if (loadFile.is_open())
    {
        std::cout << "Loading window state..." << std::endl;
        loadFile >> screenWidth >> screenHeight >> savedX >> savedY;
        loadFile.close();
    }

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Software Renderer - Raylib");

    if (argc >= 3)
    {
        std::string arg1 = argv[1];
        if (arg1 == "center")
        {
            int monitor = std::atoi(argv[2]);
            if (monitor >= 0 && monitor < GetMonitorCount())
            {
                Vector2 monitorPos = GetMonitorPosition(monitor);
                int x = (int)monitorPos.x + (GetMonitorWidth(monitor) - screenWidth) / 2;
                int y = (int)monitorPos.y + (GetMonitorHeight(monitor) - screenHeight) / 2;
                SetWindowPosition(x, y);
            }
        }
        else
        {
            int x = std::atoi(argv[1]);
            int y = std::atoi(argv[2]);
            SetWindowPosition(x, y);
        }
    }
    else
    {
        if (savedX != -1 && savedY != -1)
        {
            std::cout << "Restoring window position to: " << savedX << ", " << savedY << std::endl;
            SetWindowPosition(savedX, savedY);
        }
    }

    SetTargetFPS(60);

    std::unique_ptr<Database> db = std::make_unique<Database>();
    // db->AddTripod(5.0f);
    // The Z position is negative to make sure it is in front of the camera. The
    // camera is positioned along the +Z half-space.
    db->AddLine("DiagLine", 0.0f, 0.0f, -1.0f, 2.0f, 0.0f, -1.0f, PaintColoring::CColor::Red);

    // TestBresenhamLines lines{};
    // TestWublendedLines lines{};
    // lines.initialize(screenWidth, screenHeight);

    try
    {
        Pipeline pipeline{screenWidth, screenHeight};
        pipeline.Initialize(std::move(db));
        pipeline.InitComplete();

        // Initialize Canvas
        pipeline.Setup();

        while (!WindowShouldClose())
        {
            // ===============================================================
            // --- Input Handling ---
            // ===============================================================
            if (IsKeyDown(KEY_W)) // Move Camera Forward
            {
                // Moves the camera up in the camera's plane
                pipeline.MoveCameraBase(0.0f, -0.05f, 0.0f);
            }
            if (IsKeyDown(KEY_S))
            {
                // Moves the camera down in the camera's plane
                pipeline.MoveCameraBase(0.0f, 0.05f, 0.0f);
            }
            if (IsKeyDown(KEY_A))
            {
                // Moves the camera left in the camera's plane
                pipeline.MoveCameraBase(-0.05f, 0.0f, 0.0f);
            }
            if (IsKeyDown(KEY_D))
            {
                // Moves the camera right in the camera's plane
                pipeline.MoveCameraBase(0.05f, 0.0f, 0.0f);
            }

            if (IsKeyDown(KEY_UP))
            {
                // Moves the camera forward in the camera's plane
                pipeline.MoveCameraBase(0.0f, 0.0f, 0.05f);
            }
            if (IsKeyDown(KEY_DOWN))
            {
                // Moves the camera backward in the camera's plane
                pipeline.MoveCameraBase(0.0f, 0.0f, -0.05f);
            }
            if (IsKeyDown(KEY_LEFT))
            { /* Rotate Left */
            }
            if (IsKeyDown(KEY_RIGHT))
            { /* Rotate Right */
            }

            if (IsKeyPressed(KEY_SPACE))
            { /* Toggle Animation/Action */
            }

            pipeline.Begin();

            // ===============================================================
            // Rasterize your sceen
            pipeline.Render();
            // ===============================================================

            // lines.draw(canvas);

            pipeline.Update();

            BeginDrawing();

            pipeline.End();

            // ===============================================================
            // Raylib
            // ===============================================================
            DrawText("SoftRenderer is running.", 10, 10, 20, DARKGRAY);

            // --- Manual way of drawing FPS
            // const char *fpsText = TextFormat("FPS: %i", GetFPS());
            // int fontSize = 10;
            // int textWidth = MeasureText(fpsText, fontSize);
            // DrawText(fpsText, screenWidth - textWidth - 10, screenHeight - fontSize - 10, fontSize, ORANGE);

            DrawFPS(screenWidth - 80, screenHeight - 20);

            EndDrawing();
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Unknown exception occurred" << std::endl;
    }

    std::ofstream saveFile(configPath);
    if (saveFile.is_open())
    {
        std::cout << "Saving window state..." << std::endl;
        Vector2 pos = GetWindowPosition();
        saveFile << GetScreenWidth() << " " << GetScreenHeight() << " " << (int)pos.x << " " << (int)pos.y;
        saveFile.close();
    }

    std::cout << "Closing window..." << std::endl;
    CloseWindow();

    return 0;
}