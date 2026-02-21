#include "raylib.h"
#include "Drawing/Canvas.h"
#include <cstdlib>
#include <string>
#include <fstream>
#include <iostream>
#include <exception>

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

    try
    {
        // Initialize Canvas
        Canvas canvas(screenWidth, screenHeight);

        canvas.SetClearColor(DARKGRAY);

        while (!WindowShouldClose())
        {
            canvas.Clear();

            // Rasterize your sceen
            canvas.PutPixel((screenWidth) / 2, (screenHeight) / 2, RED); // Draw a test pixel

            canvas.Update();

            BeginDrawing();

            // ClearBackground(RAYWHITE);

            //------------------------------------------------------------------
            // Draw the Texture2D (which resides in GPU memory and was updated
            // with your pixel buffer on line 83) onto the application window
            // at coordinates (0, 0) Your PutPixel calls only change data in a
            // std::vector (RAM). canvas.Update() uploads that RAM to the GPU.
            // This line finally makes that GPU texture visible on the screen.
            //------------------------------------------------------------------
            canvas.Blit(0, 0);

            DrawText("SoftRenderer is running.", 10, 10, 20, WHITE);

            // const char *fpsText = TextFormat("FPS: %i", GetFPS());
            // int fontSize = 10;
            // int textWidth = MeasureText(fpsText, fontSize);
            // DrawText(fpsText, GetScreenWidth() - textWidth - 10, GetScreenHeight() - fontSize - 10, fontSize, ORANGE);

            DrawFPS(GetScreenWidth() - 80, GetScreenHeight() - 20);

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