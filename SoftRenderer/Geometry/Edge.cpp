#include "Edge.h"

Edge::Edge(/* args */)
{
}

Edge::Edge(EdgeProperty visibility, EdgeProperty permanent)
{
    this->visibility = visibility;
    this->permanent = permanent;
}

Edge::~Edge()
{
}

// An edge that has been determined(processed)
// to be visible overrides an edge that has been
// determined to be invisible.
//
// An edge that is invisible can be forced to visible.
// But an edge that has been determined to be visible can
// not be forced to be invisible.
void Edge::SetVisible(EdgeProperty visibility)
{
    if (permanent == PERMINENT)
        return; // visibility can't be changed

    this->visibility = visibility;
}
