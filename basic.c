#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <GL/glut.h>
#include "glm.h"

GLMmodel *pmodel=NULL;

void
drawmodel(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  
  gluLookAt(0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  if (!pmodel) {
    pmodel = glmReadOBJ("obj/missile/missile.obj");
    if (!pmodel) exit(0);
    glmUnitize(pmodel);
    glmFacetNormals(pmodel);
    glmVertexNormals(pmodel, 90.0);
  }
    
  glmDraw(pmodel, GLM_SMOOTH | GLM_MATERIAL);
  glutSwapBuffers();
}

void
reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat)width/height, 0.01, 256.0);
    glMatrixMode(GL_MODELVIEW);
}
int
main(int argc, char** argv)
{
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(512, 512);
    glutInitWindowPosition(50, 50);
    glutInit(&argc, argv);
    
    glutCreateWindow("basic");
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glutReshapeFunc(reshape);
    glutDisplayFunc(drawmodel);

    glutMainLoop();
    
    return 0;
}
