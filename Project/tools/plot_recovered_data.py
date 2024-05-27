import matplotlib.pyplot as plt
import numpy as np
"""
Recovered data from previos results from Dardel. 

Settings used:

        // Discritization Constants
        constexpr double h = 0.1;
        constexpr double dt= 0.04;
        constexpr size_t particles = 2000;

        // Simulation Data
        constexpr double t_end = 4;
        constexpr size_t skip_frames = 10;

        // Physiological Constants
        constexpr double M = 12;
        constexpr double k = 0.1;
        constexpr double R = 0.75;
        constexpr double nu = 1;
        constexpr double n = 1;
        constexpr double m = M / particles; 
"""
cases = [1,4,16,64,128]

times_omp = [21.5613,6.16694,2.24482,1.33601,1.18249]
time_serial = 37.7198*np.ones((len(cases)))
time_standard = 21.4925*np.ones((len(cases)))

plt.plot(cases, times_omp, '.-', color ="darkred", label ="OpenMP")
plt.plot(cases, time_serial, color ="darkblue", label ="Direct port")
plt.plot(cases, time_standard, color ="turquoise", label ="Optim serial")
plt.grid()
plt.legend()
plt.ylabel("Execution time")
plt.xlabel("Cores")
plt.show()