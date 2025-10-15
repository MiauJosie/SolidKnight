#include "types.h"
// VECTORRRRRRRR!!!
// DIREÇÃO E MAGNITUDE!

// funções para vetor2
Vector2 vector2_create(float x, float y)
{
    Vector2 vector2;

    vector2.x = x;
    vector2.y = y;

    return vector2;
}

Vector2 vector2_add(Vector2 a, Vector2 b)
{
    Vector2 vector2;

    vector2.x = a.x + b.x;
    vector2.y = a.y + b.y;

    return vector2;
}

Vector2 vector2_scale(Vector2 v, float scale)
{
    Vector2 vector2;

    vector2.x = v.x * scale;
    vector2.y = v.y * scale;

    return vector2;
}

// funções para retângulo
bool rectangle_intersects(Rectangle a, Rectangle b)
{
    // Guarda a veracidade da expressão abaixo, as vezes a verdade pode ser assustadora
    bool BOOo = !(a.right <= b.left || a.left >= b.right || a.bottom <= b.top || a.top >= b.bottom);

    return BOOo;
}

int rectangle_width(Rectangle r)
{
    return r.right - r.left;
}

int rectangle_height(Rectangle r)
{
    return r.bottom - r.top;
}