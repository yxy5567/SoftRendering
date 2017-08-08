#include <GL\glut.h>
#include <Windows.h>
#include <stdio.h>
#include "_BasicParameter.h"
#include "_4dVector.h"
#include "_4dMatrix.h"
#include "_drawLine.h"
#include "_3dMesh.h"
#include "_drawMesh.h"
#include "_lighting.h"
#include "_ShowString.h"
#include "_KeyboardControl.h"
#include "_PhysicsParameter.h"
#include "_ShadowMap.h"
#include "_MyDisplay.h"
#include <time.h>


void myInit(void)
{
	if (ifInitTexture1or2)
		initBitmap2();
	else
		initBitmap();
	initTexture();
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glPointSize(1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, GLdouble(maxWidth), 0.0, GLdouble(maxHeight));
}



int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(maxWidth,maxHeight);
	glutInitWindowPosition((nScreenWidth-maxWidth) / 2, (nScreenHeight - maxHeight) / 2); //确保窗口在屏幕中间
	glutCreateWindow("Soft Rendering Test");
	myInit();
	glutKeyboardFunc(myKeyboard);
	glutDisplayFunc( myDisplay );
	glutMainLoop();
	return 0;
}
