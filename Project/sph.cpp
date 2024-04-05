#include <iostream>
#include <cmath>
#include <fstream>
#include <memory>
#include <random>

const double PI = 3.14159265358979323846;

const int N = 400;     // Number of particles
const double t = 0.0;      // current time of the simulation
const double tEnd = 10.0;     // time at which simulation ends
const double dt = 0.04;   // timestep
const double M   = 12;      // star mass
const double R   = 0.75;   // star radius
const double h  = 0.1;    // smoothing length
const double k  = 0.1;    // equation of state constant
const double n  = 1;      // polytropic index
const double nu = 1;      // damping
const double lmbda = 2.0 * k * (1.0 + n) * pow(PI, -3.0 / (2.0 * n)) * 
        pow((M * tgamma(5.0 / 2.0 + n) / (R * R * R * tgamma(1.0 + n))), 1.0 / n) / (R * R);

const double m = M/N;

void print_array(double *A, int rows, int cols){
    for (int i = 0; i < rows; ++i) 
    {
        std::cout << "[ ";
        for (int j = 0; j < cols; ++j)
        {
            std::cout << A[i * cols + j] << " ";
        }
        std::cout << "]" << std::endl;
    }
}

std::unique_ptr<double[]> compute_W(double *x, double *y, double *z, int M_, int N_) {
    /*
	Gradient of the Gaussian Smoothing kernel (3D)
	x     is a vector/matrix of x positions
	y     is a vector/matrix of y positions
	z     is a vector/matrix of z positions
	h     is the smoothing length
	wx, wy, wz     is the evaluated gradient
	*/
    std::unique_ptr<double[]> W= std::make_unique<double[]>(M_ * N_);
    for (int i = 0; i < (M_ * N_); i++) 
    {
        double r = sqrt(x[i] * x[i] + y[i] * y[i] + z[i] * z[i]);
        W[i] = (1.0 / (h * sqrt(PI))) * (1.0 / (h * sqrt(PI))) * (1.0 / (h * sqrt(PI))) * exp(-r * r / (h * h));
    }
    return W;
}

struct Grad {
    std::unique_ptr<double[]> wx;
    std::unique_ptr<double[]> wy;
    std::unique_ptr<double[]> wz;
};

Grad compute_gradW(double *x, double *y, double *z, int M_, int N_){
    /*
	Gradient of the Gaussian Smoothing kernel (3D)
	x     is a vector/matrix of x positions
	y     is a vector/matrix of y positions
	z     is a vector/matrix of z positions
	h     is the smoothing length
	wx, wy, wz     is the evaluated gradient
	*/

    //Allocate memory 

    std::unique_ptr<double[]> wx = std::make_unique<double[]>(M_ * N_);
    std::unique_ptr<double[]> wy = std::make_unique<double[]>(M_ * N_);
    std::unique_ptr<double[]> wz = std::make_unique<double[]>(M_ * N_);


    //Compute
    double n[M_ * N_];
    double r[M_ * N_];
    for (int i = 0; i < (M_ * N_); i++){
        r[i] = sqrt(x[i] * x[i] + y[i] * y[i] + z[i] * z[i]);
        n[i] = -2 * exp(-r[i] * r[i] / (h * h)) / pow(h, 5) / pow(PI, 1.5);
        wx[i] = n[i] * x[i];          // wx
        wy[i] = n[i] * y[i];         // wy
        wz[i] = n[i] * z[i];        // wz
    }

    Grad gradW = {std::move(wx), std::move(wy), std::move(wz)};

    return gradW;
    
}

struct PairwiseSeparations {
    std::unique_ptr<double[]> dx;
    std::unique_ptr<double[]> dy;
    std::unique_ptr<double[]> dz;
};


PairwiseSeparations getPairwiseSeparations(double *ri, double *rj, int M_, int N_) {
    /*
	Get pairwise separations between 2 sets of coordinates
	ri    is an M x 3 matrix of positions
	rj    is an N x 3 matrix of positions
	dx, dy, dz   are M x N matrices of separations
	*/
    // Allocate memory
    
    // Allocate memory
    std::unique_ptr<double[]> dx = std::make_unique<double[]>(M_ * N_);
    std::unique_ptr<double[]> dy = std::make_unique<double[]>(M_ * N_);
    std::unique_ptr<double[]> dz = std::make_unique<double[]>(M_ * N_);

    
    // Compute pairwise separations: dx = rix - rjx, dy = riy - rjy, dz = riz - rjz
    
    for (int i = 0; i < M_; i++) {
        for (int j = 0; j < N_; j++) {
            dx[i * N_ + j] = ri[i * 3] - rj[j * 3];
            dy[i * N_ + j] = ri[i * 3 + 1] - rj[j * 3 + 1];
            dz[i * N_ + j] = ri[i * 3 + 2] - rj[j * 3 + 2];
        }
    }

    PairwiseSeparations result = {std::move(dx), std::move(dy), std::move(dz)};
    return result;
}


std::unique_ptr<double[]> getDensity(double *r, double *pos, int M_, int N_){
    /*
	Get Density at sampling locations from SPH particle distribution
	r     is an M x 3 matrix of sampling locations
	pos   is an N x 3 matrix of SPH particle positions
	m     is the particle mass
	h     is the smoothing length
	rho   is M x 1 vector of densities
	*/
    
    PairwiseSeparations result = getPairwiseSeparations(r, pos, M_, N_);
    double *dx = result.dx.get();
    double *dy = result.dy.get();
    double *dz = result.dz.get();
    
    std::unique_ptr<double[]> W = compute_W(dx, dy, dz, M_, N_);

    std::unique_ptr<double[]> rho = std::make_unique<double[]>(M_);

    for (int i = 0; i < M_; ++i) {
        rho[i] = 0.0;
        for (int j = 0; j < N; ++j) {
            rho[i] += m * W[j + i * N_];
        }
    }

    return rho;
}

std::unique_ptr<double[]> getPressure(double *rho){

    std::unique_ptr<double[]> P = std::make_unique<double[]>(N);
;

    for (int i = 0; i < N; ++i) {
        P[i] = k * pow(rho[i], 1 + 1 / n);
    }

    return P;
}


std::unique_ptr<double[]> getAcc(double *pos, double *vel, double lmbda, int N_){
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

    //Calculate densities
    
    std::unique_ptr<double[]> rho = getDensity(pos, pos, N_, N_);

    std::unique_ptr<double[]> P = getPressure(rho.get());

    PairwiseSeparations result = getPairwiseSeparations(pos, pos, N_, N_);
    double *dx = result.dx.get();
    double *dy = result.dy.get();
    double *dz = result.dz.get();

    Grad grad = compute_gradW(dx, dy, dz, N_, N_);

     // Add Pressure contribution to accelerations
    std::unique_ptr<double[]> ax = std::make_unique<double[]>(N);
    std::unique_ptr<double[]> ay = std::make_unique<double[]>(N);
    std::unique_ptr<double[]> az = std::make_unique<double[]>(N);


    for (int i = 0; i < N_; ++i) 
    {
        ax[i] = 0.0;
        ay[i] = 0.0;
        az[i] = 0.0;

        for (int j = 0; j < N_; ++j) 
        {
            int idx = i * N_ + j; 
            ax[i] += -m * (P[j] / (rho[j] * rho[j]) + P[i] / (rho[i] * rho[i])) * grad.wx[idx];
            ay[i] += -m * (P[j] / (rho[j] * rho[j]) + P[i] / (rho[i] * rho[i])) * grad.wy[idx];
            az[i] += -m * (P[j] / (rho[j] * rho[j]) + P[i] / (rho[i] * rho[i])) * grad.wz[idx];
        }
    }

    std::unique_ptr<double[]> a = std::make_unique<double[]>(N * 3);
    for (int i = 0; i < N; ++i) {
        a[i * 3] = ax[i];
        a[i * 3 + 1] = ay[i];
        a[i * 3 + 2] = az[i];
    }

    // Add external potential force
    for (int i = 0; i < N; ++i) {
        a[i * 3] -= lmbda * pos[i * 3];
        a[i * 3 + 1] -= lmbda * pos[i * 3 + 1];
        a[i * 3 + 2] -= lmbda * pos[i * 3 + 2];
    }

    // Add viscosity
    for (int i = 0; i < N; ++i) {
        a[i * 3] -= nu * vel[i * 3];
        a[i * 3 + 1] -= nu * vel[i * 3 + 1];
        a[i * 3 + 2] -= nu * vel[i * 3 + 2];
    }

    return a;
}

int main() {

    // Initialize positions and velocities

    double pos[N * 3];
    double vel[N * 3] = {0.0}; // Initialize velocities to zero
    double rr[100*3] = {0.0};

    for (int i = 0; i < 100; i++){
        rr[i * 3] = i/ 100.0;
    }
    
    // Initialize positions with random numbers within a range
    std::random_device rd;
    std::mt19937 gen(42);            
    std::normal_distribution<> dist(0.0, 1.0);

    for (int i = 0; i < N * 3; ++i) {
        pos[i] = dist(gen);
    }
    
    // Open CSV file for writing
    std::ofstream outFile("particle_positions.csv");
    if (!outFile.is_open()) {
        std::cerr << "Error: Unable to open file for writing." << std::endl;
        return 1;
    }
    std::ofstream outFile2("density.csv");
    if (!outFile2.is_open()) {
        std::cerr << "Error: Unable to open file for writing." << std::endl;
        return 1;
    }

    
    outFile << "Time,X,Y,Z" << std::endl;

    // Time evolution loop
    for (double time = t; time <= tEnd; time += dt) {
        // Compute accelerations
        std::unique_ptr<double[]> acc = getAcc(pos, vel, lmbda, N);

        // Update velocities
        for (int i = 0; i < N; ++i) {
            vel[i * 3] += acc[i * 3] * dt;
            vel[i * 3 + 1] += acc[i * 3 + 1] * dt;
            vel[i * 3 + 2] += acc[i * 3 + 2] * dt;
        }

        // Update positions
        for (int i = 0; i < N; ++i) {
            pos[i * 3] += vel[i * 3] * dt;
            pos[i * 3 + 1] += vel[i * 3 + 1] * dt;
            pos[i * 3 + 2] += vel[i * 3 + 2] * dt;
        }

        // Write positions to CSV file
        for (int i = 0; i < N; ++i) {
            outFile << time << "," << pos[i * 3] << "," << pos[i * 3 + 1] << "," << pos[i * 3 + 2] << std::endl;
        }

        std::unique_ptr<double[]> rho = getDensity(rr, pos, 100, N);

        for (int i = 0; i < 100; ++i) {
            outFile2 << rho[i] << ",";
        }
        outFile2 << std::endl;
    }

    outFile.close();
    outFile2.close();

    return 0;
}