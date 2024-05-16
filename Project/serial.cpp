#include <iostream>
#include <cmath>
#include <fstream>
#include <random>
#include <chrono>
#include "constants.hpp"

const double PI = 3.14159265358979323846;

const int N = 500;     // Number of particles
const double t = 0.0;      // current time of the simulation
const double lmbda = 2.0 * k * (1.0 + n) * pow(M_PI, -3.0 / (2.0 * n)) * 
        pow((M * tgamma(5.0 / 2.0 + n) / (R * R * R * tgamma(1.0 + n))), 1.0 / n) / (R * R);

void print_array(const double *A, int rows, int cols){
    for (int i = 0; i < rows; ++i){
        std::cout << "[ ";
        for (int j = 0; j < cols; ++j){
            std::cout << A[i * cols + j] << " ";
        }
        std::cout << "]" << std::endl;
    }
}

void compute_W(const double *x, const double *y, const double *z, int M_, int N_, double *W) {
    /*
	Gradient of the Gaussian Smoothing kernel (3D)
	x     is a vector/matrix of x positions
	y     is a vector/matrix of y positions
	z     is a vector/matrix of z positions
	h     is the smoothing length
	wx, wy, wz     is the evaluated gradient
	*/

    for (int i = 0; i < (M_ * N_); i++) {
        double r = sqrt(x[i] * x[i] + y[i] * y[i] + z[i] * z[i]);
        W[i] = (1.0 / (h * sqrt(M_PI))) * (1.0 / (h * sqrt(M_PI))) * (1.0 / (h * sqrt(M_PI))) * exp(-r * r / (h * h));
    }
}

void compute_gradW(const double *x, const double *y, const double *z, int M_, int N_, double *wx, double *wy, double *wz){
    /*
	Gradient of the Gaussian Smoothing kernel (3D)
	x     is a vector/matrix of x positions
	y     is a vector/matrix of y positions
	z     is a vector/matrix of z positions
	h     is the smoothing length
	wx, wy, wz     is the evaluated gradient
	*/
    
    double *n = new double[M_ * N_];
    double *r = new double[M_ * N_];


    for (int i = 0; i < (M_ * N_); i++) {
        r[i] = sqrt(x[i] * x[i] + y[i] * y[i] + z[i] * z[i]);
        n[i] = -2 * exp(-r[i] * r[i] / (h * h)) / pow(h, 5) / pow(M_PI, 1.5);
        wx[i] = n[i] * x[i];
        wy[i] = n[i] * y[i];
        wz[i] = n[i] * z[i];
    }

    delete[] n;
    delete[] r;
}

void getPairwiseSeparations(const double *ri, const double *rj, int M_, int N_, double *dx, double *dy, double *dz) {
    /*
	Get pairwise separations between 2 sets of coordinates
	ri    is an M x 3 matrix of positions
	rj    is an N x 3 matrix of positions
	dx, dy, dz   are M x N matrices of separations
	*/

    for (int i = 0; i < M_; i++) {
        for (int j = 0; j < N_; j++) {
            dx[i * N_ + j] = ri[i * 3] - rj[j * 3];
            dy[i * N_ + j] = ri[i * 3 + 1] - rj[j * 3 + 1];
            dz[i * N_ + j] = ri[i * 3 + 2] - rj[j * 3 + 2];
        }
    }
}

void getDensity(const double *r, const double *pos, int M_, int N_, double *rho) {
    /*
    Get Density at sampling locations from SPH particle distribution
    r     is an M x 3 matrix of sampling locations
    pos   is an N x 3 matrix of SPH particle positions
    m     is the particle mass
    h     is the smoothing length
    rho   is M x 1 vector of densities
    */

    double *dx = new double[M_ * N_];
    double *dy = new double[M_ * N_];
    double *dz = new double[M_ * N_];

    getPairwiseSeparations(r, pos, M_, N_, dx, dy, dz);

    double *W = new double[M_ * N_];
    compute_W(dx, dy, dz, M_, N_, W);


    for (int i = 0; i < M_; ++i) {
        rho[i] = 0.0;
        for (int j = 0; j < N_; ++j) {
            rho[i] += m * W[j + i * N_];
        }
    }

    delete[] dx;
    delete[] dy;
    delete[] dz;
    delete[] W;
}


void getPressure(const double *rho, double *P){
    
    for (int i = 0; i < particles; ++i) {
        P[i] = k * pow(rho[i], 1 + 1 / n);
    }
}

void getAcc(const double *pos, const double *vel, double lmbda, int N_, double *a){
    /*
	Calculate the acceleration on each SPH particle
	pos   is an N x 3 matrix of positions
	vel   is an N x 3 matrix of velocities
	m     is the particle mass
	h     is the smoothing length
	k     equation of state constant
	n     polytropic index
	lmbda external force constant
	nu    viscosity
	a     is N x 3 matrix of accelerations
	*/
    double rho[N_], P[N_];
    getDensity(pos, pos, N_, N_, rho);
    getPressure(rho, P);
    

    double *dx = new double[N_ * N_];
    double *dy = new double[N_ * N_];
    double *dz = new double[N_ * N_];
    getPairwiseSeparations(pos, pos, N_, N_, dx, dy, dz);

    double *gradWx = new double[N_ * N_];
    double *gradWy = new double[N_ * N_];
    double *gradWz = new double[N_ * N_];
    compute_gradW(dx, dy, dz, N_, N_, gradWx, gradWy, gradWz);

    for (int i = 0; i < N_; ++i) 
    {
        a[i * 3] = 0.0;
        a[i * 3 + 1] = 0.0;
        a[i * 3 + 2] = 0.0;

        for (int j = 0; j < N_; ++j) 
        {
            int idx = i * N_ + j; 
            a[i * 3] += -m * (P[j] / (rho[j] * rho[j]) + P[i] / (rho[i] * rho[i])) * gradWx[idx];
            a[i * 3 + 1] += -m * (P[j] / (rho[j] * rho[j]) + P[i] / (rho[i] * rho[i])) * gradWy[idx];
            a[i * 3 + 2] += -m * (P[j] / (rho[j] * rho[j]) + P[i] / (rho[i] * rho[i])) * gradWz[idx];
        }
    }

    delete[] dx;
    delete[] dy;
    delete[] dz;
    delete[] gradWx;
    delete[] gradWy;
    delete[] gradWz;

    for (int i = 0; i < N_; ++i) {
        a[i * 3] -= lmbda * pos[i * 3];
        a[i * 3 + 1] -= lmbda * pos[i * 3 + 1];
        a[i * 3 + 2] -= lmbda * pos[i * 3 + 2];
    }

    for (int i = 0; i < N_; ++i) {
        a[i * 3] -= nu * vel[i * 3];
        a[i * 3 + 1] -= nu * vel[i * 3 + 1];
        a[i * 3 + 2] -= nu * vel[i * 3 + 2];
    }
}

int main() {
    auto start = std::chrono::high_resolution_clock::now();

    // Initialize positions and velocities

    double pos[particles * 3];
    double vel[particles * 3] = {0.0}; // Initialize velocities to zero
    double rr[100*3] = {0.0};

    for (int i = 0; i < 100; i++){
        rr[i * 3] = i/ 100.0;
    }
    
    std::random_device rd;
    std::mt19937 gen(42);            
    std::normal_distribution<> dist(0.0, 1.0);
    srand(0xfacade);

    for (int i = 0; i < particles * 3; ++i) {
        pos[i] = dist(gen);
    }
    
    // Open CSV file for writing
    std::ofstream outFile("output/particle_positions_serial.csv");
    if (!outFile.is_open()) {
        std::cerr << "Error: Unable to open file for writing." << std::endl;
        return 1;
    }
    std::ofstream outFile2("output/density_serial.csv");
    if (!outFile2.is_open()) {
        std::cerr << "Error: Unable to open file for writing." << std::endl;
        return 1;
    }

    
    outFile << "Time,X,Y,Z" << std::endl;
    int time = 0;
    for (int i = 0; i < particles; ++i) {
            outFile << time << "," << pos[i * 3] << "," << pos[i * 3 + 1] << "," << pos[i * 3 + 2] << std::endl;
        }

    int frames = 0;
    // Time evolution loop
    
    for (double time = t+dt; time <= t_end; time += dt) {
        // Compute accelerations
        double acc[particles * 3];
        getAcc(pos, vel, lmbda, particles, acc);
        // Update velocities
        for (int i = 0; i < particles; ++i) {
            vel[i * 3] += acc[i * 3] * dt;
            vel[i * 3 + 1] += acc[i * 3 + 1] * dt;
            vel[i * 3 + 2] += acc[i * 3 + 2] * dt;
        }

        // Update positions
        for (int i = 0; i < particles; ++i) {
            pos[i * 3] += vel[i * 3] * dt;
            pos[i * 3 + 1] += vel[i * 3 + 1] * dt;
            pos[i * 3 + 2] += vel[i * 3 + 2] * dt;
        }

        // Write positions to CSV file
        if(frames++ % 10 == 0)
        for (int i = 0; i < N; ++i) {
            outFile << time << "," << pos[i * 3] << "," << pos[i * 3 + 1] << "," << pos[i * 3 + 2] << std::endl;
        }

        double rho[100];
        getDensity(rr, pos, 100, particles, rho);

        /*for (int i = 0; i < 100; ++i) {
            outFile2 << rho[i] << ",";
        }
        outFile2 << std::endl;*/
    }
    

    outFile.close();
    outFile2.close();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() << " seconds" << std::endl;

    return 0;
}
