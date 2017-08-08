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

//��ŵ�ǰ��ʾ�����zֵ����Χ��-1��+1��Ĭ�ϳ�ʼֵΪ1.01f
float** z_buffer = NULL;
//��ŵ�ǰƬԪ��RGBֵ���Ƿ�ʹ�ù������ڶ��߳���Ⱦ��
struct ScreenPixelUnit
{
	float _R;
	float _G;
	float _B;
	bool _Used;
};
ScreenPixelUnit** ScreenPixels;
//����ǰ������Ϣ�浽ƬԪ��ά�����У�������Ӧ��_Used��Ϊtrue
void drawScreenPixel(int x, int y, float z, float color3f_R, float color3f_G, float color3f_B)//RGB����0~1��float
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
//��ƬԪ��ά�����е���Ϣ��ʾ�������У�ֻ��_UsedΪtrue�Ĳ���ʾ
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
//���ƬԪ��ά����
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
//���z_buffer��ȫ������Ĭ��Ϊ1.01f������camera��Զ��
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

//����Ļ��������(x,y)�������Ϊz�ĵ�
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
