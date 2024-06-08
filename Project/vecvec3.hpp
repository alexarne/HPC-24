/**
 * @file vecvec3.hpp
 * @author HPC-Group 7
 * @brief SSE version of vec3.
 * 
 * Not faster in practice, this is due to increased memory usage, and that 
 * modern compilers (gcc12+) auto vectorization gives better results.
 */

#pragma once
#include <immintrin.h>
struct __attribute__((aligned(16))) vec3 {     
    vec3() : vval( _mm_setzero_ps() ) { }
    vec3(float x, float y, float z) : vval(_mm_set_ps(0.f, z, x, y)) { }
    vec3(__m128 a) : vval(a) {};

    float r2() const { 
        return _mm_cvtss_f32(_mm_dp_ps(vval, vval, 0x71));
    }

    vec3 operator+ (const vec3& b) const {
        return _mm_add_ps(vval, b.vval);
    }

    vec3 operator- (const vec3& b) const {
        return _mm_sub_ps(vval, b.vval);
    }

    vec3 operator* (const float s) const {
        return _mm_mul_ps(vval, _mm_set1_ps(s));
    }

    union 
    {
        struct {float x, y, z; };
        __m128 vval;
    };
};