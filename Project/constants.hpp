// Discritization Constants
constexpr float h = 0.1;
constexpr float dt= 0.005;
constexpr size_t particles = 1000;

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
