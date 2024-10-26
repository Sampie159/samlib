#include "samlib.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                   MATH                                    */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                   VEC2                                    */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

Vec2 operator+(Vec2 a, Vec2 b) {
    Vec2 res;
    res.x = a.x + b.x;
    res.y = a.y + b.y;
    return res;
}

void operator+=(Vec2& a, Vec2 b) {
    a = a + b;
}

Vec2 operator+(Vec2 a, f32 val) {
    Vec2 res;
    res.x = a.x + val;
    res.y = a.y + val;
    return res;
}

void operator+=(Vec2& a, f32 val) {
    a = a + val;
}

Vec2 operator-(Vec2 a, Vec2 b) {
    Vec2 res;
    res.x = a.x - b.x;
    res.y = a.y - b.y;
    return res;
}

void operator-=(Vec2& a, Vec2 b) {
    a = a - b;
}

Vec2 operator-(Vec2 a, f32 val) {
    Vec2 res;
    res.x = a.x - val;
    res.y = a.y - val;
    return res;
}

void operator-=(Vec2& a, f32 val) {
    a = a - val;
}

Vec2 operator*(Vec2 a, f32 scalar) {
    Vec2 res;
    res.x = a.x * scalar;
    res.y = a.y * scalar;
    return res;
}

void operator*=(Vec2& a, f32 scalar) {
    a = a * scalar;
}

Vec2 operator/(Vec2 a, f32 scalar) {
    Vec2 res;
    res.x = a.x / scalar;
    res.y = a.y / scalar;
    return res;
}

void operator/=(Vec2& a, f32 scalar) {
    a = a / scalar;
}

bool operator==(Vec2 a, Vec2 b) {
    bool res = a.x == b.x && a.y == b.y;
    return res;
}

bool operator!=(Vec2 a, Vec2 b) {
    bool res = a.x != b.x && a.y != b.y;
    return res;
}

void clamp(Vec2& vec, Vec2 min, Vec2 max) {
    CLAMP(vec.x, min.x, max.x);
    CLAMP(vec.y, min.y, max.y);
}

f32 dot(Vec2 a, Vec2 b) {
    f32 res = a.x * b.x + a.y * b.y;
    return res;
}

f32 length_sq(Vec2 vec) {
    return dot(vec, vec);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                   VEC3                                    */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

Vec3 operator+(Vec3 a, Vec3 b) {
    Vec3 res;
    res.x = a.x + b.x;
    res.y = a.y + b.y;
    res.z = a.z + b.z;
    return res;
}

void operator+=(Vec3& a, Vec3 b) {
    a = a + b;
}

Vec3 operator+(Vec3 a, f32 val) {
    Vec3 res;
    res.x = a.x + val;
    res.y = a.y + val;
    res.z = a.z + val;
    return res;
}

void operator+=(Vec3& a, f32 val) {
    a = a + val;
}

Vec3 operator-(Vec3 a, Vec3 b) {
    Vec3 res;
    res.x = a.x - b.x;
    res.y = a.y - b.y;
    res.z = a.z - b.z;
    return res;
}

void operator-=(Vec3& a, Vec3 b) {
    a = a - b;
}

Vec3 operator-(Vec3 a, f32 val) {
    Vec3 res;
    res.x = a.x - val;
    res.y = a.y - val;
    res.z = a.z - val;
    return res;
}

void operator-=(Vec3& a, f32 val) {
    a = a - val;
}

Vec3 operator*(Vec3 a, f32 scalar) {
    Vec3 res;
    res.x = a.x * scalar;
    res.y = a.y * scalar;
    res.z = a.z * scalar;
    return res;
}

void operator*=(Vec3& a, f32 scalar) {
    a = a * scalar;
}

Vec3 operator/(Vec3 a, f32 scalar) {
    Vec3 res;
    res.x = a.x / scalar;
    res.y = a.y / scalar;
    res.z = a.z / scalar;
    return res;
}

void operator/=(Vec3& a, f32 scalar) {
    a = a / scalar;
}

bool operator==(Vec3 a, Vec3 b) {
    bool res = a.x == b.x && a.y == b.y && a.z == b.z;
    return res;
}

bool operator!=(Vec3 a, Vec3 b) {
    bool res = a.x != b.x && a.y != b.y && a.z != b.z;
    return res;
}

void clamp(Vec3& vec, Vec3 min, Vec3 max) {
    CLAMP(vec.x, min.x, max.x);
    CLAMP(vec.y, min.y, max.y);
    CLAMP(vec.z, min.z, max.z);
}

f32 dot(Vec3 a, Vec3 b) {
    f32 res = a.x * b.x + a.y * b.y + a.z * b.z;
    return res;
}

f32 length_sq(Vec3 vec) {
    return dot(vec, vec);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                   VEC4                                    */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

Vec4 operator+(Vec4 a, Vec4 b) {
    Vec4 res;
    res.x = a.x + b.x;
    res.y = a.y + b.y;
    res.z = a.z + b.z;
    res.w = a.w + b.w;
    return res;
}

void operator+=(Vec4& a, Vec4 b) {
    a = a + b;
}

Vec4 operator+(Vec4 a, f32 val) {
    Vec4 res;
    res.x = a.x + val;
    res.y = a.y + val;
    res.z = a.z + val;
    res.w = a.w + val;
    return res;
}

void operator+=(Vec4& a, f32 val) {
    a = a + val;
}

Vec4 operator-(Vec4 a, Vec4 b) {
    Vec4 res;
    res.x = a.x - b.x;
    res.y = a.y - b.y;
    res.z = a.z - b.z;
    res.w = a.w - b.w;
    return res;
}

void operator-=(Vec4& a, Vec4 b) {
    a = a - b;
}

Vec4 operator-(Vec4 a, f32 val) {
    Vec4 res;
    res.x = a.x - val;
    res.y = a.y - val;
    res.z = a.z - val;
    res.w = a.w - val;
    return res;
}

void operator-=(Vec4& a, f32 val) {
    a = a - val;
}

Vec4 operator*(Vec4 a, f32 scalar) {
    Vec4 res;
    res.x = a.x * scalar;
    res.y = a.y * scalar;
    res.z = a.z * scalar;
    res.w = a.w * scalar;
    return res;
}

void operator*=(Vec4& a, f32 scalar) {
    a = a * scalar;
}

Vec4 operator/(Vec4 a, f32 scalar) {
    Vec4 res;
    res.x = a.x / scalar;
    res.y = a.y / scalar;
    res.z = a.z / scalar;
    res.w = a.w / scalar;
    return res;
}

void operator/=(Vec4& a, f32 scalar) {
    a = a / scalar;
}

bool operator==(Vec4 a, Vec4 b) {
    bool res = a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
    return res;
}

bool operator!=(Vec4 a, Vec4 b) {
    bool res = a.x != b.x && a.y != b.y && a.z != b.z && a.w != b.w;
    return res;
}

void clamp(Vec4& vec, Vec4 min, Vec4 max) {
    CLAMP(vec.x, min.x, max.x);
    CLAMP(vec.y, min.y, max.y);
    CLAMP(vec.z, min.z, max.z);
    CLAMP(vec.w, min.w, max.w);
}

f32 dot(Vec4 a, Vec4 b) {
    f32 res = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    return res;
}

f32 length_sq(Vec4 vec) {
    return dot(vec, vec);
}
