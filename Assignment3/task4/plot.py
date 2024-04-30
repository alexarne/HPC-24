from matplotlib import pyplot as plt

cases = [8,16,32,64,128]
parallel_times = []
parallel_reduce_times = []
pi_parallel = []
pi_parallel_reduce = []
for case in cases:
    with open(f'output/parallel_{case}.txt', 'r') as file:
        for i,r in enumerate(file):
            if (i%2)!=0:
                parallel_times.append(float(r.split()[2]))
            else:
                pi_parallel.append(float(r.split()[1]))
    file.close()

for case in cases:
    with open(f'output/parallel_reduce_{case}.txt', 'r') as file:
        for i,r in enumerate(file):
            if (i%2)!=0:
                parallel_reduce_times.append(float(r.split()[2]))
            else:
                pi_parallel_reduce.append(float(r.split()[1]))
    file.close()

plt.figure()
plt.plot(cases, parallel_times, label="MPI")
plt.plot(cases, parallel_reduce_times, label="MPI with reduce")
plt.grid()
plt.xlabel("# of processes")
plt.ylabel("Elapsed time")
plt.legend()
plt.show()

print(f'Estimated pi (MPI): {pi_parallel}')
print(f'\nEstimated pi (MPI reduce): {pi_parallel_reduce}')
