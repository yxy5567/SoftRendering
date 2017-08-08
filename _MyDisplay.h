#pragma once
#include "_BasicParameter.h"
#include "_drawMesh.h"
#include "_ShadowMap.h"



void myDisplay()
{
	if (_showMode >= 1 && _showMode <= 7)
	{
		//显示帧
		if (_showRotateMode != 0 || ifFreeFall)//1 2 3 4 5 6 7,旋转500次,FPS计数(7模式是花式旋转)
		{
			int FrameCount = 0;
			double TimeSpan = 0;
			int FPS = 0, tCount = 0;
			char FPS_str[12];
			if (_showMode == 6)
			{
				threadCount = 0;
			}
			for (int i = 0; i < 800; i++)
			{
				m1.~Mesh();
				m1.initialCube();
				if (_showMode == 2)
				{
					m2.~Mesh();
					m2.initialGND();
				}
				glClear(GL_COLOR_BUFFER_BIT);

				if (ifFreeFall)
				{
					//float t_clock = float(i)*0.05;
					//y_shift = basic_FreeFall(t_clock, 0, 1);
					//cout << y_shift << endl;
					float t_clock = 0.05f;
					if (i == 0)
					{
						velocity_current = 1.0f;
						y_current = 0.5f;
					}
					basic_FreeFallnRebound(t_clock, 0.9);
					//cout << y_shift << endl;
				}

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
					if (i <= 300)
					{
						rotate_X_Angle += 2;
					}
					else if (i <= 600)
					{
						rotate_Y_Angle += 2;
					}
					else if (i <= 800)
					{
						rotate_Z_Angle += 2;
					}
					y_shift += 0.0015;
				}
				if (_showRotateMode == 8)
				{
					if (i <= 300)
					{
						rotate_X_Angle -= 2;
					}
					else if (i <= 600)
					{
						rotate_Y_Angle -= 2;
					}
					else if (i <= 800)
					{
						rotate_Z_Angle -= 2;
					}
					y_shift -= 0.0015;
				}
				if (_showMode != 6)
				{
					drawMesh(m1, _showMode);
				}
				else
					drawMesh_thread(m1, 2);
				glEnd();
				glFlush();
			}
			_showRotateMode = 0;
			ifFreeFall = false;
		}
		//显示单帧，FPS表示为0
		if (_showRotateMode == 0)
		{
			m1.~Mesh();
			m1.initialCube();
			glClear(GL_COLOR_BUFFER_BIT);
			glBegin(GL_POINTS);
			if (_showMode != 6)
				drawMesh(m1, _showMode);
			else
				drawMesh_thread(m1, 2);
			glEnd();
			glFlush();
		}
	}
	if (_showMode == 8)
	{
		clearZbuffer();
		clearShadowMap();
		m1.~Mesh();
		m1.initialCube();
		m2.~Mesh();
		m2.initialCube();
		//TODO:这里初始化阴影
		m1_shadow.~Mesh();
		m1_shadow.initialCube();
		drawMesh_shadow(m1_shadow, 2);
		glClear(GL_COLOR_BUFFER_BIT);
		glBegin(GL_POINTS);
		if(ifCubeVisible)
			drawMesh(m1, 2);
		isClearZBuffer = false;
		drawGround(m2, 2);
		glEnd();
		glFlush();
	}
}