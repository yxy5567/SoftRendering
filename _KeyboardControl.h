#pragma once
////---------------------------------------------------------------------
////    Basic Keyboard Control Function
////
////    Programmer:             Yang Xiaoyang
////    Last modification date: 2017-7-31
////    Creation date:          2017-7-28
////---------------------------------------------------------------------
#include "_BasicParameter.h"

//接收到键盘按下的不同按键所触发的命令
void myKeyboard(unsigned char theKey, int mouseX, int mouseY)
{
	switch (theKey)
	{
	case 'q':
	{
		_showRotateMode = 1;
		rotate_X_Angle += 2;
		rotate_X_Angle = (rotate_X_Angle >= 360) ? (rotate_X_Angle - 360) : rotate_X_Angle;
		glutPostRedisplay();
		break;
	}
	case 'w':
	{
		_showRotateMode = 2;
		rotate_Y_Angle += 2;
		rotate_Y_Angle = (rotate_Y_Angle >= 360) ? (rotate_Y_Angle - 360) : rotate_Y_Angle;
		glutPostRedisplay();
		break;
	}
	case 'e':
	{
		_showRotateMode = 3;
		rotate_Z_Angle += 2;
		rotate_Z_Angle = (rotate_Z_Angle >= 360) ? (rotate_Z_Angle - 360) : rotate_Z_Angle;
		glutPostRedisplay();
		break;
	}
	case 'a':
	{
		_showRotateMode = 4;
		rotate_X_Angle -= 2;
		rotate_X_Angle = (rotate_X_Angle <= 0) ? (rotate_X_Angle + 360) : rotate_X_Angle;
		glutPostRedisplay();
		break;
	}
	case 's':
	{
		_showRotateMode = 5;
		rotate_Y_Angle -= 2;
		rotate_Y_Angle = (rotate_Y_Angle <= 0) ? (rotate_Y_Angle + 360) : rotate_Y_Angle;
		glutPostRedisplay();
		break;
	}
	case 'd':
	{
		_showRotateMode = 6;
		rotate_Z_Angle -= 2;
		rotate_Z_Angle = (rotate_Z_Angle <= 0) ? (rotate_Z_Angle + 360) : rotate_Z_Angle;
		glutPostRedisplay();
		break;
	}
	case 'r':
	{
		//花式螺旋上天
		_showRotateMode = 7; 
		glutPostRedisplay();
		break;
	}
	case 'f':
	{
		//逆向花式旋转
		_showRotateMode = 8; 
		glutPostRedisplay();
		break;
	}
	case 't':
	{
		ifFreeFall = true;
		glutPostRedisplay();
		break;
	}
	case 'Q':
	{
		vec_WorldParallelLight_x += 0.1;
		vec_WorldParallelLight_x = (vec_WorldParallelLight_x >= vec_WorldParrallelLight_MAX) ? (vec_WorldParallelLight_x - vec_WorldParrallelLight_MAX) : vec_WorldParallelLight_x;
		vec_WorldPointLight_x += 0.1;
		glutPostRedisplay();
		break;
	}
	case 'W':
	{
		vec_WorldParallelLight_y += 0.1;
		vec_WorldParallelLight_y = (vec_WorldParallelLight_y >= vec_WorldParrallelLight_MAX) ? (vec_WorldParallelLight_y - vec_WorldParrallelLight_MAX) : vec_WorldParallelLight_y;
		vec_WorldPointLight_y += 0.1;
		glutPostRedisplay();
		break;
	}
	case 'E':
	{
		vec_WorldParallelLight_z += 0.1;
		vec_WorldParallelLight_z = (vec_WorldParallelLight_z >= vec_WorldParrallelLight_MAX) ? (vec_WorldParallelLight_z - vec_WorldParrallelLight_MAX) : vec_WorldParallelLight_z;
		vec_WorldPointLight_z += 0.1;
		glutPostRedisplay();
		break;
	}

	case 'A':
	{
		vec_WorldParallelLight_x -= 0.1;
		vec_WorldParallelLight_x = (vec_WorldParallelLight_x <= -1.0f*vec_WorldParrallelLight_MAX) ? (vec_WorldParallelLight_x + vec_WorldParrallelLight_MAX) : vec_WorldParallelLight_x;
		vec_WorldPointLight_x -= 0.1;
		glutPostRedisplay();
		break;
	}

	case 'S':
	{
		vec_WorldParallelLight_y -= 0.1;
		vec_WorldParallelLight_y = (vec_WorldParallelLight_y <= -1.0f*vec_WorldParrallelLight_MAX) ? (vec_WorldParallelLight_y + vec_WorldParrallelLight_MAX) : vec_WorldParallelLight_y;
		vec_WorldPointLight_y -= 0.1;
		glutPostRedisplay();
		break;
	}

	case 'D':
	{
		vec_WorldParallelLight_z -= 0.1;
		vec_WorldParallelLight_z = (vec_WorldParallelLight_z <= -1.0f*vec_WorldParrallelLight_MAX) ? (vec_WorldParallelLight_z + vec_WorldParrallelLight_MAX) : vec_WorldParallelLight_z;
		vec_WorldPointLight_z -= 0.1;
		glutPostRedisplay();
		break;
	}
	case '1':
	{
		_showMode = 1;
		glutPostRedisplay();
		break;
	}
	case '2':
	{
		_showMode = 2;
		glutPostRedisplay();
		break;
	}
	case '3':
	{
		_showMode = 3;
		glutPostRedisplay();
		break;
	}
	case '4':
	{
		_showMode = 4;
		glutPostRedisplay();
		break;
	}
		case '5':
	{
		_showMode = 5;
		glutPostRedisplay();
		break;
	}
	case '6':
	{
		_showMode = 6;
		glutPostRedisplay();
		break;
	}
	case '8':
	{
		_showMode = 8;
		if (ifCubeVisible == true)
			ifCubeVisible = false;
		else
			ifCubeVisible = true;
		glutPostRedisplay();
		break;
	}
	case '+':
	{
		cubeScale += 0.05;
		glutPostRedisplay();
		break;
	}
	case '-':
	{
		if (cubeScale>0.2)
			cubeScale -= 0.05;
		glutPostRedisplay();
		break;
	}
	default:
		break;
	}
}