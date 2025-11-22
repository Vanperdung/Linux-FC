#pragma once

#include <cstdint>

template <typename T>
class Vector3
{
public:
    constexpr Vector3()
        : x(0), y(0), z(0) {}
    constexpr Vector3(const T x0, const T y0, const T z0)
        : x(x0), y(y0), z(z0) {}

    Vector3<T> &operator=(const Vector3<T> &v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }
    Vector3<T> &operator+=(const Vector3<T> &v)
    {
        x += v.x; 
        y += v.y; 
        z += v.z; 
        return *this;
    }
    Vector3<T> &operator-=(const Vector3<T> &v)
    {
        x -= v.x; 
        y -= v.y; 
        z -= v.z; 
        return *this;
    }

    bool operator==(const Vector3<T> &v) { return (x == v.x && y == v.y && z == v.z); }
    bool operator!= (const Vector3<T> &v) { return (x != v.x || y != v.y || z != v.z); }

    void zero() { x = y = z = 0; }
    bool isZero() { return (x == 0 && y == 0 && z == 0); }

    T x, y, z;
};

typedef Vector3<float> Vector3f;
typedef Vector3<double> Vector3d;
typedef Vector3<int16_t> Vector3i;
typedef Vector3<uint16_t> Vector3ui;
typedef Vector3<int32_t> Vector3l;
typedef Vector3<uint32_t> Vector3ul;