#include <iostream>
#include <vector>
#include <stdint.h>
#include <tuple>

#include <random>

#define _USE_MATH_DEFINES
#include <math.h>
#include <tgmath.h>


// Discritization Constants
constexpr float h = 0.1;
constexpr float dt= 0.04;
constexpr size_t particles = 400;

// Physiological Constants
constexpr float M = 2;
constexpr float k = 0.1;
constexpr float R = 0.75;
constexpr float nu = 1;
constexpr float n = 1;
constexpr float m = M / particles; 

// pow is not constexpr but it should be evaluated during optimization
const float lambda = 2.f * k * (1 + n) * std::pow(M_PI, -3.f/(2*n)) * std::pow(M * std::tgamma(5.f/2.f + n) / (R * R * R * std::tgamma(1 + n)), 1.f/n) / (R * R);

// Simulation Data
constexpr float t_end = 10;
constexpr float skip_frames = 10;

struct vec3 {
    float x, y, z;
    float r2() const { return x*x + y*y * z*z; }

    vec3 operator+ (const vec3& b) const {
        return {x + b.x, y + b.y, z + b.z};
    }

    vec3 operator- (const vec3& b) const {
        return {x - b.x, y - b.y, z - b.z};
    }

    vec3 operator* (const float s) const {
        return {s * x, s * y, s * z};
    }
};


// m * P_i / rho_i^2                 https://przepisytradycyjne.pl/idealne-ciasto-na-pierogi
float mpirogi2[particles];

vec3 points[particles];
vec3 velocities[particles];
vec3 accelerations[particles];


// Kernel function and gradient
float W(float r2) {
    const float scalar =  1.0f / std::pow((h * std::sqrt(M_PI)), 3);
    return scalar * std::exp(-r2 / (h * h));
}

vec3 gradW(const vec3 &p) {
    const float scalar = -2.f * std::exp(-p.r2() / h*h) * std::pow(M_PI, -3.f/2.f) * std::pow(h, 5);
    return p * scalar;
}



void calc_mpirogi2(const size_t particle_index) {
    float rho = 0;
    auto pos = points[particle_index];

    for(int i = 0; i < particles; i++)
        if(i != particle_index)
            rho += W( (pos - points[i]).r2() );

    rho *= m;

    // P = (k \rho^{1 + n^{-1}})
    // m \frac{P}{\rho^2} = m k \rho ^{-1 + n^{-1}}
    mpirogi2[particle_index] = m * k * std::pow(rho, 1.f/n - 1);
}

void calc_accelleration(const size_t particle_index) {
    vec3 sum = {0, 0, 0};

    const vec3 pos = points[particle_index];
    for(int i = 0; i < particles; i++) 
        if(i != particle_index) {
            const float scalar = mpirogi2[particle_index] + mpirogi2[i];
            sum = sum + gradW(pos - points[particle_index]) * -scalar;
        }

    accelerations[particle_index] = sum - (pos * lambda) - (velocities[particle_index] * nu);
}





float t;

void step() {
    // first kick
    for(int i = 0; i < particles; i++)
        velocities[i] = velocities[i] + accelerations[i] * (dt / 2);

    for(int i = 0; i < particles; i++)
        points[i] = points[i] + velocities[i] * dt;

    // update mprhogi2 values for acceleration
    for(int i = 0; i < particles; i++)
        calc_mpirogi2(i);


    for(int i = 0; i < particles; i++)
        calc_accelleration(i);

    // second kick
    for(int i = 0; i < particles; i++)
        velocities[i] = velocities[i] + accelerations[i] * (dt / 2);

    t += dt;
}




float rand_float() {
    return (float)rand() / (float)RAND_MAX;
}

int main() {
    srand(0xfacade);

    for(int i = 0; i < particles; i++)
        points[i] = {rand_float(), rand_float(), rand_float()};

    
    // update mprhogi2 values for acceleration
    for(int i = 0; i < particles; i++)
        calc_mpirogi2(i);

    for(int i = 0; i < particles; i++)
        calc_accelleration(i);
    

    while(t < t_end)
        step();

    return 0;
}
