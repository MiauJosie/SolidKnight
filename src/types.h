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

Vector2 vector2_create(float x, float y);
Vector2 vector2_add(Vector2 a, Vector2 b);
Vector2 vector2_scale(Vector2 v, float scale);
bool rectangles_intersect(Rectangle a, Rectangle b);
int rectangle_width(Rectangle r);
int rectangle_height(Rectangle r);
float get_clamped(float value, float min, float max);
float sign_here_please(float value);
float move_towards(float current, float target, float max_delta);