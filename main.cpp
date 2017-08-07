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
#include <time.h>


void myInit(void)
{
	initBitmap2();
	initTexture();
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glPointSize(1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, GLdouble(maxWidth), 0.0, GLdouble(maxHeight));
}


void myDisplay()
{
	
	if (_showMode != 0 )
	{
		if (_showRotateMode != 0)//1 2 3 4 5 6 7,旋转500次,FPS计数(7模式是花式旋转)
		{
			int FrameCount = 0;
			double TimeSpan = 0;
			int FPS = 0, tCount = 0;
			char FPS_str[12];
			if (_showMode == 6)
			{
				threadCount = 0;
			}
			for (int i = 0; i < 500; i++)
			{
				m1.~Mesh();
				m1.initialCube();
				if (_showMode == 2)
				{
					m2.~Mesh();
					m2.initialGND();
				}
				glClear(GL_COLOR_BUFFER_BIT);


				if (FrameCount == 0)
				{
					tstart = timeGetTime();
				}
				if (FrameCount == 60)
				{
					tstop = timeGetTime();
					TimeSpan = double(tstop - tstart);
					FPS = int(60.0f / (TimeSpan / 1000.0f));
					FrameCount = -1;
					tCount = threadCount;
				}
				FrameCount++;

				selectFont(48, ANSI_CHARSET, "Comic Sans MS");
				glColor3f(1.0f, 0.0f, 0.0f);
				glRasterPos2f(600, 750);
				drawString("FPS:");
				if (_showMode == 6)
				{
					glRasterPos2f(220, 750);
					drawString("Thread Count:");
					glRasterPos2f(480, 750);
					itoa(threadCount, FPS_str, 10);
					drawString(FPS_str);
				}
				glRasterPos2f(700, 750);
				itoa(FPS, FPS_str, 10);
				drawString(FPS_str);

				glBegin(GL_POINTS);
				glColor3f(GLfloat(0), GLfloat(0), GLfloat(0));
				if (_showRotateMode == 1)
				{
					rotate_X_Angle += 2;
				}
				if (_showRotateMode == 2)
				{
					rotate_Y_Angle += 2;
				}
				if (_showRotateMode == 3)
				{
					rotate_Z_Angle += 2;
				}
				if (_showRotateMode == 4)
				{
					rotate_X_Angle -= 2;
				}
				if (_showRotateMode == 5)
				{
					rotate_Y_Angle -= 2;
				}
				if (_showRotateMode == 6)
				{
					rotate_Z_Angle -= 2;
				}
				if (_showRotateMode == 7)
				{
					if (i <= 150)
					{
						rotate_X_Angle += 2;
					}
					else if (i <= 300)
					{
						rotate_Y_Angle += 2;
					}
					else if (i <= 500)
					{
						rotate_Z_Angle += 2;
					}
					y_shift += 0.0015;
				}
				if (_showRotateMode == 8)
				{
					if (i <= 150)
					{
						rotate_X_Angle -= 2;
					}
					else if (i <= 300)
					{
						rotate_Y_Angle -= 2;
					}
					else if (i <= 500)
					{
						rotate_Z_Angle -= 2;
					}
					y_shift -= 0.0015;
				}
				if (_showMode != 6)
				{
					drawMesh(m1, _showMode);
					if(_showMode==2)
						drawGround(m2, _showMode);
				}
					
				else
					drawMesh_thread(m1, 2);
				glEnd();
				glFlush();
			}
			_showRotateMode = 0;
		}
		if (_showRotateMode == 0)//显示一次，FPS表示为0
		{
			m1.~Mesh();
			m1.initialCube();
			m2.~Mesh();
			m2.initialGND();
			glClear(GL_COLOR_BUFFER_BIT);
			glBegin(GL_POINTS);
			//glColor3f(GLfloat(0), GLfloat(0), GLfloat(0));

			if (_showMode != 6)
				drawMesh(m1, _showMode);
			else
				drawMesh_thread(m1, 2);
			if (_showMode == 2)
			{
				drawGround(m2, _showMode);
			}
			glEnd();
			glFlush();
		}
		/*
		if (_showMode == 2) //显示地板
		{
			m2.~Mesh();
			m2.initialGND();
			glClear(GL_COLOR_BUFFER_BIT);
			glBegin(GL_POINTS);
			drawMesh(m2, _showMode);
			glEnd();
			glFlush();
		}
		*/
		
	}




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
