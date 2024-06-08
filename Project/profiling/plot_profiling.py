##
# @file plot_profiling.py
# @brief plot craypat results
# Must change label and filename manually
#
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import numpy as np
import re

cases = [1,2,4,8,16, 32, 64, 128]
means = []
std = []

name = 'craypat_single'

for case in cases:
    data = np.zeros((10))
    for i in range(10):
        with open(f'./craypat_outputs/{name}{case}_iter{i+1}.txt', 'r') as file:
            data[i] = float(re.findall( 'Avg Process Time:[\s]+([\d.]+) secs',file.read() )[0])

    means.append(np.mean(data))
    std.append(np.std(data))

plt.errorbar(cases, means, yerr=std, fmt='o', capsize=5, color = 'orange', label = 'MPI (1 cpu per task)')
ax= plt.gca()
plt.xlabel("Tasks")
plt.ylabel("Execution time")
plt.grid()
plt.legend()
plt.figure()

IO_means = []; IO_std = []; MP_means = []; MP_std = []; acc_means = []; acc_std = []
MPI_means = []; MPI_std = []

for case in cases:
    IO_data = np.zeros((10))
    MP_data = np.zeros((10))
    MPI_data = np.zeros((10))
    acc_data = np.zeros((10))
    for i in range(10):
        with open(f'../craypat_outputs/{name}{case}_iter{i+1}.txt', 'r') as file:
            for line in file:
                linelist = [x for x in line.split(" ") if x != '']
                
                if "std::ostream::_M_insert<>\n" in linelist:
                    IO_data[i] = linelist[1][:-1]
                
                if "PTHREAD\n" in linelist:
                    MP_data[i] = linelist[1][:-1]
                
                if "MPI\n" in linelist:
                    MPI_data[i] = linelist[1][:-1]
                
                if "calc_accelleration\n" in linelist:
                    acc_data[i] = linelist[1][:-1]

                if "Observation:" in linelist:
                    break
                
                

    IO_means.append(np.mean(IO_data));IO_std.append(np.std(IO_data))
    MP_means.append(np.mean(MP_data));MP_std.append(np.std(MP_data))
    MPI_means.append(np.mean(MPI_data));MPI_std.append(np.std(MPI_data))
    acc_means.append(np.mean(acc_data));acc_std.append(np.std(acc_data))

def percent_tick(x, pos): #Only used as a tool for plotting
    return f'{x:.0f}%'

plt.errorbar(cases, IO_means, yerr=IO_std, linestyle = '--',fmt='o', capsize=5, color = 'orange', label = 'I/O Operations')
plt.errorbar(cases, MP_means, yerr=MP_std, linestyle = '--',fmt='o', capsize=5, color = 'blue', label = 'Thread management')
plt.errorbar(cases, MPI_means, yerr=MPI_std, linestyle = '--',fmt='o', capsize=5, color = 'turquoise', label = 'MPI management')
plt.errorbar(cases, acc_means, yerr=acc_std, linestyle = '--',fmt='o', capsize=5, color = 'red', label = 'Evaluating acceleration')
ax= plt.gca()
plt.gca().yaxis.set_major_formatter(ticker.FuncFormatter(percent_tick))
plt.xlabel("Processes")
plt.ylabel("Time spent")
plt.grid()
plt.legend()
plt.show()