#include <GL/glut.h>
#include <iostream>

// Define cuboid vertices
GLfloat vertices[][3] = {
    {-0.6, -0.6, -0.6}, // 0
    {0.6, -0.6, -0.6},  // 1
    {0.6, 0.6, -0.6},   // 2
    {-0.6, 0.6, -0.6},  // 3
    {-0.6, -0.6, 0.6},  // 4
    {0.6, -0.6, 0.6},   // 5
    {0.6, 0.6, 0.6},    // 6
    {-0.6, 0.6, 0.6}    // 7
};

// Define clipping window (rectangular prism)
GLfloat xmin = -0.15, ymin = -0.3, zmin = -0.3;
GLfloat xmax = 0.15, ymax = 0.3, zmax = 0.3;

// Flag to track whether clipping is enabled
bool clippingEnabled = false;

// Cyrus-Beck algorithm implementation
void cyrus_beck(GLfloat p1[3], GLfloat p2[3], GLfloat &t_enter, GLfloat &t_exit) {
    GLfloat dx = p2[0] - p1[0];
    GLfloat dy = p2[1] - p1[1];
    GLfloat dz = p2[2] - p1[2];

    t_enter = 0;
    t_exit = 1;

    GLfloat p[] = {-dx, dx, -dy, dy, -dz, dz};
    GLfloat q[] = {p1[0] - xmin, xmax - p1[0], p1[1] - ymin, ymax - p1[1], p1[2] - zmin, zmax - p1[2]};

    for (int i = 0; i < 6; ++i) {
        if (p[i] == 0) {
            if (q[i] < 0) {
                t_enter = -1;
                t_exit = -1;
                return; // Line is parallel to a boundary but outside
            }
        } else {
            GLfloat t = q[i] / p[i];
            if (p[i] < 0) {
                t_enter = std::max(t_enter, t);
            } else {
                t_exit = std::min(t_exit, t);
            }
        }
    }

    if (t_enter > t_exit) {
        t_enter = -1;
        t_exit = -1;
        return; // Line is outside the window
    }
}

// Function to update cuboid vertices based on clipping
void updateCuboid() {
    for (int i = 0; i < 8; ++i) {
        GLfloat x = vertices[i][0];
        GLfloat y = vertices[i][1];
        GLfloat z = vertices[i][2];

        // Clip against xmin plane
        if (x < xmin) {
            x = xmin;
        }
        // Clip against xmax plane
        else if (x > xmax) {
            x = xmax;
        }

        // Clip against ymin plane
        if (y < ymin) {
            y = ymin;
        }
        // Clip against ymax plane
        else if (y > ymax) {
            y = ymax;
        }

        // Clip against zmin plane
        if (z < zmin) {
            z = zmin;
        }
        // Clip against zmax plane
        else if (z > zmax) {
            z = zmax;
        }

        vertices[i][0] = x;
        vertices[i][1] = y;
        vertices[i][2] = z;
    }
}

// Keyboard callback function
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'c':
        case 'C':
            clippingEnabled = !clippingEnabled; // Toggle clipping state
            if (clippingEnabled) {
                updateCuboid(); // Update cuboid vertices if clipping is enabled
            }
            glutPostRedisplay(); // Trigger redisplay
            break;
        default:
            break;
    }
}

void drawCuboid() {
    glBegin(GL_QUADS);
    // Front face
    glColor3f(1.0, 0.0, 0.0);
    glVertex3fv(vertices[0]);
    glVertex3fv(vertices[1]);
    glVertex3fv(vertices[2]);
    glVertex3fv(vertices[3]);

    // Back face
    glColor3f(0.0, 1.0, 0.0);
    glVertex3fv(vertices[4]);
    glVertex3fv(vertices[5]);
    glVertex3fv(vertices[6]);
    glVertex3fv(vertices[7]);

    // Top face
    glColor3f(0.0, 0.0, 1.0);
    glVertex3fv(vertices[3]);
    glVertex3fv(vertices[2]);
    glVertex3fv(vertices[6]);
    glVertex3fv(vertices[7]);

    // Bottom face
    glColor3f(1.0, 1.0, 0.0);
    glVertex3fv(vertices[0]);
    glVertex3fv(vertices[1]);
    glVertex3fv(vertices[5]);
    glVertex3fv(vertices[4]);

    // Right face
    glColor3f(1.0, 0.0, 1.0);
    glVertex3fv(vertices[1]);
    glVertex3fv(vertices[2]);
    glVertex3fv(vertices[6]);
    glVertex3fv(vertices[5]);

    // Left face
    glColor3f(0.0, 1.0, 1.0);
    glVertex3fv(vertices[0]);
    glVertex3fv(vertices[3]);
    glVertex3fv(vertices[7]);
    glVertex3fv(vertices[4]);
    glEnd();
}


void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glLoadIdentity();

    // Set up the perspective projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.0, 0.1, 10.0); // Field of view 45 degrees, aspect ratio 1.0, near plane 0.1, far plane 10.0
    gluLookAt(2.0, 2.0, 2.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); // Eye position at (2, 2, 2), look at the origin, up direction is positive y-axis

    // Draw clipping window as wireframe rectangular prism
    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_LINE_LOOP);
    glVertex3f(xmin, ymin, zmin);
    glVertex3f(xmax, ymin, zmin);
    glVertex3f(xmax, ymax, zmin);
    glVertex3f(xmin, ymax, zmin);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glVertex3f(xmin, ymin, zmax);
    glVertex3f(xmax, ymin, zmax);
    glVertex3f(xmax, ymax, zmax);
    glVertex3f(xmin, ymax, zmax);
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(xmin, ymin, zmin);
    glVertex3f(xmin, ymin, zmax);

    glVertex3f(xmax, ymin, zmin);
    glVertex3f(xmax, ymin, zmax);

    glVertex3f(xmax, ymax, zmin);
    glVertex3f(xmax, ymax, zmax);

    glVertex3f(xmin, ymax, zmin);
    glVertex3f(xmin, ymax, zmax);
    glEnd();

    // Draw cuboid
    drawCuboid();

    glutSwapBuffers();
}

void init() {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_DEPTH_TEST);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Cyrus-Beck Line Clipping Algorithm");
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard); // Register keyboard callback function
    init();
    glutMainLoop();
    return 0;
}
