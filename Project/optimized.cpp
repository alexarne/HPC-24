/**
 * @file optimized.cpp
 * @author HPC-Group 7
 * @brief Optimized Smoothed-Particle Hydrodynamics simulation of Toy Star
 * 
 */
#include "optimized_core.hpp"

double t;
size_t frame = 0;

/**
 * @brief Simulate a time step in the simulation
 * 
 */
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

/**
 * @brief Write the current positions to the output file
 * 
 * @param out_file The output file
 */
void write_positions(std::ofstream& out_file) {
    for (int i = 0; i < particles; i++)
        out_file << t << "," << points[i].x << "," << points[i].y << "," << points[i].z << "\n";
}

/**
 * @brief Write the current density samples to the output file
 * 
 * @param out_file The output file
 */
void write_density(std::ofstream& out_file) {
    for (int i = 0; i < 99; i++)
        out_file << rhos[i] << ",";
    out_file << rhos[99]<< "\n";
}  

int main(int argc, char* argv[]) {
    auto start = std::chrono::high_resolution_clock::now();
    // Open CSV file for writing
    std::ofstream out_file("output/particle_positions_optimized.csv", std::ios::trunc);
    if (!out_file.is_open()) {
        std::cerr << "Error: Unable to open file for writing." << std::endl;
        return 1;
    }
    out_file << std::scientific<< std::setprecision(15);
    
    out_file << "Time,X,Y,Z" << "\n";

    std::ofstream out_file_rho("output/density_optimized.csv", std::ios::trunc);
    if (!out_file_rho.is_open()) {
        std::cerr << "Error: Unable to open file for writing." << std::endl;
        return 1;
    }

    init_particles();
    
    write_positions(out_file);

    // update mprhogi2 values for acceleration
    for(int i = 0; i < particles; i++)
        calc_pirogi2(i);

    for(int i = 0; i < particles; i++)
        calc_accelleration(i);
    
    int iter = 0;
    int num_iters = round(t_end/dt);
    while(iter < num_iters) {
        step();
        if(frame % skip_frames == 0)
            write_positions(out_file);

        for (int i = 0; i < 100; i++)
            calc_rho(i);

        write_density(out_file_rho);
        iter++;
    }

    out_file.close();
    out_file_rho.close();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() << " seconds" << std::endl;
    return 0;
}
