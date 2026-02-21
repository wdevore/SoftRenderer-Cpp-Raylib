#include "raylib.h"
#include <cstdlib>
#include <string>
#include <fstream>
#include <iostream>

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
    InitWindow(screenWidth, screenHeight, "SoftRenderer - Raylib");

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

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText("SoftRenderer is running.", 190, 200, 20, LIGHTGRAY);

        EndDrawing();
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