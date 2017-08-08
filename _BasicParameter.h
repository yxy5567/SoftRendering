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

//1 : �߿�ģʽ
//2 : �޹���������
//3 : �޹���������
//4 : �й���������
//5 : �й���������
//6 �����̣߳��޹���������
int _showMode = 2; 
//�ڰ���qweasdrf�󣬸�ģʽ��Ϊ1~8��ת500�Σ�FPS���������������0��
int _showRotateMode=0;
//����ͳ���߳��ܹ��������˶��ٴ�
int threadCount = 0;
//�������
const int matrix_M = 4, matrix_N = 4;
//���ڴ�С�����أ�
const int maxWidth=800,maxHeight=800;
//Դ����ͼƬ��С�����أ�����InitBmp()��InitBmp2��ֵ�����ٱ仯
GLint imagewidth,imageheight;
//��ǰ��ʾ���ֱ���x
const int nScreenWidth = GetSystemMetrics(SM_CXSCREEN); 
//��ǰ��ʾ���ֱ���y
const int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
//���ڱ��������Ĺ�դ�����������ڶ��������е�index
int v1_index, v2_index, v0_index;
//������ģ������ϵ��X,Y,Z����ת�ĽǶ�
int rotate_Y_Angle = 0, rotate_X_Angle = 0, rotate_Z_Angle = 0;
//������ģ������ϵ����y���ƽ��
float y_shift = 0;
//��������ű���
float cubeScale = 1.0, cubeScale_y=1.0;
//textCoord_X[i][j][k]�����i��surface�еĵ�j��face�ĵ�k�������U/V����[0,1]
float*** textCoord_U, *** textCoord_V;
//txtr_RGB[i][j][k]�����i�е�j�У�Ҳ��������[j][i]����RGBֵ��K��012��Ӧ��RGB��
GLubyte ***txtr_RGB;

//�������

//�����⣬�̶�ֵ
float vec_AmbientLight_x = 100 / 255.0f, vec_AmbientLight_y = 100 / 255.0f, vec_AmbientLight_z = 100 / 255.0f;
//float vec_AmbientLight_x = 10 / 255.0f, vec_AmbientLight_y = 10 / 255.0f, vec_AmbientLight_z = 10 / 255.0f;
//��������׼ֵ
const float diffuseOrigin_R = 150 / 255.0f, diffuseOrigin_G = 150 / 255.0f, diffuseOrigin_B = 150 / 255.0f;
//const float diffuseOrigin_R = 150 / 255.0f, diffuseOrigin_G = 0 / 255.0f, diffuseOrigin_B = 0 / 255.0f;
//ƽ�й�������ֵ
float vec_WorldParallelLight_x = -9.5, vec_WorldParallelLight_y = 5.0, vec_WorldParallelLight_z = 2.4;
//ƽ�й�������ֵ�������ֵ
const float vec_WorldParrallelLight_MAX = 10;
//��������ϵ�е��Դ����
float vec_WorldPointLight_x = -0.51,vec_WorldPointLight_y = 0.81,vec_WorldPointLight_z = 0.51;
//��ʱ������������ʾ֡��

//��Ӱ���
//shadow map�Ĵ�С
const int shadowMapWidth = 400, shadowMapHeight = 400;
//��Ӱ���λ�ã�����ռ��У�
float ShadowCamera_WLD_X = 1.0f, ShadowCamera_WLD_Y = 1.0f, ShadowCamera_WLD_Z = 1.0f;

DWORD tstart, tstop;

//�������
//�������忪��
bool ifFreeFall = false; 

bool isClearZBuffer = true;

bool ifCubeVisible = false;

bool ifInitTexture1or2 = false;