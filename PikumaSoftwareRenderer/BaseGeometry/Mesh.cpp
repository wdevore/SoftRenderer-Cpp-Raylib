#include <stdio.h>
#include <string.h>

#include "Mesh.h"
#include "upng.h"

namespace Geometry
{
    Mesh::Mesh(/* args */)
    {
    }

    Mesh::~Mesh()
    {
    }

    int Mesh::loadMesh(const std::string &objectFile, const std::string &textureFile, const Maths::Vector3f &scale, const Maths::Vector3f &translation, const Maths::Vector3f &rotation)
    {
        int status = loadObjectData(objectFile);
        if (status != 0)
            return status;

        status = loadTextureData(textureFile);

        this->scale.set(scale);
        this->rotation.set(rotation);
        this->translation.set(translation);

        return status;
    }

    int Mesh::loadObjectData(const std::string &path)
    {
        FILE *file;
        file = fopen(path.c_str(), "r");
        if (file == nullptr)
        {
            fprintf(stderr, "Error: Could not open or find file %s\n", path.c_str());
            return -1;
        }

        char line[1024];
        std::vector<Maths::Texture2f> texcoords{};

        int colorIndex = 0; // ======== color Debug  ============

        while (fgets(line, 1024, file))
        {
            // Vertex information
            if (strncmp(line, "v ", 2) == 0)
            {
                Maths::Vector3f vertex{};
                sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
                vertices.push_back(vertex);
            }

            // Texture coordinate information
            if (strncmp(line, "vt ", 3) == 0)
            {
                Maths::Texture2f texcoord{};
                sscanf(line, "vt %f %f", &texcoord.u, &texcoord.v);
                texcoords.push_back(texcoord);
            }

            // Face information
            if (strncmp(line, "f ", 2) == 0)
            {
                int vertex_indices[3];
                int texture_indices[3];
                int normal_indices[3];
                sscanf(
                    line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                    &vertex_indices[0], &texture_indices[0], &normal_indices[0],
                    &vertex_indices[1], &texture_indices[1], &normal_indices[1],
                    &vertex_indices[2], &texture_indices[2], &normal_indices[2]);

                // uint32_t color;
                // switch (colorIndex)
                // {
                // case 0:
                //     color = 0xFF0000FF;
                //     break;
                // case 1:
                //     color = 0xFF00FF00;
                //     break;
                // case 2:
                //     color = 0xFFFF0000;
                //     break;
                // case 3:
                //     color = 0xFF00FFFF;
                //     break;
                // case 4:
                //     color = 0xFFFFFF00;
                //     break;
                // case 5:
                //     color = 0xFF000000;
                //     break;
                // default:
                //     color = 0xFFFFFFFF;
                //     break;
                // }
                Face face = {
                    vertex_indices[0],
                    vertex_indices[1],
                    vertex_indices[2],
                    texcoords[texture_indices[0] - 1],
                    texcoords[texture_indices[1] - 1],
                    texcoords[texture_indices[2] - 1],
                    0xFFFFFFFF};
                if (colorIndex > 5)
                    colorIndex = 0;
                else
                    colorIndex++;

                faces.push_back(face);
            }
        }
        fclose(file);

        return 0;
    }

    int Mesh::loadTextureData(const std::string &path)
    {
        upng_t *png_image = upng_new_from_file(path.c_str());
        if (png_image != nullptr)
        {
            upng_decode(png_image);
            if (upng_get_error(png_image) == UPNG_EOK)
            {
                texture.image = png_image;
            }
            else
                return -2;
        }

        return 0;
    }

} // namespace Geometry
