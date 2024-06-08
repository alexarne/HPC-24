/**
 * @file optimized_core.hpp
 * @author HPC-Group 7
 * @brief Contains core functions used in the optimized implementation of the project.
 * 
 * The contents of this file is used for both the optimized serial version,
 * OpenMP and OpenMPI implementation.
 */

#pragma once

#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip> 
#include <stdint.h>
#include <random>

#define _USE_MATH_DEFINES
#include <math.h>
#include <tgmath.h>

#include "constants.hpp"
#include "vec3.hpp"

constexpr double ih1 = 1.0 / h;
constexpr double ih2 = 1.0 / (h * h);
constexpr double ih3 = 1.0 / (h * h * h);
constexpr double ih5 = 1.0 / (h * h * h * h * h);

// pow is not constexpr but it should be evaluated during optimization
const double lambda = 2.0 * k * (1 + n) * std::pow(M_PI, -3.0/(2*n)) * std::pow(M * std::tgamma(5.0/2.0 + n) / (R * R * R * std::tgamma(1 + n)), 1.0/n) / (R * R);


/**
 * @brief Contains the values of P_i / rho_i^2
 * 
 * When read out quickly, it sounds a bit like pirogi 2. Hence the name.
 * https://przepisytradycyjne.pl/idealne-ciasto-na-pierogi 
 */
double pirogi2[particles];

/**Stores values of density for radial rr points*/ 
double rhos[100];
vec3 rr[100];

vec3 points[particles];             //!< particle positions
vec3 velocities[particles];         //!< particle velocities
vec3 accelerations[particles];      //!< particle accelerations


/**
 * @brief Kernel function evaluated at p
 * 
 * @param p
 * @return * double 
 */
double W(const vec3 &p) {
    const double scalar =  ih3 * std::pow(std::sqrt(M_PI), -3);
    return scalar * std::exp(-p.r2() * ih2);
}

/**
 * @brief Kernel function gradient at p
 * 
 * @param p 
 * @return vec3 
 */
vec3 gradW(const vec3 &p) {
    const double s1 = -2.0 * std::pow(M_PI, -1.5f) * std::pow(h, -5.0);
    const double s2 =  s1 * std::exp(-p.r2() * ih2);
    return p * s2;
}


/**
 * @brief Computes density of particle at rr[index], result is stored in rhos[index]
 * 
 * @param index 
 */
void calc_rho(const size_t index) {
    double rho = 0;
    auto pos = rr[index];

    for(int i = 0; i < particles; i++)
        rho += m* W(pos - points[i]);
    
    rhos[index] = rho;
}


/**
 * @brief Computes and stores the \frac{P}{\rho^2} value of a particle. 
 * 
 * @param particle_index 
 */
void calc_pirogi2(const size_t particle_index) {
    double rho = 0;
    auto pos = points[particle_index];

    for(int i = 0; i < particles; i++)
        rho += m * W(pos - points[i]);

    // P = (k \rho^{1 + n^{-1}}) \implies
    // \frac{P}{\rho^2} = k \rho ^{-1 + n^{-1}}
    pirogi2[particle_index] = k * std::pow(rho, -1.0 + 1.0/n);
}

/**
 * @brief Computes and stores the acceleration of a particle. 
 * 
 * @param particle_index 
 */
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

void init_particles() {
    std::random_device rd;
    std::mt19937 gen(42);            
    std::normal_distribution<> dist(0.0, 1.0);
    srand(0xfacade);

    for(int i = 0; i < particles; i++)
        points[i] = {(double)dist(gen), (double)dist(gen), (double)dist(gen)};
    
    for (int i = 0; i < 100; i++){
        rr[i].x = i/ 100.0;
    }
}