/*
================================================================================

Graphics Math Library version 1.1.2 (public domain)

 GMath is a single-header C++ library for vector, matrix, and quaternion math,
primarily for use in graphics applications. As a C++ only library it uses
operator overloading to implement math operations, which is a bit cleaner and
more intuitive than C style function calls. SSE optimizations, along with
some misc math/matrix functions are borrowed from the Handmade Math library
(also public domain).

Tested with MSVC compiler v15.9.11.

================================================================================

USAGE

To use the library, you must place the following two lines:

#define GMATH_IMPLEMENTATION
#include "GMath.h"

in exactly one C++ file. You are then free to include this header file wherever
needed. If you wish to use the library without SSE intrinsics, you must also
define GMATH_NO_INTRINSICS in that same source file, like so:

#define GMATH_NO_INTRINSICS
#define GMATH_IMPLEMENTATION
#include "GMath.h"

If you would like to avoid including <math.h>, you'll need to define the
following, substituting your own functions for my_x_function:

#define GMATH_EXP my_exp_function
#define GMATH_LOG my_log_function
#define GMATH_SIN my_sin_function
#define GMATH_COS my_cos_function
#define GMATH_TAN my_tan_function
#define GMATH_ACOS my_acos_function
#define GMATH_ATAN my_atan_function
#define GMATH_SQRT my_sqrt_function
#define GMATH_ATAN2 my_atan2_function

#define GMATH_IMPLEMENTATION
#include "GMath.h"

If you define all of these functions, then GMath will not include the <math.h>
header. This is useful if you'd like to avoid depending on the CRT library.

When creating a camera projection matrix, the default behavior for GMATH is to
use the range [-1..1] for depth. If you'd like to use the range [0..1], you must
define GMATH_DEPTH_ZERO_TO_ONE in the source file, like so:

#define GMATH_DEPTH_ZERO_TO_ONE
#define GMATH_IMPLEMENTATION
#include "GMath.h"

Finally, GMath provides a set of functions for printing vector, matrix, and
quaternion types, which is disabled by default. To enable printing via IOStream,
define GMATH_USE_IOSTREAM in the source file, like so:

#define GMATH_USE_IOSTREAM
#define GMATH_IMPLEMENTATION
#include "GMath.h"

================================================================================

LICENSE

This software is dual-licensed to the public domain and under the following
license: you are granted a perpetual, irrevocable license to copy, modify,
publish, and distribute this file as you see fit.

================================================================================

CREDITS

Primary Author: Matthew McLaurin (matthew@matthewmclaurin.com)

GMath includes parts of version 1.9.0 of the Handmade Math library, which you
can find at the following link:

https://github.com/HandmadeMath/Handmade-Math

Handmade Math Credits:
Primary Author: Zakary Strange (zak@strangedev.net && @strangezak)

Functionality: Matt Mascarenhas (@miblo_), Aleph, FieryDrake (@fierydrake),
Gingerbill (@TheGingerBill), Ben Visness (@bvisness),
Trinton Bullard (@Peliex_Dev), @AntonDan

Fixes: Jeroen van Rijn (@J_vanRijn), Kiljacken (@Kiljacken),
Insofaras (@insofaras), Daniel Gibson (@DanielGibson)

===============================================================================

STILL TO DO

- Put functions into a GMath namespace, to prevent name collisions. This is a
real necessity, especially since windows.h defines Min() and Max() by default.
It would be good to provide a macro to disable namespacing. It would have to be
defined before ANY include of this file, but that's already the case for 
GMATH_NO_INTRINSICS anyway.

- Move functions around in this file, to better organize them. Currently, almost
everything is defined inline above the implementation guard. The exceptions are
definitions which rely on other types, like type conversion functions, and stuff
that I just added at the bottom for no reason.

- Add an IVec4 type which makes use of intrinsics the same way that Vec4 does.
The reason there's not already an IVec4 type is that I don't usually need one.
It would primarily be used as a linear color struct, I imagine.

- Add left/right hand versions of matrix initializers/conversion functions. The
D3D9 manual has good examples of these.

- Allow the user to specify which direction is up, if we can find a way to do
that. I think it would only affect the static initializers? Not totally sure.

- Add more helper functions, generally. Things like NearlyEqual and Distance
for vectors, more matrix helpers, etc. Maybe quaternion functions that don't
normalize, etc.

- Put this on GitHub, but make it clear in the readme that people should just go
use the Handmade Math library instead, because it's more compatible and probably
better maintained than this modified version.

================================================================================
*/

#ifndef GMATH_H
#define GMATH_H

#ifdef _MSC_VER
#pragma warning(disable:4201)
#endif

#ifdef __clang__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-anonymous-struct"
#endif

#if defined(__GNUC__) && (__GNUC__ == 4 && __GNUC_MINOR__ < 8)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"
#endif

#ifndef GMATH_NO_INTRINSICS
#ifdef _MSC_VER
// MSVC supports SSE in amd64 mode or _M_IX86_FP >= 1 (2 means SSE2).
#if defined(_M_AMD64) || ( defined(_M_IX86_FP) && _M_IX86_FP >= 1 )
#define GMATH_USE_SSE 1
#endif
#else // If not MSVC, check if SSE is supported anyway.
#ifdef __SSE__
#define GMATH_USE_SSE 1
#endif
#endif
#endif

#ifdef GMATH_USE_SSE
#include <xmmintrin.h>
#endif

#ifdef GMATH_USE_IOSTREAM
#include <iostream>
#endif

#if !defined(GMATH_SIN) || !defined(GMATH_COS) || !defined(GMATH_TAN) || \
!defined(GMATH_SQRT) || !defined(GMATH_EXP) || !defined(GMATH_LOG) ||    \
!defined(GMATH_ACOS) || !defined(GMATH_ATAN)|| !defined(GMATH_ATAN2)
#include <math.h>
#endif

#ifndef GMATH_SIN
#define GMATH_SIN sinf
#endif

#ifndef GMATH_COS
#define GMATH_COS cosf
#endif

#ifndef GMATH_TAN
#define GMATH_TAN tanf
#endif

#ifndef GMATH_SQRT
#define GMATH_SQRT sqrtf
#endif

#ifndef GMATH_EXP
#define GMATH_EXP expf
#endif

#ifndef GMATH_LOG
#define GMATH_LOG logf
#endif

#ifndef GMATH_ACOS
#define GMATH_ACOS acosf
#endif

#ifndef GMATH_ATAN
#define GMATH_ATAN atanf
#endif

#ifndef GMATH_ATAN2
#define GMATH_ATAN2 atan2f
#endif

#define GMATH_PI 3.14159265359f
#define GMATH_HALF_PI 1.57079632679
#define GMATH_TWO_PI 6.28318530718
#define GMATH_E 2.71828182845

#define GMATH_MIN(a, b) (a) > (b) ? (b) : (a)
#define GMATH_MAX(a, b) (a) < (b) ? (b) : (a)
#define GMATH_ABS(a) ((a) > 0 ? (a) : -(a))
#define GMATH_MOD(a, b) ((a) % (b)) >= 0 ? ((a) % (b)) : (((a) % (b)) + (b))

// Forward declarations for types.
struct IVec2;
struct IVec3;
struct Vec2;
struct Vec3;
struct Vec4;
struct Mat4;
struct Quat;

// TODO(Matt): This doesn't really belong here. Move it to a different math library.
inline size_t ComputePadding(size_t alignment, size_t min_offset)
{
    return (alignment - min_offset % alignment) % alignment;
}

// Integer vector types (two and three components).
struct IVec2
{
    union
    {
        struct {int x, y;};
        struct {int r, g;};
        struct {int width, height;};
        int data[2];
    };
    inline int& operator[](int i) {return data[i];}
    inline const int& operator[](int i) const {return data[i];}
    inline IVec2 operator-() const {return {-x, -y};}
    inline IVec2& operator++()
    {
        ++x;++y;
        return *this;
    }
    inline IVec2& operator--()
    {
        --x;
        --y;
        return *this;
    }
    inline IVec2& operator+=(IVec2 vec)
    {
        x += vec.x;
        y += vec.y;
        return *this;
    }
    inline IVec2& operator+=(int scalar)
    {
        x += scalar;
        y += scalar;
        return *this;
    }
    inline IVec2& operator-=(IVec2 vec)
    {
        x -= vec.x;
        y -= vec.y;
        return *this;
    }
    inline IVec2& operator-=(int scalar)
    {
        x -= scalar;
        y -= scalar;
        return *this;
    }
    inline operator Vec2() const;
    const static IVec2 Zero;
    const static IVec2 One;
    const static IVec2 Right;
    const static IVec2 Up;
    const static IVec2 Left;
    const static IVec2 Down;
};
inline IVec2 CreateIVec2() {return {};};
inline IVec2 CreateIVec2(int fill) {return {fill, fill};};
inline IVec2 CreateIVec2(int x, int y) {return {x, y};};
const IVec2 IVec2::Zero = {0, 0};
const IVec2 IVec2::One = {1, 1};
const IVec2 IVec2::Right = {1, 0};
const IVec2 IVec2::Up = {0, 1};
const IVec2 IVec2::Left = {-1, 0};
const IVec2 IVec2::Down = {0, -1};

inline IVec2 operator*(int a, IVec2 b) {return {b.x * a, b.y * a};}
inline IVec2 operator*(IVec2 a, int b) {return {a.x * b, a.y * b};}
inline IVec2 operator/(int a, IVec2 b) {return {b.x / a, b.y / a};}
inline IVec2 operator/(IVec2 a, int b) {return {a.x / b, a.y / b};}
inline IVec2 operator+(int a, IVec2 b) {return {b.x + a, b.y + a};}
inline IVec2 operator+(IVec2 a, int b) {return {a.x + b, a.y + b};}
inline IVec2 operator-(int a, IVec2 b) {return {b.x - a, b.y - a};}
inline IVec2 operator-(IVec2 a, int b) {return {a.x - b, a.y - b};}
inline IVec2 operator*(IVec2 a, IVec2 b) {return {a.x * b.x, a.y};}
inline IVec2 operator/(IVec2 a, IVec2 b) {return {a.x / b.x, a.y / b.y};}
inline IVec2 operator+(IVec2 a, IVec2 b) {return {a.x + b.x, a.y + b.y};}
inline IVec2 operator-(IVec2 a, IVec2 b) {return {a.x - b.x, a.y - b.y};}
inline bool operator==(IVec2 a, IVec2 b) {return (a.x == b.x && a.y == b.y);}
#ifdef GMATH_USE_IOSTREAM
std::ostream& operator<<(std::ostream& a, IVec2 b) {return a << "(" << b.x << ", " << b.y << ")";}
#endif
struct IVec3
{
    union
    {
        struct {int x, y, z;};
        struct {int r, g, b;};
        struct {int width, height, depth;};
        struct {IVec2 xy; int ignored_0;};
        struct {int ignored_1; IVec2 yz;};
        struct {IVec2 rg; int ignored_2;};
        struct {int ignored_3; IVec2 gb;};
        int data[3];
    };
    inline int& operator[](int i) {return data[i];}
    inline const int& operator[](int i) const {return data[i];}
    inline IVec3 operator-() const {return {-x, -y, -z};}
    inline IVec3& operator++()
    {
        ++x;
        ++y;
        ++z;
        return *this;
    }
    inline IVec3& operator--()
    {
        --x;
        --y;
        --z;
        return *this;
    }
    inline IVec3& operator+=(IVec3 vec)
    {
        x += vec.x;
        y += vec.y;
        z += vec.z;
        return *this;
    }
    inline IVec3& operator+=(int scalar)
    {
        x += scalar;
        y += scalar;
        z += scalar;
        return *this;
    }
    inline IVec3& operator-=(IVec3 vec)
    {
        x -= vec.x;
        y -= vec.y;
        z -= vec.z;
        return *this;
    }
    inline IVec3& operator-=(int scalar)
    {
        x -= scalar;
        y -= scalar;
        z -= scalar;
        return *this;
    }
    inline IVec3& operator*=(int scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }
    inline IVec3& operator/=(int scalar)
    {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }
    inline operator Vec3() const;
    const static IVec3 Zero;
    const static IVec3 One;
    const static IVec3 Right;
    const static IVec3 Up;
    const static IVec3 Left;
    const static IVec3 Down;
    const static IVec3 Forward;
    const static IVec3 Backward;
};
inline IVec3 CreateIVec3() {return {};};
inline IVec3 CreateIVec3(int fill) {return {fill, fill, fill};};
inline IVec3 CreateIVec3(IVec2 xy, int z) {return {xy.x, xy.y, z};};
inline IVec3 CreateIVec3(int x, IVec2 yz) {return {x, yz.x, yz.y};};
inline IVec3 CreateIVec3(int x, int y, int z) {return {x, y, z};};
const IVec3 IVec3::Zero = {0, 0, 0};
const IVec3 IVec3::One = {1, 1, 1};
const IVec3 IVec3::Right = {1, 0, 0};
const IVec3 IVec3::Up = {0, 1, 0};
const IVec3 IVec3::Left = {-1, 0, 0};
const IVec3 IVec3::Down = {0, -1, 0};
const IVec3 IVec3::Forward = {0, 0, -1};
const IVec3 IVec3::Backward = {0, 0, 1};
inline IVec3 operator*(int a, IVec3 b) {return {b.x * a, b.y * a, b.z * a};}
inline IVec3 operator*(IVec3 a, int b) {return {a.x * b, a.y * b, a.z * b};}
inline IVec3 operator/(int a, IVec3 b) {return {b.x / a, b.y / a, b.z / a};}
inline IVec3 operator/(IVec3 a, int b) {return {a.x / b, a.y / b, a.z / b};}
inline IVec3 operator+(int a, IVec3 b) {return {b.x + a, b.y + a, b.z + a};}
inline IVec3 operator+(IVec3 a, int b) {return {a.x + b, a.y + b, a.z + b};}
inline IVec3 operator-(int a, IVec3 b) {return {b.x - a, b.y - a, b.z - a};}
inline IVec3 operator-(IVec3 a, int b) {return {a.x - b, a.y - b, a.z - b};}
inline IVec3 operator*(IVec3 a, IVec3 b) {return {a.x * b.x, a.y * b.y, a.z * b.z};}
inline IVec3 operator/(IVec3 a, IVec3 b) {return {a.x / b.x, a.y / b.y, a.z / b.z};}
inline IVec3 operator+(IVec3 a, IVec3 b) {return {a.x + b.x, a.y + b.y, a.z + b.z};}
inline IVec3 operator-(IVec3 a, IVec3 b) {return {a.x - b.x, a.y - b.y, a.z - b.z};}
inline bool operator==(IVec3 a, IVec3 b) {return (a.x == b.x && a.y == b.y && a.z == b.z);}
#ifdef GMATH_USE_IOSTREAM
std::ostream& operator<<(std::ostream& a, IVec3 b) {return a << "(" << b.x << ", " << b.y << ", " << b.z << ")";}
#endif

// Floating point vector types (two, three, and four components).
// Four component vector uses SSE optimizations if enabled.
struct Vec2
{
    union
    {
        struct {float x, y;};
        struct {float r, g;};
        struct {float u, v;};
        struct {float width, height;};
        float data[2];
    };
    inline float& operator[](int i) {return data[i];}
    inline const float& operator[](int i) const {return data[i];}
    inline Vec2 operator-() const {return {-x, -y};}
    inline Vec2& operator+=(Vec2 vec)
    {
        x += vec.x;
        y += vec.y;
        return *this;
    }
    inline Vec2& operator+=(float scalar)
    {
        x += scalar;
        y += scalar;
        return *this;
    }
    inline Vec2& operator-=(Vec2 vec)
    {
        x -= vec.x;
        y -= vec.y;
        return *this;
    }
    inline Vec2& operator-=(float scalar)
    {
        x -= scalar;
        y -= scalar;
        return *this;
    }
    inline Vec2& operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }
    inline Vec2& operator/=(float scalar)
    {
        x /= scalar;
        y /= scalar;
        return *this;
    }
    inline operator IVec2() const;
    const static Vec2 Zero;
    const static Vec2 One;
    const static Vec2 Right;
    const static Vec2 Up;
    const static Vec2 Left;
    const static Vec2 Down;
};
inline Vec2 CreateVec2() {return {};};
inline Vec2 CreateVec2(float fill) {return {fill, fill};};
inline Vec2 CreateVec2(float x, float y) {return {x, y};};
const Vec2 Vec2::Zero = {0.0f, 0.0f};
const Vec2 Vec2::One = {1.0f, 1.0f};
const Vec2 Vec2::Right = {1.0f, 0.0f};
const Vec2 Vec2::Up = {0.0f, 1.0f};
const Vec2 Vec2::Left = {-1.0f, 0.0f};
const Vec2 Vec2::Down = {0.0f, -1.0f};
inline Vec2 operator*(float a, Vec2 b) {return {a * b.x, a * b.y};}
inline Vec2 operator*(Vec2 a, float b) {return {a.x * b, a.y * b};}
inline Vec2 operator/(float a, Vec2 b) {return {a / b.x, a / b.y};}
inline Vec2 operator/(Vec2 a, float b) {return {a.x / b, a.y / b};}
inline Vec2 operator+(float a, Vec2 b) {return {a + b.x, a + b.y};}
inline Vec2 operator+(Vec2 a, float b) {return {a.x + b, a.y + b};}
inline Vec2 operator-(float a, Vec2 b) {return {a - b.x, a - b.y};}
inline Vec2 operator-(Vec2 a, float b) {return {a.x - b, a.y - b};}
inline Vec2 operator*(Vec2 a, Vec2 b) {return {a.x * b.x, a.y * b.y};}
inline Vec2 operator/(Vec2 a, Vec2 b) {return {a.x / b.y, a.y / b.y};}
inline Vec2 operator+(Vec2 a, Vec2 b) {return {a.x + b.x, a.y + b.y};}
inline Vec2 operator-(Vec2 a, Vec2 b) {return {a.x - b.x, a.y - b.y};}
inline bool operator==(Vec2 a, Vec2 b) {return (a.x == b.x && a.y == b.y);}
#ifdef GMATH_USE_IOSTREAM
std::ostream& operator<<(std::ostream& a, Vec2 b) {return a << "(" << b.x << ", " << b.y << ")";}
#endif
struct Vec3
{
    union
    {
        struct {float x, y, z;};
        struct {float r, g, b;};
        struct {float u, v, w;};
        struct {float width, height, depth;};
        float data[3];
        
        struct {Vec2 xy; float ignored_0;};
        struct {float ignored_1; Vec2 yz;};
        struct {Vec2 uv; float ignored_2;};
        struct {float ignored_3; Vec2 vw;};
        struct {Vec2 rg; float ignored_4;};
        struct {float ignored_5; Vec2 gb;};
    };
    inline float& operator[](int i) {return data[i];}
    inline const float& operator[](int i) const {return data[i];}
    inline Vec3 operator-() const {return {-x, -y, -z};}
    inline Vec3& operator+=(Vec3 vec)
    {
        x += vec.x;
        y += vec.y;
        z += vec.z;
        return *this;
    }
    inline Vec3& operator+=(float scalar)
    {
        x += scalar;
        y += scalar;
        z += scalar;
        return *this;
    }
    inline Vec3& operator-=(Vec3 vec)
    {
        x -= vec.x;
        y -= vec.y;
        z -= vec.z;
        return *this;
    }
    inline Vec3& operator-=(float scalar)
    {
        x -= scalar;
        y -= scalar;
        z -= scalar;
        return *this;
    }
    inline Vec3& operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }
    inline Vec3& operator/=(float scalar)
    {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }
    inline operator IVec3() const;
    const static Vec3 Zero;
    const static Vec3 One;
    const static Vec3 Right;
    const static Vec3 Up;
    const static Vec3 Left;
    const static Vec3 Down;
    const static Vec3 Forward;
    const static Vec3 Backward;
    const static Vec3 Red;
    const static Vec3 Green;
    const static Vec3 Blue;
    const static Vec3 Cyan;
    const static Vec3 Yellow;
    const static Vec3 Purple;
    const static Vec3 Black;
    const static Vec3 White;
};
inline Vec3 CreateVec3() {return {};};
inline Vec3 CreateVec3(float fill) {return {fill, fill, fill};};
inline Vec3 CreateVec3(Vec2 xy, float z) {return {xy.x, xy.y, z};};
inline Vec3 CreateVec3(float x, Vec2 yz) {return {x, yz.x, yz.y};};
inline Vec3 CreateVec3(float x, float y, float z) {return {x, y, z};};
const Vec3 Vec3::Zero = {0.0f, 0.0f, 0.0f};
const Vec3 Vec3::One = {1.0f, 1.0f, 1.0f};
const Vec3 Vec3::Right = {1.0f, 0.0f, 0.0f};
const Vec3 Vec3::Up = {0.0f, 1.0f, 0.0f};
const Vec3 Vec3::Left = {-1.0f, 0.0f, 0.0f};
const Vec3 Vec3::Down = {0.0f, -1.0f, 0.0f};
const Vec3 Vec3::Forward = {0.0f, 0.0f, -1.0f};
const Vec3 Vec3::Backward = {0.0f, 0.0f, 1.0f};
const Vec3 Vec3::Red = {1.0f, 0.0f, 0.0f};
const Vec3 Vec3::Green = {0.0f, 1.0f, 0.0f};
const Vec3 Vec3::Blue = {0.0f, 0.0f, 1.0f};
const Vec3 Vec3::Cyan = {0.0f, 1.0f, 1.0f};
const Vec3 Vec3::Yellow = {1.0f, 1.0f, 0.0f};
const Vec3 Vec3::Purple = {1.0f, 0.0f, 1.0f};
const Vec3 Vec3::Black = {0.0f, 0.0f, 0.0f};
const Vec3 Vec3::White = {1.0f, 1.0f, 1.0f};
inline Vec3 operator*(float a, Vec3 b) {return {b.x * a, b.y * a, b.z * a};}
inline Vec3 operator*(Vec3 a, float b) {return {a.x * b, a.y * b, a.z * b};}
inline Vec3 operator/(float a, Vec3 b) {return {b.x / a, b.y / a, b.z / a};}
inline Vec3 operator/(Vec3 a, float b) {return {a.x / b, a.y / b, a.z / b};}
inline Vec3 operator+(float a, Vec3 b) {return {b.x + a, b.y + a, b.z + a};}
inline Vec3 operator+(Vec3 a, float b) {return {a.x + b, a.y + b, a.z + b};}
inline Vec3 operator-(float a, Vec3 b) {return {b.x - a, b.y - a, b.z - a};}
inline Vec3 operator-(Vec3 a, float b) {return {a.x - b, a.y - b, a.z - b};}
inline Vec3 operator*(Vec3 a, Vec3 b) {return {a.x * b.x, a.y * b.y, a.z * b.z};}
inline Vec3 operator/(Vec3 a, Vec3 b) {return {a.x / b.x, a.y / b.y, a.z / b.z};}
inline Vec3 operator+(Vec3 a, Vec3 b) {return {a.x + b.x, a.y + b.y, a.z + b.z};}
inline Vec3 operator-(Vec3 a, Vec3 b) {return {a.x - b.x, a.y - b.y, a.z - b.z};}
inline bool operator==(Vec3 a, Vec3 b) {return (a.x == b.x && a.y == b.y && a.z == b.z);}
#ifdef GMATH_USE_IOSTREAM
std::ostream& operator<<(std::ostream& a, Vec3 b)
{
    return a << "(" << b.x << ", " << b.y << ", " << b.z << ")";
}
#endif

struct Vec4
{
    union
    {
        float data[4];
        struct
        {
            union
            {
                Vec3 xyz;
                struct {float x, y, z;};
            };
            float w;
        };
        
        struct
        {
            union
            {
                Vec3 rgb;
                struct {float r, g, b;};
            };
            float a;
        };
        
        struct
        {
            Vec2 xy;
            float ignored_0;
            float ignored_1;
        };
        
        struct
        {
            float ignored_2;
            Vec2 yz;
            float ignored_3;
        };
        
        struct
        {
            float ignored_4;
            float ignored_5;
            Vec2 zw;
        };
        
        // SSE type (four packed single precision floats).
#ifdef GMATH_USE_SSE
        __m128 data_sse;
#endif
    };
    inline float& operator[](int i) {return data[i];}
    inline const float& operator[] (int i) const {return data[i];}
    inline Vec4 operator-() const {return {-x, -y, -z, -w};}
    
    // Almost all Vec4 operators/functions from this point forward have two
    // implementations: One when SSE is enabled, and one for when it isn't.
    inline Vec4& operator+=(Vec4 vec)
    {
#ifdef GMATH_USE_SSE
        data_sse = _mm_add_ps(data_sse, vec.data_sse);
#else
        x += vec.x;
        y += vec.y;
        z += vec.z;
        w += vec.w;
#endif
        return *this;
    }
    inline Vec4& operator+=(float val)
    {
#ifdef GMATH_USE_SSE
        __m128 scalar = _mm_set1_ps(val);
        data_sse = _mm_add_ps(data_sse, scalar);
#else
        x += val;
        y += val;
        z += val;
        w += val;
#endif
        return *this;
    }
    inline Vec4& operator-=(Vec4& vec)
    {
#ifdef GMATH_USE_SSE
        data_sse = _mm_sub_ps(data_sse, vec.data_sse);
#else
        x -= vec.x;
        y -= vec.y;
        z -= vec.z;
        w -= vec.w;
#endif
        return *this;
    }
    inline Vec4& operator-=(float val)
    {
#ifdef GMATH_USE_SSE
        __m128 scalar = _mm_set1_ps(val);
        data_sse = _mm_sub_ps(data_sse, scalar);
#else
        x -= val;
        y -= val;
        z -= val;
        w -= val;
#endif
        return *this;
    }
    inline Vec4& operator*=(float val)
    {
#ifdef GMATH_USE_SSE
        __m128 scalar = _mm_set1_ps(val);
        data_sse = _mm_mul_ps(data_sse, scalar);
#else
        x *= val;
        y *= val;
        z *= val;
        w *= val;
#endif
        return *this;
    }
    inline Vec4& operator/=(float val)
    {
#ifdef GMATH_USE_SSE
        __m128 scalar = _mm_set1_ps(val);
        data_sse = _mm_div_ps(data_sse, scalar);
#else
        x /= val;
        y /= val;
        z /= val;
        w /= val;
#endif
        return *this;
    }
    const static Vec4 Zero;
    const static Vec4 One;
    const static Vec4 Right;
    const static Vec4 Up;
    const static Vec4 Left;
    const static Vec4 Down;
    const static Vec4 Forward;
    const static Vec4 Backward;
    const static Vec4 Red;
    const static Vec4 Green;
    const static Vec4 Blue;
    const static Vec4 Cyan;
    const static Vec4 Yellow;
    const static Vec4 Purple;
    const static Vec4 Black;
    const static Vec4 White;
};
inline Vec4 CreateVec4();
inline Vec4 CreateVec4(float fill);
inline Vec4 CreateVec4(Vec3 xyz, float w);
inline Vec4 CreateVec4(float x, float y, float z, float w);
const Vec4 Vec4::Zero = {0.0f, 0.0f, 0.0f, 0.0f};
const Vec4 Vec4::One = {1.0f, 1.0f, 1.0f, 1.0f};
const Vec4 Vec4::Right = {1.0f, 0.0f, 0.0f, 0.0f};
const Vec4 Vec4::Up = {0.0f, 1.0f, 0.0f, 0.0f};
const Vec4 Vec4::Left = {-1.0f, 0.0f, 0.0f, 0.0f};
const Vec4 Vec4::Down = {0.0f, -1.0f, 0.0f, 0.0f};
const Vec4 Vec4::Forward = {0.0f, 0.0f, -1.0f, 0.0f};
const Vec4 Vec4::Backward = {0.0f, 0.0f, 1.0f, 0.0f};
const Vec4 Vec4::Red = {1.0f, 0.0f, 0.0f, 1.0f};
const Vec4 Vec4::Green = {0.0f, 1.0f, 0.0f, 1.0f};
const Vec4 Vec4::Blue = {0.0f, 0.0f, 1.0f, 1.0f};
const Vec4 Vec4::Cyan = {0.0f, 1.0f, 1.0f, 1.0f};
const Vec4 Vec4::Yellow = {1.0f, 1.0f, 0.0f, 1.0f};
const Vec4 Vec4::Purple = {1.0f, 0.0f, 1.0f, 1.0f};
const Vec4 Vec4::Black = {0.0f, 0.0f, 0.0f, 0.0f};
const Vec4 Vec4::White = {1.0f, 1.0f, 1.0f, 1.0f};

inline Vec4 operator*(float a, Vec4 b)
{
    Vec4 result;
#ifdef GMATH_USE_SSE
    __m128 scalar = _mm_set1_ps(a);
    result.data_sse = _mm_mul_ps(scalar, b.data_sse);
#else
    result =  {a * b.x, a * b.y, a * b.z, a * b.w};
#endif
    return result;
}
inline Vec4 operator*(Vec4 a, float b)
{
    Vec4 result;
#ifdef GMATH_USE_SSE
    __m128 scalar = _mm_set1_ps(b);
    result.data_sse = _mm_mul_ps(a.data_sse, scalar);
#else
    result =  {a.x * b, a.y * b, a.z * b, a.w * b};
#endif
    return result;
}
inline Vec4 operator/(float a, Vec4 b)
{
    Vec4 result;
#ifdef GMATH_USE_SSE
    __m128 scalar = _mm_set1_ps(a);
    result.data_sse = _mm_div_ps(scalar, b.data_sse);
#else
    result =  {a / b.x, a / b.y, a / b.z, a / b.w};
#endif
    return result;
}
inline Vec4 operator/(Vec4 a, float b)
{
    Vec4 result;
#ifdef GMATH_USE_SSE
    __m128 scalar = _mm_set1_ps(b);
    result.data_sse = _mm_div_ps(a.data_sse, scalar);
#else
    result =  {a.x / b, a.y / b, a.z / b, a.w / b};
#endif
    return result;
}
inline Vec4 operator+(float a, Vec4 b)
{
    Vec4 result;
#ifdef GMATH_USE_SSE
    __m128 scalar = _mm_set1_ps(a);
    result.data_sse = _mm_add_ps(scalar, b.data_sse);
#else
    result =  {a + b.x, a + b.y, a + b.z, a + b.w};
#endif
    return result;
}
inline Vec4 operator+(Vec4 a, float b)
{
    Vec4 result;
#ifdef GMATH_USE_SSE
    __m128 scalar = _mm_set1_ps(b);
    result.data_sse = _mm_add_ps(a.data_sse, scalar);
#else
    result =  {a.x + b, a.y + b, a.z + b, a.w + b};
#endif
    return result;
}
inline Vec4 operator-(float a, Vec4 b)
{
    Vec4 result;
#ifdef GMATH_USE_SSE
    __m128 scalar = _mm_set1_ps(a);
    result.data_sse = _mm_sub_ps(scalar, b.data_sse);
#else
    result =  {a - b.x, a - b.y, a - b.z, a - b.w};
#endif
    return result;
}
inline Vec4 operator-(Vec4 a, float b)
{
    Vec4 result;
#ifdef GMATH_USE_SSE
    __m128 scalar = _mm_set1_ps(b);
    result.data_sse = _mm_add_ps(a.data_sse, scalar);
#else
    result =  {a.x - b, a.y - b, a.z - b, a.w - b};
#endif
    return result;
}
inline Vec4 operator*(Vec4 a, Vec4 b)
{
    Vec4 result;
#ifdef GMATH_USE_SSE
    result.data_sse = _mm_mul_ps(a.data_sse, b.data_sse);
#else
    result = {a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w};
#endif
    return result;
}
inline Vec4 operator/(Vec4 a, Vec4 b)
{
    Vec4 result;
#ifdef GMATH_USE_SSE
    result.data_sse = _mm_div_ps(a.data_sse, b.data_sse);
#else
    result = {a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w};
#endif
    return result;
}
inline Vec4 operator+(Vec4 a, Vec4 b)
{
    Vec4 result;
#ifdef GMATH_USE_SSE
    result.data_sse = _mm_add_ps(a.data_sse, b.data_sse);
#else
    result = {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
#endif
    return result;
}
inline Vec4 operator-(Vec4 a, Vec4 b)
{
    Vec4 result;
#ifdef HANDMADE_MATH__USE_SSE
    result.data_sse = _mm_sub_ps(a.data_sse, b.data_sse);
#else
    result = {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
#endif
    return result;
}
inline bool operator==(Vec4 a, Vec4 b)
{
    // TODO(Matt): SSE optimization?
    return (a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w);
}
#ifdef GMATH_USE_IOSTREAM
std::ostream& operator<<(std::ostream& a, Vec4 b)
{
    return a << "(" << b.x << ", " << b.y << ", " << b.z << ", " << b.w << ")";
}
#endif

// 4x4 Matrix type, column major. Uses SSE if enabled.

struct Mat4
{
    union
    {
        Vec4 columns[4];
        
#ifdef GMATH_USE_SSE
        __m128 data_sse[4];
#endif
    };
    inline Vec4 operator[](int i) {return columns[i];}
    inline const Vec4 operator[](int i) const {return columns[i];}
    const static Mat4 Zero;
    const static Mat4 Identity;
};
inline Mat4 CreateMat4();
inline Mat4 CreateMat4(float diagonal)
{
    Mat4 mat;
    mat[0] = {1.0f, 0.0f, 0.0f, 0.0f};
    mat[1] = {0.0f, 1.0f, 0.0f, 0.0f};
    mat[2] = {0.0f, 0.0f, 1.0f, 0.0f};
    mat[3] = {0.0f, 0.0f, 0.0f, 1.0f};
    return mat;
};
inline Mat4 CreateMat4(Quat quat);
const Mat4 Mat4::Zero = {};
const Mat4 Mat4::Identity = CreateMat4(1.0f);
inline Mat4 operator+(Mat4 a, Mat4 b)
{
    Mat4 result;
    result[0] = a[0] + b[0];
    result[1] = a[1] + b[1];
    result[2] = a[2] + b[2];
    result[3] = a[3] + b[3];
    return result;
}
inline Mat4 operator-(Mat4 a, Mat4 b)
{
    Mat4 result;
    result[0] = a[0] - b[0];
    result[1] = a[1] - b[1];
    result[2] = a[2] - b[2];
    result[3] = a[3] - b[3];
    return result;
}
inline Mat4 operator*(Mat4 mat, float val)
{
    Mat4 result;
    result[0] = mat[0] * val;
    result[1] = mat[1] * val;
    result[2] = mat[2] * val;
    result[3] = mat[3] * val;
    return result;
}
inline Mat4 operator/(Mat4 mat, float val)
{
    Mat4 result;
    result[0] = mat[0] / val;
    result[1] = mat[1] / val;
    result[2] = mat[2] / val;
    result[3] = mat[3] / val;
    return result;
}
inline Vec4 operator*(Mat4 a, Vec4 b);
inline Mat4 operator*(Mat4 a, Mat4 b);
#ifdef GMATH_USE_IOSTREAM
std::ostream& operator<<(std::ostream& a, Mat4 b)
{
    for (int i = 0; i < 4; ++i)
    {
        a << "| " << b[0][i] << ", " << b[1][i] << ", " << b[2][i] << ", " << b[3][i] << " |" << std::endl;
    }
    return a;
}
#endif

// Quaternion type, uses SSE if enabled.

struct Quat
{
    union
    {
        float data[4];
        struct
        {
            union
            {
                Vec3 xyz;
                struct {float x, y, z;};
            };
            float w;
        };
#ifdef GMATH_USE_SSE
        __m128 data_sse;
#endif
    };
    const static Quat Zero;
    const static Quat Identity;
};
inline Quat CreateQuat();
inline Quat CreateQuat(float fill);
inline Quat CreateQuat(Vec3 axis, float angle);
inline Quat CreateQuat(float x, float y, float z, float w);
inline Quat CreateQuat(Vec4 vec);
inline Quat CreateQuat(Mat4 mat);
const Quat Quat::Zero = {0.0f, 0.0f, 0.0f, 0.0f};
const Quat Quat::Identity = {0.0f, 0.0f, 0.0f, 1.0f};
inline Quat operator+(Quat a, Quat b)
{
    Quat result;
#ifdef GMATH_USE_SSE
    result.data_sse = _mm_add_ps(a.data_sse, b.data_sse);
#else
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    result.w = a.w + b.w;
#endif
    return result;
}
inline Quat operator-(Quat a, Quat b)
{
    Quat result;
#ifdef GMATH_USE_SSE
    result.data_sse = _mm_sub_ps(a.data_sse, b.data_sse);
#else
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    result.w = a.w - b.w;
#endif
    return result;
}

inline Quat operator*(Quat a, Quat b)
{
    Quat result;
#ifdef GMATH_USE_SSE
    __m128 result_one = _mm_xor_ps(_mm_shuffle_ps(a.data_sse, a.data_sse, _MM_SHUFFLE(0, 0, 0, 0)), _mm_setr_ps(0.0f, -0.0f, 0.0f, -0.0f));
    __m128 result_two = _mm_shuffle_ps(b.data_sse, b.data_sse, _MM_SHUFFLE(0, 1, 2, 3));
    __m128 result_three = _mm_mul_ps(result_two, result_one);
    
    result_one = _mm_xor_ps(_mm_shuffle_ps(a.data_sse, a.data_sse, _MM_SHUFFLE(1, 1, 1, 1)) , _mm_setr_ps(0.0f, 0.0f, -0.0f, -0.0f));
    result_two = _mm_shuffle_ps(b.data_sse, b.data_sse, _MM_SHUFFLE(1, 0, 3, 2));
    result_three = _mm_add_ps(result_three, _mm_mul_ps(result_two, result_one));
    
    result_one = _mm_xor_ps(_mm_shuffle_ps(a.data_sse, a.data_sse, _MM_SHUFFLE(2, 2, 2, 2)), _mm_setr_ps(-0.0f, 0.0f, 0.0f, -0.0f));
    result_two = _mm_shuffle_ps(b.data_sse, b.data_sse, _MM_SHUFFLE(2, 3, 0, 1));
    result_three = _mm_add_ps(result_three, _mm_mul_ps(result_two, result_one));
    
    result_one = _mm_shuffle_ps(a.data_sse, a.data_sse, _MM_SHUFFLE(3, 3, 3, 3));
    result_two = _mm_shuffle_ps(b.data_sse, b.data_sse, _MM_SHUFFLE(3, 2, 1, 0));
    result.data_sse = _mm_add_ps(result_three, _mm_mul_ps(result_two, result_one));
#else
    result.x = (a.x * b.w) + (a.y * b.z) - (a.z * b.y) + (a.w * b.x);
    result.y = (-a.x * b.z) + (a.y * b.w) + (a.z * b.x) + (a.w * b.y);
    result.z = (a.x * b.y) - (a.y * b.x) + (a.z * b.w) + (a.w * b.z);
    result.w = (-a.x * b.x) - (a.y * b.y) - (a.z * b.z) + (a.w * b.w);
#endif
    return result;
}

inline Quat operator*(Quat a, float b)
{
    Quat result;
#ifdef GMATH_USE_SSE
    __m128 scalar = _mm_set1_ps(b);
    result.data_sse = _mm_mul_ps(a.data_sse, scalar);
#else
    result = {a.x * b, a.y * b, a.z * b, a.w * b};
#endif
    return result;
}

inline Quat operator*(float a, Quat b)
{
    Quat result;
#ifdef GMATH_USE_SSE
    __m128 scalar = _mm_set1_ps(a);
    result.data_sse = _mm_mul_ps(scalar, b.data_sse);
#else
    result = {a * b.x, a * b.y, a * b.z, a * b.w};
#endif
    return result;
}
inline Quat operator/(Quat a, float b)
{
    Quat result;
#ifdef GMATH_USE_SSE
    __m128 scalar = _mm_set1_ps(b);
    result.data_sse = _mm_div_ps(a.data_sse, scalar);
#else
    result = {a.x / b, a.y / b, a.z / b, a.w / b};
#endif
    return result;
}

inline Quat operator/(float a, Quat b)
{
    Quat result;
#ifdef GMATH_USE_SSE
    __m128 scalar = _mm_set1_ps(a);
    result.data_sse = _mm_div_ps(scalar, b.data_sse);
#else
    result = {a / b.x, a / b.y, a / b.z, a / b.w};
#endif
    return result;
}


#ifdef GMATH_USE_IOSTREAM
std::ostream& operator<<(std::ostream& a, Quat b)
{
    return a << "(" << b.x << ", " << b.y << ", " << b.z << ", " << b.w << ")";
}
#endif

// Math function wrappers (inline, but enable compiler optimization if you care
// about these being fast).

inline float Sin(float radians) {return GMATH_SIN(radians);}
inline float Cos(float radians) {return GMATH_COS(radians);}
inline float Tan(float radians) {return GMATH_TAN(radians);}
inline float ACos(float cos) {return GMATH_ACOS(cos);}
inline float ATan(float tan) {return GMATH_ATAN(tan);}
inline float Exp(float val) {return GMATH_EXP(val);}
inline float Log(float val) {return GMATH_LOG(val);}
inline float ATan2(float y, float x) {return GMATH_ATAN2(y, x);}
inline float Pow(float val, float exp) {return GMATH_EXP(exp * GMATH_LOG(val));}
inline float Sqrt(float val)
{
#ifdef GMATH_USE_SSE
    __m128 in = _mm_set_ss(val);
    __m128 out = _mm_sqrt_ss(in);
    return _mm_cvtss_f32(out);
#else
    return GMATH_SQRT(val);
#endif
}
inline float RSqrt(float val)
{
#ifdef GMATH_USE_SSE
    __m128 in = _mm_set_ss(val);
    __m128 out = _mm_rsqrt_ss(in);
    return _mm_cvtss_f32(out);
#else
    return 1.0f / GMATH_SQRT(val);
#endif
}

// Utility functions.

inline float Radians(float degrees) {return degrees * (GMATH_PI / 180.0f);}
inline float Degrees(float radians) {return (radians * 180.0f) / GMATH_PI;}
inline float Min(float a, float b) {return GMATH_MIN(a, b);}
inline float Max(float a, float b) {return GMATH_MAX(a, b);}
inline float Abs(float val) {return GMATH_ABS(val);}
inline int Mod(int a, int b) {return GMATH_MOD(a, b);}
inline float Pow(float base, int exponent)
{
    float result = 1.0f;
    float mul = (exponent < 0) ? 1.0f / base : base;
    int x = (exponent < 0) ? -exponent : exponent;
    while (x)
    {
        if (x & 1)
        {
            result *= mul;
        }
        mul *= mul;
        x >>= 1;
    }
    return result;
}

inline float Clamp(float val, float min, float max)
{
    return GMATH_MAX(min, GMATH_MIN(max, val));
}
inline float Lerp(float a, float b, float alpha)
{
    float clamped_alpha = Clamp(alpha, 0.0f, 1.0f);
    return (1.0f - clamped_alpha) * a + clamped_alpha * b;
}
inline int Dot(IVec2 a, IVec2 b)
{
    return a.x * b.x + a.y * b.y;
}
inline int Dot(IVec3 a, IVec3 b)
{
    return a.x * b.x + a.y + b.y + a.z * b.z;
}

inline float Dot(Vec2 a, Vec2 b)
{
    return a.x * b.x + a.y * b.y;
}

inline float Dot(Vec3 a, Vec3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
inline float Dot(Vec4 a, Vec4 b)
{
    float result;
#ifdef GMATH_USE_SSE
    __m128 result_one = _mm_mul_ps(a.data_sse, b.data_sse);
    __m128 result_two = _mm_shuffle_ps(result_one, result_two, _MM_SHUFFLE(2, 3, 0, 1));
    result_one = _mm_add_ps(result_one, result_two);
    result_two = _mm_shuffle_ps(result_one, result_one, _MM_SHUFFLE(0, 1, 2, 3));
    result_one = _mm_add_ps(result_one, result_two);
    _mm_store_ss(&result, result_one);
#else
    result = (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
#endif
    return result;
}

inline Vec3 Cross(Vec3 a, Vec3 b)
{
    return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}

inline int LengthSquared(IVec2 vec)
{
    return Dot(vec, vec);
}

inline int LengthSquared(IVec3 vec)
{
    return Dot(vec, vec);
}

inline float LengthSquared(Vec2 vec)
{
    return Dot(vec, vec);
}

inline float LengthSquared(Vec3 vec)
{
    return Dot(vec, vec);
}

inline float LengthSquared(Vec4 vec)
{
    return Dot(vec, vec);
}

inline float Length(Vec2 vec)
{
    return Sqrt(LengthSquared(vec));
}

inline float Length(Vec3 vec)
{
    return Sqrt(LengthSquared(vec));
}

inline float Length(Vec4 vec)
{
    return Sqrt(LengthSquared(vec));
}

// Each vector type has three normalize functions:
// Normalize returns a zero vector in case of a divide-by-zero.
// SafeNormalize returns a zero vector if length < tolerance.
// FastNormalize uses an inverse square root, and does no divide-by-zero check.

inline Vec2 Normalize(Vec2 vec)
{
    float length = Length(vec);
    return (length == 0.0f) ? Vec2::Zero : vec / length;
}

inline Vec2 SafeNormalize(Vec2 vec, float tolerance = 0.001f)
{
    float length = Length(vec);
    return (length < tolerance) ? Vec2::Zero : vec / length;
}

inline Vec2 FastNormalize(Vec2 vec)
{
    return vec * RSqrt(Dot(vec, vec));
}

inline Vec3 Normalize(Vec3 vec)
{
    float length = Length(vec);
    return (length == 0.0f) ? Vec3::Zero : vec / length;
}

inline Vec3 SafeNormalize(Vec3 vec, float tolerance = 0.001f)
{
    float length = Length(vec);
    return (length < tolerance) ? Vec3::Zero : vec / length;
}

inline Vec3 FastNormalize(Vec3 vec)
{
    return vec * RSqrt(Dot(vec, vec));
}
inline Vec4 Normalize(Vec4 vec)
{
    float length = Length(vec);
    return (length == 0.0f) ? Vec4::Zero : vec / length;
}

inline Vec4 SafeNormalize(Vec4 vec, float tolerance = 0.001f)
{
    float length = Length(vec);
    return (length < tolerance) ? Vec4::Zero : vec / length;
}

inline Vec4 FastNormalize(Vec4 vec)
{
    return vec * RSqrt(Dot(vec, vec));
}

inline Vec2 ClampLength(Vec2 vec, float min, float max)
{
    float length = Length(vec);
    if (length < min || length > max) vec = Normalize(vec) * Clamp(length, min, max);
    return vec;
}

inline Vec3 ClampLength(Vec3 vec, float min, float max)
{
    float length = Length(vec);
    if (length < min || length > max) vec = Normalize(vec) * Clamp(length, min, max);
    return vec;
}

inline Vec4 ClampLength(Vec4 vec, float min, float max)
{
    float length = Length(vec);
    if (length < min || length > max) vec = Normalize(vec) * Clamp(length, min, max);
    return vec;
}

#ifdef GMATH_USE_SSE
inline __m128 LinearCombineSSE(__m128 left, Mat4 right)
{
    __m128 result;
    result = _mm_mul_ps(_mm_shuffle_ps(left, left, 0x00), right.data_sse[0]);
    result = _mm_add_ps(result, _mm_mul_ps(_mm_shuffle_ps(left, left, 0x55), right.data_sse[1]));
    result = _mm_add_ps(result, _mm_mul_ps(_mm_shuffle_ps(left, left, 0xaa), right.data_sse[2]));
    result = _mm_add_ps(result, _mm_mul_ps(_mm_shuffle_ps(left, left, 0xff), right.data_sse[3]));
    return result;
}
#endif

inline Mat4 Transpose(Mat4 mat)
{
#ifdef GMATH_USE_SSE
    _MM_TRANSPOSE4_PS(mat.data_sse[0], mat.data_sse[1], mat.data_sse[2], mat.data_sse[3]);
#else
    Vec4 row1 = {mat.columns[0].x, mat.columns[1].x, mat.columns[2].x, mat.columns[3].x};
    Vec4 row2 = {mat.columns[0].y, mat.columns[1].y, mat.columns[2].y, mat.columns[3].y};
    Vec4 row3 = {mat.columns[0].z, mat.columns[1].z, mat.columns[2].z, mat.columns[3].z};
    Vec4 row4 = {mat.columns[0].w, mat.columns[1].w, mat.columns[2].w, mat.columns[3].w};
    mat[0] = row1;
    mat[1] = row2;
    mat[3] = row3;
    mat[4] = row4;
#endif
    return mat;
}


// Graphics transformations.

inline Mat4 CreateOrthoMatrix(float left, float right, float bottom, float top, float near, float far)
{
    Mat4 result = {};
    result[0][0] = 2.0f / (right - left);
    result[1][1] = 2.0f / (top - bottom);
#ifdef GMATH_DEPTH_ZERO_TO_ONE
    result[2][2] = 1.0f / (near - far);
#else
    result[2][2] = 2.0f / (near - far);
#endif
    result[3][3] = 1.0f;
    result[3][0] = (left + right) / (left - right);
    result[3][1] = (bottom + top) / (bottom - top);
    result[3][2] = near / (near - far);
    return result;
}

inline Mat4 CreateOrthoMatrix(float width, float height, float depth, float near_clip)
{
    return CreateOrthoMatrix(-(width * 0.5f), width * 0.5f, -(height * 0.5f), height * 0.5f, near_clip, near_clip + depth);
}

inline Mat4 CreateOrthoMatrix(Vec3 extent, float near_clip)
{
    return CreateOrthoMatrix(extent.width, extent.height, extent.depth, near_clip);
}

inline Mat4 CreatePerspectiveMatrix(float fov, float aspect, float near, float far)
{
    Mat4 result = {};
    float cotan = 1.0f / Tan(fov * (GMATH_PI / 360.f));
    result[0][0] = cotan / aspect;
    result[1][1] = cotan;
    result[2][3] = -1.0f;
    result[2][2] = far / (near - far);
#ifdef GMATH_DEPTH_ZERO_TO_ONE
    result[3][2] = (near * far) / (near - far);
#else
    result[3][2] = (2.0f * near * far) / (near - far);
#endif
    return result;
}

inline Mat4 CreateTranslationMatrix(Vec3 translation)
{
    Mat4 result = CreateMat4(1.0f);
    result[3] = CreateVec4(translation, 1.0f);
    return result;
}


inline Mat4 CreateScalingMatrix(Vec3 scale)
{
    Mat4 result = {};
    result[0][0] = scale.x;
    result[1][1] = scale.y;
    result[2][2] = scale.z;
    result[3][3] = 1.0f;
    return result;
}

inline Mat4 CreateRotationMatrix(float angle, Vec3 axis);
inline Mat4 CreateLookAtMatrix(Vec3 eye_location, Vec3 target, Vec3 world_up);

// Quaternion math.

float Dot(Quat a, Quat b)
{
    float result;
#ifdef GMATH_USE_SSE
    __m128 result_one = _mm_mul_ps(a.data_sse, b.data_sse);
    __m128 result_two = _mm_shuffle_ps(result_one, result_one, _MM_SHUFFLE(2, 3, 0, 1));
    result_one = _mm_add_ps(result_one, result_two);
    result_two = _mm_shuffle_ps(result_one, result_one, _MM_SHUFFLE(0, 1, 2, 3));
    result_one = _mm_add_ps(result_one, result_two);
    _mm_store_ss(&result, result_one);
#else
    result = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
#endif
    return result;
}

inline Quat Normalize(Quat quat)
{
    float length = Sqrt(Dot(quat, quat));
    return (length == 0.0f) ? Quat::Zero : quat / length;
}

inline Quat Lerp(Quat a, Quat b, float alpha)
{
    Quat result;
#ifdef GMATH_USE_SSE
    float clamped_alpha = Clamp(alpha, 0.0f, 1.0f);
    __m128 a_scalar = _mm_set1_ps(1.0f - clamped_alpha);
    __m128 b_scalar = _mm_set1_ps(clamped_alpha);
    __m128 result_one = _mm_mul_ps(a.data_sse, a_scalar);
    __m128 result_two = _mm_mul_ps(b.data_sse, b_scalar);
    result.data_sse = _mm_add_ps(result_one, result_two);
#else
    result.x = Lerp(a.x, b.x, alpha);
    result.y = Lerp(a.y, b.y, alpha);
    result.z = Lerp(a.z, b.z, alpha);
    result.w = Lerp(a.w, b.w, alpha);
#endif
    return Normalize(result);
}

Quat Slerp(Quat a, Quat b, float alpha);
Quat Invert(Quat quat);

#endif // GMATH_H

#ifdef GMATH_IMPLEMENTATION

// Vec4/Quat constructors.

Vec4 CreateVec4(float fill)
{
    Vec4 vec;
#ifdef GMATH_USE_SSE
    vec.data_sse = _mm_set1_ps(1.0f);
#else
    vec = {x, y, z, w};
    return vec;
#endif
}

Vec4 CreateVec4(Vec3 xyz, float w)
{
    Vec4 vec;
#ifdef GMATH_USE_SSE
    vec.data_sse = _mm_setr_ps(xyz.x, xyz.y, xyz.z, w);
#else
    vec = {xyz,x, xyz.y, xyz.z, w};
    return vec;
#endif
}

Vec4 CreateVec4(float x, float y, float z, float w)
{
    Vec4 vec;
#ifdef GMATH_USE_SSE
    vec.data_sse = _mm_setr_ps(x, y, z, w);
#else
    vec = {x, y, z, w};
    return vec;
#endif
}

Quat CreateQuat(float x, float y, float z, float w)
{
    Quat quat;
#ifdef GMATH_USE_SSE
    quat.data_sse = _mm_setr_ps(x, y, z, w);
#else
    quat = {x, y, z, w};
    return quat;
#endif
}

Quat CreateQuat(Vec4 vec)
{
    Quat quat;
#ifdef GMATH_USE_SSE
    quat.data_sse = vec.data_sse;
#else
    quat = {vec.x, vec.y, vec.z, vec.w};
    return quat;
#endif
}

Quat CreateQuat(float fill)
{
    Quat quat;
#ifdef GMATH_USE_SSE
    quat.data_sse = _mm_set1_ps(fill);
#else
    quat = {fill, fill, fill, fill};
#endif
    return quat;
}

Quat CreateQuat(Vec3 axis, float angle)
{
    Quat result;
    axis = Normalize(axis);
    float sin = Sin(angle / 2.0f);
    result.xyz = axis * sin;
    result.w = Cos(angle / 2.0f);
    return result;
}

Quat Slerp(Quat a, Quat b, float alpha)
{
    float clamped_alpha = Clamp(alpha, 0.0f, 1.0f);
    float angle = ACos(Dot(a, b));
    Quat left = a * Sin((1.0f - clamped_alpha) * angle);
    Quat right = b * Sin(clamped_alpha * angle);
    return (left + right) * (1.0f / Sin(angle));
}

// Type conversions.
IVec2::operator Vec2() const
{
    return {(float)x, (float)y};
}

Vec2::operator IVec2() const
{
    return {(int)x, (int)y};
}

IVec3::operator Vec3() const
{
    return {(float)x, (float)y, (float)z};
}

Vec3::operator IVec3() const
{
    return {(int)x, (int)y, (int)z};
}
// Matrix math.

Mat4 operator*(Mat4 a, Mat4 b)
{
    Mat4 result;
#ifdef GMATH_USE_SSE
    result.data_sse[0] = LinearCombineSSE(b.data_sse[0], a);
    result.data_sse[1] = LinearCombineSSE(b.data_sse[1], a);
    result.data_sse[2] = LinearCombineSSE(b.data_sse[2], a);
    result.data_sse[3] = LinearCombineSSE(b.data_sse[3], a);
#else
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            result[i][j] = a[0][j] * b[i][0] + a[1][j] * b[i][1] + a[2][j] * b[i][2] + a[3][j] * b[i][3];
        }
    }
#endif
    return result;
}

Vec4 operator*(Mat4 mat, Vec4 vec)
{
    Vec4 result;
#ifdef GMATH_USE_SSE
    result.data_sse = LinearCombineSSE(vec.data_sse, mat);
#else
    for (int i = 0; i < 4; ++i)
    {
        result[i] = mat[0][i] * vec[0] + mat[1][i] * vec[1] + mat[2][i] * vec[2] + mat[3][i] * vec[3];
    }
#endif
    return result;
}

Mat4 CreateRotationMatrix(Vec3 axis, float angle)
{
    Mat4 result = {};
    axis = Normalize(axis);
    
    float sin = Sin(Radians(angle));
    float cos = Cos(Radians(angle));
    float cos_value = 1.0f - cos;
    result[0][0] = (axis.x * axis.x * cos_value) + cos;
    result[0][1] = (axis.x * axis.y * cos_value) + (axis.z * sin);
    result[0][2] = (axis.x * axis.z * cos_value) - (axis.y * sin);
    
    result[1][0] = (axis.y * axis.x * cos_value) - (axis.z * sin);;
    result[1][1] = (axis.y * axis.y * cos_value) + cos;
    result[1][2] = (axis.y * axis.z * cos_value) + (axis.x - sin);
    
    result[2][0] = (axis.z * axis.x * cos_value) + (axis.y * sin);
    result[2][1] = (axis.z * axis.y * cos_value) - (axis.x * sin);
    result[2][2] = (axis.z * axis.z * cos_value) + cos;
    result[3][3] = 1.0f;
    return result;
}

Mat4 CreateLookAtMatrix(Vec3 location, Vec3 target, Vec3 world_up)
{
    Mat4 result;
    Vec3 forward = Normalize(target - location);
    Vec3 right = Normalize(Cross(forward, world_up));
    Vec3 up = Cross(right, forward);
    result[0] = {right.x, up.x, -forward.x, 0.0f};
    result[1] = {right.y, up.y, -forward.y, 0.0f};
    result[2] = {right.z, up.z, -forward.z, 0.0f};
    result[3] = {-Dot(right, location), -Dot(up, location), -Dot(forward, location), 1.0f};
    return result;
}

Quat Invert(Quat quat)
{
    return CreateQuat(-quat.x, -quat.y, -quat.z, quat.w) / Dot(quat, quat);
}

Mat4 CreateMat4(Quat quat)
{
    Mat4 mat = {};
    quat = Normalize(quat);
    float xx = quat.x * quat.x;
    float yy = quat.y * quat.y;
    float zz = quat.z * quat.z;
    float xy = quat.x * quat.y;
    float xz = quat.x * quat.z;
    float yz = quat.y * quat.z;
    float wx = quat.w * quat.x;
    float wy = quat.w * quat.y;
    float wz = quat.w * quat.z;
    mat[0][0] = 1.0f - 2.0f * (yy + zz);
    mat[0][1] = 2.0f * (xy + wz);
    mat[0][2] = 2.0f * (xz - wy);
    mat[0][3] = 0.0f;
    
    mat[1][0] = 2.0f * (xy - wz);
    mat[1][1] = 1.0f - 2.0f * (xx + zz);
    mat[1][2] = 2.0f * (yz + wx);
    mat[1][3] = 0.0f;
    
    mat[2][0] = 2.0f * (xz + wy);
    mat[2][1] = 2.0f * (yz - wx);
    mat[2][2] = 1.0f - 2.0f * (xx + yy);
    mat[2][3] = 0.0f;
    
    mat[3][0] = 0.0f;
    mat[3][1] = 0.0f;
    mat[3][2] = 0.0f;
    mat[3][3] = 1.0f;
    return mat;
}

// This method taken from Mike Day at Insomniac Games.
// https://d3cw3dd2w32x2b.cloudfront.net/wp-content/uploads/2015/01/matrix-to-quat.pdf
//
// Note that as mentioned at the top of the paper, the paper assumes the matrix
// would be *post*-multiplied to a vector to rotate it, meaning the matrix is
// the transpose of what we're dealing with. But, because our matrices are
// stored in column-major order, the indices *appear* to match the paper.
Quat CreateQuat(Mat4 mat)
{
    float t;
    Quat q;
    if (mat[2][2] < 0.0f)
    {
        if (mat[0][0] > mat[1][1])
        {
            t = 1.0f + mat[0][0] - mat[1][1] - mat[2][2];
            q = {t, mat[0][1] + mat[1][0], mat[2][0] + mat[0][2], mat[1][2] - mat[2][1]};
        }
        else
        {
            t = 1.0f - mat[0][0] + mat[1][1] - mat[2][2];
            q = {mat[0][1] + mat[1][0], t, mat[1][2] + mat[2][1], mat[2][0] - mat[0][2]};
        }
    }
    else
    {
        if (mat[0][0] < -mat[1][1])
        {
            t = 1.0f - mat[0][0] - mat[1][1] + mat[2][2];
            q = {mat[2][0] + mat[0][2], mat[1][2] + mat[2][1], t, mat[0][1] - mat[1][0]};
        }
        else
        {
            t = 1.0f + mat[0][0] + mat[1][1] + mat[2][2];
            q = {mat[1][2] - mat[2][1], mat[2][0] - mat[0][2], mat[0][1] - mat[1][0], t};
        }
    }
    
    return q * (0.5f / Sqrt(t));
}

#endif /* GMATH_IMPLEMENTATION */
