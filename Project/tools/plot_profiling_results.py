import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import re

cases = [100, 200, 500, 1000, 1500, 2000]

def read(study, cases):
    IPC = []; IPC_std = []
    L1_misses = []; L1_misses_std = []
    LLC_misses = []; LLC_misses_std = []
    for case in cases:
        with open(f"../profiling_outputs/{study}{case}.txt", "r") as file:
            for i, line in enumerate(file):
                if i == 5:
                    tmp = re.split(r'\s+', line)
                    IPC.append(float(tmp[4])); IPC_std.append(float(tmp[10][:-1])*float(tmp[4])*0.01)
                if i == 7:
                    tmp = re.split(r'\s+', line)
                    L1_misses.append(float(tmp[4][:-1])); L1_misses_std.append(float(tmp[11][:-1])*float(tmp[4][:-1])*0.01)
                if i == 10:
                    tmp = re.split(r'\s+', line)
                    LLC_misses.append(float(tmp[4][:-1])); LLC_misses_std.append(float(tmp[11][:-1])*float(tmp[4][:-1])*0.01)
        file.close()
    return IPC, IPC_std, L1_misses, L1_misses_std, LLC_misses, LLC_misses_std

def percent_tick(x, pos): #Only used as a tool for plotting
    return f'{x:.0f}%'
            
IPC1,IPC1_dev,L1_1,L1_1_dev,LLC1,LLC1_dev = read('serial', cases)
IPC2,IPC2_dev,L1_2,L1_2_dev,LLC2,LLC2_dev = read('standard', cases)

## Plot IPC
plt.errorbar(cases, IPC1, yerr=IPC1_dev, fmt='o', capsize=5, label = 'Direct port')
plt.errorbar(cases, IPC2, yerr=IPC2_dev, fmt='o', capsize=5, label = 'Optim serial')
plt.xlabel("Particles")
plt.ylabel("IPC")
plt.grid()
plt.legend()

plt.figure()


## Plot L1_misses
plt.errorbar(cases, L1_1, yerr=L1_1_dev, fmt='o', capsize=5, label = 'Direct port')
plt.errorbar(cases, L1_2, yerr=L1_2_dev, fmt='o', capsize=5, label = 'Optim serial')
plt.xlabel("Particles")
plt.ylabel("L1 dcache misses")
plt.gca().yaxis.set_major_formatter(ticker.FuncFormatter(percent_tick))
plt.grid()
plt.legend()

plt.figure()


## Plot LLC_misses
plt.errorbar(cases, LLC1, yerr=LLC1_dev, fmt='o', capsize=5, label = 'Direct port')
plt.errorbar(cases, LLC2, yerr=LLC2_dev, fmt='o', capsize=5, label = 'Optim serial')
plt.xlabel("Particles")
plt.ylabel("LLC load misses")
plt.gca().yaxis.set_major_formatter(ticker.FuncFormatter(percent_tick))
plt.grid()
plt.legend()
plt.show()