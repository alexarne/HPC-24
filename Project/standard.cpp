#include <iostream>
#include <fstream>
#include <chrono>
#include <stdint.h>
#include <random>

#define _USE_MATH_DEFINES
#include <math.h>
#include <tgmath.h>


#include "constants.hpp"

constexpr double ih1 = 1.0 / h;
constexpr double ih2 = 1.0 / (h * h);
constexpr double ih3 = 1.0 / (h * h * h);
constexpr double ih5 = 1.0 / (h * h * h * h * h);

// pow is not constexpr but it should be evaluated during optimization
const double lambda = 2.0 * k * (1 + n) * std::pow(M_PI, -3.0/(2*n)) * std::pow(M * std::tgamma(5.0/2.0 + n) / (R * R * R * std::tgamma(1 + n)), 1.0/n) / (R * R);

struct vec3 {
    double x, y, z;
    double r2() const { return x*x + y*y + z*z; }

    vec3 operator+ (const vec3& b) const {
        return {x + b.x, y + b.y, z + b.z};
    } 

    vec3 operator- (const vec3& b) const {
        return {x - b.x, y - b.y, z - b.z};
    }

    vec3 operator* (const double s) const {
        return {s * x, s * y, s * z};
    }
};

// P_i / rho_i^2                 https://przepisytradycyjne.pl/idealne-ciasto-na-pierogi
double pirogi2[particles];

vec3 points[particles];
vec3 velocities[particles];
vec3 accelerations[particles];


// Kernel function and gradient
double W(const vec3 &p) {
    const double scalar =  ih3 * std::pow(std::sqrt(M_PI), -3);
    return scalar * std::exp(-p.r2() * ih2);
}

vec3 gradW(const vec3 &p) {
    const double s1 = -2.0 * std::pow(M_PI, -1.5f) * std::pow(h, -5.0);
    const double s2 =  s1 * std::exp(-p.r2() * ih2);
    return p * s2;
}


void calc_pirogi2(const size_t particle_index) {
    double rho = 0;
    auto pos = points[particle_index];

    for(int i = 0; i < particles; i++)
        rho += m * W(pos - points[i]);

    // P = (k \rho^{1 + n^{-1}})
    // \frac{P}{\rho^2} = m k \rho ^{-1 + n^{-1}}
    pirogi2[particle_index] = k * std::pow(rho, -1.0 + 1.0/n);
}

void calc_accelleration(const size_t particle_index) {
    vec3 sum = {0, 0, 0};

    const vec3 pos = points[particle_index];
    for(int i = 0; i < particles; i++) 
        if(i != particle_index) {
            const double scalar = pirogi2[particle_index] + pirogi2[i];
            sum = sum + (gradW(pos - points[i]) * scalar);
        }

    accelerations[particle_index] = (sum * -m) + (pos * -lambda) + (velocities[particle_index] * -nu);
}


double t;
size_t frame = 0;
void step() {
    // first kick
    for(int i = 0; i < particles; i++)
        velocities[i] = velocities[i] + accelerations[i] * (dt / 2);

    for(int i = 0; i < particles; i++)
        points[i] = points[i] + velocities[i] * dt;

    // update mprhogi2 values for acceleration
    for(int i = 0; i < particles; i++)
        calc_pirogi2(i);


    for(int i = 0; i < particles; i++)
        calc_accelleration(i);

    // second kick
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
    auto start = std::chrono::high_resolution_clock::now();
    // Open CSV file for writing
    std::ofstream out_file("output/particle_positions_standard.csv", std::ios::trunc);
    if (!out_file.is_open()) {
        std::cerr << "Error: Unable to open file for writing." << std::endl;
        return 1;
    }
    out_file << std::fixed << std::setprecision(15);
    
    out_file << "Time,X,Y,Z" << "\n";

    std::random_device rd;
    std::mt19937 gen(42);            
    std::normal_distribution<> dist(0.0, 1.0);
    srand(0xfacade);

    for(int i = 0; i < particles; i++)
        points[i] = {(double)dist(gen), (double)dist(gen), (double)dist(gen)};

    write_positions(out_file);

    // update mprhogi2 values for acceleration
    for(int i = 0; i < particles; i++)
        calc_pirogi2(i);

    for(int i = 0; i < particles; i++)
        calc_accelleration(i);
    
    while(t < t_end) {
        step();
        if(frame % skip_frames == 0)
            write_positions(out_file);
    }

    out_file.close();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() << " seconds" << std::endl;
    return 0;
}
