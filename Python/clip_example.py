import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from mpl_toolkits.mplot3d.art3d import Poly3DCollection

cube = [
    ([(-0.5, -0.5, -0.5), (0.5, -0.5, -0.5), (0.5, 0.5, -0.5), (-0.5, 0.5, -0.5)], (1, 0, 0)),  # Front face
    ([(-0.5, -0.5, 0.5), (0.5, -0.5, 0.5), (0.5, 0.5, 0.5), (-0.5, 0.5, 0.5)], (0, 1, 0)),        # Back face
    ([(-0.5, -0.5, -0.5), (-0.5, -0.5, 0.5), (-0.5, 0.5, 0.5), (-0.5, 0.5, -0.5)], (0, 0, 1)),    # Left face
    ([(0.5, -0.5, -0.5), (0.5, -0.5, 0.5), (0.5, 0.5, 0.5), (0.5, 0.5, -0.5)], (1, 0, 1)),        # Right face
    ([(-0.5, -0.5, -0.5), (0.5, -0.5, -0.5), (0.5, -0.5, 0.5), (-0.5, -0.5, 0.5)], (0, 1, 1)),    # Bottom face
    ([(-0.5, 0.5, -0.5), (0.5, 0.5, -0.5), (0.5, 0.5, 0.5), (-0.5, 0.5, 0.5)], (1, 1, 0))         # Top face
]

# Clipping window coordinates
xmin, xmax = -0.2, 0.2
ymin, ymax = -0.5, 0.5
zmin, zmax = -0.1, 0.1

limit = 1
clip_Flag = 0

# Initialize camera position
eyeX, eyeY, eyeZ = 2.0, 2.0, 2.0
step = 1.0


def draw_face(face):
    vertices = []
    for vertex in face[0]:
        x, y, z = vertex
        if clip_Flag == 1:
            x = min(max(x, xmin), xmax)
            y = min(max(y, ymin), ymax)
            z = min(max(z, zmin), zmax)
        vertices.append((x, y, z))  # Append (x, y, z) tuple
    ax.add_collection3d(Poly3DCollection([vertices], color=face[1]))

def draw_cube(cube):
    for face in cube:
        draw_face(face)


def draw_clipping_window_outline():
    front_face = [(xmin, ymin, zmin), (xmax, ymin, zmin), (xmax, ymax, zmin), (xmin, ymax, zmin)]
    back_face = [(xmin, ymin, zmax), (xmax, ymin, zmax), (xmax, ymax, zmax), (xmin, ymax, zmax)]
    left_face = [(xmin, ymin, zmin), (xmin, ymin, zmax), (xmin, ymax, zmax), (xmin, ymax, zmin)]
    right_face = [(xmax, ymin, zmin), (xmax, ymin, zmax), (xmax, ymax, zmax), (xmax, ymax, zmin)]
    bottom_face = [(xmin, ymin, zmin), (xmin, ymin, zmax), (xmax, ymin, zmax), (xmax, ymin, zmin)]
    top_face = [(xmin, ymax, zmin), (xmin, ymax, zmax), (xmax, ymax, zmax), (xmax, ymax, zmin)]

    faces = [front_face, back_face, left_face, right_face, bottom_face, top_face]
    for face in faces:
        x_vals, y_vals, z_vals = zip(*face)
        ax.plot(x_vals + x_vals[:1], y_vals + y_vals[:1], z_vals + z_vals[:1], color='black')


def draw_axes():
    ax.set_xlabel('X Label')
    ax.set_ylabel('Y Label')
    ax.set_zlabel('Z Label')

def display():
    ax.clear()
    ax.set_xlim(0, limit)
    ax.set_ylim(0, limit)
    ax.set_zlim(0, limit)
    draw_axes()
    draw_clipping_window_outline()
    draw_cube(cube)
    plt.show()

def key(event):
    global clip_Flag
    if clip_Flag == 1:
        clip_Flag = 0
    else:
        clip_Flag = 1
    display()

def on_key(event):
    global eyeX, eyeY, eyeZ
    if event.key == 'right':
        eyeX += step
    elif event.key == 'left':
        eyeX -= step
    elif event.key == 'up':
        eyeY += step
    elif event.key == 'down':
        eyeY -= step
    elif event.key == 'pageup':
        eyeZ += step
    elif event.key == 'pagedown':
        eyeZ -= step
    elif event.key == 'c':
        global clip_Flag
        if clip_Flag == 1:
            clip_Flag = 0
        else:
            clip_Flag = 1
    ax.view_init(elev=eyeY, azim=eyeX)
    display()



if __name__=="__main__":
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    fig.canvas.mpl_connect('key_press_event', on_key)
    display()
