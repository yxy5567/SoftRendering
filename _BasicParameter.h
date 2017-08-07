#pragma once
////---------------------------------------------------------------------
////    Basic Parameters
////
////    Programmer:             Yang Xiaoyang
////    Last modification date: 2017-7-31
////    Creation date:          2017-7-17
////---------------------------------------------------------------------
#include <GL\glut.h>
#include <GL\glu.h>
#include <GL\Gl.h>
#ifndef PI
#define PI 3.14159265358979323846f
#endif

//1 : 线框模式
//2 : 无光照无纹理
//3 : 无光照有纹理
//4 : 有光照无纹理
//5 : 有光照有纹理
//6 ：多线程，无光照无纹理
int _showMode = 2; 
//在按下qweasdrf后，该模式变为1~8，转500次，FPS计数。接下来变成0。
int _showRotateMode=0;
//用于统计线程总共被调用了多少次
int threadCount = 0;
//矩阵阶数
const int matrix_M = 4, matrix_N = 4;
//窗口大小（像素）
const int maxWidth=800,maxHeight=800;
//源纹理图片大小（像素），在InitBmp()或InitBmp2后赋值，不再变化
GLint imagewidth,imageheight;
//当前显示器分辨率x
const int nScreenWidth = GetSystemMetrics(SM_CXSCREEN); 
//当前显示器分辨率y
const int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
//用于保存待处理的光栅的三个顶点在顶点数组中的index
int v1_index, v2_index, v0_index;
//物体绕模型坐标系的X,Y,Z轴旋转的角度
int rotate_Y_Angle = 0, rotate_X_Angle = 0, rotate_Z_Angle = 0;
//物体在模型坐标系中沿y轴的平移
float y_shift = 0;
//物体的缩放比例
float cubeScale = 1.0;
//textCoord_X[i][j][k]代表第i个surface中的第j个face的第k个顶点的U/V坐标[0,1]
float*** textCoord_U, *** textCoord_V;
//txtr_RGB[i][j][k]代表第i行第j列（也就是坐标[j][i]）的RGB值（K的012对应的RGB）
GLubyte ***txtr_RGB;

//光照相关

//环境光，固定值
float vec_AmbientLight_x = 100 / 255.0f, vec_AmbientLight_y = 100 / 255.0f, vec_AmbientLight_z = 100 / 255.0f;
//float vec_AmbientLight_x = 10 / 255.0f, vec_AmbientLight_y = 10 / 255.0f, vec_AmbientLight_z = 10 / 255.0f;
//漫反射光基准值
const float diffuseOrigin_R = 150 / 255.0f, diffuseOrigin_G = 150 / 255.0f, diffuseOrigin_B = 150 / 255.0f;
//const float diffuseOrigin_R = 150 / 255.0f, diffuseOrigin_G = 0 / 255.0f, diffuseOrigin_B = 0 / 255.0f;
//平行光照向量值
float vec_WorldParallelLight_x = -9.5, vec_WorldParallelLight_y = 5.0, vec_WorldParallelLight_z = 2.4;
//平行光照向量值分量最大值
const float vec_WorldParrallelLight_MAX = 10;
//世界坐标系中点光源坐标
float vec_WorldPointLight_x = -0.51,vec_WorldPointLight_y = 0.81,vec_WorldPointLight_z = 0.51;
//计时变量，用于显示帧数

//阴影相关
//阴影相机位置（世界空间中）
float ShadowCamera_WLD_X = 1.0f, ShadowCamera_WLD_Y = 1.0f, ShadowCamera_WLD_Z = 1.0f;

DWORD tstart, tstop;