import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import numpy as np
import re

cases = [1,2,4,8,16,32,64,128]

def read(study, cases):
    time = []; time_dev = []
    IPC = []; IPC_std = []
    L1_misses = []; L1_misses_std = []
    for case in cases:
        with open(f"../profiling_outputs/{study}_{case}.txt", "r") as file:
            for i, line in enumerate(file):
                if i == 5:
                    tmp = re.split(r'\s+', line)
                    IPC.append(float(tmp[4])); IPC_std.append(float(tmp[10][:-1])*float(tmp[4])*0.01)
                if i == 7:
                    tmp = re.split(r'\s+', line)
                    L1_misses.append(float(tmp[4][:-1])); L1_misses_std.append(float(tmp[11][:-1])*float(tmp[4][:-1])*0.01)
                if i == 14:
                    tmp = re.split(r'\s+', line)
                    time.append(float(tmp[1])); time_dev.append(float(tmp[3]))
        file.close()
    return time, time_dev, IPC, IPC_std, L1_misses, L1_misses_std

def percent_tick(x, pos): #Only used as a tool for plotting
    return f'{x:.0f}%'
            
t1, t1dev, IPC1,IPC1_dev,L1_1,L1_1_dev = read('standard_omp3000', cases)

speedup = t1[0]/np.array(t1)
## Plot execution time
plt.errorbar(cases, t1, yerr=t1dev, fmt='o', capsize=5, color = 'darkgreen', label = 'OpenMP')
ax= plt.gca()
plt.xlabel("Cores")
plt.ylabel("Execution time")
plt.grid()
plt.legend()

plt.figure()

plt.plot(cases, speedup, '.-', color = 'darkgreen', label = 'OpenMP')
ax= plt.gca()
plt.xlabel("Cores")
plt.ylabel("Relative speed-up")
plt.grid()
plt.legend()

plt.show()
