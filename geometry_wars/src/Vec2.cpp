#include "Vec2.h"

Vec2::Vec2() {}

Vec2::Vec2(float xin, float yin)
    : x(xin), y(yin)
{
}

Vec2 Vec2::operator + (const Vec2& rhs) const {
    return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator - (const Vec2& rhs) const {
    return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 Vec2::operator * (const Vec2& rhs) const {
    return Vec2(x * rhs.x, y * rhs.y);
}

Vec2 Vec2::operator / (const Vec2& rhs) const {
    return Vec2(x / rhs.x, y / rhs.y);
}

void Vec2::operator += (const Vec2& rhs) {
    x += rhs.x;
    y += rhs.y;
}

void Vec2::operator -= (const Vec2& rhs) {
    x -= rhs.x;
    y -= rhs.y;
}

void Vec2::operator *= (const Vec2& rhs) {
    x *= rhs.x;
    y *= rhs.y;
}

void Vec2::operator /= (const Vec2& rhs) {
    x /= rhs.x;
    y /= rhs.y;
}

bool Vec2::operator == (const Vec2& rhs) const {
    return x == rhs.x && y == rhs.y;
}

bool Vec2::operator != (const Vec2& rhs) const {
    return x != rhs.x || y != rhs.y;
}

float Vec2::dist(const Vec2& rhs) const {
    Vec2 distVec = Vec2(rhs.x - x, rhs.y - y);

    return sqrtf(distVec.x * distVec.x + distVec.y * distVec.y);
}

Vec2 Vec2::normalize() const {
    float length = std::sqrt(x * x + y * y);

    if (length == 0) {
        return Vec2(0, 0);
    }

    return Vec2(x / length, y / length);
}
