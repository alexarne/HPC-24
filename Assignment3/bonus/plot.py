from matplotlib import pyplot as plt

cases = [16,36,64,144,256]
times = []
frob = []

for case in cases:
    with open(f'output_{case}.txt', 'r') as file:
        for i,r in enumerate(file):
            if i==2:
                times.append(float(r.split()[2]))
            if i==4:
                frob.append(float(r.split()[3]))
    file.close()

plt.plot(cases, times, '.-', color='darkblue')
plt.grid()
plt.xlabel("Processes")
plt.ylabel("Computational time [s]")
plt.show()

print(f'Computation times: {times}')
print(f"Frobenius norm: {frob}")