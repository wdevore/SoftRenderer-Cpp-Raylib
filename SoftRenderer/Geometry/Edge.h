#pragma once

enum EdgeProperty
{
    PERMINENT,
    NONPERMINENT,
    VISIBLE,
    INVISIBLE,
    NOFACE,
    FRONTFACE,
    BACKFACE
};

class Edge
{
private:
    EdgeProperty permanent = PERMINENT;

public:
    EdgeProperty visibility = INVISIBLE;
    EdgeProperty face = NOFACE;

    Edge(/* args */);
    Edge(EdgeProperty visibility, EdgeProperty permanent = PERMINENT);
    ~Edge();

    void SetFaceType(EdgeProperty face) { this->face = face; };
    bool CanBeVisible() { return visibility == VISIBLE ? true : false; }
    void SetVisible(EdgeProperty visibility);
};
