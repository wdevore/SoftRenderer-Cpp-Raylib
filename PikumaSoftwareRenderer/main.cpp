#include "raylib.h"

#include <cstdlib>
#include <string>
#include <fstream>
#include <iostream>
#include <exception>
#include <memory>

#include "Pipeline.h"
#include "Constants.h"
#include "KeyControl.h"
#include "TriAxisLines.h"
#include "GridLines.h"

void CustomDrawFPS(int posX, int posY)
{
    Color color = ORANGE; // Good FPS
    int fps = GetFPS();

    if ((fps < 30) && (fps >= 15))
        color = YELLOW; // Warning FPS
    else if (fps < 15)
        color = RED; // Low FPS

    DrawText(TextFormat("%2i FPS", fps), posX, posY, 20, color);
}

std::string GetAssetPath(const std::string &localPath)
{
    if (FileExists(localPath.c_str()))
        return localPath;
    if (FileExists(("../" + localPath).c_str()))
        return "../" + localPath;
    if (FileExists(("../PikumaSoftwareRenderer/" + localPath).c_str()))
        return "../PikumaSoftwareRenderer/" + localPath;

    return localPath; // Fallback to the original path
}

int main(int argc, char *argv[])
{
    int screenWidth = 800;  //  320
    int screenHeight = 450; //   240
    int savedX = -1;
    int savedY = -1;

    std::string configFile = "window.state";
    std::string directory = "./PikumaSoftwareRenderer/";
    // std::string fullPath = directory + configFile;
    std::string fullPath = "/home/iposthuman/Development/cpp/SoftRenderer-Cpp-Raylib/PikumaSoftwareRenderer/" + configFile;

    bool fileExists = FileExists((fullPath).c_str());

    if (fileExists)
    {
        std::cout << "State file '" << fullPath << "' found in PikumaSoftwareRenderer folder." << std::endl;
    }
    else
    {
        std::cout << "State file found in current directory." << std::endl;
        return -1;
    }

    std::ifstream loadFile(fullPath);
    if (loadFile.is_open())
    {
        std::cout << "Loading window state..." << std::endl;
        loadFile >> screenWidth >> screenHeight >> savedX >> savedY;
        loadFile.close();
    }

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Pipeline Renderer - Raylib");

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
        Pipeline pipeline{screenWidth, screenHeight};

        std::unique_ptr<Geometry::LineCollection> triAxisLines = std::make_unique<Geometry::TriAxisLines>();
        triAxisLines->build();
        // Pass it to the pipeline.
        pipeline.addLineCollection(std::move(triAxisLines));

        std::unique_ptr<Geometry::LineCollection> gridLines = std::make_unique<Geometry::GridLines>();
        gridLines->build();
        // Pass it to the pipeline.
        pipeline.addLineCollection(std::move(gridLines));

        std::unique_ptr<Geometry::Mesh> mesh = std::make_unique<Geometry::Mesh>();
        int status = 0;
        mesh->loadMesh(GetAssetPath("Assets/cube.obj"),
                       GetAssetPath("Assets/cube.png"),
                       Maths::Vector3f{1, 1, 1},
                       Maths::Vector3f{0, 0, 0},
                       Maths::Vector3f{0, 0, 0});
        if (status != 0)
        {
            std::cout << "Error loading mesh: " << status << std::endl;
        }

        if (status == 0)
        {
            int index = pipeline.addMesh(std::move(mesh));

            // Initialize Canvas
            pipeline.Setup();
        }
        bool mouseDown = false;

        pipeline.setRenderMethod(Pipeline::RenderMethod::FILL_TRIANGLE);

        IOControl::KeyControl keyS{KEY_S};
        IOControl::KeyControl keyF{KEY_F};
        IOControl::KeyControl keyW{KEY_W};
        IOControl::KeyControl keyX{KEY_X};
        IOControl::KeyControl keyL{KEY_L};

        IOControl::KeyControl key1{KEY_ONE};
        IOControl::KeyControl key2{KEY_TWO};
        IOControl::KeyControl key3{KEY_THREE};
        IOControl::KeyControl key4{KEY_FOUR};
        IOControl::KeyControl key5{KEY_FIVE};
        IOControl::KeyControl key6{KEY_SIX};

        while (!WindowShouldClose())
        {
            float deltaTime = GetFrameTime();

            pipeline.Begin(deltaTime);

            // ===============================================================
            // --- Input Handling ---
            // ===============================================================
            keyS.update();
            keyF.update();
            keyW.update();
            keyX.update();
            keyL.update();

            key1.update();
            key2.update();
            key3.update();
            key4.update();
            key5.update();
            key6.update();

            if (keyW.isActive())
            {
                std::cout << "W pressed" << std::endl;
            }
            // We only want to detect the first event and reset on keyup.
            if (keyS.isTapped())
            {
                pipeline.toggleSmoothControl();
                std::cout << "Smooth control Enabled: " << (pipeline.smoothControlEnabled() ? "Yes" : "No") << std::endl;
            }
            if (IsKeyDown(KEY_A))
            {
            }
            if (IsKeyDown(KEY_D))
            {
            }

            if (IsKeyDown(KEY_UP))
            {
            }
            if (IsKeyDown(KEY_DOWN))
            {
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

            if (keyX.isTapped())
            {
                pipeline.shouldCullBackfaces = !pipeline.shouldCullBackfaces;
                std::cout << "Backface culling Enabled: " << (pipeline.shouldCullBackfaces ? "Yes" : "No") << std::endl;
            }
            if (keyL.isTapped())
            {
                pipeline.shouldCalcFlatShading = !pipeline.shouldCalcFlatShading;
                std::cout << "Flat shading Enabled: " << (pipeline.shouldCalcFlatShading ? "Yes" : "No") << std::endl;
            }

            if (keyF.isTapped())
            {
                pipeline.FocusCamera();
                std::cout << "Camera focused on object" << std::endl;
            }

            if (key1.isTapped())
            {
                pipeline.setRenderMethod(Pipeline::RenderMethod::WIRE_VERTEX);
                std::cout << "Wire vertex mode" << std::endl;
            }

            if (key2.isTapped())
            {
                pipeline.setRenderMethod(Pipeline::RenderMethod::WIRE);
                std::cout << "Wire mode" << std::endl;
            }

            if (key3.isTapped())
            {
                pipeline.setRenderMethod(Pipeline::RenderMethod::FILL_TRIANGLE);
                std::cout << "Fill triangle mode" << std::endl;
            }

            if (key4.isTapped())
            {
                pipeline.setRenderMethod(Pipeline::RenderMethod::FILL_TRIANGLE_WIRE);
                std::cout << "Fill triangle wire mode" << std::endl;
            }

            if (key5.isTapped())
            {
                pipeline.setRenderMethod(Pipeline::RenderMethod::TEXTURED);
                std::cout << "Textured mode" << std::endl;
            }

            if (key6.isTapped())
            {
                pipeline.setRenderMethod(Pipeline::RenderMethod::TEXTURED_WIRE);
                std::cout << "Textured wire mode" << std::endl;
            }

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                mouseDown = true;

                pipeline.OnMouseDown(GetMouseX(), GetMouseY());
            }
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            {
                mouseDown = false;

                pipeline.OnMouseUp();
            }

            // Check if the mouse moved this frame
            Vector2 mouseDelta = GetMouseDelta();
            if (mouseDelta.x != 0.0f || mouseDelta.y != 0.0f)
            {
                if (mouseDown)
                {
                    //     std::cout << "Mouse moved: " << mouseDelta.x << ", " << mouseDelta.y << std::endl;
                    //     std::cout << "Mouse position: " << GetMouseX() << ", " << GetMouseY() << std::endl;
                    pipeline.OnMouseMove(GetMouseX(), GetMouseY(), (int)mouseDelta.x, (int)mouseDelta.y);
                }
                else if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE))
                {
                    pipeline.OnMousePan(GetMouseX(), GetMouseY(), (int)mouseDelta.x, (int)mouseDelta.y);
                }
            }

            float wheel = GetMouseWheelMove();
            if (wheel != 0)
            {
                pipeline.OnMouseWheel(wheel);
            }

            // ===============================================================
            // Rasterize your sceen
            pipeline.Render();
            // ===============================================================

            pipeline.Update();

            BeginDrawing();

            pipeline.End();

            // ===============================================================
            // Raylib
            // ===============================================================
            DrawText("Pikuma Renderer is running.", 10, 10, 20, GRAY);

            // --- Manual way of drawing FPS
            // const char *fpsText = TextFormat("FPS: %i", GetFPS());
            // int fontSize = 10;
            // int textWidth = MeasureText(fpsText, fontSize);
            // DrawText(fpsText, screenWidth - textWidth - 10, screenHeight - fontSize - 10, fontSize, ORANGE);

            CustomDrawFPS(screenWidth - 80, screenHeight - 20);

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

    std::cout << "========== Exiting program ==========" << std::endl;

    std::ofstream saveFile(configFile);
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