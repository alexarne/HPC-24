/**
 * @file optimized_mpi.cpp
 * @author HPC-Group 7
 * @brief OpenMPI implementation using a tree topology
 * 
 */

#include "optimized_core.hpp"
#include <mpi.h>

#define SYNC_ACCELERATION 0x20
#define SYNC_PIROGI 0x21

#define SHARE_ACCELRATION 0x22
#define SHARE_PIROGI 0x23

#define SEND_REGIONS 0x24

//! struct containing the responsible range and rank of a node. 
struct mpi_node
{
    int rank;
    int range[2] = {0, particles};

    mpi_node() : rank(-1) {}
    mpi_node(int rank) : rank(rank) {};
    int count() {return range[1] - range[0];}
    int& left() {return range[0];}
    int& right() {return range[1];}
};


int size, provided;
mpi_node self_node(0); //!< node of the current process/

std::vector<mpi_node> children;
int parent_rank;

inline bool is_root() { return parent_rank < 0; }
inline bool has_parent() { return parent_rank >= 0; }

/**
 * @brief Prepares and assigns the responsible regions for every process
 * 
 * Each process is will be assigned to compute particles/size particles,
 * and the remaining region is split evenly among its children.
 */
void init_tree_sizes() {
    MPI_Status status;
    if(has_parent())
        MPI_Recv(self_node.range, 2, MPI_INT, parent_rank, SEND_REGIONS, MPI_COMM_WORLD, &status);
    
    if(children.size() == 0) return;

    std::vector<int> partition_points = {self_node.left() + (int)particles / size};
    if(children.size() == 2) 
        partition_points.push_back((partition_points[0] + self_node.right()) / 2);

    partition_points.push_back(self_node.right());

    for(int i = 0; i < children.size(); i++) {
        children[i].left() = partition_points[i];
        children[i].right()= partition_points[i + 1];
    }

    for(auto& child : children)
        MPI_Send(child.range, 2, MPI_INT, child.rank, SEND_REGIONS, MPI_COMM_WORLD);
}

//! Trickle down the P_i / rho_i^2 values to all processes.
void sync_pirogi() {
    MPI_Status status;
    if(has_parent())
        MPI_Recv(pirogi2, particles, MPI_DOUBLE, parent_rank, SYNC_PIROGI, MPI_COMM_WORLD, &status);
    
    for(auto child : children)
        MPI_Send(pirogi2, particles, MPI_DOUBLE, child.rank, SYNC_PIROGI, MPI_COMM_WORLD);
}

//! Trickle down the accelecation values to all processes
void sync_accleraion() {
    const int vec_size = sizeof(vec3) / sizeof(double);
    
    MPI_Status status;
    if(has_parent())
        MPI_Recv(accelerations, particles * vec_size, MPI_DOUBLE, parent_rank, SYNC_ACCELERATION, MPI_COMM_WORLD, &status);

    for(auto target : children)
        MPI_Send(accelerations, particles * vec_size, MPI_DOUBLE, target.rank, SYNC_ACCELERATION, MPI_COMM_WORLD);
}

/**
 * @brief Compute responsible values of pirogi, and distribute the results to all processes.
 * 
 * Specifically, results are computed, trickled up to the root node, 
 * which then sends the results down to all processes using sync_pirogi.
 */
void mpi_pirogi() {
    // region computed by current node
    const int self_right = children.size() ? children[0].range[0] : self_node.range[1];
    const int self_left = self_node.range[0];

    #pragma omp parallel for
    for(int i = self_left; i < self_right; i++)
        calc_pirogi2(i);


    MPI_Request requests[2];
    for(int i = 0; i < children.size(); i++) {
        auto& child = children[i];
        MPI_Irecv(&pirogi2[child.left()], child.count(), MPI_DOUBLE, child.rank, SHARE_PIROGI, MPI_COMM_WORLD, &requests[i]);
    }

    MPI_Status statuses[2];
    MPI_Waitall(children.size(), requests, statuses);


    if(has_parent())
        MPI_Send(&pirogi2[self_node.left()], self_node.count(), MPI_DOUBLE, parent_rank, SHARE_PIROGI, MPI_COMM_WORLD);

    sync_pirogi();
}

/**
 * @brief Compute responsible acceleration values, and distribute the results to all processes.
 * 
 * Specifically, results are computed, trickled up to the root node, 
 * which then sends the results down to all processes using sync_acceleration.
 */
void mpi_acceleration() {
    // region computed by current node
    const int self_right = children.size() ? children[0].range[0] : self_node.range[1];
    const int self_left = self_node.range[0];

    #pragma omp parallel for
    for(int i = self_left; i < self_right; i++)
        calc_accelleration(i);

    
    const int vec_size = sizeof(vec3) / sizeof(double);
    
    MPI_Request requests[2];
    for(int i = 0; i < children.size(); i++) {
        auto& child = children[i];
        MPI_Irecv(&accelerations[child.left()], child.count() * vec_size, MPI_DOUBLE, child.rank, SHARE_ACCELRATION, MPI_COMM_WORLD, &requests[i]);
    }

    MPI_Status statuses[2];
    MPI_Waitall(children.size(), requests, statuses);

    if(has_parent())
        MPI_Send(&accelerations[self_node.left()], self_node.count() * vec_size, MPI_DOUBLE, parent_rank, SHARE_ACCELRATION, MPI_COMM_WORLD);

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

    parent_rank = t_id / 2 - 1;
    
    init_tree_sizes();
    
    if(is_root()) {
        // Open CSV file for writing
        out_file = std::ofstream("output/particle_positions_optimized_mpi.csv", std::ios::trunc);
        if (!out_file.is_open()) {
            std::cerr << "Error: Unable to open file for writing." << std::endl;
            MPI_Finalize();
            
            return 1;
        }
        out_file << std::scientific<< std::setprecision(15);
        out_file << "Time,X,Y,Z" << "\n";
    }


    init_particles();
    write_data();

    // update pirogi2 values for acceleration
    mpi_pirogi();
    mpi_acceleration();

    int iter = 0;
    int num_iters = round(t_end/dt);
    while(iter < num_iters) {
        step();
        
        if(is_root()) {
            #pragma omp parallel for
            for (int i = 0; i < 100; i++)
                calc_rho(i);

            if(frame % skip_frames == 0)
                write_data();
        }
        

        iter++;
    }
    
    MPI_Finalize();
    return 0;
}
