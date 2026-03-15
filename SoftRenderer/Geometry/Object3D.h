#pragma once

#include <vector>
#include <string>
#include <memory>

#include "Matrix4f.h"
#include "Vertex3f.h"
#include "Triangle.h"
#include "CColor.h"

class Object3D
{
public:
    enum class ObjectType
    {
        Base,
        Line,
        WireMesh,
        FlatShaded
    };

    enum class ColorType
    {
        Color,
        WuColor
    };

    Object3D();
    virtual ~Object3D() = 0; // Makes class abstract

    std::string name{};
    ColorType colorType = ColorType::Color;

    bool animate{};

    Vector3f position{};

    virtual void Initialize(PaintColoring::CColor bg, PaintColoring::CColor fg, int intensityBits) = 0;

    std::vector<Vertex3f> vertices{};
    std::vector<Vertex3f> vertexNormals{};
    std::vector<std::unique_ptr<Triangle>> triangles{};

    void reset();
    void addVertex(Vertex3f v);

    void setPosition(float x, float y, float z);
    void setOrientation(float x, float y, float z, float angle);

    // ========== Transforms ===============
    Matrix4f &GetModelToWorldMatrix();
    Matrix4f &GetNormalTransformMatrix();

    // ========== Types ===============
    ObjectType GetType() const { return type; }
    void SetType(ObjectType type) { this->type = type; }
    bool IsOfType(ObjectType type) const { return this->type == type; }

    virtual void AddTriangle(int i1, int i2, int i3);

    int GetVertexCount() { return vertices.size(); }

    virtual void SetAnimate(bool animate) = 0;

    void SetToCColor() { colorType = ColorType::Color; }
    void SetToWuColor() { colorType = ColorType::WuColor; }

protected:
    Vector3f p1{};
    Vector3f p2{};
    Vector3f p3{};
    ObjectType type = ObjectType::Base;

private:
    Matrix4f translation{};
    Matrix4f rotation{};
    AxisAngle4f aa{};

    // The combined rotation and translation
    Matrix4f transform{};
};