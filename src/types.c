#include "types.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// AABB!
bool rectangles_intersect(Rectangle a, Rectangle b)
{
    // Guarda a veracidade da expressão abaixo, as vezes a verdade pode ser assustadora
    bool BOOo = !(a.right <= b.left || a.left >= b.right || a.bottom <= b.top || a.top >= b.bottom);

    return BOOo;
}

float get_clamped(float value, float min, float max)
{
    if (value < min)
    {
        return min;
    }
    else if (value > max)
    {
        return max;
    }
    else
    {
        return value;
    }
}

float sign_here_please(float value)
{
    if (value > 0)
    {
        return 1.0f;
    }
    else if (value < 0)
    {
        return -1.0f;
    }
    else
    {
        return 0.0f;
    }
}

float move_towards(float current, float target, float max_delta)
{
    // pega o valor absoluto
    if (fabs(target - current) <= max_delta)
    {
        return target;
    }
    return current + sign_here_please(target - current) * max_delta;
}