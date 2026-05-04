#pragma once

class Trans
{
public:

    struct VECTOR {
        float x;
        float y;
        float z;
    };

    struct Vector2 {
        float x;
        float y;
    };

    // VECTOR => Vector2 •ÏŠ·
    Vector2 ToVector2(const Vector2& v) {
        return { v.x, v.y };
    }
};


