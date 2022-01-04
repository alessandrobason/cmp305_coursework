#pragma once

#include <math.h>

#include "types.h"
#include "MathUtils.h"

template<typename T>
struct vec2 {
    T x, y;

    constexpr vec2()
        : x(0), y(0) {}

    constexpr vec2(const T &value_for_all)
        : x(value_for_all), y(value_for_all) {}

    constexpr vec2(const T &x, const T &y)
        : x(x), y(y) {}

    template<typename T2>
    constexpr vec2(const vec2<T2> &other)
        : x((T)other.x), y((T)other.y) {}

    constexpr vec2(const float2 &other)
        : x(other.x), y(other.y) {}

    static vec2 one() {
        return { 1, 1 };
    }

    static vec2 zero() {
        return { 0, 0 };
    }

    operator float2() const {
        return {
            (f32)x, (f32)y
        };
    }

    bool operator==(const vec2 &other) const {
        return (
            x == other.x && y == other.y
        );
    }

    bool operator!=(const vec2 &other) const {
        return (
            x != other.x || y != other.y
        );
    }

    vec2 &operator=(const vec2 &other) {
        x = other.x;
        y = other.y;
        return *this;
    }

    vec2 operator-() const {
        return vec2{ -x, -y };
    }

    vec2 operator+(const vec2 &other) const {
        return vec2{
            x + other.x, y + other.y
        };
    }

    vec2 operator-(const vec2 &other) const {
        return vec2{
            x - other.x, y - other.y
        };
    }

    vec2 operator+(const T &other) const {
        return vec2{
            x + other, y + other
        };
    }

    vec2 operator-(const T &other) const {
        return vec2{
            x - other, y - other
        };
    }

    vec2 operator*(const vec2 &other) const {
        return vec2{
            x * other.x, y * other.y
        };
    }

    vec2 operator/(const vec2 &other) const {
        return vec2{
            x / other.x, y / other.y
        };
    }

    vec2 operator*(const T &other) const {
        return vec2{
            x * other, y * other
        };
    }

    vec2 operator/(const T &other) const {
        return vec2{
            x / other, y / other
        };
    }

    void operator+=(const vec2 &other) {
        x += other.x;
        y += other.y;
    }

    void operator-=(const vec2 &other) {
        x -= other.x;
        y -= other.y;
    }

    void operator+=(const T &other) {
        x += other;
        y += other;
    }

    void operator-=(const T &other) {
        x -= other;
        y -= other;
    }

    void operator*=(const vec2 &other) {
        x *= other.x;
        y *= other.y;
    }

    void operator/=(const vec2 &other) {
        x /= other.x;
        y /= other.y;
    }

    void operator*=(const T &other) {
        x *= other;
        y *= other;
    }

    void operator/=(const T &other) {
        x /= other;
        y /= other;
    }

    T mag2() const {
        return pow2(x) + pow2(y);
    }

    T mag() const {
        return (T)sqrt(mag2());
    }

    void normalize() {
        T magnitude = mag();
        if (magnitude) {
            x /= magnitude;
            y /= magnitude;
        }
    }

    vec2<T> normalized() const {
        vec2<T> res = *this;
        res.normalize();
        return res;
    }
};

template<typename T>
struct vec3 {
    T x, y, z;

    constexpr vec3()
        : x(0), y(0), z(0) {}

    constexpr vec3(const T &value_for_all)
        : x(value_for_all), y(value_for_all), z(value_for_all) {}

    constexpr vec3(const T &x, const T &y, const T &z)
        : x(x), y(y), z(z) {}

    constexpr vec3(const vec2<T> &other, const T &z = 0)
        : x(other.x), y(other.y), z(z) {}

    constexpr vec3(const float3 &other)
        : x(other.x), y(other.y), z(other.z) {}

    static vec3 one() {
        return { 1, 1, 1 };
    }

    static vec3 zero() {
        return { 0, 0, 0 };
    }

    operator float3() const {
        return {
            (f32)x, (f32)y, (f32)z
        };
    }

    bool operator==(const vec3 &other) const {
        return (
            x == other.x && y == other.y && z == other.z
            );
    }

    bool operator!=(const vec3 &other) const {
        return (
            x != other.x || y != other.y || z != other.z
            );
    }

    vec3 &operator=(const vec3 &other) {
        x = other.x;
        y = other.y;
        z = other.z;
        return *this;
    }

    vec3 operator-() const {
        return vec3{ -x, -y, -z };
    }

    vec3 operator+(const vec3 &other) const {
        return vec3{
            x + other.x, y + other.y, z + other.z
        };
    }

    vec3 operator-(const vec3 &other) const {
        return vec3{
            x - other.x, y - other.y, z - other.z
        };
    }

    vec3 operator*(const T &other) const {
        return vec3{
            x * other, y * other, z * other
        };
    }

    vec3 operator/(const T &other) const {
        return vec3{
            x / other, y / other, z / other
        };
    }

    void operator+=(const vec3 &other) {
        x += other.x;
        y += other.y;
        z += other.z;
    }

    void operator-=(const vec3 &other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
    }

    void operator*=(const T &other) {
        x *= other;
        y *= other;
        z *= other;
    }

    void operator/=(const T &other) {
        x /= other;
        y /= other;
        z /= other;
    }

    T mag2() const {
        return pow2(x) + pow2(y) + pow2(z);
    }

    T mag() const {
        return sqrt(mag2());
    }

    void normalize() {
        T magnitude = mag();
        if (magnitude) {
            x /= magnitude;
            y /= magnitude;
            z /= magnitude;
        }
    }

    vec3<T> normalized() const {
        vec3<T> res = *this;
        res.normalize();
        return res;
    }
};

template<typename T>
struct vec4 {
    T x, y, z, w;

    constexpr vec4()
        : x(0), y(0), z(0), w(0) {}

    constexpr vec4(const T &value_for_all)
        : x(value_for_all), y(value_for_all), z(value_for_all), w(value_for_all) {}

    constexpr vec4(const T &x, const T &y, const T &z, const T &w)
        : x(x), y(y), z(z), w(w) {}

    constexpr vec4(const vec2<T> &v1, const vec2<T> &v2)
        : x(v1.x), y(v1.y), z(v2.x), w(v2.y) {}

    constexpr vec4(const vec3<T> &v, T w = 0)
        : x(v.x), y(v.y), z(v.z), w(w) {}

    constexpr vec4(const float4 &other)
        : x(other.x), y(other.y), z(other.z), w(other.w) {}

    static vec4 one() {
        return { 1, 1, 1, 1 };
    }

    static vec4 zero() {
        return { 0, 0, 0, 0 };
    }

    operator float4() const {
        return {
            (f32)x, (f32)y, (f32)z, (f32)w
        };
    }

    bool operator==(const vec4 &other) const {
        return (
            x == other.x && y == other.y && z == other.z && w == other.w
            );
    }

    bool operator!=(const vec4 &other) const {
        return (
            x != other.x || y != other.y || z != other.z || w != other.w
            );
    }

    vec4 &operator=(const vec4 &other) {
        x = other.x;
        y = other.y;
        z = other.z;
        w = other.w;
        return *this;
    }

    vec4 operator-() const {
        return vec4{ -x, -y, -z, -w };
    }

    vec4 operator+(const vec4 &other) const {
        return vec4{
            x + other.x, y + other.y, z + other.z, w + other.w
        };
    }

    vec4 operator-(const vec4 &other) const {
        return vec4{
            x - other.x, y - other.y, z - other.z, w - other.w
        };
    }

    vec4 operator*(const T &other) const {
        return vec4{
            x * other, y * other, z * other, w * other
        };
    }

    vec4 operator/(const T &other) const {
        return vec4{
            x / other, y / other, z / other, w / other
        };
    }

    void operator+=(const vec4 &other) {
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;
    }

    void operator-=(const vec4 &other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;
    }

    void operator*=(const T &other) {
        x *= other;
        y *= other;
        z *= other;
        w *= other;
    }

    void operator/=(const T &other) {
        x /= other;
        y /= other;
        z /= other;
        w /= other;
    }

    T mag2() const {
        return pow2(x) + pow2(y) + pow2(z) + pow2(w);
    }

    T mag() const {
        return sqrt(mag2());
    }

    void normalize() {
        T magnitude = mag();
        if (magnitude) {
            x /= magnitude;
            y /= magnitude;
            z /= magnitude;
            w /= magnitude;
        }
    }

    vec4<T> normalized() const {
        vec4<T> res = *this;
        res.normalize();
        return res;
    }

    void setPosition(const vec2<T> &pos) {
        x = pos.x;
        y = pos.y;
    }

    vec2<T> position() const {
        return { x, y };
    }

    void setSize(const vec2<T> &size) {
        z = size.x;
        w = size.y;
    }

    vec2<T> size() const {
        return { z, w };
    }

    void scale(const T &s) {
        x *= s;
        y *= s;
        z *= s;
        w *= s;
    }

    void scale(const T &sx, const T &sy) {
        x *= sx;
        y *= sy;
        z *= sx;
        w *= sy;
    }

    void inflate(const T &amount) {
        x -= amount;
        y -= amount;
        z += amount * 2;
        w += amount * 2;
    }

    vec2<T> center() const {
        return { x + (z / 2), y + (w / 2) };
    }

    vec2<T> localCenter() const {
        return { z / 2, w / 2 };
    }

    vec2<T> topLeft() const {
        return { x, y };
    }

    vec2<T> topRight() const {
        return { x + z, y };
    }

    vec2<T> bottomLeft() const {
        return { x, y + w };
    }

    vec2<T> bottomRight() const {
        return { x + z, y + w };
    }

    vec2<T> toLocal(const vec2<T> &point) const {
        return { point.x - x, point.y - y };
    }
};

template<typename T>
T dot(const vec2<T> &v1, const vec2<T> &v2) {
    return (v1.x * v2.x) + (v1.y * v2.y);
}

template<typename T>
T dot(const vec3<T> &v1, const vec3<T> &v2) {
    return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

template<typename T>
T dot(const vec4<T> &v1, const vec4<T> &v2) {
    return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z) + (v1.w * v2.w);
}

template<typename T>
vec3<T> cross(const vec3<T> &v1, const vec3<T> &v2) {
    return {
        (v1.y * v2.z) - (v1.z * v2.y),
        (v1.z * v2.x) - (v1.x * v2.z),
        (v1.x * v2.y) - (v1.y * v2.x)
    };
}

typedef vec2<u32> vec2u;
typedef vec2<i32> vec2i;
typedef vec2<f32> vec2f;
typedef vec2<f64> vec2d;

typedef vec3<u32> vec3u;
typedef vec3<i32> vec3i;
typedef vec3<f32> vec3f;
typedef vec3<f64> vec3d;

typedef vec4<u32> vec4u;
typedef vec4<i32> vec4i;
typedef vec4<f32> vec4f;
typedef vec4<f64> vec4d;