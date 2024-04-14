from matplotlib import pyplot as plt
import numpy as np

data = np.genfromtxt('avg.csv', delimiter=',')
cases = [1,32,64,128]
data = np.reshape(data, (-1,5,4))

mean_list = np.zeros((data.shape[0],))
std_list = np.zeros((data.shape[0],))
for i in range(data.shape[0]):
    mean = np.mean(data[i][:,0])
    std = np.std(data[i][:,0])
    print(f'Test: {i + 1}. Mean: {mean}. Std dev: {std}')
    mean_list[i] = mean
    std_list[i] = std
plt.plot(cases, mean_list, color = "black")
plt.errorbar(cases, mean_list, yerr = std_list, fmt = "o")
plt.title("Bandwidth (MB/s)")
plt.xlabel("# threads")
plt.savefig("plot.png")
plt.show()
exit()
