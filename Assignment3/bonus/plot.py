from matplotlib import pyplot as plt

cases = [16,36,64,144,256]
times = []

for case in cases:
    with open(f'output/parallel_{case}.txt', 'r') as file:
        for i,r in enumerate(file):
            if i==0:
                times.append(float(r.split()[2]))
            
    file.close()



print(times)