import numpy as np
from matplotlib import pyplot as plt


fig = plt.figure(figsize=(20, 10), dpi=80)
grid = plt.GridSpec(3, 2, wspace=0.2, hspace=0.3) 

# Assign subplots to respective grid positions
ax1 = plt.subplot(grid[0:2, 0])  # ax1 spans the first two rows, first column
ax2 = plt.subplot(grid[2, 0])    # ax2 is in the third row, first column
ax3 = plt.subplot(grid[0:2, 1])  # ax3 spans the first two rows, second column
ax4 = plt.subplot(grid[2,1])
# Initialize data
rr = np.zeros((100, 3))
rlin = np.linspace(0, 1, 100)
rr[:, 0] = rlin

# Load data from CSV files
pos = np.genfromtxt(f"../output/particle_positions_serial.csv", delimiter=',', skip_header=True)
pos_optimized = np.genfromtxt(f"../output/particle_positions_optimized.csv", delimiter=',', skip_header=True)
rhos = np.genfromtxt(f"../output/density_serial.csv", delimiter=',')
rhos_optimized = np.genfromtxt(f"../output/density_optimized.csv", delimiter=',')
# Determine N
ts = pos[:, 0]
N = len(ts[ts == ts[0]])

# Loop to update plots
for frame in range(int(len(ts) / N)):
    # Update ax1
    plt.sca(ax1)
    plt.cla()
    plt.scatter(pos[N*frame-N:frame*N, 1], pos[N*frame-N:frame*N, 2], cmap=plt.cm.autumn, s=10, alpha=0.5)
    ax1.set(xlim=(-1.4, 1.4), ylim=(-1.2, 1.2))
    ax1.set_aspect('equal', 'box')
    ax1.set_xticks([-1, 0, 1])
    ax1.set_yticks([-1, 0, 1])
    ax1.set_facecolor('black')
    ax1.set_facecolor((.1, .1, .1))
    ax1.set_title("Serial")

    # Update ax3
    plt.sca(ax3)
    plt.cla()
    plt.scatter(pos_optimized[N*frame-N:frame*N, 1], pos_optimized[N*frame-N:frame*N, 2], cmap=plt.cm.autumn, s=10, alpha=0.5)
    ax3.set(xlim=(-1.4, 1.4), ylim=(-1.2, 1.2))
    ax3.set_aspect('equal', 'box')
    ax3.set_xticks([-1, 0, 1])
    ax3.set_yticks([-1, 0, 1])
    ax3.set_facecolor('black')
    ax3.set_facecolor((.1, .1, .1))
    ax3.set_title("Standard")

    # Update ax2
    plt.sca(ax2)
    plt.cla()
    ax2.set(xlim=(0, 1))#, ylim=(0, 3))
    #ax2.set_aspect(0.1)
    plt.plot(rlin, rhos[frame-1, :], color='blue')

    # Update ax4
    plt.sca(ax4)
    plt.cla()
    ax4.set(xlim=(0, 1))#, ylim=(0, 3))
    #ax4.set_aspect(0.1)
    plt.plot(rlin, rhos_optimized[frame-1, :], color='blue')

    # Pause to create animation effect
    plt.pause(0.01)

