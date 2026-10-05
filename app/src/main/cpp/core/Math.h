#pragma once

#include <cmath>
#include <cstring>
#include <algorithm>
#include <cstdint>

namespace nexvora {
namespace math {

constexpr float PI       = 3.14159265358979323846f;
constexpr float TWO_PI   = 6.28318530717958647692f;
constexpr float HALF_PI  = 1.57079632679489661923f;
constexpr float DEG2RAD  = PI / 180.0f;
constexpr float RAD2DEG  = 180.0f / PI;
constexpr float EPSILON  = 1e-6f;

inline float clamp(float v, float lo, float hi) {
    return std::max(lo, std::min(hi, v));
}

inline float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

inline float absf(float v) { return v < 0.0f ? -v : v; }

inline bool nearlyEqual(float a, float b, float eps = EPSILON) {
    return absf(a - b) <= eps;
}

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;

    Vec2() = default;
    Vec2(float x_, float y_) : x(x_), y(y_) {}

    Vec2 operator+(const Vec2& o) const { return {x + o.x, y + o.y}; }
    Vec2 operator-(const Vec2& o) const { return {x - o.x, y - o.y}; }
    Vec2 operator*(float s) const { return {x * s, y * s}; }
    Vec2 operator/(float s) const { return {x / s, y / s}; }
    Vec2 operator-() const { return {-x, -y}; }

    Vec2& operator+=(const Vec2& o) { x += o.x; y += o.y; return *this; }
    Vec2& operator-=(const Vec2& o) { x -= o.x; y -= o.y; return *this; }
    Vec2& operator*=(float s) { x *= s; y *= s; return *this; }
    Vec2& operator/=(float s) { x /= s; y /= s; return *this; }

    float length() const { return std::sqrt(x * x + y * y); }
    float lengthSquared() const { return x * x + y * y; }

    Vec2 normalized() const {
        float len = length();
        if (len < EPSILON) return {0.0f, 0.0f};
        return {x / len, y / len};
    }

    void normalize() {
        float len = length();
        if (len >= EPSILON) { x /= len; y /= len; }
    }

    static float dot(const Vec2& a, const Vec2& b) {
        return a.x * b.x + a.y * b.y;
    }

    static float distance(const Vec2& a, const Vec2& b) {
        return (a - b).length();
    }

    static Vec2 lerp(const Vec2& a, const Vec2& b, float t) {
        return {math::lerp(a.x, b.x, t), math::lerp(a.y, b.y, t)};
    }
};

inline Vec2 operator*(float s, const Vec2& v) { return v * s; }

struct Vec3 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    Vec3() = default;
    Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

    Vec3 operator+(const Vec3& o) const { return {x + o.x, y + o.y, z + o.z}; }
    Vec3 operator-(const Vec3& o) const { return {x - o.x, y - o.y, z - o.z}; }
    Vec3 operator*(float s) const { return {x * s, y * s, z * s}; }
    Vec3 operator/(float s) const { return {x / s, y / s, z / s}; }
    Vec3 operator-() const { return {-x, -y, -z}; }

    Vec3& operator+=(const Vec3& o) { x += o.x; y += o.y; z += o.z; return *this; }
    Vec3& operator-=(const Vec3& o) { x -= o.x; y -= o.y; z -= o.z; return *this; }
    Vec3& operator*=(float s) { x *= s; y *= s; z *= s; return *this; }
    Vec3& operator/=(float s) { x /= s; y /= s; z /= s; return *this; }

    float length() const { return std::sqrt(x * x + y * y + z * z); }
    float lengthSquared() const { return x * x + y * y + z * z; }

    Vec3 normalized() const {
        float len = length();
        if (len < EPSILON) return {0.0f, 0.0f, 0.0f};
        return {x / len, y / len, z / len};
    }

    void normalize() {
        float len = length();
        if (len >= EPSILON) { x /= len; y /= len; z /= len; }
    }

    static float dot(const Vec3& a, const Vec3& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    static Vec3 cross(const Vec3& a, const Vec3& b) {
        return {
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        };
    }

    static float distance(const Vec3& a, const Vec3& b) {
        return (a - b).length();
    }

    static Vec3 lerp(const Vec3& a, const Vec3& b, float t) {
        return {
            math::lerp(a.x, b.x, t),
            math::lerp(a.y, b.y, t),
            math::lerp(a.z, b.z, t)
        };
    }
};

inline Vec3 operator*(float s, const Vec3& v) { return v * s; }

struct Vec4 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 1.0f;

    Vec4() = default;
    Vec4(float x_, float y_, float z_, float w_ = 1.0f) : x(x_), y(y_), z(z_), w(w_) {}
    explicit Vec4(const Vec3& v, float w_ = 1.0f) : x(v.x), y(v.y), z(v.z), w(w_) {}

    Vec3 xyz() const { return {x, y, z}; }
};

struct Quat {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 1.0f;

    Quat() = default;
    Quat(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_) {}

    static Quat identity() { return {0.0f, 0.0f, 0.0f, 1.0f}; }

    static Quat fromAxisAngle(const Vec3& axis, float radians) {
        Vec3 n = axis.normalized();
        float half = radians * 0.5f;
        float s = std::sin(half);
        return {n.x * s, n.y * s, n.z * s, std::cos(half)};
    }

    static Quat fromEuler(float pitch, float yaw, float roll) {
        float cy = std::cos(yaw * 0.5f);
        float sy = std::sin(yaw * 0.5f);
        float cp = std::cos(pitch * 0.5f);
        float sp = std::sin(pitch * 0.5f);
        float cr = std::cos(roll * 0.5f);
        float sr = std::sin(roll * 0.5f);

        Quat q;
        q.w = cr * cp * cy + sr * sp * sy;
        q.x = sr * cp * cy - cr * sp * sy;
        q.y = cr * sp * cy + sr * cp * sy;
        q.z = cr * cp * sy - sr * sp * cy;
        return q;
    }

    Quat operator*(const Quat& o) const {
        return {
            w * o.x + x * o.w + y * o.z - z * o.y,
            w * o.y - x * o.z + y * o.w + z * o.x,
            w * o.z + x * o.y - y * o.x + z * o.w,
            w * o.w - x * o.x - y * o.y - z * o.z
        };
    }

    float length() const {
        return std::sqrt(x * x + y * y + z * z + w * w);
    }

    Quat normalized() const {
        float len = length();
        if (len < EPSILON) return identity();
        return {x / len, y / len, z / len, w / len};
    }

    void normalize() {
        float len = length();
        if (len >= EPSILON) {
            x /= len; y /= len; z /= len; w /= len;
        }
    }

    Quat conjugate() const { return {-x, -y, -z, w}; }

    Vec3 rotate(const Vec3& v) const {
        Quat qv{v.x, v.y, v.z, 0.0f};
        Quat r = (*this) * qv * conjugate();
        return {r.x, r.y, r.z};
    }

    static Quat slerp(const Quat& a, const Quat& b, float t) {
        Quat qa = a.normalized();
        Quat qb = b.normalized();
        float d = qa.x * qb.x + qa.y * qb.y + qa.z * qb.z + qa.w * qb.w;

        if (d < 0.0f) {
            qb = {-qb.x, -qb.y, -qb.z, -qb.w};
            d = -d;
        }

        if (d > 0.9995f) {
            Quat r{
                lerp(qa.x, qb.x, t),
                lerp(qa.y, qb.y, t),
                lerp(qa.z, qb.z, t),
                lerp(qa.w, qb.w, t)
            };
            return r.normalized();
        }

        float theta0 = std::acos(clamp(d, -1.0f, 1.0f));
        float theta = theta0 * t;
        float sinTheta = std::sin(theta);
        float sinTheta0 = std::sin(theta0);

        float s0 = std::cos(theta) - d * sinTheta / sinTheta0;
        float s1 = sinTheta / sinTheta0;

        return {
            s0 * qa.x + s1 * qb.x,
            s0 * qa.y + s1 * qb.y,
            s0 * qa.z + s1 * qb.z,
            s0 * qa.w + s1 * qb.w
        };
    }
};

struct Mat4 {
    float m[16];

    Mat4() { identity(); }

    void identity() {
        std::memset(m, 0, sizeof(m));
        m[0] = m[5] = m[10] = m[15] = 1.0f;
    }

    static Mat4 identityMatrix() {
        Mat4 r;
        return r;
    }

    static Mat4 translation(float x, float y, float z) {
        Mat4 r;
        r.m[12] = x;
        r.m[13] = y;
        r.m[14] = z;
        return r;
    }

    static Mat4 translation(const Vec3& t) {
        return translation(t.x, t.y, t.z);
    }

    static Mat4 scale(float x, float y, float z) {
        Mat4 r;
        r.m[0] = x;
        r.m[5] = y;
        r.m[10] = z;
        return r;
    }

    static Mat4 scale(const Vec3& s) {
        return scale(s.x, s.y, s.z);
    }

    static Mat4 rotationX(float radians) {
        Mat4 r;
        float c = std::cos(radians);
        float s = std::sin(radians);
        r.m[5] = c;  r.m[6] = s;
        r.m[9] = -s; r.m[10] = c;
        return r;
    }

    static Mat4 rotationY(float radians) {
        Mat4 r;
        float c = std::cos(radians);
        float s = std::sin(radians);
        r.m[0] = c;  r.m[2] = -s;
        r.m[8] = s;  r.m[10] = c;
        return r;
    }

    static Mat4 rotationZ(float radians) {
        Mat4 r;
        float c = std::cos(radians);
        float s = std::sin(radians);
        r.m[0] = c;  r.m[1] = s;
        r.m[4] = -s; r.m[5] = c;
        return r;
    }

    static Mat4 fromQuaternion(const Quat& q) {
        Quat n = q.normalized();
        float xx = n.x * n.x, yy = n.y * n.y, zz = n.z * n.z;
        float xy = n.x * n.y, xz = n.x * n.z, yz = n.y * n.z;
        float wx = n.w * n.x, wy = n.w * n.y, wz = n.w * n.z;

        Mat4 r;
        r.m[0]  = 1.0f - 2.0f * (yy + zz);
        r.m[1]  = 2.0f * (xy + wz);
        r.m[2]  = 2.0f * (xz - wy);
        r.m[3]  = 0.0f;
        r.m[4]  = 2.0f * (xy - wz);
        r.m[5]  = 1.0f - 2.0f * (xx + zz);
        r.m[6]  = 2.0f * (yz + wx);
        r.m[7]  = 0.0f;
        r.m[8]  = 2.0f * (xz + wy);
        r.m[9]  = 2.0f * (yz - wx);
        r.m[10] = 1.0f - 2.0f * (xx + yy);
        r.m[11] = 0.0f;
        r.m[12] = 0.0f;
        r.m[13] = 0.0f;
        r.m[14] = 0.0f;
        r.m[15] = 1.0f;
        return r;
    }

    static Mat4 TRS(const Vec3& t, const Quat& r, const Vec3& s) {
        return translation(t) * fromQuaternion(r) * scale(s);
    }

    static Mat4 perspective(float fovyRadians, float aspect, float zNear, float zFar) {
        Mat4 r;
        std::memset(r.m, 0, sizeof(r.m));
        float f = 1.0f / std::tan(fovyRadians * 0.5f);
        r.m[0]  = f / aspect;
        r.m[5]  = f;
        r.m[10] = (zFar + zNear) / (zNear - zFar);
        r.m[11] = -1.0f;
        r.m[14] = (2.0f * zFar * zNear) / (zNear - zFar);
        return r;
    }

    static Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
        Vec3 f = (center - eye).normalized();
        Vec3 s = Vec3::cross(f, up).normalized();
        Vec3 u = Vec3::cross(s, f);

        Mat4 r;
        r.m[0] = s.x; r.m[4] = s.y; r.m[8]  = s.z;
        r.m[1] = u.x; r.m[5] = u.y; r.m[9]  = u.z;
        r.m[2] = -f.x; r.m[6] = -f.y; r.m[10] = -f.z;
        r.m[12] = -Vec3::dot(s, eye);
        r.m[13] = -Vec3::dot(u, eye);
        r.m[14] =  Vec3::dot(f, eye);
        r.m[15] = 1.0f;
        return r;
    }

    Mat4 operator*(const Mat4& o) const {
        Mat4 result;
        for (int col = 0; col < 4; ++col) {
            for (int row = 0; row < 4; ++row) {
                result.m[col * 4 + row] =
                    m[0 * 4 + row] * o.m[col * 4 + 0] +
                    m[1 * 4 + row] * o.m[col * 4 + 1] +
                    m[2 * 4 + row] * o.m[col * 4 + 2] +
                    m[3 * 4 + row] * o.m[col * 4 + 3];
            }
        }
        return result;
    }

    Mat4 transposed() const {
        Mat4 r;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                r.m[i * 4 + j] = m[j * 4 + i];
        return r;
    }

    Mat4 inverseAffine() const {
        float r00 = m[0], r01 = m[4], r02 = m[8];
        float r10 = m[1], r11 = m[5], r12 = m[9];
        float r20 = m[2], r21 = m[6], r22 = m[10];
        float tx = m[12], ty = m[13], tz = m[14];

        Mat4 r;
        r.m[0] = r00; r.m[4] = r10; r.m[8]  = r20;
        r.m[1] = r01; r.m[5] = r11; r.m[9]  = r21;
        r.m[2] = r02; r.m[6] = r12; r.m[10] = r22;
        r.m[3] = 0;   r.m[7] = 0;   r.m[11] = 0;
        r.m[15] = 1;

        r.m[12] = -(r00 * tx + r01 * ty + r02 * tz);
        r.m[13] = -(r10 * tx + r11 * ty + r12 * tz);
        r.m[14] = -(r20 * tx + r21 * ty + r22 * tz);
        return r;
    }

    const float* data() const { return m; }
    float* data() { return m; }
};

} // namespace math
} // namespace nexvora
