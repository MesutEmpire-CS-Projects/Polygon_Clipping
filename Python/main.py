import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from mpl_toolkits.mplot3d.art3d import Poly3DCollection


# Define cuboid vertices
vertices = np.array([
    [-0.6, -0.6, -0.6],  # 0
    [0.6, -0.6, -0.6],   # 1
    [0.6, 0.6, -0.6],    # 2
    [-0.6, 0.6, -0.6],   # 3
    [-0.6, -0.6, 0.6],   # 4
    [0.6, -0.6, 0.6],    # 5
    [0.6, 0.6, 0.6],     # 6
    [-0.6, 0.6, 0.6]     # 7
])

# Define clipping window (rectangular prism)
xmin, ymin, zmin = -0.15, -0.3, -0.3
xmax, ymax, zmax = 0.15, 0.3, 0.3

# Flag to track whether clipping is enabled
clippingEnabled = False

# Cyrus-Beck algorithm implementation
def cyrus_beck(p1, p2):
    dx = p2[0] - p1[0]
    dy = p2[1] - p1[1]
    dz = p2[2] - p1[2]

    t_enter = 0
    t_exit = 1

    p = [-dx, dx, -dy, dy, -dz, dz]
    q = [p1[0] - xmin, xmax - p1[0], p1[1] - ymin, ymax - p1[1], p1[2] - zmin, zmax - p1[2]]

    for i in range(6):
        if p[i] == 0:
            if q[i] < 0:
                t_enter = -1
                t_exit = -1
                return t_enter, t_exit  # Line is parallel to a boundary but outside
        else:
            t = q[i] / p[i]
            if p[i] < 0:
                t_enter = max(t_enter, t)
            else:
                t_exit = min(t_exit, t)

    if t_enter > t_exit:
        t_enter = -1
        t_exit = -1
        return t_enter, t_exit  # Line is outside the window

    return t_enter, t_exit

# Function to update cuboid vertices based on clipping
def updateCuboid():
    global vertices
    for i in range(len(vertices)):
        x, y, z = vertices[i]

        # Clip against xmin plane
        if x < xmin:
            x = xmin
        # Clip against xmax plane
        elif x > xmax:
            x = xmax

        # Clip against ymin plane
        if y < ymin:
            y = ymin
        # Clip against ymax plane
        elif y > ymax:
            y = ymax

        # Clip against zmin plane
        if z < zmin:
            z = zmin
        # Clip against zmax plane
        elif z > zmax:
            z = zmax

        vertices[i] = [x, y, z]

# Keyboard callback function
def keyboard(event):
    global clippingEnabled
    if event.key == 'c' or event.key == 'C':
        clippingEnabled = not clippingEnabled  # Toggle clipping state
        if clippingEnabled:
            updateCuboid()  # Update cuboid vertices if clipping is enabled
        fig.canvas.draw_idle()  # Trigger redisplay

# Draw cuboid
def drawCuboid():
    ax.quads = []
    # Front face
    ax.add_collection3d(
        Poly3DCollection([vertices[[0, 1, 2, 3]]], facecolors='red', linewidths=1, edgecolors='black'))
    # Back face
    ax.add_collection3d(
        Poly3DCollection([vertices[[4, 5, 6, 7]]], facecolors='green', linewidths=1, edgecolors='black'))
    # Top face
    ax.add_collection3d(
        Poly3DCollection([vertices[[3, 2, 6, 7]]], facecolors='blue', linewidths=1, edgecolors='black'))
    # Bottom face
    ax.add_collection3d(
        Poly3DCollection([vertices[[0, 1, 5, 4]]], facecolors='yellow', linewidths=1, edgecolors='black'))
    # Right face
    ax.add_collection3d(
        Poly3DCollection([vertices[[1, 2, 6, 5]]], facecolors='purple', linewidths=1, edgecolors='black'))
    # Left face
    ax.add_collection3d(
        Poly3DCollection([vertices[[0, 3, 7, 4]]], facecolors='cyan', linewidths=1, edgecolors='black'))

# Display
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

# Draw clipping window as wireframe rectangular prism
ax.plot([xmin, xmax, xmax, xmin, xmin], [ymin, ymin, ymax, ymax, ymin], [zmin, zmin, zmin, zmin, zmin], 'g-')
ax.plot([xmin, xmax, xmax, xmin, xmin], [ymin, ymin, ymax, ymax, ymin], [zmax, zmax, zmax, zmax, zmax], 'g-')
ax.plot([xmin, xmin], [ymin, ymin], [zmin, zmax], 'g-')
ax.plot([xmax, xmax], [ymin, ymin], [zmin, zmax], 'g-')
ax.plot([xmin, xmin], [ymax, ymax], [zmin, zmax], 'g-')
ax.plot([xmax, xmax], [ymax, ymax], [zmin, zmax], 'g-')

# Set axis limits based on cuboid vertices
x_vals, y_vals, z_vals = vertices[:, 0], vertices[:, 1], vertices[:, 2]
ax.set_xlim(min(x_vals), max(x_vals))
ax.set_ylim(min(y_vals), max(y_vals))
ax.set_zlim(min(z_vals), max(z_vals))

# Draw cuboid
drawCuboid()

ax.set_xlabel('X')
ax.set_ylabel('Y')
ax.set_zlabel('Z')

# Register keyboard callback function
fig.canvas.mpl_connect('key_press_event', keyboard)

plt.show()
