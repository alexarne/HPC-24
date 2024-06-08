/**
 * @file vec3.hpp
 * @author HPC-Group 7
 * @brief \copybrief vec3
 * 
 */

/**
 * @brief minimalistic 3d-vector data structure 
 *
 * Contains the needed operations for the project. 
 */
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
