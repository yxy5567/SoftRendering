#pragma once
////---------------------------------------------------------------------
////    Basic drawLine Function
////
////    Programmer:             Yang Xiaoyang
////    Last modification date: 2017-7-17
////    Creation date:          2017-7-17
////---------------------------------------------------------------------
#include <Windows.h>
#include <stdio.h>
#include "_BasicParameter.h"
#include "_drawPixel.h"
#include "_4dMatrix.h"
//在屏幕上画一条从像素(x1,y1)到像素(x2,y2)的线段
void drawLine(int x1, int y1, int x2, int y2)
{
	if (x1 == x2)
	{
		for (int i = (y1 <= y2 ? y1 : y2); i <= (y1 <= y2 ? y2 : y1); i++)
		{
			if (x1 >= 0 && x1 <= maxWidth - 1 && i >= 0 && i <= maxHeight - 1)
				glVertex2i(GLint(x1), GLint(i));
		}
		return;
	}
	if (y1 == y2)
	{
		for (int i = (x1 <= x2 ? x1 : x2); i <= (x1 <= x2 ? x2 : x1); i++)
		{
			if (i >= 0 && i <= maxWidth - 1 && y1 >= 0 && y1 <= maxHeight - 1)
				glVertex2i(GLint(i), GLint(y1));
		}
		return;
	}
	//Bresenhams's Line Algorithm , 分为斜率的绝对值大于等于1和小于1两种情况
	if (abs(x1 - x2) >= abs(y1 - y2))
	{
		int start_x = x1 < x2 ? x1 : x2;
		int start_y = x1 < x2 ? y1 : y2;
		int end_x = x1 < x2 ? x2 : x1;
		int end_y = x1 < x2 ? y2 : y1;
		//判断斜率正负
		int PosNeg = start_y < end_y ? 1 : -1;
		//A = 2× change in Y value
		int A = 2 * PosNeg * (end_y - start_y);
		//B = A − 2 × change in X value
		int B = A - 2 * (end_x - start_x);
		//P = A− change in X value
		int P = A - (end_x - start_x); 
		int last_y = start_y;
		int x;
		if (start_x >= 0 && start_x <= maxWidth - 1 && start_y >= 0 && start_y <= maxHeight - 1)
			glVertex2i(GLint(start_x), GLint(start_y));
		if (end_x >= 0 && end_x <= maxWidth - 1 && end_y >= 0 && end_y <= maxHeight - 1)
			glVertex2i(GLint(end_x), GLint(end_y));
		for (x = start_x + 1; x < end_x; x++)
		{
			if (P < 0)
			{
				if (x >= 0 && x <= maxWidth - 1 && last_y >= 0 && last_y <= maxHeight - 1)
				{
					glVertex2i(x, last_y);
				//	cout << x << "  " << last_y<<endl;
				}
				P += A;
			}
			else
			{
				if (x >= 0 && x <= maxWidth - 1 && (last_y + PosNeg * 1) >= 0 && (last_y + PosNeg * 1) <= maxHeight - 1)
				{
					glVertex2i(x, last_y + PosNeg * 1);
				//	cout << x << "  " << (last_y + PosNeg * 1)<<endl;
				}
				P += B;
				last_y += PosNeg * 1;
			}
		}
	}
	if (abs(x1 - x2) < abs(y1 - y2))
	{
		int start_y = y1 < y2 ? y1 : y2;
		int start_x = y1 < y2 ? x1 : x2;
		int end_y = y1 < y2 ? y2 : y1;
		int end_x = y1 < y2 ? x2 : x1;
		int PosNeg = start_x < end_x ? 1 : -1;
		int A = 2 * PosNeg * (end_x - start_x);
		int B = A - 2 * (end_y - start_y);
		int P = A - (end_y - start_y);
		int last_x = start_x;
		int y;
		if (start_x >= 0 && start_x <= maxWidth - 1 && (start_y) >= 0 && (start_y) <= maxHeight - 1)
			glVertex2i(GLint(start_x), GLint(start_y));
		if (end_x >= 0 && end_x <= maxWidth - 1 && (end_y) >= 0 && (end_y) <= maxHeight - 1)
			glVertex2i(GLint(end_x), GLint(end_y));
		for (y = start_y + 1; y < end_y; y++)
		{
			if (P < 0)
			{
				if (last_x >= 0 && last_x <= maxWidth - 1 && y >= 0 && y <= maxHeight - 1)
					glVertex2i(last_x, y);
				P += A;
			}
			else
			{
				if ((last_x + PosNeg * 1) >= 0 && (last_x + PosNeg * 1) <= maxWidth - 1 && y >= 0 && y <= maxHeight - 1)
					glVertex2i(last_x + PosNeg * 1, y);
				P += B;
				last_x += PosNeg * 1;
			};
		}
	}
}
