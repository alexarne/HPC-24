import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import numpy as np
import re

cases = [1,2,4,8,12,16]
means = []
std = []

for case in cases:
    data = np.zeros((10))
    for i in range(10):
        with open(f'../craypat_outputs/craypat_{case}_iter{i+1}.txt', 'r') as file:
            data[i] = float(re.findall( 'Avg Process Time:[\s]+([\d.]+) secs',file.read() )[0])
    means.append(np.mean(data))
    std.append(np.std(data))

plt.errorbar(cases, means, yerr=std, fmt='o', capsize=5, color = 'darkred', label = 'MPI (16 cpus per task)')
ax= plt.gca()
plt.xlabel("Tasks")
plt.ylabel("Execution time")
plt.grid()
plt.legend()
plt.show()