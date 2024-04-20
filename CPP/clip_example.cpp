#include <GL/glut.h>
#include <iostream>
#include <sstream>
#include <vector>
using namespace std;

struct Point {
    float x;
    float y;
    float z;
};

struct Color {
    float r;
    float g;
    float b;
};

struct Face {
    vector<Point> vertices;
    Color color;
};

vector<Face> cube = {
        { { {-0.5, -0.5, -0.5}, {0.5, -0.5, -0.5}, {0.5, 0.5, -0.5}, {-0.5, 0.5, -0.5} }, {1, 0, 0} }, // Front face
        { { {-0.5, -0.5, 0.5}, {0.5, -0.5, 0.5}, {0.5, 0.5, 0.5}, {-0.5, 0.5, 0.5} }, {0, 1, 0} },     // Back face
        { { {-0.5, -0.5, -0.5}, {-0.5, -0.5, 0.5}, {-0.5, 0.5, 0.5}, {-0.5, 0.5, -0.5} }, {0, 0, 1} },   // Left face
        { { {0.5, -0.5, -0.5}, {0.5, -0.5, 0.5}, {0.5, 0.5, 0.5}, {0.5, 0.5, -0.5} }, {1, 0, 1} },       // Right face
        { { {-0.5, -0.5, -0.5}, {0.5, -0.5, -0.5}, {0.5, -0.5, 0.5}, {-0.5, -0.5, 0.5} }, {0, 1, 1} },  // Bottom face
        { { {-0.5, 0.5, -0.5}, {0.5, 0.5, -0.5}, {0.5, 0.5, 0.5}, {-0.5, 0.5, 0.5} }, {1, 1, 0} }        // Top face
};

// Clipping window coordinates
const float xmin = -0.2, xmax = 0.2;
const float ymin = -0.5, ymax = 0.5;
const float zmin = -0.1, zmax = 0.1;

float w, h, tip = 0, turn = 0,limit=5;
int clip_Flag=0;
float eyeX = 2.0f, eyeY = 2.0f, eyeZ = 2.0f; // Initial camera position
float centerX = 0.0f, centerY = 0.0f, centerZ = 0.0f; // Look at origin
float upX = 0.0f, upY = 1.0f, upZ = 0.0f; // Up vector


void drawFace(const Face& face) {
    glColor3f(face.color.r, face.color.g, face.color.b);
    glBegin(GL_POLYGON);
    for (const auto& vertex : face.vertices) {
        float x = vertex.x, y = vertex.y, z = vertex.z;
        if(clip_Flag == 1){
            if (x < xmin) x = xmin;
            if (x > xmax) x = xmax;
            if (y < ymin) y = ymin;
            if (y > ymax) y = ymax;
            if (z < zmin) z = zmin;
            if (z > zmax) z = zmax;
        }
        glVertex3f(x, y, z);
    }
    glEnd();
}

void drawCube(const vector<Face>& cube) {
    for (const auto& face : cube) {
        drawFace(face);
    }
}


void drawClippingWindowOutline() {
    glColor3f(0.0, 0.0, 0.0);


    // Draw front face
    glBegin(GL_LINE_LOOP);
    glVertex3f(xmin, ymin, zmin);
    glVertex3f(xmax, ymin, zmin);
    glVertex3f(xmax, ymax, zmin);
    glVertex3f(xmin, ymax, zmin);
    glEnd();

    // Draw back face
    glBegin(GL_LINE_LOOP);
    glVertex3f(xmin, ymin, zmax);
    glVertex3f(xmax, ymin, zmax);
    glVertex3f(xmax, ymax, zmax);
    glVertex3f(xmin, ymax, zmax);
    glEnd();

    // Draw left face
    glBegin(GL_LINES);
    glVertex3f(xmin, ymin, zmin);
    glVertex3f(xmin, ymin, zmax);

    glVertex3f(xmin, ymax, zmin);
    glVertex3f(xmin, ymax, zmax);
    glEnd();

    // Draw right face
    glBegin(GL_LINES);
    glVertex3f(xmax, ymin, zmin);
    glVertex3f(xmax, ymin, zmax);

    glVertex3f(xmax, ymax, zmin);
    glVertex3f(xmax, ymax, zmax);
    glEnd();

    // Draw top face
    glBegin(GL_LINES);
    glVertex3f(xmin, ymax, zmin);
    glVertex3f(xmin, ymax, zmax);

    glVertex3f(xmax, ymax, zmin);
    glVertex3f(xmax, ymax, zmax);
    glEnd();

    // Draw bottom face
    glBegin(GL_LINES);
    glVertex3f(xmin, ymin, zmin);
    glVertex3f(xmin, ymin, zmax);

    glVertex3f(xmax, ymin, zmin);
    glVertex3f(xmax, ymin, zmax);
    glEnd();
}


void drawLabels(){

    // Markings and labels
    glColor3f(1, 1, 1); // white
    glRasterPos3f(limit+0.1, 0.0, 0.0); // Label for X axis
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'X');
    glRasterPos3f(0.0, limit+0.1, 0.0); // Label for Y axis
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'Y');
    glRasterPos3f(0.0, 0.0, limit+0.1); // Label for Z axis
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'Z');

    float step = 1.0f; // Marking step size
    glColor3f(1, 1, 1); // white
    glBegin(GL_LINES);
    for (float i = step; i <= limit; i += step) {
        glVertex3f(i, 0, 0.1); // Markings on X axis
        glVertex3f(i, 0, -0.1);
        glVertex3f(0, i, 0.1); // Markings on Y axis
        glVertex3f(0, i, -0.1);
        glVertex3f(0.1, 0, i); // Markings on Z axis
        glVertex3f(-0.1, 0, i);
    }
    glEnd();

    // Draw numerical values
    std::stringstream ss;
    std::string numStr;
    for (float i = step; i <= limit; i += step) {
        ss.str("");
        ss << i;
        numStr = ss.str();
        glRasterPos3f(i - 0.05, 0.1, 0.0);
        for (int j = 0; j < numStr.length(); j++) {
            glutBitmapCharacter(GLUT_BITMAP_8_BY_13, numStr[j]);
        }
        glRasterPos3f(-0.1, i - 0.05, 0.0);
        for (int j = 0; j < numStr.length(); j++) {
            glutBitmapCharacter(GLUT_BITMAP_8_BY_13, numStr[j]);
        }
        glRasterPos3f(-0.1, 0.1, i - 0.05);
        for (int j = 0; j < numStr.length(); j++) {
            glutBitmapCharacter(GLUT_BITMAP_8_BY_13, numStr[j]);
        }
    }
}

void drawAxes() {
    glPushMatrix();
//    glRotatef(tip, 1, 0, 0);
//    glRotatef(turn, 0, 1, 0);
    glLineWidth(2.0);
    glBegin(GL_LINES);
    // X axis
    glColor3f(1, 0, 0); // Red
    glVertex3f(0, 0, 0);
    glVertex3f(limit, 0, 0);
    // Y axis
    glColor3f(0, 1, 0); // Green
    glVertex3f(0, 0, 0);
    glVertex3f(0, limit, 0);
    // Z axis
    glColor3f(0, 0, 1); // Blue
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, limit);
    glEnd();

    drawLabels();

    glPopMatrix();
}

void display(void) {
    glViewport(0, 0, w, h);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ);

    drawAxes();

    glPushMatrix();
//    glRotatef(tip, 1, 0, 0);
//    glRotatef(turn, 0, 1, 0);
    drawClippingWindowOutline();

    drawCube(cube);

    glPopMatrix();

    glutSwapBuffers();
}

void reshape(int nw, int nh) {
    w = nw;
    h = nh;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (GLfloat)w / (GLfloat)h, 1.0f, 10.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Turn(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_RIGHT: eyeX += 0.1f; break;
        case GLUT_KEY_LEFT: eyeX -= 0.1f; break;
        case GLUT_KEY_UP: eyeY += 0.1f; break;
        case GLUT_KEY_DOWN: eyeY -= 0.1f; break;
        case GLUT_KEY_PAGE_UP: eyeZ += 0.1f; break;
        case GLUT_KEY_PAGE_DOWN: eyeZ -= 0.1f; break;
    }
}

void key(unsigned char ch, int x, int y){
    if (clip_Flag == 1) {
        clip_Flag = 0;
    }else {
        clip_Flag=1;
    }
    glutPostRedisplay();
}

void init() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1, 0.2, 0.1, 1.0);
}
int main(int argc, char** argv) {

    glutInit(&argc, argv);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(400, 300);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
    glutCreateWindow("Clip Example");
    init();
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutKeyboardFunc(key);
    glutReshapeFunc(reshape);
    glutSpecialFunc(Turn);
    glutMainLoop();
    return 0;
}