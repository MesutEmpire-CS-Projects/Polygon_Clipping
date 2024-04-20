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
        { { {-0.5, -0.5, -0.5}, {0.5, -0.5, -0.5}, {0.5, 0.5, -0.5}, {-0.5, 0.5, -0.5} }, {1, 0, 0} },// Front face
        { { {-0.5, -0.5, 0.5}, {0.5, -0.5, 0.5}, {0.5, 0.5, 0.5}, {-0.5, 0.5, 0.5} }, {0, 1, 0} },     // Back face
        { { {-0.5, -0.5, -0.5}, {-0.5, -0.5, 0.5}, {-0.5, 0.5, 0.5}, {-0.5, 0.5, -0.5} }, {0, 0, 1} },   // Left face
        { { {0.5, -0.5, -0.5}, {0.5, -0.5, 0.5}, {0.5, 0.5, 0.5}, {0.5, 0.5, -0.5} }, {1, 0, 1} },       // Right face
       { { {-0.5, -0.5, -0.5}, {0.5, -0.5, -0.5}, {0.5, -0.5, 0.5}, {-0.5, -0.5, 0.5} }, {0, 1, 1} },  // Bottom face
      { { {-0.5, 0.5, -0.5}, {0.5, 0.5, -0.5}, {0.5, 0.5, 0.5}, {-0.5, 0.5, 0.5} }, {1, 1, 0} }        // Top face
};

// Clipping window coordinates
const float xmin = -0.2, xmax = 0.2;
const float ymin = -0.3, ymax = 0.3;
const float zmin = -0.2, zmax = 0.5;

float w, h, tip = 0, turn = 0,limit=5;
int clip_Flag=0;
float eyeX = 2.0f, eyeY = 2.0f, eyeZ = 2.0f; // Initial camera position
float centerX = 0.0f, centerY = 0.0f, centerZ = 0.0f; // Look at origin
float upX = 0.0f, upY = 1.0f, upZ = 0.0f; // Up vector

// Defining region codes
const int INSIDE = 0; // 0000
const int LEFT = 1; // 0001
const int RIGHT = 2; // 0010
const int BOTTOM = 4; // 0100
const int TOP = 8; // 1000
const int BACK = 16; //10000
const int FRONT = 32; // 100000




int computeCode(float x, float y, float z)
{
    // initialized as being inside
    int code = INSIDE;

    if (x < xmin) // to the left of rectangle
        code |= LEFT;
    else if (x > xmax) // to the right of rectangle
        code |= RIGHT;
    if (y < ymin) // below the rectangle
        code |= BOTTOM;
    else if (y > ymax) // above the rectangle
        code |= TOP;
    if (z < zmin) // below the rectangle
        code |= BACK;
    else if (z > zmax) // above the rectangle
        code |= FRONT;

    return code;
}

vector<Point> cohenSutherlandClip(const vector<Point>& cubeVertices, float x1, float y1, float z1, float x2, float y2, float z2) {
    // Compute region codes for P1, P2
    int code1 = computeCode(x1, y1, z1);
    int code2 = computeCode(x2, y2, z2);
    vector<Point> clippedLines;

    while (true) {
        if (!(code1 | code2)) {
            // If both endpoints are inside the clipping window
            clippedLines.push_back({x1, y1, z1});
            clippedLines.push_back({x2, y2, z2});
            break;
        }
        else if (code1 & code2) {
            float x, y, z;
            int code_out = (code1 != 0) ? code1 : code2;

            if (code1) {
                // Both endpoints are outside in the same region, replace with the corresponding face of the clipping window
                if (code1 & LEFT || code1 & RIGHT) {
                    // Replace with left or right face of the clipping window
                    if (code1 & LEFT) {
                        x = xmin;
                        y = y1;
                        z = z1;
                    } else {
                        x = xmax;
                        y = y1;
                        z = z1;
                    }
                } else if (code1 & TOP || code1 & BOTTOM) {
                    // Replace with top or bottom face of the clipping window
                    if (code1 & TOP) {
                        x = x1;
                        y = ymax;
                        z = z1;
                    } else {
                        x = x1;
                        y = ymin;
                        z = z1;
                    }
                } else if (code1 & BACK || code1 & FRONT) {
                    // Replace with back or front face of the clipping window
                    if (code1 & BACK) {
                        x = x1;
                        y = y1;
                        z = zmin;
                    } else {
                        x = x1;
                        y = y1;
                        z = zmax;
                    }
                }
            } else {
                // Both endpoints are outside in the same region, replace with the corresponding face of the clipping window
                if (code2 & LEFT || code2 & RIGHT) {
                    // Replace with left or right face of the clipping window
                    if (code2 & LEFT) {
                        x = xmin;
                        y = y2;
                        z = z2;
                    } else {
                        x = xmax;
                        y = y2;
                        z = z2;
                    }
                } else if (code2 & TOP || code2 & BOTTOM) {
                    // Replace with top or bottom face of the clipping window
                    if (code2 & TOP) {
                        x = x2;
                        y = ymax;
                        z = z2;
                    } else {
                        x = x2;
                        y = ymin;
                        z = z2;
                    }
                } else if (code2 & BACK || code2 & FRONT) {
                    // Replace with back or front face of the clipping window
                    if (code2 & BACK) {
                        x = x2;
                        y = y2;
                        // Only update z-coordinate if the original z-coordinate is within the clipping volume
                        z = (z2 < zmin) ? zmin : ((z2 > zmax) ? zmax : z2);
                    } else {
                        x = x2;
                        y = y2;
                        // Only update z-coordinate if the original z-coordinate is within the clipping volume
                        z = (z2 < zmin) ? zmin : ((z2 > zmax) ? zmax : z2);
                    }
                }
            }

            // Update the endpoint and its code
            if (code_out == code1) {
                x1 = x;
                y1 = y;
                z1 = z;
                code1 = computeCode(x1, y1, z1);
            } else {
                x2 = x;
                y2 = y;
                z2 = z;
                code2 = computeCode(x2, y2, z2);
            }
        }
        else {
            // Some segment of the line lies within the clipping window
            int code_out = (code1 != 0) ? code1 : code2;
            float x, y, z;
            // Calculate intersection point
            // Update x, y, and z accordingly based on code_out

            // Update the point that lies outside the clipping window
            if (code_out & TOP) {
                // Point is above the clip rectangle
                x = x1 + (x2 - x1) * (ymax - y1) / (y2 - y1);
                y = ymax;
                z = z1 + (z2 - z1) * (ymax - y1) / (y2 - y1);
            } else if (code_out & BOTTOM) {
                // Point is below the rectangle
                x = x1 + (x2 - x1) * (ymin - y1) / (y2 - y1);
                y = ymin;
                z = z1 + (z2 - z1) * (ymin - y1) / (y2 - y1);
            }
// Update z-coordinate based on x-coordinate
            else if (code_out & RIGHT) {
                // Point is to the right of rectangle
                y = y1 + (y2 - y1) * (xmax - x1) / (x2 - x1);
                x = xmax;
                z = z1 + (z2 - z1) * (xmax - x1) / (x2 - x1);
            } else if (code_out & LEFT) {
                // Point is to the left of rectangle
                y = y1 + (y2 - y1) * (xmin - x1) / (x2 - x1);
                x = xmin;
                z = z1 + (z2 - z1) * (xmin - x1) / (x2 - x1);
            } else if (code_out & FRONT) {
                // Point is in front of the clipping window
                x = x1 + (x2 - x1) * (zmax - z1) / (z2 - z1);
                z = zmax;
                y = y1 + (y2 - y1) * (zmax - z1) / (z2 - z1);
            } else if (code_out & BACK) {
                // Point is behind the clipping window
                x = x1 + (x2 - x1) * (zmin - z1) / (z2 - z1);
                z = zmin;
                y = y1 + (y2 - y1) * (zmin - z1) / (z2 - z1);
            }

            // Update the endpoint and its code
            if (code_out == code1) {
                x1 = x;
                y1 = y;
                z1 = z;
                code1 = computeCode(x1, y1, z1);
            } else {
                x2 = x;
                y2 = y;
                z2 = z;
                code2 = computeCode(x2, y2, z2);
            }
        }
    }
    return clippedLines;
}


void drawFace(Face& face) {
    glColor3f(face.color.r, face.color.g, face.color.b);
    glBegin(GL_POLYGON);
    int counter = 1;

    for (const auto& vertex : face.vertices) {

        float x = vertex.x, y = vertex.y, z = vertex.z;
        vector<Point> lines ;
        if(clip_Flag == 1){
            vector<Point> clippedLines = cohenSutherlandClip(face.vertices, x, y,z, face.vertices[counter].x, face.vertices[counter].y,face.vertices[counter].z);
            for (const auto& line :clippedLines) {
                glVertex3f(line.x, line.y, line.z);
            }
        }
        else{
            glVertex3f(x, y, z);
        }

        if (counter >= 3 ){
            counter =0;
        } else{
            counter +=1;
        }

    }
    glEnd();
}

void drawCube(vector<Face>& cube) {
    for (auto& face : cube) {
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
    drawClippingWindowOutline();
    drawCube(cube);
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
    glutCreateWindow("Cohen Sutherland 3D Clipping Algorithm");
    init();
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutKeyboardFunc(key);
    glutReshapeFunc(reshape);
    glutSpecialFunc(Turn);
    glutMainLoop();
    return 0;
}