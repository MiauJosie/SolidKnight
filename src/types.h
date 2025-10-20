#pragma once
#include <stdbool.h>

typedef struct
{
    float x;
    float y;
} Vector2;

typedef struct
{
    int left;
    int top;
    int right;
    int bottom;
} Rectangle;

// funções para vetor2
Vector2 vector2_create(float x, float y);
Vector2 vector2_add(Vector2 a, Vector2 b);
Vector2 vector2_scale(Vector2 v, float scale);

// funções para retângulo
bool rectangles_intersect(Rectangle a, Rectangle b);
int rectangle_width(Rectangle r);
int rectangle_height(Rectangle r);

float get_clamped(float value, float min, float max);