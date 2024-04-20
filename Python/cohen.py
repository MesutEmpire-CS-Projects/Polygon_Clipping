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

# Defining region codes
INSIDE = 0  # 0000
LEFT = 1  # 0001
RIGHT = 2  # 0010
BOTTOM = 4  # 0100
TOP = 8  # 1000
BACK = 16  # 10000
FRONT = 32  # 100000

# Assuming xmin, xmax, ymin, ymax, zmin, zmax are defined somewhere

def computeCode(x, y, z):
    # initialized as being inside
    code = INSIDE

    if x < xmin:  # to the left of rectangle
        code |= LEFT
    elif x > xmax:  # to the right of rectangle
        code |= RIGHT
    if y < ymin:  # below the rectangle
        code |= BOTTOM
    elif y > ymax:  # above the rectangle
        code |= TOP
    if z < zmin:  # below the rectangle
        code |= BACK
    elif z > zmax:  # above the rectangle
        code |= FRONT

    return code

def cohenSutherlandClip(cubeVertices, x1, y1, z1, x2, y2, z2):
    # Compute region codes for P1, P2
    code1 = computeCode(x1, y1, z1)
    code2 = computeCode(x2, y2, z2)
    clippedLines = []

    while True:
        if not (code1 | code2):
            # If both endpoints are inside the clipping window
            clippedLines.append((x1, y1, z1))
            clippedLines.append((x2, y2, z2))
            break
        elif code1 & code2:
            x, y, z = 0, 0, 0
            code_out = code1 if code1 != 0 else code2

            if code1:
                # Both endpoints are outside in the same region, replace with the corresponding face of the clipping window
                if code1 & LEFT or code1 & RIGHT:
                    # Replace with left or right face of the clipping window
                    x = xmin if code1 & LEFT else xmax
                    y = y1
                    z = z1
                elif code1 & TOP or code1 & BOTTOM:
                    # Replace with top or bottom face of the clipping window
                    x = x1
                    y = ymax if code1 & TOP else ymin
                    z = z1
                elif code1 & BACK or code1 & FRONT:
                    # Replace with back or front face of the clipping window
                    x = x1
                    y = y1
                    z = zmin if code1 & BACK else zmax
            else:
                # Both endpoints are outside in the same region, replace with the corresponding face of the clipping window
                if code2 & LEFT or code2 & RIGHT:
                    # Replace with left or right face of the clipping window
                    x = xmin if code2 & LEFT else xmax
                    y = y2
                    z = z2
                elif code2 & TOP or code2 & BOTTOM:
                    # Replace with top or bottom face of the clipping window
                    x = x2
                    y = ymax if code2 & TOP else ymin
                    z = z2
                elif code2 & BACK or code2 & FRONT:
                    # Replace with back or front face of the clipping window
                    x = x2
                    y = y2
                    z = zmin if code2 & BACK else zmax

            # Update the endpoint and its code
            if code_out == code1:
                x1, y1, z1 = x, y, z
                code1 = computeCode(x1, y1, z1)
            else:
                x2, y2, z2 = x, y, z
                code2 = computeCode(x2, y2, z2)
        else:
            # Some segment of the line lies within the clipping window
            code_out = code1 if code1 != 0 else code2
            x, y, z = 0, 0, 0
            # Calculate intersection point
            # Update x, y, and z accordingly based on code_out

            # Update the point that lies outside the clipping window
            if code_out & TOP:
                # Point is above the clip rectangle
                x = x1 + (x2 - x1) * (ymax - y1) / (y2 - y1)
                y = ymax
                z = z1 + (z2 - z1) * (ymax - y1) / (y2 - y1)
            elif code_out & BOTTOM:
                # Point is below the rectangle
                x = x1 + (x2 - x1) * (ymin - y1) / (y2 - y1)
                y = ymin
                z = z1 + (z2 - z1) * (ymin - y1) / (y2 - y1)
            # Update z-coordinate based on x-coordinate
            elif code_out & RIGHT:
                # Point is to the right of rectangle
                y = y1 + (y2 - y1) * (xmax - x1) / (x2 - x1)
                x = xmax
                z = z1 + (z2 - z1) * (xmax - x1) / (x2 - x1)
            elif code_out & LEFT:
                # Point is to the left of rectangle
                y = y1 + (y2 - y1) * (xmin - x1) / (x2 - x1)
                x = xmin
                z = z1 + (z2 - z1) * (xmin - x1) / (x2 - x1)
            elif code_out & FRONT:
                # Point is in front of the clipping window
                x = x1 + (x2 - x1) * (zmax - z1) / (z2 - z1)
                z = zmax
                y = y1 + (y2 - y1) * (zmax - z1) / (z2 - z1)
            elif code_out & BACK:
                # Point is behind the clipping window
                x = x1 + (x2 - x1) * (zmin - z1) / (z2 - z1)
                z = zmin
                y = y1 + (y2 - y1) * (zmin - z1) / (z2 - z1)

            # Update the endpoint and its code
            if code_out == code1:
                x1, y1, z1 = x, y, z
                code1 = computeCode(x1, y1, z1)
            else:
                x2, y2, z2 = x, y, z
                code2 = computeCode(x2, y2, z2)

    return clippedLines


def draw_face(face):
    # Extracting vertices and color from the face
    vertices = face[0]
    color = face[1]

    if clip_Flag == 1:
        clipped_vertices = []
        for i in range(len(vertices)):
            x, y, z = vertices[i]
            clipped_lines = cohenSutherlandClip(vertices, x, y, z, vertices[(i+1)%len(vertices)][0], vertices[(i+1)%len(vertices)][1], vertices[(i+1)%len(vertices)][2])
            clipped_vertices.extend(clipped_lines)
        vertices = clipped_vertices

    # Extract x, y, z from vertices
    x = [vertex[0] for vertex in vertices]
    y = [vertex[1] for vertex in vertices]
    z = [vertex[2] for vertex in vertices]

    # Plotting the polygon
    ax.add_collection3d(Poly3DCollection([list(zip(x, y, z))], color=color))

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
