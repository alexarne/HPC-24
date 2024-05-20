#include <iostream>
#include <fstream>
#include <chrono>
#include <stdint.h>
#include <random>
#include <vector>
#include <mpi.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <tgmath.h>

#include "constants.hpp"


#define SYNC_ACCELERATION 0x20
#define SYNC_PIROGI 0x21

#define SHARE_ACCELRATION 0x22
#define SHARE_PIROGI 0x23

#define SEND_REGIONS 0x24


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

struct mpi_node
{
    int rank;
    int range[2] = {0, particles};

    mpi_node(int rank) : rank(rank) {};
    int count() {return range[1] - range[0];}
    int& left() {return range[0];}
    int& right() {return range[1];}
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

int size, provided;
mpi_node self_node;

std::vector<mpi_node> children;
int parent_rank;

inline bool is_root() { return parent_rank < 0; }
inline bool has_parent() { return parent_rank >= 0; }


// set up responsible regions (1/3 1/3 1/3)
void init_tree_sizes() {
    MPI_Status status;
    if(has_parent())
        MPI_Recv(self_node.range, 2, MPI_INT, parent_rank, SEND_REGIONS, MPI_COMM_WORLD, &status);
    
    if(children.size() == 0) return;

    std::vector<int> partition_points(children.size());
    for(int i = 1; i <= partition_points.size(); i++)
        partition_points[i - 1] = self_node.left() + (i * self_node.count()) / children.size();

    for(int i = 0; i < children.size(); i++) {
        children[i].left() = partition_points[i];
        children[i].right()= partition_points[i + 1];
    }

    for(auto& child : children)
        MPI_Send(child.range, 2, MPI_INT, child.rank, SEND_REGIONS, MPI_COMM_WORLD);
}

// SEND DATA DOWN THE TREE
void sync_pirogi() {
    MPI_Status status;
    if(has_parent())
        MPI_Recv(pirogi2, particles, MPI_DOUBLE, parent_rank, SYNC_PIROGI, MPI_COMM_WORLD, &status);
    
    for(auto child : children)
        MPI_Send(pirogi2, particles, MPI_DOUBLE, child.rank, SYNC_PIROGI, MPI_COMM_WORLD);
}

void sync_accleraion() {
    const int vec_size = sizeof(vec3) / sizeof(double);
    
    MPI_Status status;
    if(has_parent())
        MPI_Recv(accelerations, particles * vec_size, MPI_DOUBLE, parent_rank, SYNC_ACCELERATION, MPI_COMM_WORLD, &status);

    for(auto target : children)
        MPI_Send(accelerations, particles * vec_size, MPI_DOUBLE, target.rank, SYNC_ACCELERATION, MPI_COMM_WORLD);
}

// CALCULATE AND SEND DATA UP THE TREE
void mpi_pirogi() {
    // region computed by current node
    const int self_right = children.size() ? children[0].range[0] : self_node.range[1];
    const int self_left = self_node.range[0];

    #pragma omp parallel for
    for(int i = self_left; i < self_right; i++)
        calc_pirogi2(i);

    if(has_parent())
        MPI_Send(&pirogi2[self_node.left()], self_right - self_left, MPI_DOUBLE, parent_rank, SHARE_PIROGI, MPI_COMM_WORLD);
    
    MPI_Request requests[2];
    for(int i = 0; i < children.size(); i++) {
        auto& child = children[i];
        MPI_Irecv(&pirogi2[child.left()], child.count(), MPI_DOUBLE, child.rank, SHARE_PIROGI, MPI_COMM_WORLD, &requests[i]);
    }

    MPI_Status statuses[2];
    MPI_Waitall(children.size(), requests, statuses);

    sync_pirogi();
}

void mpi_acceleration() {
    // region computed by current node
    const int self_right = children.size() ? children[0].range[0] : self_node.range[1];
    const int self_left = self_node.range[0];

    #pragma omp parallel for
    for(int i = self_left; i < self_right; i++)
        calc_accelleration(i);

    const int vec_size = sizeof(vec3) / sizeof(double);
    if(has_parent())
        MPI_Send(&accelerations[self_node.left()], (self_right - self_left) * vec_size, MPI_DOUBLE, parent_rank, SHARE_ACCELRATION, MPI_COMM_WORLD);
    
    MPI_Request requests[2];
    for(int i = 0; i < children.size(); i++) {
        auto& child = children[i];
        MPI_Irecv(&accelerations[child.left()], child.count() * vec_size, MPI_DOUBLE, child.rank, SHARE_ACCELRATION, MPI_COMM_WORLD, &requests[i]);
    }

    MPI_Status statuses[2];
    MPI_Waitall(children.size(), requests, statuses);

    sync_accleraion();
}

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
    }

    mpi_pirogi();

    mpi_acceleration();

    // second kick
    #pragma omp parallel for
    for(int i = 0; i < particles; i++)
        velocities[i] = velocities[i] + accelerations[i] * (dt / 2);

    t += dt;
    frame++;
}

std::ofstream out_file;
void write_data() {
    if(is_root() == false)
        return;

    for (int i = 0; i < particles; i++)
        out_file << t << "," << points[i].x << "," << points[i].y << "," << points[i].z << "\n";
}

int main(int argc, char* argv[]) {
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &self_node.rank);

    // setup tree topology
    const int t_id = self_node.rank + 1; // 1 indexed binary tree node id
    if(2 * t_id <= size) children.push_back(mpi_node(2 * t_id - 1));
    if(2 * t_id + 1 <= size) children.push_back(mpi_node(2 * t_id));

    parent_rank = (t_id / 2) - 1;
    
    init_tree_sizes();


    
    if(is_root()) {
        // Open CSV file for writing
        out_file = std::ofstream("output/particle_positions_standard_omp.csv", std::ios::trunc);
        if (!out_file.is_open()) {
            std::cerr << "Error: Unable to open file for writing." << std::endl;
            MPI_Finalize();
            
            return 1;
        }
    }



    std::random_device rd;
    std::mt19937 gen(42);            
    std::normal_distribution<> dist(0.0, 1.0);
    srand(0xfacade);
    
    for(int i = 0; i < particles; i++)
        points[i] = {(double)dist(gen), (double)dist(gen), (double)dist(gen)};


    // update mprhogi2 values for acceleration
    mpi_pirogi();
    mpi_acceleration();
    
    while(t < t_end-dt) {
        step();
        
        if(frame % skip_frames == 0)
            write_data();
    }

    MPI_Finalize();
    return 0;
}
