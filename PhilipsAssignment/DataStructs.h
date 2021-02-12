#pragma once
#include <iostream>
struct GeometryExtents
{
    GeometryExtents()
    {
        reset();
    }

    void reset()
    {
        aMin[0] = aMin[1] = aMin[2] = FLT_MIN;
        aMax[0] = aMax[1] = aMax[2] = FLT_MIN;
    }

    void update(float* aVertex)
    {
        if (aVertex[0] < aMin[0])
            aMin[0] = aVertex[0];
        else if (aVertex[0] > aMax[0])
            aMax[0] = aVertex[0];

        if (aVertex[1] < aMin[1])
            aMin[1] = aVertex[1];
        else if (aVertex[1] > aMax[1])
            aMax[1] = aVertex[1];

        if (aVertex[2] < aMin[2])
            aMin[2] = aVertex[2];
        else if (aVertex[2] > aMax[2])
            aMax[2] = aVertex[2];
    }
    float aMin[3];
    float aMax[3];
};