#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GLUT/glut.h>
#include "RgbImage.h"
#include <time.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>

GLint    wScreen=800, hScreen=500;

class Sphere {
  public:
    double x,y,z;

    // We also set the color values of the sphere inside the sphere's
    // constructor for simplicity sake.
    double r,g,b;

   Sphere(double _x, double _y, double _z) {
        x = _x;
        y = _y;
        z = _z;

        r = 1.0;
        g = 0.0;
        b = 0.0;

    }
    void Render() {
        glPushMatrix();
        glTranslatef(x, y, z);
        glColor3f(r, g, b);
        glScalef(1.0, 1.0, 1.0);
        glutSolidSphere(1, 50, 50);
        glPopMatrix();
    }

      void Move() {
        //if z is at front, move to start over in the back
        if (z >= 0) {
            z = -176;
        }
        z++;
    }

};


Sphere* sphere;

void Render3DEnvironment()
{
    sphere->Render();
    sphere->Move();
}

int main(int argc, char* argv[])
{

    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize (wScreen, hScreen);
    glutInitWindowPosition (400, 100);
    glutCreateWindow ("{jh,nbarreto}@dei.uc.pt-CG      ");

    sphere = new Sphere(50,50,50);

    glutDisplayFunc(Render3DEnvironment);

    glutMainLoop();
    

    return 0;

}