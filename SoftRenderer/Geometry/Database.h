#pragma once

#include <vector>
#include <memory>
#include <string>

#include "Object3D.h"
#include "LightBase.h"
#include "GlobalIllumination.h"
#include "CColor.h"

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

    std::vector<std::unique_ptr<LightBase>> lights{};

public:
    Database(/* args */);
    ~Database();

    GlobalIllumination globalIllumination{};

    int GetVertexCount() { return vertexCount; }
    int GetTriCount() { return triCount; }
    int GetLightCount() { return lights.size(); }
    int GetObjectCount() { return objects.size(); }
    std::vector<std::unique_ptr<Object3D>> &GetObjects() { return objects; }
    Object3D *GetObject(std::string name);

    LightBase *GetLight(int index);
    void AddObject(std::unique_ptr<Object3D> o);

    // ========= Objects ===========
    void AddLine(std::string name,
                 float px, float py, float pz,
                 float qx, float qy, float qz,
                 const PaintColoring::CColor &color);

    void AddTripodAxies(float scale);
    void AddTetrahedron(float px, float py, float pz,
                        float height, float baseScale, bool animate,
                        const PaintColoring::CColor &color);

    void AddPlane(float width, float height,
                  float px, float py, float pz,
                  int r, int g, int b);

    void AddFlatPlane(float width, float height,
                      float px, float py, float pz,
                      bool animate,
                      const PaintColoring::CColor &color);

    void AddFlatTriangle(float width, float height,
                         float px, float py, float pz,
                         bool animate,
                         const PaintColoring::CColor &color);

    void AddLineSphere(std::string name, float divisions, float radius,
                       float px, float py, float pz,
                       PaintColoring::CColor &color);

    void AddPointLight(std::string name,
                       float px, float py, float pz,
                       float intensity,
                       PaintColoring::CColor &color);
};
