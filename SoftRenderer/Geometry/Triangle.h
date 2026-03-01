#pragma once

#include <vector>
#include <ostream>

#include "Vertex3f.h"

class Triangle
{
private:
    /* data */
public:
    // A triangle consists of 3 vertices (CCW order)
    int i1{};
    int i2{};
    int i3{};

    // Edge visibility
    std::vector<int> edges{};

    Vertex3f center{};
    Vector3f normal{};

    // Temporary calculation vectors
    Vector3f tv1{};
    Vector3f tv2{};
    Vector3f tv3{};

    Triangle(/* args */);
    Triangle(int i1, int i2, int i3);
    ~Triangle();

    void Initialize();
    void SetEdgeIndex(int edge, int index);
    /// @brief
    /// @param edge is 1-based
    /// @return
    int GetEdgeIndex(int edge) { return edges[edge - 1]; }

    Vector3f &CalcNormal(const Vertex3f &p1, const Vertex3f &p2, const Vertex3f &p3);
    Vector3f &CalcNonNormalized(const Vertex3f &p1, const Vertex3f &p2, const Vertex3f &p3);
    void SetNormal(Vector3f v);
    void SetNormal(Vertex3f p1, Vertex3f p2, Vertex3f p3);

    void SetCenter(Vertex3f p1, Vertex3f p2, Vertex3f p3);

    friend std::ostream &operator<<(std::ostream &os, const Triangle &t)
    {
        return os << t.i1 << " : " << t.i2 << " : " << t.i3;
    }
};
