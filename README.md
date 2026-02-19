# SoftRenderer-Cpp-Raylib
A port of my Java software renderer to Cpp and Raylib

# Solution to changing projects

1) You need to update the *settings.json* file and the trailing file path to match which project folder you want to switch to. For example, to switch from "SimpleWindow" to "SoftRenderer" you would need to change:

```json
{
    "cmake.sourceDirectory": "/home/iposthuman/Development/cpp/SoftRenderer-Cpp-Raylib/SimpleWindow"
}
```
to
```json
{
    "cmake.sourceDirectory": "/home/iposthuman/Development/cpp/SoftRenderer-Cpp-Raylib/SoftRenderer"
}
```
2) Go back to the CMake extension.
3) Hover over the the *Project Status* bar and click **Delete cache and Reconfigure** on the right of the bar where the icons appear.
4) Now hover over the *Edit* icon under the *Launch* section. When you click it you will see the **Debug** and **Launch** entries will switch to the new project. 
5) Finally hover over the *Edit* icon under the *Build* section. A dropdown will allow you to select new project.
6) Now you can compile and run