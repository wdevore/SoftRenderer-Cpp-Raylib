# Requirements
A small start application using Raylib and CMake.

# Starter Shell
Create a new "HelloWorld" application--in the SimpleWindow folder-- that contains only the nessecary code to show a window using Raylib and basic fonts.

I want the project to use CMake in a moderately simple way. 

- I prefer if Raylib can be included via Cmake's FetchContent_Declare feature.
- If not then attempt to include it via CMake's find. 
- And last, if none of those options are possible then I'll download the Raylib SDK manually.
- No sub folders for any source files.

# Enhancements

## Positioning the window

I want the window to appear in my center monitor where VSCode is running. Enhance the code allow passing in a position.

## launch.json arguments
I would like to pass the window position via the *launch.json* file

## Center window on a specific monitor
I would also like to specify which monitor the window should appear on if it is centered. If centered then override manual positioning.

# Questions
## Can you explain how GetMonitorPosition works in a multi-monitor setup?
In a multi-monitor environment, the operating system creates a large "Virtual Screen" that combines all your displays.
*   **Coordinates:** The top-left corner of your **Primary Monitor** is usually `(0, 0)`.
*   **Offsets:** Other monitors are positioned relative to the primary one.
    *   If Monitor 1 (1920x1080) is to the right of Monitor 0, `GetMonitorPosition(1)` will likely return `(1920, 0)`.
    *   If a monitor is to the left of the primary, its X coordinate will be negative.
*   **Usage:** When you want to center a window on a specific monitor, you take that monitor's "virtual" position and add the calculated offset (half monitor width minus half window width).

## How can I make the window resizable and remember its position between sessions?
*   **Resizable:** You need to set a configuration flag before initializing the window.
    ```cpp
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 450, "Title");
    ```
*   **Persistence:** Raylib does not save window state automatically. You must implement this manually:
    1.  **On Close:** Before the loop ends, get the values using `GetWindowPosition()` and `GetScreenWidth()`/`GetScreenHeight()`. Save these integers to a file (e.g., `window.cfg` or a JSON file).
    2.  **On Startup:** Read the file. Pass the width/height to `InitWindow` and call `SetWindowPosition` with the saved X/Y coordinates.

## How can I use the ${command:cmake.launchTargetPath} variable in launch.json to automatically find the executable?
This variable is provided by the VS Code CMake Tools extension. It automatically resolves to the absolute path of the executable target you have selected in the CMake status bar (bottom of the window). This makes your `launch.json` portable, as it doesn't rely on hardcoded paths like `build/HelloWorld`.

To use it, replace the `program` field in your `launch.json`:
```json
"program": "${command:cmake.launchTargetPath}"
```

## Add a check to ensure the saved window position is within the current monitor bounds?