#pragma once

#include <vector>
#include <memory>

#include "Object3D.h"
#include "LightBase.h"
#include "GlobalIllumination.h"

enum Shading
{
    GOURAUD,
    FLAT,
    PHONG
};

class Database
{
private:
    // Totals
    int vertexCount{};
    int triCount{};

    std::vector<std::unique_ptr<Object3D>> objects{};
    GlobalIllumination illumination{};

    std::vector<std::unique_ptr<LightBase>> lights{};

public:
    Database(/* args */);
    ~Database();

    int GetVertexCount() { return vertexCount; }
    int GetTriCount() { return triCount; }
    int GetLightCount() { return lights.size(); }
    int GetObjectCount() { return objects.size(); }

    LightBase *GetLight(int index);
    void AddObject(std::unique_ptr<Object3D> o);
};
