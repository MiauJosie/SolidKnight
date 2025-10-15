#pragma once
#include <stdbool.h>

typedef struct
{
    int x;
    int y;
} Vector2;

typedef struct
{
    int left;   // top_left_x
    int top;    // top_left_y
    int right;  // width
    int bottom; // height
} Rectangle;

// funções para vetor2
Vector2 vector2_create(int x, int y);
Vector2 vector2_add(Vector2 a, Vector2 b);
Vector2 vector2_scale(Vector2 v, int scale);

// funções para retângulo
bool rectangles_intersect(Rectangle a, Rectangle b);
int rectangle_width(Rectangle r);
int rectangle_height(Rectangle r);