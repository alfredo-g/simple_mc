#ifndef GAME_MATH_H
#define GAME_MATH_H

union v2 {
    f32 E[2];
    struct {
        f32 x, y;
    };
};

inline v2 
V2(f32 x, f32 y) {
    v2 result;
    
    result.x = x;
    result.y = y;
    
    return result;
}

union v2i {
    i32 E[2];
    struct {
        i32 x, y;
    };
};

inline v2i
V2i(i32 x, i32 y) {
    v2i result;
    
    result.x = x;
    result.y = y;
    
    return result;
}

union v2u {
    u32 E[2];
    struct {
        u32 x, y;
    };
};

inline v2u 
V2u(u32 x, u32 y) {
    v2u result;
    
    result.x = x;
    result.y = y;
    
    return result;
}

union v3 {
    f32 E[3];
    struct {
        f32 x, y, z;
    };
};

inline v3
V3(f32 x, f32 y, f32 z) {
    v3 result;
    
    result.x = x;
    result.y = y;
    result.z = z;
    
    return result;
}

union v3i {
    i32 E[3];
    struct {
        i32 x, y, z;
    };
};

inline v3i
V3i(i32 x, i32 y, i32 z) {
    v3i result;
    
    result.x = x;
    result.y = y;
    result.z = z;
    
    return result;
}

struct m4x4 {
    f32 E[4][4];
};

inline v2i
V2i(v2 a) {
    v2i result;
    
    result.x = (i32)a.x;
    result.y = (i32)a.y;
    
    return result;
}

inline v2u
V2u(v2 a) {
    v2u result;
    
    result.x = (u32)a.x;
    result.y = (u32)a.y;
    
    return result;
}

///
/// NOTE: v2 Operations
///

inline v2u
operator*(u32 a, v2u b) {
    v2u result;
    
    result.x = a*b.x;
    result.y = a*b.y;
    
    return result;
}

inline v2
operator*(f32 a, v2 b) {
    v2 result;
    
    result.x = a*b.x;
    result.y = a*b.y;
    
    return result;
}

inline v2i
operator*(f32 a, v2i b) {
    v2i result;
    
    result.x = a*b.x;
    result.y = a*b.y;
    
    return result;
}

inline v2
operator/(v2 a, v2 b) {
    v2 result;
    
    result.x = a.x / b.x;
    result.y = a.y / b.y;
    
    return result;
}

inline v2
operator/(v2 a, f32 b) {
    v2 result;
    
    result.x = a.x / b;
    result.y = a.y / b;
    
    return result;
}

inline v2i
operator/(v2i a, i32 b) {
    v2i result;
    
    result.x = a.x / b;
    result.y = a.y / b;
    
    return result;
}

inline v2
operator-(v2 a, v2 b) {
    v2 result;
    
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    
    return result;
}

inline v2i
operator-(v2i a, v2i b) {
    v2i result;
    
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    
    return result;
}

inline v2i
operator-(v2i a, i32 b) {
    v2i result;
    
    result.x = a.x - b;
    result.y = a.y - b;
    
    return result;
}

inline v2
operator-(v2 a, f32 b) {
    v2 result;
    
    result.x = a.x - b;
    result.y = a.y - b;
    
    return result;
}

inline v2
operator+(v2 a, v2 b) {
    v2 result;
    
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    
    return result;
}

inline v2
operator+(v2 a, f32 b) {
    v2 result;
    
    result.x = a.x + b;
    result.y = a.y + b;
    
    return result;
}

inline v2i
operator+(v2i a, i32 b) {
    v2i result;
    
    result.x = a.x + b;
    result.y = a.y + b;
    
    return result;
}

///
/// NOTE: v3 Operations
///

inline v3
operator*(f32 a, v3 b) {
    v3 result;
    
    result.x = a*b.x;
    result.y = a*b.y;
    result.z = a*b.z;
    
    return result;
}

inline v3
operator*(v3 b, f32 a) {
    v3 result = a*b;
    return result;
}

inline v3
operator-(v3 a, v3 b) {
    v3 result;
    
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    
    return result;
}

inline v3
operator-(v3 a, f32 b) {
    v3 result;
    
    result.x = a.x - b;
    result.y = a.y - b;
    result.z = a.z - b;
    
    return result;
}

inline v3
operator-=(v3 a, v3 b) {
    v3 result;
    
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    
    return result;
}

inline v3
operator+(v3 a, v3 b) {
    v3 result;
    
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    
    return result;
}

inline v3
operator+(v3 a, f32 b) {
    v3 result;
    
    result.x = a.x + b;
    result.y = a.y + b;
    result.z = a.z + b;
    
    return result;
}

inline v3
operator+=(v3 a, v3 b) {
    v3 result;
    
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    
    return result;
}

inline f32
SquareRoot(f32 a) {
    f32 result = sqrtf(a);
    return result;
}

inline f32
Inner(v3 a, v3 b) {
    f32 result = a.x*b.x + a.y*b.y + a.z*b.z;
    return result;
}

inline f32
LengthSq(v3 a) {
    f32 result = Inner(a, a);
    return result;
}

inline f32
Length(v3 a) {
    f32 result = SquareRoot(LengthSq(a));
    return result;
}

inline v3
Normalize(v3 a) {
    v3 result = a * (1.0f / Length(a));
    return result;
}

inline v3
Cross(v3 a, v3 b) {
    v3 result;
    
    result.x = (a.y*b.z) - (a.z*b.y);
    result.y = (a.z*b.x) - (a.x*b.z);
    result.z = (a.x*b.y) - (a.y*b.x);
    
    return result;
}

#if 1
inline m4x4
LookAt(v3 eye, v3 center, v3 up) {
    v3 f(Normalize(center - eye));
    v3 s(Normalize(Cross(f, up)));
    v3 u(Cross(s, f));
    
    m4x4 result;
    result.E[0][0] =  s.x;
    result.E[1][0] =  s.y;
    result.E[2][0] =  s.z;
    result.E[0][1] =  u.x;
    result.E[1][1] =  u.y;
    result.E[2][1] =  u.z;
    result.E[0][2] = -f.x;
    result.E[1][2] = -f.y;
    result.E[2][2] = -f.z;
    result.E[3][0] = -Inner(s, eye);
    result.E[3][1] = -Inner(u, eye);
    result.E[3][2] =  Inner(f, eye);
    return result;
}

inline m4x4
Perspective(f32 fovY, f32 aspect, f32 zNear, f32 zFar) {
    //assert(abs(aspect - std::numeric_limits<T>::epsilon()) > static_cast<T>(0));
    const f32 tanHalfFovy = tanf(fovY / 2.0f);
    
    m4x4 result;
    result.E[0][0] = 1.0f / (aspect * tanHalfFovy);
    result.E[1][1] = 1.0f / (tanHalfFovy);
    result.E[2][2] = - (zFar + zNear) / (zFar - zNear);
    result.E[2][3] = - 1.0f;
    result.E[3][2] = - (2.0f * zFar * zNear) / (zFar - zNear);
    return result;
    
}
#endif

///
///
///

inline f32 
MapRange(f32 value, f32 inMin, f32 inMax, f32 outMin, f32 outMax) {
    f32 result = (value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
    return result;
}

inline f32
Radians(f32 degrees) {
    f32 result = degrees * 0.01745329251994329576923690768489f;
    return result;
}

#endif //GAME_MATH_H
