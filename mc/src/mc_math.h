#ifndef GAME_MATH_H
#define GAME_MATH_H

union v2 {
    f32 E[2];
    struct {
        f32 x, y;
    };
};

union v2i {
    i32 E[2];
    struct {
        i32 x, y;
    };
};

union v2u {
    u32 E[2];
    struct {
        u32 x, y;
    };
};

union v3 {
    f32 E[3];
    struct {
        f32 x, y, z;
    };
};

union v3i {
    i32 E[3];
    struct {
        i32 x, y, z;
    };
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

///
///
///

inline f32 
MapRange(f32 value, f32 inMin, f32 inMax, f32 outMin, f32 outMax) {
    f32 result;
    
    result = (value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
    
    return result;
}

#endif //GAME_MATH_H
