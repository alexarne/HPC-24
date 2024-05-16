// Discritization Constants
constexpr double h = 0.1;
constexpr double dt= 0.04;
constexpr size_t particles = 2000;

// Simulation Data
constexpr float t_end = 4;
constexpr size_t skip_frames = 10;

// Physiological Constants
constexpr float M = 12;
constexpr float k = 0.1;
constexpr float R = 0.75;
constexpr float nu = 1;
constexpr float n = 1;
constexpr float m = M / particles; 
