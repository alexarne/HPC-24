/**
 * @file optimized_omp.cpp
 * @author HPC-Group 7
 * @brief OpenMP implementation of optimized.
 * 
 */

#include "optimized_core.hpp"

double t;
size_t frame = 0;
void step() {
    // first kick
    #pragma omp parallel
    {
        #pragma omp for
        for(int i = 0; i < particles; i++)
            velocities[i] = velocities[i] + accelerations[i] * (dt / 2);

        
        #pragma omp for
        for(int i = 0; i < particles; i++)
            points[i] = points[i] + velocities[i] * dt;

        // update mprhogi2 values for acceleration
        #pragma omp for
        for(int i = 0; i < particles; i++)
            calc_pirogi2(i);

        #pragma omp for
        for(int i = 0; i < particles; i++)
            calc_accelleration(i);

        // second kick
        #pragma omp for
        for(int i = 0; i < particles; i++)
            velocities[i] = velocities[i] + accelerations[i] * (dt / 2);
    }

    t += dt;
    frame++;
}


void write_positions(std::ofstream& out_file) {
    for (int i = 0; i < particles; i++)
        out_file << t << "," << points[i].x << "," << points[i].y << "," << points[i].z << "\n";
}

void write_density(std::ofstream& out_file) {
    for (int i = 0; i < 99; i++)
        out_file << rhos[i] << ",";
    out_file << rhos[99]<< "\n";
}  

int main(int argc, char* argv[]) {
    auto start = std::chrono::high_resolution_clock::now();
    // Open CSV file for writing
    std::ofstream out_file("output/particle_positions_optimized_omp.csv", std::ios::trunc);
    if (!out_file.is_open()) {
        std::cerr << "Error: Unable to open file for writing." << std::endl;
        return 1;
    }

    out_file << std::scientific<< std::setprecision(15);
    out_file << "Time,X,Y,Z" << "\n";

    std::ofstream out_file_rho("output/density_optimized_omp.csv", std::ios::trunc);
    if (!out_file_rho.is_open()) {
        std::cerr << "Error: Unable to open file for writing." << std::endl;
        return 1;
    }

    init_particles();

    write_positions(out_file);

    // update mprhogi2 values for acceleration
    #pragma omp parallel
    {
        #pragma omp for
        for(int i = 0; i < particles; i++)
            calc_pirogi2(i);

        #pragma omp for
        for(int i = 0; i < particles; i++)
            calc_accelleration(i);
    }

    int iter = 0;
    int num_iters = round(t_end/dt);
    while(iter < num_iters) {
        step();
        if(frame % skip_frames == 0)
            write_positions(out_file);

        #pragma omp parallel for
        for (int i = 0; i < 100; i++)
            calc_rho(i);

        write_density(out_file_rho);

        iter++;
    }

    out_file.close();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() << " seconds" << std::endl;
    return 0;
}
