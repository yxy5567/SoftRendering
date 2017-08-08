#pragma once
////---------------------------------------------------------------------
////    Basic Draw Pixel Function
////
////    Programmer:             Yang Xiaoyang
////    Last modification date: 2017-7-31
////    Creation date:          2017-7-17
////---------------------------------------------------------------------
#include <Windows.h>
#include "_BasicParameter.h"

//存放当前显示任务的z值，范围从-1到+1，默认初始值为1.01f
float** z_buffer = NULL;
//存放当前片元的RGB值和是否被使用过（用于多线程渲染）
struct ScreenPixelUnit
{
	float _R;
	float _G;
	float _B;
	bool _Used;
};
ScreenPixelUnit** ScreenPixels;
//将当前像素信息存到片元二维数组中，并将对应的_Used置为true
void drawScreenPixel(int x, int y, float z, float color3f_R, float color3f_G, float color3f_B)//RGB都是0~1的float
{
	if (x >= 0 && x <= maxWidth - 1 && y >= 0 && y <= maxHeight - 1)
	{
		if (z_buffer[x][y] > z)
		{
			ScreenPixels[x][y]._R = color3f_R;
			ScreenPixels[x][y]._G = color3f_G;
			ScreenPixels[x][y]._B = color3f_B;
			z_buffer[x][y] = z;
			ScreenPixels[x][y]._Used = true;
		}
	}
}
//将片元二维数组中的信息显示到窗口中，只有_Used为true的才显示
void showScreenUnits()
{
	for (int i = 0; i < maxHeight; i++)
	{
		for (int j = 0; j < maxWidth; j++)
		{
			if (ScreenPixels[i][j]._Used)
			{
				glColor3f(ScreenPixels[i][j]._R, ScreenPixels[i][j]._G, ScreenPixels[i][j]._B);
				glVertex2i(j, i);
			}
		}
	}
}
//清空片元二维数组
void ClearScreenUnits()
{
	if (ScreenPixels == NULL)
	{
		ScreenPixels = new ScreenPixelUnit* [maxHeight];
		for (int i = 0; i < maxHeight; i++)
		{
			ScreenPixels[i] = new ScreenPixelUnit[maxWidth];
			for (int j = 0; j < maxWidth; j++)
			{
				ScreenPixels[i][j]._R = 0.0f;
				ScreenPixels[i][j]._G = 0.0f;
				ScreenPixels[i][j]._B = 0.0f;
				ScreenPixels[i][j]._Used = false;
			}
		}
	}
	else
	{
		for (int i = 0; i < maxHeight; i++)
		{
			for (int j = 0; j < maxWidth; j++)
			{
				ScreenPixels[i][j]._R = 0.0f;
				ScreenPixels[i][j]._G = 0.0f;
				ScreenPixels[i][j]._B = 0.0f;
				ScreenPixels[i][j]._Used = false;
			}
		}
	}
}
//清空z_buffer，全体数据默认为1.01f（距离camera最远）
void clearZbuffer()
{
	if (z_buffer == NULL) //initial z_buffer
	{
		z_buffer = new float*[maxWidth];
		for (int i = 0; i < maxWidth; i++)
		{
			z_buffer[i] = new float[maxHeight];
			for (int j = 0; j < maxHeight; j++)
			{
				z_buffer[i][j] = 1.01f;
			}
		}
		return;
	}
	for (int i = 0; i < maxWidth; i++)
	{
		for (int j = 0; j < maxHeight; j++)
		{
			z_buffer[i][j] = 1.01f;
		}
	}
	return;
}

//在屏幕像素坐标(x,y)处画深度为z的点
void drawPixel(int x, int y, float z)
{
	if (x >= 0 && x <= maxWidth - 1 && y >= 0 && y <= maxHeight - 1)
	{
		if (z_buffer[x][y] > z)
		{
			//glColor3f(GLfloat(0), GLfloat(0), GLfloat(0));
			glVertex2i(x, y);
			z_buffer[x][y] = z;
		}
	}
}
