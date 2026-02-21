# SoftRenderer-Cpp-Raylib
A port of my Java software renderer to Cpp and Raylib

# Building and Running

This repository uses a root `CMakeLists.txt` to manage sub-projects. You do not need to edit `settings.json` to switch projects.

1.  **Clean Setup:**
    *   Remove `cmake.sourceDirectory` from `.vscode/settings.json`.
    *   **Delete the `build` folder** entirely to prevent cache conflicts.
    *   Open the Command Palette (`Ctrl+Shift+P`) and run **CMake: Select a Kit** (choose your compiler, e.g., GCC or Clang).
    *   Run **CMake: Delete Cache and Reconfigure** to ensure the project is indexed.

2.  **Select Targets (Command Palette):**
    *   Press `Ctrl+Shift+P`.
    *   Run **CMake: Set Build Target** -> Choose `SoftRenderer`.
    *   Run **CMake: Set Launch/Debug Target** -> Choose `SoftRenderer` (executable).
    *   *Note: You must set **both**. Build Target controls compilation; Launch/Debug Target controls execution.*

3.  **Build and Run:**
    *   Click "Build" or press `F7`.
    *   Click "Play" (Run) or press `Shift+F5`.