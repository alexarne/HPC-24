from matplotlib import pyplot as plt
import numpy as np

for n in [1,32,64,128]:
    for i in range(5):
        with open(f"./results/results_{n}_{i+1}.txt", "r") as file:
            averages = []
            for k,l in enumerate(file):
                if k in (26,27,28,29):
                    averages.append(l.split()[1])
        file.close()
        with open("./avg.csv", "a") as file:
            file.writelines(",".join(averages))
            file.writelines("\n")
        file.close()

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
plt.errorbar(cases, mean_list, yerr = std_list, fmt = ".")
plt.title("Bandwidth (MB/s)")
plt.xlabel("# threads")
plt.savefig("plot.png")

for n in ['dynamic', 'guided', 'static']:
    for i in range(5):
        with open(f"./results_{n}128/result_{i+1}.txt", "r") as file:
            averages = []
            for k,l in enumerate(file):
                if k in (26,27,28,29):
                    averages.append(l.split()[1])
        file.close()
        with open("./avg_schedule.csv", "a") as file:
            file.writelines(",".join(averages))
            file.writelines("\n")
        file.close()


data = np.genfromtxt('avg_schedule.csv', delimiter=',')
cases = ['dynamic', 'guided', 'static']
data = np.reshape(data, (-1,5,4))
mean_list = np.zeros((data.shape[0],))
std_list = np.zeros((data.shape[0],))
for i in range(data.shape[0]):
    mean = np.mean(data[i][:,0])
    std = np.std(data[i][:,0])
    print(f'Test: {cases[i]}. Mean: {mean}. Std dev: {std}')
    mean_list[i] = mean
    std_list[i] = std
plt.figure()
plt.plot(cases, mean_list, color = "black")
plt.errorbar(cases, mean_list, yerr = std_list, fmt = ".")
plt.title("Bandwidth (MB/s)")
plt.xlabel("Schedule")
plt.show()
        
exit()
