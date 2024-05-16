#include <iostream>
#include <fstream>

#include <stdint.h>
#include <random>

#define _USE_MATH_DEFINES
#include <math.h>
#include <tgmath.h>

#include "vecvec3.hpp"

#include "constants.hpp"

constexpr float ih1 = 1.f / h;
constexpr float ih2 = 1.f / (h * h);
constexpr float ih3 = 1.f / (h * h * h);
constexpr float ih5 = 1.f / (h * h * h * h * h);

// pow is not constexpr but it should be evaluated during optimization
const float lambda = 2.f * k * (1 + n) * std::pow(M_PI, -3.f/(2*n)) * std::pow(M * std::tgamma(5.f/2.f + n) / (R * R * R * std::tgamma(1 + n)), 1.f/n) / (R * R);

// P_i / rho_i^2                 https://przepisytradycyjne.pl/idealne-ciasto-na-pierogi
float pirogi2[particles];

vec3 points[particles];
vec3 velocities[particles];
vec3 accelerations[particles];


// Kernel function and gradient
float W(const vec3 &p) {
    const float scalar =  ih3 * std::pow(std::sqrt(M_PI), -3);
    return scalar * std::exp(-p.r2() * ih2);
}

vec3 gradW(const vec3 &p) {
    const float s1 = -2.f * std::pow(M_PI, -1.5f) * std::pow(h, -5.f);
    const float s2 =  s1 * std::exp(-p.r2() * ih2);
    return p * s2;
}


void calc_pirogi2(const size_t particle_index) {
    float rho = 0;
    auto pos = points[particle_index];

    for(int i = 0; i < particles; i++)
        rho += m * W(pos - points[i]);

    // P = (k \rho^{1 + n^{-1}})
    // \frac{P}{\rho^2} = m k \rho ^{-1 + n^{-1}}
    pirogi2[particle_index] = k * std::pow(rho, -1.f + 1.f/n);
}

void calc_accelleration(const size_t particle_index) {
    vec3 sum = {0, 0, 0};

    const vec3 pos = points[particle_index];
    for(int i = 0; i < particles; i++) 
        if(i != particle_index) {
            const float scalar = pirogi2[particle_index] + pirogi2[i];
            sum = sum + (gradW(pos - points[i]) * scalar);
        }

    accelerations[particle_index] = (sum * -m) + (pos * -lambda) + (velocities[particle_index] * -nu);
}


float t;
size_t frame = 0;
void step() {
    // first kick
    #pragma omp parallel for
    for(int i = 0; i < particles; i++)
        velocities[i] = velocities[i] + accelerations[i] * (dt / 2);

    
    #pragma omp parallel for
    for(int i = 0; i < particles; i++)
        points[i] = points[i] + velocities[i] * dt;

    // update mprhogi2 values for acceleration
    #pragma omp parallel for
    for(int i = 0; i < particles; i++)
        calc_pirogi2(i);

    #pragma omp parallel for
    for(int i = 0; i < particles; i++)
        calc_accelleration(i);

    // second kick
    #pragma omp parallel for
    for(int i = 0; i < particles; i++)
        velocities[i] = velocities[i] + accelerations[i] * (dt / 2);

    t += dt;
    frame++;
}


void write_positions(std::ofstream& out_file) {
    for (int i = 0; i < particles; i++)
        out_file << t << "," << points[i].x << "," << points[i].y << "," << points[i].z << "\n";
}

int main(int argc, char* argv[]) {
    // Open CSV file for writing
    std::ofstream out_file("particle_positions.csv", std::ios::trunc);
    if (!out_file.is_open()) {
        std::cerr << "Error: Unable to open file for writing." << std::endl;
        return 1;
    }
    
    out_file << "Time,X,Y,Z" << "\n";

    std::random_device rd;
    std::mt19937 gen(42);            
    std::normal_distribution<> dist(0.f, 1.f);

    srand(0xfacade);
    for(int i = 0; i < particles; i++)
        points[i] = {(float)dist(gen), (float)dist(gen), (float)dist(gen)};

    write_positions(out_file);

    // update mprhogi2 values for acceleration
    #pragma omp parallel for
    for(int i = 0; i < particles; i++)
        calc_pirogi2(i);

    #pragma omp parallel for
    for(int i = 0; i < particles; i++)
        calc_accelleration(i);
    
    while(t < t_end) {
        step();
        if(frame % skip_frames == 0)
            write_positions(out_file);
    }

    out_file.close();
    return 0;
}
