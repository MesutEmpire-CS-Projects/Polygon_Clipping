import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

# Global variables for cube translation
translateX = 0.0
translateY = 0.0
translateZ = 0.0

# Define viewport dimensions
viewport_min = -1.0
viewport_max = 1.0

# Define the viewport plane (for Cyrus-Beck algorithm)
viewport_normal = np.array([0, 0, 1])  # Normal vector of the viewport plane
viewport_point = np.array([0, 0, 0])   # Point on the viewport plane

def clip_line(p1, p2):
    # Cyrus-Beck line clipping algorithm for 3D lines
    def compute_t(p1, p2):
        return (np.dot(viewport_normal, viewport_point - p1)) / (np.dot(viewport_normal, p2 - p1))

    t_enter = 0.0
    t_exit = 1.0

    t = compute_t(p1, p2)
    if t is not None:
        if np.dot(viewport_normal, p2 - p1) < 0:
            t_enter = max(t_enter, t)
        else:
            t_exit = min(t_exit, t)

    t = compute_t(p2, p1)
    if t is not None:
        if np.dot(viewport_normal, p1 - p2) < 0:
            t_enter = max(t_enter, t)
        else:
            t_exit = min(t_exit, t)

    if t_enter < t_exit:
        clipped_p1 = p1 + (p2 - p1) * t_enter
        clipped_p2 = p1 + (p2 - p1) * t_exit
        return clipped_p1, clipped_p2
    else:
        return None, None

def draw_cube(ax, center=(0,0,0), size=1):
    
    # Define cube vertices
    vertices = np.array([[center[0] - size/2, center[1] - size/2, center[2] - size/2],
                         [center[0] + size/2, center[1] - size/2, center[2] - size/2],
                         [center[0] + size/2, center[1] + size/2, center[2] - size/2],
                         [center[0] - size/2, center[1] + size/2, center[2] - size/2],
                         [center[0] - size/2, center[1] - size/2, center[2] + size/2],
                         [center[0] + size/2, center[1] - size/2, center[2] + size/2],
                         [center[0] + size/2, center[1] + size/2, center[2] + size/2],
                         [center[0] - size/2, center[1] + size/2, center[2] + size/2]])

    # Add translation
    vertices += np.array([translateX, translateY, translateZ])

    # Define cube edges
    edges = [[vertices[0], vertices[1]], [vertices[1], vertices[2]], [vertices[2], vertices[3]], [vertices[3], vertices[0]],
             [vertices[4], vertices[5]], [vertices[5], vertices[6]], [vertices[6], vertices[7]], [vertices[7], vertices[4]],
             [vertices[0], vertices[4]], [vertices[1], vertices[5]], [vertices[2], vertices[6]], [vertices[3], vertices[7]]]

    # Plot clipped cube edges
    for edge in edges:
        p1, p2 = clip_line(edge[0], edge[1])
        if p1 is not None and p2 is not None:
            x, y, z = zip(p1, p2)
            ax.plot(x, y, z, 'k')

def keyboard(event):
    global translateX, translateY, translateZ
    if event.key == 'b':
        translateY += 0.1
    elif event.key == 'g':
        translateY -= 0.1
    elif event.key == 'left':
        translateX -= 0.1
    elif event.key == 'right':
        translateX += 0.1
    elif event.key == 'up':
        translateZ += 0.1
    elif event.key == 'down':
        translateZ -= 0.1

    # Clear current plot and redraw
    ax.clear()
    draw_cube(ax)
    ax.set_xlim([-1, 1])
    ax.set_ylim([-1, 1])
    ax.set_zlim([-1, 1])
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')
    ax.set_title('3D Cube')
    plt.draw()

# Create a new figure and a 3D axis
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

# Set initial axis limits
ax.set_xlim([-1, 1])
ax.set_ylim([-1, 1])
ax.set_zlim([-1, 1])

# Draw the initial cube
draw_cube(ax)

# Connect the keyboard event handler
fig.canvas.mpl_connect('key_press_event', keyboard)

# Set labels and title
ax.set_xlabel('X')
ax.set_ylabel('Y')
ax.set_zlabel('Z')
ax.set_title('3D Cube')

# Show the plot
plt.show()
