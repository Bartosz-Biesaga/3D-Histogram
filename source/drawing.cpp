#include "../include/pch.h"
#include "../include/drawing.h"

namespace Drawing {
    Spherical camera(3.0f, 1.0f, 0.2f);
    bool perspectiveProjection = true;

    void initOpenGL() {
        glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
        glEnable(GL_DEPTH_TEST);
    }

    void reshapeScreen(sf::Vector2u size) {
        glViewport(0, 0, (GLsizei)size.x, (GLsizei)size.y);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        if (perspectiveProjection) {
            gluPerspective(45., (GLdouble)size.x / (GLdouble)size.y, 0.1, 100.0);
        }
        else {
            float magic_number = camera.distance / 2.2;
            glOrtho(-magic_number * ((GLdouble)size.x / (GLdouble)size.y), magic_number * ((GLdouble)size.x / (GLdouble)size.y), -magic_number, magic_number, -3., 12.0);
        }
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

    void drawScene() {
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        Spherical northOfCamera(camera.distance, camera.theta, camera.phi + 0.01f);
        gluLookAt(camera.getX(), camera.getY(), camera.getZ(),
            0.0, 0.0, 0.0,
            northOfCamera.getX(), northOfCamera.getY(), northOfCamera.getZ());

        //uklad
        glBegin(GL_LINES);
            glColor3f(1.0, 0.0, 0.0); glVertex3f(0, 0, 0); glVertex3f(1.0, 0, 0);
            glColor3f(0.0, 1.0, 0.0); glVertex3f(0, 0, 0); glVertex3f(0, 1.0, 0);
            glColor3f(0.0, 0.0, 1.0); glVertex3f(0, 0, 0); glVertex3f(0, 0, 1.0);
        glEnd();

        //Linie przerywane
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(2, 0xAAAA);
        glBegin(GL_LINES);
            glColor3f(1.0, 0.0, 0.0); glVertex3f(0, 0, 0); glVertex3f(-1.0, 0, 0);
            glColor3f(0.0, 1.0, 0.0); glVertex3f(0, 0, 0); glVertex3f(0, -1.0, 0);
            glColor3f(0.0, 0.0, 1.0); glVertex3f(0, 0, 0); glVertex3f(0, 0, -1.0);
        glEnd();
        glDisable(GL_LINE_STIPPLE);

        //szescian
        glLineWidth(2.0);
        glColor3f(0, 0, 0);
        glBegin(GL_LINES);
            for (unsigned char i = 0; i < 2; i++) {
                for (unsigned char j = 0; j < 2; j++) {
                    glVertex3f(-0.3f + 0.6f * (i ^ j), -0.3f + 0.6f * j, -0.3f); glVertex3f(-0.3f + 0.6f * (i ^ j), -0.3f + 0.6f * j, 0.3f);
                    glVertex3f(-0.3f, -0.3f + 0.6f * (i ^ j), -0.3f + 0.6f * j); glVertex3f(0.3f, -0.3f + 0.6f * (i ^ j), -0.3f + 0.6f * j);
                    glVertex3f(-0.3f + 0.6f * (i ^ j), -0.3f, -0.3f + 0.6f * j); glVertex3f(-0.3f + 0.6f * (i ^ j), 0.3f, -0.3f + 0.6f * j);
                }
            }
        glEnd();
    }
}