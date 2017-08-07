#pragma once
////---------------------------------------------------------------------
////    Basic Show String Function with OpenGL
////
////    Programmer:             Yang Xiaoyang
////    Last modification date: 2017-7-31
////    Creation date:          2017-7-26
////---------------------------------------------------------------------
#include <windows.h>
#include "_BasicParameter.h"

// ASCII�ַ��ܹ�ֻ��0��127��һ��128���ַ�
#define MAX_CHAR       128
//��δָ��λ�ã���OpenGL�ڴ���������ַ���
void drawString(const char* str) 
{
	static int isFirstCall = 1;
	static GLuint lists;

	if (isFirstCall) 
	{ // ����ǵ�һ�ε��ã�ִ�г�ʼ��
		// Ϊÿһ��ASCII�ַ�����һ����ʾ�б�
		isFirstCall = 0;
		// ����MAX_CHAR����������ʾ�б����
		lists = glGenLists(MAX_CHAR);
		// ��ÿ���ַ��Ļ������װ����Ӧ����ʾ�б���
		wglUseFontBitmaps(wglGetCurrentDC(), 0, MAX_CHAR, lists);
	}
	// ����ÿ���ַ���Ӧ����ʾ�б�������ÿ���ַ�
	for (; *str != '\0'; ++str)
		glCallList(lists + *str);
}
//ѡ������
void selectFont(int size, int charset, const char* face) 
{
	HFONT hFont = CreateFontA(size, 0, 0, 0, FW_MEDIUM, 0, 0, 0,
		charset, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, face);
	HFONT hOldFont = (HFONT)SelectObject(wglGetCurrentDC(), hFont);
	DeleteObject(hOldFont);
}