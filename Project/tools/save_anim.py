##
# @file save_anim.py
# @brief creates ./animation.gif/ from particle outputs found in ../output/
#
import numpy as np
from matplotlib import pyplot as plt
from PIL import Image
import io

# Create a figure with only the plot area
fig, ax = plt.subplots(figsize=(30, 15), dpi=80, facecolor='white')
fig.subplots_adjust(left=0, right=1, top=1, bottom=0)  # Remove margins

# Initialize data
rr = np.zeros((100, 3))
rlin = np.linspace(0, 1, 100)
rr[:, 0] = rlin

# Load data from CSV files
pos = np.genfromtxt(f"../output/particle_positions_optimized.csv", delimiter=',', skip_header=True)
rhos = np.genfromtxt(f"../output/density_optimized.csv", delimiter=',')

# Determine N
ts = pos[:, 0]
N = len(ts[ts == ts[0]])

# Initialize a list to store frames
frames = []

# Loop to update plots
for frame in range(int(len(ts) / N)):
    # Update plot
    ax.clear()
    ax.scatter(pos[N*frame-N:frame*N, 1], pos[N*frame-N:frame*N, 2], cmap=plt.cm.autumn, s=5, alpha=0.5)
    ax.set(xlim=(-3, 3), ylim=(-3, 3))
    ax.set_aspect('equal', 'box')
    ax.axis('off')  # Hide axes

    # Capture the current figure as an image and append it to the list of frames
    buf = io.BytesIO()
    plt.savefig(buf, format='png', bbox_inches='tight', pad_inches=0)
    buf.seek(0)
    frame_img = Image.open(buf)
    frames.append(frame_img)

    # Pause to create animation effect
    plt.pause(0.015)

# Save frames as a GIF
frames[0].save('animation.gif', format='GIF', append_images=frames[1:], save_all=True, duration=100, loop=0)

