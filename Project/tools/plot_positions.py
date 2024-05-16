import numpy as np
from matplotlib import pyplot as plt

fig = plt.figure(figsize=(4,5), dpi=80)
grid = plt.GridSpec(3, 1, wspace=0.0, hspace=0.3)
ax1 = plt.subplot(grid[0:2,0])
ax2 = plt.subplot(grid[2,0])
rr = np.zeros((100,3))
rlin = np.linspace(0,1,100)
rr[:,0] =rlin

pos = np.genfromtxt(f"particle_positions.csv", delimiter=',', skip_header=True)
rhos = np.genfromtxt(f"density.csv", delimiter=','); rhos = rhos[:,:-1]

#Determine N:
ts = pos[:,0]
N = len(ts[ts==ts[0]])

for frame in range(int(len(ts)/N)):
    plt.sca(ax1)
    plt.cla()
    plt.scatter(pos[N*frame-N:frame*N,1],pos[N*frame-N:frame*N,2], cmap=plt.cm.autumn, s=10, alpha=0.5)
    ax1.set(xlim=(-1.4, 1.4), ylim=(-1.2, 1.2))
    ax1.set_aspect('equal', 'box')
    ax1.set_xticks([-1,0,1])
    ax1.set_yticks([-1,0,1])
    ax1.set_facecolor('black')
    ax1.set_facecolor((.1,.1,.1))

    plt.sca(ax2)
    plt.cla()
    ax2.set(xlim=(0, 1), ylim=(0, 3))
    ax2.set_aspect(0.1)
    plt.plot(rlin, rhos[frame,:], color='blue')

    plt.pause(0.01)
