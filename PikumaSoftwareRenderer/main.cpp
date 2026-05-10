#include "raylib.h"

#include <cstdlib>
#include <string>
#include <fstream>
#include <iostream>
#include <exception>
#include <memory>

#include "Pipeline.h"
#include "Constants.h"

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

        std::unique_ptr<Geometry::LineCollection> lineCollection = std::make_unique<Geometry::LineCollection>();

        Geometry::Line lineXAxis{Maths::Vector4f{-1.0f, 0.0f, 0.0f}, Maths::Vector4f{1.0f, 0.0f, 0.0f}, CColor::Red};
        lineXAxis.translation.set(5, 0.0, 15);
        lineXAxis.scale.set(1, 1, 1);
        // +Angle = CCW rotation
        // lineXAxis.rotation.set(0, 0, 45.0 * Maths::DEGTORAD);
        lineCollection->addLine(lineXAxis);

        Geometry::Line lineYAxis{Maths::Vector4f{0.0f, -1.0f, 0.0f}, Maths::Vector4f{0.0f, 1.0f, 0.0f}, CColor::Green};
        lineYAxis.translation.set(5, 0.0, 15);
        lineYAxis.scale.set(1, 1, 1);
        lineCollection->addLine(lineYAxis);

        Geometry::Line lineZAxis{Maths::Vector4f{0.0f, 0.0f, -1.0f}, Maths::Vector4f{0.0f, 0.0f, 1.0f}, CColor::Orange};
        lineZAxis.translation.set(5, 0.0, 15);
        lineZAxis.scale.set(1, 1, 1);
        lineCollection->addLine(lineZAxis);

        // Finally pass it to the pipeline.
        pipeline.addLineCollection(std::move(lineCollection));

        std::unique_ptr<Geometry::Mesh> mesh = std::make_unique<Geometry::Mesh>();
        int status = 0;
        mesh->loadMesh(GetAssetPath("Assets/cube.obj"),
                       GetAssetPath("Assets/cube.png"),
                       Maths::Vector3f{1, 1, 1},
                       Maths::Vector3f{0, 0.0, 15},
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

        while (!WindowShouldClose())
        {
            float deltaTime = GetFrameTime();

            pipeline.Begin(deltaTime);

            // ===============================================================
            // --- Input Handling ---
            // ===============================================================
            if (IsKeyDown(KEY_W))
            {
                // Moves the camera up in the camera's plane
            }
            if (IsKeyDown(KEY_S))
            {
                // Moves the camera down in the camera's plane
            }
            if (IsKeyDown(KEY_A))
            {
                // Moves the camera left in the camera's plane
            }
            if (IsKeyDown(KEY_D))
            {
                // Moves the camera right in the camera's plane
            }

            if (IsKeyDown(KEY_UP))
            {
                // Moves the camera forward in the camera's plane
            }
            if (IsKeyDown(KEY_DOWN))
            {
                // Moves the camera backward in the camera's plane
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

            if (IsKeyPressed(KEY_F))
            {
                pipeline.FocusCamera();
                std::cout << "Camera focused on object" << std::endl;
            }

            if (IsKeyPressed(KEY_ONE))
            {
                pipeline.setRenderMethod(Pipeline::RenderMethod::WIRE_VERTEX);
                std::cout << "Wire vertex mode" << std::endl;
            }

            if (IsKeyPressed(KEY_TWO))
            {
                pipeline.setRenderMethod(Pipeline::RenderMethod::WIRE);
                std::cout << "Wire mode" << std::endl;
            }

            if (IsKeyPressed(KEY_THREE))
            {
                pipeline.setRenderMethod(Pipeline::RenderMethod::FILL_TRIANGLE);
                std::cout << "Fill triangle mode" << std::endl;
            }

            if (IsKeyPressed(KEY_FOUR))
            {
                pipeline.setRenderMethod(Pipeline::RenderMethod::FILL_TRIANGLE_WIRE);
                std::cout << "Fill triangle wire mode" << std::endl;
            }

            if (IsKeyPressed(KEY_FIVE))
            {
                pipeline.setRenderMethod(Pipeline::RenderMethod::TEXTURED);
                std::cout << "Textured mode" << std::endl;
            }

            if (IsKeyPressed(KEY_SIX))
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