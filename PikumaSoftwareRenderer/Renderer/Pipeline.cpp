#include <iostream>
#include <cmath>

#include "Pipeline.h"
#include "Matrix4.h"
#include "Triangle.h"
#include "Polygon.h"
#include "Vector3f.h"
#include "Constants.h"

Pipeline::~Pipeline()
{
}

void Pipeline::Setup()
{
    canvas.initialize(width, height);
    // painting.Initialize(width, height);

    canvas.SetClearColor(DARKGRAY);

    // Initialize the scene light direction
    light.direction.set(0, 0, 1);

    // ------------ Setup an initial camera target and orbit radius -----------
    // Look explicitly at the axis's translation coordinates (0, 0, 15) with radius 15
    camera.initialize(Maths::Vector3f{0, 0, 15}, 15.0f);
    // Elevate slightly so it's not looking perfectly straight-on initially
    camera.rotate(0.0f, 25.0f * Maths::DEGTORAD);
    camera.yaw = camera.desiredYaw;     // Snap to skip intro animation
    camera.pitch = camera.desiredPitch; // Snap to skip intro animation
    camera.updatePosition();

    // ------------ Setup initial view matrix
    Maths::Vector3f up_direction{0, 1, 0};
    camera.makeLookAt(up_direction);
    std::cout << camera.vm << std::endl;

    // ------------- Setup initial projection matrix ----------------
    float aspect_y = (float)height / (float)width;
    float aspect_x = (float)width / (float)height;
    // float fov_y = 60.0f * Maths::DEGTORAD; // the same as 180/3, or 60deg = 30deg-up + 30deg-down
    float fov_y = 45.0f * Maths::DEGTORAD; // = 22.5deg-up + 22.5deg-down
    float fov_x = std::atan(std::tan(fov_y / 2) * aspect_x) * 2;
    std::cout << "fov_x: " << fov_x * Maths::RADTODEG / 2 << std::endl;

    float znear = 1.0;
    float zfar = 50.0;

    camera.makePerspective(fov_y, aspect_y, znear, zfar);
    projMatrix.set(camera.pm);

    // Initialize frustum planes with a point and a normal
    frustum.initialize(fov_x, fov_y, znear, zfar);

    std::cout << "Pipeline setup complete." << std::endl;
}

Pipeline::Pipeline(int width, int height) : width(width), height(height)
{
    painter.Initialize(width, height);
    trianglesAfterClipping.resize(Geometry::Polygon::MAX_NUM_POLY_TRIANGLES);
    trianglesToRender.resize(Geometry::Triangle::MAX_TRIANGLES);
    linesToRender.resize(Geometry::Line::MAX_LINES);
}

void Pipeline::Begin(float deltaTime)
{
    this->deltaTime = deltaTime;

    // Initialize the counter of triangles to render for the current frame
    trianglesToRenderCount = 0;
    linesToRenderCount = 0;

    canvas.Clear();

    painter.reset();
}

void Pipeline::Update()
{

    canvas.Update(); // Updates color buffer
}

void Pipeline::End()
{
    //------------------------------------------------------------------
    // Draw the Texture2D (which resides in GPU memory and was updated
    // with your pixel buffer) onto the application window
    // at coordinates (0, 0) Your PutPixel calls only change data in a
    // std::vector (RAM). canvas.Update() uploads that RAM to the GPU.
    // This line finally makes that GPU texture visible on the screen.
    //------------------------------------------------------------------
    canvas.Blit(0, 0); // Copies color buffer to screen
}

int Pipeline::addMesh(std::unique_ptr<Geometry::Mesh> mesh)
{
    meshes.push_back(std::move(*mesh));
    return meshes.size() - 1;
}

int Pipeline::addLineCollection(std::unique_ptr<Geometry::LineCollection> collection)
{
    lineCollections.push_back(std::move(*collection));
    return lineCollections.size() - 1;
}

void Pipeline::Render()
{
    camera.update(deltaTime); // Process smooth camera inertia

    painter.DrawDottedGrid(canvas, CColor::Orange);
    // painter.DrawRectangle(canvas, 50, 50, 100, 100, CColor::Magenta);

    for (auto &&lineCollection : lineCollections)
    {
        ProcessPipelineLines(lineCollection);
    }

    for (auto &&mesh : meshes)
    {
        ProcessPipelineMesh(mesh);
    }

    // Loop all triangles from the triangles_to_render array
    for (int i = 0; i < trianglesToRenderCount; i++)
    {
        Geometry::Triangle triangle = trianglesToRender[i];

        if (shouldRenderFilledTriangle())
        {
            CColor faceColor{triangle.color};

            painter.DrawFilledTriangle(canvas, triangle, faceColor);
        }

        if (shouldRenderWire())
        {
            painter.DrawTriangleWire(canvas, triangle.points[0].x, triangle.points[0].y,
                                     triangle.points[1].x, triangle.points[1].y,
                                     triangle.points[2].x, triangle.points[2].y, CColor::White);
        }

        if (shouldRenderWireVertex())
        {
            int rectangleSize = 4;
            int x = triangle.points[0].x - rectangleSize / 2;
            int y = triangle.points[0].y - rectangleSize / 2;
            painter.DrawRectangle(canvas, x, y, rectangleSize, rectangleSize, CColor::Orange);
            x = triangle.points[1].x - rectangleSize / 2;
            y = triangle.points[1].y - rectangleSize / 2;
            painter.DrawRectangle(canvas, x, y, rectangleSize, rectangleSize, CColor::Orange);
            x = triangle.points[2].x - rectangleSize / 2;
            y = triangle.points[2].y - rectangleSize / 2;
            painter.DrawRectangle(canvas, x, y, rectangleSize, rectangleSize, CColor::Orange);
        }
    }

    for (int i = 0; i < linesToRenderCount; i++)
    {
        Geometry::Line line = linesToRender[i];

        painter.DrawDDALine(canvas, line.points[0].x, line.points[0].y,
                            line.points[1].x, line.points[1].y, line.color);
    }
}

///////////////////////////////////////////////////////////////////////////////
// Process the graphics pipeline stages for all the mesh triangles
///////////////////////////////////////////////////////////////////////////////
// +-------------+
// | Model space |  <-- original mesh vertices
// +-------------+
// |   +-------------+
// `-> | World space |  <-- multiply by world matrix
//     +-------------+
//     |   +--------------+
//     `-> | Camera space |  <-- multiply by view matrix
//         +--------------+
//         |    +------------+
//         `--> |  Clipping  |  <-- clip against the six frustum planes
//              +------------+
//              |    +------------+
//              `--> | Projection |  <-- multiply by projection matrix
//                   +------------+
//                   |    +-------------+
//                   `--> | Image space |  <-- apply perspective divide
//                        +-------------+
//                        |    +--------------+
//                        `--> | Screen space |  <-- ready to render
//                             +--------------+
///////////////////////////////////////////////////////////////////////////////
void Pipeline::ProcessPipelineMesh(Geometry::Mesh &mesh)
{
    mesh.rotation.x += 0.005f;
    mesh.rotation.y += 0.005f;
    mesh.rotation.z += 0.005f;

    // Create scale, rotation, and translation matrices that will be used to multiply the mesh vertices
    scaleMatrix.setScale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
    rotationMatrixX.setRotationX(mesh.rotation.x);
    rotationMatrixY.setRotationY(mesh.rotation.y);
    rotationMatrixZ.setRotationZ(mesh.rotation.z);
    translationMatrix.setTranslation(mesh.translation.x, mesh.translation.y, mesh.translation.z);

    Maths::Vector3f up_direction{0, 1, 0};
    camera.makeLookAt(up_direction);
    int culledFaces = 0;

    // Loop all triangle faces of our mesh
    for (auto &face : mesh.faces)
    {
        // std::cout << "------- Processing face -------" << std::endl;

        Maths::Vector3f *face_vertices[3];
        face_vertices[0] = &mesh.vertices[face.a - 1];
        face_vertices[1] = &mesh.vertices[face.b - 1];
        face_vertices[2] = &mesh.vertices[face.c - 1];

        Maths::Vector4f transformed_vertices[3];

        // Loop all three vertices of this current face and apply transformations
        for (int v = 0; v < 3; v++)
        {
            Maths::Vector4f transformedVertex;
            transformedVertex.setFrom3(*face_vertices[v]);

            // Create a World Matrix combining scale, rotation, and translation matrices
            Matrix4 worldMatrix{};

            // Order matters: First scale, then rotate, then translate. [T]*[R]*[S]*v
            worldMatrix.multiply(scaleMatrix, worldMatrix);
            worldMatrix.multiply(rotationMatrixZ, worldMatrix);
            worldMatrix.multiply(rotationMatrixY, worldMatrix);
            worldMatrix.multiply(rotationMatrixX, worldMatrix);
            worldMatrix.multiply(translationMatrix, worldMatrix);

            // Multiply the world matrix by the original vector
            worldMatrix.multiply(transformedVertex);

            // Multiply the view matrix by the vector to transform the scene to camera space
            camera.vm.multiply(transformedVertex);

            // Save transformed vertex in the array of transformed vertices
            transformed_vertices[v].set(transformedVertex);
        }

        // Calculate the triangle face normal
        Maths::Vector3f faceNormal{};
        Geometry::getNormal(transformed_vertices, faceNormal);
        // std::cout << faceNormal << std::endl;

        // Backface culling test to see if the current face should be projected
        if (shouldCullBackfaces)
        {
            // Find the vector between vertex A in the triangle and the camera origin
            Maths::Vector3f camera_ray;
            Maths::Vector3f origin{0.0f, 0.0f, 0.0f};
            camera_ray.sub(origin, transformed_vertices[0]);
            // std::cout << camera_ray << std::endl;

            // Calculate how aligned the camera ray is with the face normal (using dot product)
            Maths::Vector3f ray;
            float dotNormalCamera = faceNormal.dot(camera_ray);
            // std::cout << dotNormalCamera << std::endl;

            // Backface culling, bypassing triangles that are looking away from the camera
            if (dotNormalCamera < 0)
            {
                // std::cout << "****** Culled face ****** " << std::endl;
                culledFaces++;
                continue;
            }
        }

        // Create a polygon from the original transformed triangle to be clipped
        Geometry::Polygon polygon{};
        polygon.setFromTriangle(
            transformed_vertices[0], transformed_vertices[1], transformed_vertices[2],
            face.a_uv, face.b_uv, face.c_uv);
        // std::cout << "original: \n"
        //           << polygon << std::endl;

        // Clip the polygon modify with potential new vertices
        frustum.clip(polygon);
        // std::cout << "clipped: \n"
        //           << polygon << std::endl;

        // Break the clipped polygon apart back into a list of triangles
        int numTrianglesAfterClipping = 0;

        Geometry::trianglesFromPolygon(polygon, trianglesAfterClipping, numTrianglesAfterClipping);

        // Loops all the assembled triangles after clipping
        for (int triangle_index = 0; triangle_index < numTrianglesAfterClipping; triangle_index++)
        {
            Geometry::Triangle *triangleAfterClipping = &trianglesAfterClipping[triangle_index];

            // TODO: make a std::vector in class
            Maths::Vector4f projectedPoints[3];

            float hw = (float)width / 2.0;
            float hh = (float)height / 2.0;

            // Loop all three vertices to perform projection and conversion to screen space
            for (int v = 0; v < 3; v++)
            {
                // Project the current vertex using a perspective projection matrix
                projMatrix.multiply(triangleAfterClipping->points[v], projectedPoints[v]);

                // Perform perspective divide
                if (projectedPoints[v].w != 0)
                {
                    projectedPoints[v].x /= projectedPoints[v].w;
                    projectedPoints[v].y /= projectedPoints[v].w;
                    projectedPoints[v].z /= projectedPoints[v].w;
                }

                // Flip vertically since the y values of the 3D mesh grow bottom->up and in screen space y values grow top->down
                projectedPoints[v].y *= -1;

                // Scale into the view
                projectedPoints[v].x *= hw;
                projectedPoints[v].y *= hh;

                // Translate the projected points to the middle of the screen
                projectedPoints[v].x += hw;
                projectedPoints[v].y += hh;
            }

            // Calculate the shade intensity based on how aliged is the normal with the flipped light direction ray
            float lightIntensityFactor = -faceNormal.dot(light.direction);

            // Calculate the triangle color based on the light angle
            uint32_t triangleColor = light.applyLightIntensity(face.color, lightIntensityFactor);

            // Save the projected triangle in the array of triangles to render
            if (trianglesToRenderCount < Geometry::Triangle::MAX_TRIANGLES)
            {
                trianglesToRender[trianglesToRenderCount].set(projectedPoints,
                                                              triangleAfterClipping->texcoords,
                                                              triangleColor,
                                                              mesh.texture.image);
                trianglesToRenderCount++;
            }
        }
        // std::cout << "Triangles to render: " << trianglesToRenderCount << std::endl;
        // std::cout << "------- End Processing face ------- " << std::endl;
    }

    // std::cout << "==== Finished processing pipeline for mesh ===== " << culledFaces << std::endl;
}

void Pipeline::ProcessPipelineLines(Geometry::LineCollection &lines)
{
    // TODO: add animation of properties

    camera.makeLookAt(upDirection);

    for (auto &line : lines.lines)
    {
        // Create scale, rotation, and translation matrices that will be used to multiply the mesh vertices
        scaleMatrix.setScale(line.scale.x, line.scale.y, line.scale.z);
        // rotationMatrixX.setRotationX(line.rotation.x);
        // rotationMatrixY.setRotationY(line.rotation.y);
        rotationMatrixZ.setRotationZ(line.rotation.z);
        translationMatrix.setTranslation(line.translation.x, line.translation.y, line.translation.z);

        Maths::Vector4f *line_vertices[2];
        line_vertices[0] = &line.points[0];
        line_vertices[1] = &line.points[1];

        Maths::Vector4f transformed_vertices[2];

        // int line_index = 0;

        // Loop all 2 vertices of this current line and apply transformations
        Maths::Vector4f transformedVertex;
        for (int v = 0; v < 2; v++)
        {
            transformedVertex.set(*line_vertices[v]);

            // Create a World Matrix combining scale, rotation, and translation matrices
            Matrix4 worldMatrix{};

            // Order matters: First scale, then rotate, then translate. [T]*[R]*[S]*v
            worldMatrix.multiply(scaleMatrix, worldMatrix);
            worldMatrix.multiply(rotationMatrixZ, worldMatrix);
            worldMatrix.multiply(rotationMatrixY, worldMatrix);
            worldMatrix.multiply(rotationMatrixX, worldMatrix);
            worldMatrix.multiply(translationMatrix, worldMatrix);

            // Multiply the world matrix by the original vector
            worldMatrix.multiply(transformedVertex);

            // Multiply the view matrix by the vector to transform the scene to camera space
            camera.vm.multiply(transformedVertex);

            // Save transformed vertex in the array of transformed vertices
            transformed_vertices[v].set(transformedVertex);
        }

        // Create a Line from the original transformed Line to be clipped
        Geometry::Line lineClipped{transformed_vertices[0], transformed_vertices[1], line.color};

        // Clip the line and modify with potentially new vertices
        Geometry::ClippedInfo clipped = frustum.clip(lineClipped);
        if (clipped == Geometry::ClippedInfo::OUTSIDE)
            continue;

        // Loops all the assembled lines after clipping
        // Geometry::Line *lineAfterClipping = &linesToRender[line_index++];

        // TODO: make a std::vector in class
        Maths::Vector4f projectedPoints[2];

        float hw = (float)width / 2.0;
        float hh = (float)height / 2.0;

        // Loop all two vertices to perform projection and conversion to screen space
        for (int v = 0; v < 2; v++)
        {
            // Project the current vertex using a perspective projection matrix
            projMatrix.multiply(lineClipped.points[v], projectedPoints[v]);

            // Perform perspective divide
            if (projectedPoints[v].w != 0)
            {
                projectedPoints[v].x /= projectedPoints[v].w;
                projectedPoints[v].y /= projectedPoints[v].w;
                projectedPoints[v].z /= projectedPoints[v].w;
            }

            // Flip vertically since the y values of the 3D mesh grow bottom->up and in screen space y values grow top->down
            projectedPoints[v].y *= -1;

            // Scale into the view
            projectedPoints[v].x *= hw;
            projectedPoints[v].y *= hh;

            // Translate the projected points to the middle of the screen
            projectedPoints[v].x += hw;
            projectedPoints[v].y += hh;
        }

        linesToRender[linesToRenderCount].set(projectedPoints[0], projectedPoints[1], line.color);
        linesToRenderCount++;
    }
}

// =========== Object manipulation =================
void Pipeline::rotateOnX(int index, float angle)
{
    meshes[index].rotation.x += angle;
}

void Pipeline::rotateOnY(int index, float angle)
{
    meshes[index].rotation.y += angle;
}

void Pipeline::rotateOnZ(int index, float angle)
{
    meshes[index].rotation.z += angle;
}

void Pipeline::setScale(int index, Maths::Vector3f scale)
{
    meshes[index].scale = scale;
}

void Pipeline::setRotation(int index, Maths::Vector3f rotation)
{
    meshes[index].rotation = rotation;
}

void Pipeline::setTranslation(int index, Maths::Vector3f translation)
{
    meshes[index].translation = translation;
}

// =========== Clipping =================
void Pipeline::clipPolygonAgainstPlane(Geometry::Polygon &polygon, int plane)
{
}

// =========== Camera manipulation =================
void Pipeline::MoveCameraBase(float dx, float dy, float dz)
{
    // camera.MoveCameraBase(dx, dy, dz);
}

void Pipeline::OnMouseDown(int x, int y)
{
    // camera.OnMouseDown(x, y);
}

void Pipeline::OnMouseUp()
{
    // camera.OnMouseUp();
}

void Pipeline::OnMouseMove(int x, int y, int dx, int dy)
{
    // std::cout << "Mouse moved: " << dx << ", " << dy << std::endl;

    float sensitivity = 0.5f;
    float yaw = dx * sensitivity;
    float pitch = dy * sensitivity;

    camera.rotate(yaw * deltaTime, pitch * deltaTime);
}

void Pipeline::OnMousePan(int x, int y, int dx, int dy)
{
    float sensitivity = 0.05f;
    camera.pan(dx * sensitivity, dy * sensitivity, upDirection);
}

void Pipeline::OnMouseWheel(float delta)
{
    float speed = 20.0f;

    if (delta > 0)
    {
        camera.zoom(speed * deltaTime);
    }
    else
    {
        camera.zoom(-speed * deltaTime);
    }
}
