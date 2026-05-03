#pragma once

#include <vector>
#include <string>

#include "Vector3f.h"
#include "Face.h"
#include "Texture2f.h"

namespace Geometry
{
    class Mesh
    {
    private:
        /* data */
    public:
        std::vector<Maths::Vector3f> vertices; // dynamic array of vertices
        std::vector<Face> faces;               // dynamic array of faces
        Maths::Texture2f texture{};            // PNG texture
        Maths::Vector3f scale{};               // scale in x, y, and z
        Maths::Vector3f rotation{};            // rotation in x, y, and z
        Maths::Vector3f translation{};         // translation in x, y, and z

        int meshCount{0};

        Mesh(/* args */);
        ~Mesh();

        int loadMesh(const std::string &objectFile, const std::string &textureFile,
                     const Maths::Vector3f &scale, const Maths::Vector3f &translation, const Maths::Vector3f &rotation);

        int loadObjectData(const std::string &path);
        int loadTextureData(const std::string &path);
    };

} // namespace Geometry
