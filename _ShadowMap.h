#pragma once
////---------------------------------------------------------------------
////    Basic Shadow Map Creation Function
////
////    Programmer:             Yang Xiaoyang
////    Last modification date: 2017-8-8
////    Creation date:          2017-8-3
////---------------------------------------------------------------------
#include <GL\glut.h>
#include <Windows.h>
#include <stdio.h>
#include <ctime>
#include <thread>
#include <mutex> 
#include "_BasicParameter.h"
#include "_4dVector.h"
#include "_4dMatrix.h"
#include "_drawLine.h"
#include "_drawPixel.h"
#include "_3dMesh.h"
#include "_lighting.h"
#include "_3dLineSurfaceEquation.h"
#include "_ShadowMap.h"
#include "_drawMesh.h"

//��ά����ֱ�洢�ڵ����shadow map�ͱ��ڵ����map
float ** _shadowMap;
float ** groundMap;
//��״�����
Mesh m1_shadow;
//int testcount_shadowmap;
//int testcount_drawground;
//������������
Vector4D_float* vertCoord_Screenf_shadow;
Vector4D_float* vertCoord_clipSpace_shadow;
Vector4D_float* vertCoord_worldSpace_shadow;
//��ʱ�ò���
Vector4D_float* vertZ_GroundToShadow;
//shadow map������ռ䵽�ü��ռ��е�����
Matrix4D_float m_WLDtoCLP_shadow, m_MDLtoWLD_shadow, m_MDLtoCLP_shadow;
//��������
bool isInShadow(const Vector4D_float P_worldSpace);

//��shadow map���ӻ�����ʾ�����½�
void showShadowMap()
{
	for (int i = 0; i < shadowMapHeight; i++)
	{
		for (int j = 0; j < shadowMapWidth; j++)
		{

			float color = _shadowMap[i][j] - groundMap[i][j];
		//	glColor3f(color, color, color);
			glColor3f(_shadowMap[i][j], _shadowMap[i][j], _shadowMap[i][j]);
			glVertex2i(j, i);
		}
	}
}
//��ʼ��shadowmap��ÿһ֡��Ҫ���³�ʼ��һ��
void clearShadowMap()
{
	if (_shadowMap == 0)
	{
		_shadowMap = new float*[shadowMapHeight];
		for (int i = 0; i < shadowMapHeight; i++)
			_shadowMap[i] = new float[shadowMapWidth];
	}
	for (int i = 0; i < shadowMapHeight; i++)
	{
		for (int j = 0; j < shadowMapWidth; j++)
		{
			_shadowMap[i][j] = 1.01f;
		}
	}
}
//����shadow mapʱ���е�����任�����Ҵ洢�м����
void coordTrans_shadowMap(Mesh& m, Matrix4D_float& mat_Trans)
{
	Matrix4D_float mat_MDLtoWLD, mat_WLDtoVIW, mat_VIWtoCLP;
	mat_MDLtoWLD.unitMatrix();
	mat_WLDtoVIW.unitMatrix();
	mat_VIWtoCLP.unitMatrix();
	//Transformation Operations;
	Matrix4D_float mat1, mat2, m3, m4, m5, m6;
	//---From model space to world space---
	m5.MatrixRotate_X(rotate_X_Angle);
	m4.MatrixRotate_Z(rotate_Z_Angle);
	m3.MatrixRotate_Y(rotate_Y_Angle);
	mat1.MatrixScale(cubeScale, cubeScale, cubeScale);
	mat2.MatrixShift(0, -0.3, 0);
	mat_MDLtoWLD = mat2*mat1*m3*m4*m5;
	//---From world space to view space---
	//mat1.MatrixShift(-1, -0.8, 3);
	mat1.MatrixShift(-0.7, -0.5, 3);
	mat2.MatrixRotate_X(-10);
	m3.MatrixRotate_Y(15);
	m4.NegnateZ();//��Ҫ����ԶҪ��
	mat_WLDtoVIW = m4*m3*mat2*mat1*mat_WLDtoVIW;
	//---From view space to clip space NDC--- z range [-w,w]
	mat1.MatrixFrustum(60, 1, 1, 15);
	mat_VIWtoCLP = mat1;
	//�洢�м����
	m_WLDtoCLP_shadow = mat_VIWtoCLP*mat_WLDtoVIW;
	m_MDLtoWLD_shadow = mat_MDLtoWLD;
	m_MDLtoCLP_shadow = mat_Trans;
	mat_Trans = mat_VIWtoCLP*mat_WLDtoVIW*mat_MDLtoWLD;
//		mat_Trans.showMatrix(); //for testing
	return;
}
//���ƶ�ƬԪ����Ϣ�洢��shadowmap��
void drawPixel_shadow(int x,int y,float z)
{
	if (x >= 0 && x <= shadowMapWidth - 1 && y >= 0 && y <= shadowMapHeight - 1)
	{
		//SOLVED BUG(2017 08 08������������Ū����
		if (_shadowMap[y][x] > z)
		{
			_shadowMap[y][x] = z;
		}
	}
}
//����shadow mapʱ�������ǹ�դ
void drawUpRaster_shadow(int* x, int* y, float* z, int* vertexIndex, int surfaceindex, int faceindex, bool ifTexture)
{
	int x_left, x_right;
	float z_left, z_right, z_temp;
	for (int i = y[0]; i >= y[2]; i--)
	{
		//get the proper x/z_left, x/z_right
		solveUpRasterBound(x_left, x_right, z_left, z_right, x, y, z, i); 
		//ֻ��һ���㣬���ұ߽��غ�
		if (x_left == x_right) 
		{
			//������ı�shadowMap�����ֵ
			drawPixel_shadow(x_left, i, z_left);
		}
		//���ұ߽粻�غ�	
		else
		{
			for (int j = x_left; j <= x_right; j++)
			{
				if (z_left == z_right)
					z_temp = z_left;
				else
				{
					float t = float(j - x_left) / float(x_right - x_left);
					z_temp = (z_right - z_left)*((float)j - (float)x_left) / ((float)x_right - (float)x_left) + z_left;
				}
				//������ı�shadowMap�����ֵ
				drawPixel_shadow(j, i, z_temp);
			}
		}
	}
}
//����shadow mapʱ�������ǹ�դ
void drawLowRaster_shadow(int* x, int* y, float* z, int* vertexIndex, int surfaceindex, int faceindex, bool ifTexture)
{
	int x_left, x_right;
	float z_left, z_right, z_temp;
	for (int i = y[0]; i >= y[2]; i--)
	{
		//get the proper x/z_left, x/z_right
		solveLowRasterBound(x_left, x_right, z_left, z_right, x, y, z, i); 
		//ֻ��һ���㣬���ұ߽��غ�
		if (x_left == x_right) 
		{
			//������ı�shadowMap�����ֵ
			drawPixel_shadow(x_left, i, z_left);
		}
		else
		{
			for (int j = x_left; j <= x_right; j++)
			{
				if (z_left == z_right)
					z_temp = z_left;
				else
				{
					float t = float(j - x_left) / float(x_right - x_left);
					z_temp = (z_right - z_left)*((float)j - (float)x_left) / ((float)x_right - (float)x_left) + z_left;
				}
				//������ı�shadowMap�����ֵ
				drawPixel_shadow(j, i, z_temp);
			}
		}
	}
}
//����shadow mapʱ��դ���ƺ���
void drawRaster_shadow(Vector4D_float* vec, int* vIndex, int surfaceindex, int faceindex, bool ifTexture)
{
	int x[3], y[3];
	x[0] = vec[0].get_X(), x[1] = vec[1].get_X(), x[2] = vec[2].get_X(), y[0] = vec[0].get_Y(), y[1] = vec[1].get_Y(), y[2] = vec[2].get_Y(); //(x[0],y[0])��ߵ㣬(x[2],y[2])��͵�
	float z[3];
	z[0] = vec[0].get_Z(), z[1] = vec[1].get_Z(), z[2] = vec[2].get_Z();

	if (y[1] <= y[2]) swap(x[1], x[2], y[1], y[2], z[1], z[2], vIndex[1], vIndex[2]);
	if (y[0] <= y[1]) swap(x[0], x[1], y[0], y[1], z[0], z[1], vIndex[0], vIndex[1]);
	if (y[1] <= y[2]) swap(x[1], x[2], y[1], y[2], z[1], z[2], vIndex[1], vIndex[2]);

	if (y[0] == y[1])
	{
		drawUpRaster_shadow(x, y, z, vIndex, surfaceindex, faceindex, ifTexture);
		return;
	}
	else if (y[2] == y[1])
	{
		drawLowRaster_shadow(x, y, z, vIndex, surfaceindex, faceindex, ifTexture);
		return;
	}
	//�ȷ�������Ҳ�������ǵ����ͣ���ֳ�������+������Ȼ��ݹ�
	int tmpx = int(((float)y[1] - (float)y[2])*((float)x[0] - (float)x[2]) / ((float)y[0] - (float)y[2]) + x[2]);
	float tmpz;
	if (z[0] == z[2])
	{
		tmpz = z[0];
	}
	else
	{
		//TODO:20170804 NDC��Z�Ĳ�ֵ
		tmpz = ((float)y[1] - (float)y[2])*(z[0] - z[2]) / ((float)y[0] - (float)y[2]) + z[2];
	}
	Vector4D_float* vec_up = new Vector4D_float[3];
	Vector4D_float* vec_down = new Vector4D_float[3];
	vec_up[0].setVec(x[0], y[0], z[0], 1);
	vec_up[1].setVec(x[1], y[1], z[1], 1);
	vec_up[2].setVec(tmpx, y[1], tmpz, 1);
	drawRaster_shadow(vec_up, vIndex, surfaceindex, faceindex, ifTexture);

	vec_down[0].setVec(x[2], y[2], z[2], 1);
	vec_down[1].setVec(x[1], y[1], z[1], 1);
	vec_down[2].setVec(tmpx, y[1], tmpz, 1);
	drawRaster_shadow(vec_down, vIndex, surfaceindex, faceindex, ifTexture);

	return;
}
//����shadow mapʱ����״�庯��
void drawMesh_shadow(Mesh& m, int mode)
{
	//��ʼ��
	clearShadowMap();
	Matrix4D_float mat_Trans;
	//����ת������ģ������ϵ���ü�����ϵ��
	coordTrans_shadowMap(m1_shadow, mat_Trans);
	//�������Ļ����
	if (vertCoord_Screenf_shadow != NULL) delete[] vertCoord_Screenf_shadow;
	vertCoord_Screenf_shadow = new Vector4D_float[m.m_numVerts];
	//����Ĳü��ռ�����
	if (vertCoord_clipSpace_shadow != NULL) delete[] vertCoord_clipSpace_shadow;
	vertCoord_clipSpace_shadow = new Vector4D_float[m.m_numVerts];
	//���������ռ�����
	if (vertCoord_worldSpace_shadow != NULL) delete[] vertCoord_worldSpace_shadow;
	vertCoord_worldSpace_shadow = new Vector4D_float[m.m_numVerts];
	
	//��ģ�Ϳռ䵽��Ļ�ռ�ı任����
	float screen_X, screen_Y;
	for (int i = 0; i < m.m_numVerts; i++)
	{
		vertCoord_clipSpace_shadow[i] = m.m_pt[i]; //ģ������ϵ�е����ж��������
		vertCoord_clipSpace_shadow[i] = mat_Trans * vertCoord_clipSpace_shadow[i]; //�ü�����ϵ�ж�Ӧ�����ж��������

		//�Ӳü�����ϵ��shadow map�ġ�������Ļ���ռ�
		screen_X = vertCoord_clipSpace_shadow[i].get_X()*float(shadowMapWidth) / (vertCoord_clipSpace_shadow[i].get_W()*2.0f) + float(shadowMapWidth) / 2.0f;
		screen_Y = vertCoord_clipSpace_shadow[i].get_Y()*float(shadowMapHeight) / (vertCoord_clipSpace_shadow[i].get_W()*2.0f) + float(shadowMapHeight) / 2.0f;
		//X��YΪ�������꣬ZΪ���
		vertCoord_Screenf_shadow[i].set_X(screen_X);
		vertCoord_Screenf_shadow[i].set_Y(screen_Y);
		vertCoord_Screenf_shadow[i].set_Z(vertCoord_clipSpace_shadow[i].get_Z() / vertCoord_clipSpace_shadow[i].get_W());
	}
	int v1, v2, v3;
	Vector4D_float* vec = new Vector4D_float[3];
	//�����դ���л���
	int i = 0, j = 0;
	for (int i = 0; i < m.m_numSurfaces; i++)
	{
		for (int j = 0; j < m.m_surface[i].m_nFaces; j++)
		{
			v1 = m.m_surface[i].m_face[j].m_vert[0].m_vertIndex;
			v2 = m.m_surface[i].m_face[j].m_vert[1].m_vertIndex;
			v3 = m.m_surface[i].m_face[j].m_vert[2].m_vertIndex;
			int vIndex[3] = { v1,v2,v3 };
			//��ÿ����դ����Ϣ�浽shadow map��
			vec[0].setVec(vertCoord_Screenf_shadow[v1].get_X(), vertCoord_Screenf_shadow[v1].get_Y(), vertCoord_Screenf_shadow[v1].get_Z(), vertCoord_Screenf_shadow[v1].get_W());
			vec[1].setVec(vertCoord_Screenf_shadow[v2].get_X(), vertCoord_Screenf_shadow[v2].get_Y(), vertCoord_Screenf_shadow[v2].get_Z(), vertCoord_Screenf_shadow[v2].get_W());
			vec[2].setVec(vertCoord_Screenf_shadow[v3].get_X(), vertCoord_Screenf_shadow[v3].get_Y(), vertCoord_Screenf_shadow[v3].get_Z(), vertCoord_Screenf_shadow[v3].get_W());
			drawRaster_shadow(vec, vIndex, i, j, false);
		}
	}
}
//�ж�Clip Space�е�ĳ���Ƿ���shadow�У����ڵ�����Zֵ����shadow map�ж�ӦƬԪ��Zֵ���������õ���Ϣ�浽Ground Map��
bool isInShadow(const Vector4D_float P_worldSpace)
{
	Vector4D_float vectemp;
	Vector4D_float P_clipSpace_shadow = m_WLDtoCLP_shadow*P_worldSpace;
	float screen_X = P_clipSpace_shadow.get_X()*float(shadowMapWidth) / (P_clipSpace_shadow.get_W()*2.0f) + float(shadowMapWidth) / 2.0f;
	float screen_Y = P_clipSpace_shadow.get_Y()*float(shadowMapHeight) / (P_clipSpace_shadow.get_W()*2.0f) + float(shadowMapHeight) / 2.0f;
	float screen_Z = P_clipSpace_shadow.get_Z() / P_clipSpace_shadow.get_W();
	int _X = int(screen_X);
	int _Y = int(screen_Y);
	_X = _X > 0 ? _X : 0;
	_Y = _Y > 0 ? _Y : 0;
	_X = _X < shadowMapWidth ? _X : shadowMapWidth-1;
	_Y = _Y < shadowMapHeight ? _Y : shadowMapHeight-1;
	//����ȵ���ground map�ж�Ӧ������ʱ��������Ϣ����
	if (groundMap[_Y][_X] > screen_Z)
		groundMap[_Y][_X] = screen_Z;
	//�ж��Ƿ���shadow��
	if (screen_Z < _shadowMap[_Y][_X]) 
		return(false);
	else
		return(true);	
}
/*
bool isInShadow_new(const int _x, const int _y, int* vertexIndex)
{
	float m, n;
	float X_A = vertCoord_Screenf_Ground[vertexIndex[0]].get_X();
	float X_B = vertCoord_Screenf_Ground[vertexIndex[1]].get_X();
	float X_C = vertCoord_Screenf_Ground[vertexIndex[2]].get_X();
	float Y_A = vertCoord_Screenf_Ground[vertexIndex[0]].get_Y();
	float Y_B = vertCoord_Screenf_Ground[vertexIndex[1]].get_Y();
	float Y_C = vertCoord_Screenf_Ground[vertexIndex[2]].get_Y();

	float X_P = _x;
	float Y_P = _y;

	m = ((Y_C - Y_A)*(X_P - X_A) - (X_C - X_A)*(Y_P - Y_A)) / ((Y_C - Y_A)*(X_B - X_A) - (X_C - X_A)*(Y_B - Y_A));
	n = ((Y_B - Y_A)*(X_P - X_A) - (X_B - X_A)*(Y_P - Y_A)) / ((Y_B - Y_A)*(X_C - X_A) - (X_B - X_A)*(Y_C - Y_A));
	float X_P_shadow = vertZ_GroundToShadow[vertexIndex[0]].get_X() + m*(vertZ_GroundToShadow[vertexIndex[1]].get_X() - vertZ_GroundToShadow[vertexIndex[0]].get_X()) + n*(vertZ_GroundToShadow[vertexIndex[2]].get_X() - vertZ_GroundToShadow[vertexIndex[0]].get_X());
	float Y_P_shadow = vertZ_GroundToShadow[vertexIndex[0]].get_Y() + m*(vertZ_GroundToShadow[vertexIndex[1]].get_Y() - vertZ_GroundToShadow[vertexIndex[0]].get_Y()) + n*(vertZ_GroundToShadow[vertexIndex[2]].get_Y() - vertZ_GroundToShadow[vertexIndex[0]].get_Y());
	float Z_P_shadow = vertZ_GroundToShadow[vertexIndex[0]].get_Z() + m*(vertZ_GroundToShadow[vertexIndex[1]].get_Z() - vertZ_GroundToShadow[vertexIndex[0]].get_Z()) + n*(vertZ_GroundToShadow[vertexIndex[2]].get_Z() - vertZ_GroundToShadow[vertexIndex[0]].get_Z());

	int _X = int(X_P_shadow);
	int _Y = int(Y_P_shadow);
	_X = _X > 0 ? _X : 0;
	_Y = _Y > 0 ? _Y : 0;

	if (groundMap[_Y][_X] > Z_P_shadow)
		groundMap[_Y][_X] = Z_P_shadow;

	if (Z_P_shadow < _shadowMap[_Y][_X])
		return(false);
	else
		return(true);
}
*/
//����surfaceindex��������ɫ
void setGroundColor(int surfaceindex)
{
	switch (surfaceindex)
	{
	case 0:
		{
		glColor3f(0.5, 0, 0);
		break;
		}
	case 1:
	{
		glColor3f(1, 0, 0);
		break;
	}
	case 2:
	{
		glColor3f(0, 0.5, 0);
		break;
	}
	case 3:
	{
		glColor3f(0, 1, 0);
		break;
	}
	case 4:
	{
		glColor3f(0, 0, 0.5);
		break;
	}
	case 5:
	{
		glColor3f(0, 0, 1);
		break;
	}
	default:
		break;
	}
}

//�����ڵ�����������ǹ�դ������Ļ��������World Space�е����꣬���ж��Ƿ�����Ӱ��
void drawUpRaster_Ground(int* x, int* y, float* z, int* vertexIndex, int surfaceindex, int faceindex, bool ifTexture)
{
	int x_left, x_right;
	float z_left, z_right, z_temp;
	for (int i = y[0]; i >= y[2]; i--)
	{
		solveUpRasterBound(x_left, x_right, z_left, z_right, x, y, z, i); //get the proper x/z_left, x/z_right

		if (x_left == x_right) //ֻ��һ���㣬���ұ߽��غ�
		{
			//����õ���ƬԪ������ռ��е�����
			Vector4D_float v = getCoordWorld_Ground(m2, surfaceindex, faceindex, x_left, i);
			//�ж�����ռ��и������Ƿ�����Ӱ�У�������ground map
			if (isInShadow(v))
			{
				//����Ӱ��
				glColor3f(0, 0, 0);
			}
			else
			{
				//������Ӱ��
				setGroundColor(surfaceindex);
			}
			drawPixel(x_left, i, z_left);
		}
		
		else
		{
			for (int j = x_left; j <= x_right; j++)
			{
				if (z_left == z_right)
					z_temp = z_left;
				else
				{
					float t = float(j - x_left) / float(x_right - x_left);
					z_temp = (z_right - z_left)*((float)j - (float)x_left) / ((float)x_right - (float)x_left) + z_left;
				}
				//drawPixel_shadow(j, i, z_temp);
				Vector4D_float v = getCoordWorld_Ground(m2, surfaceindex, faceindex, j, i);
				if (isInShadow(v))
				{
					glColor3f(0, 0, 0);
				}
				else
				{
					setGroundColor(surfaceindex);
				}
				drawPixel(j, i, z_temp);
			}
		}
		
	}
}

//������
void drawLowRaster_Ground(int* x, int* y, float* z, int* vertexIndex, int surfaceindex, int faceindex, bool ifTexture)
{
	int x_left, x_right;
	float z_left, z_right, z_temp;
	for (int i = y[0]; i >= y[2]; i--)
	{
		solveLowRasterBound(x_left, x_right, z_left, z_right, x, y, z, i); //get the proper x/z_left, x/z_right

		if (x_left == x_right) //ֻ��һ���㣬���ұ߽��غ�
		{
			Vector4D_float v = getCoordWorld_Ground(m2, surfaceindex, faceindex, x_left, i);
			if (isInShadow(v))
			{
				glColor3f(0, 0, 0);
			}
			else
			{
				setGroundColor(surfaceindex);
			}
			drawPixel(x_left, i, z_left);
		}
		
		else
		{
			for (int j = x_left; j <= x_right; j++)
			{
				if (z_left == z_right)
					z_temp = z_left;
				else
				{
					float t = float(j - x_left) / float(x_right - x_left);
					z_temp = (z_right - z_left)*((float)j - (float)x_left) / ((float)x_right - (float)x_left) + z_left;
				}
				Vector4D_float v = getCoordWorld_Ground(m2, surfaceindex, faceindex, j, i);
				if (isInShadow(v))
				{
					glColor3f(0, 0, 0);
				}
				else
				{
					setGroundColor(surfaceindex);
				}
				drawPixel(j, i, z_temp);
			}
		}
		
	}
}
//�����ڵ�����Ĺ�դ
void drawRaster_Ground(Vector4D_float* vec, int* vIndex, int surfaceindex, int faceindex, bool ifTexture)
{

	int x[3], y[3];
	x[0] = vec[0].get_X(), x[1] = vec[1].get_X(), x[2] = vec[2].get_X(), y[0] = vec[0].get_Y(), y[1] = vec[1].get_Y(), y[2] = vec[2].get_Y(); //(x[0],y[0])��ߵ㣬(x[2],y[2])��͵�
	float z[3];
	z[0] = vec[0].get_Z(), z[1] = vec[1].get_Z(), z[2] = vec[2].get_Z();

	if (y[1] <= y[2]) swap(x[1], x[2], y[1], y[2], z[1], z[2], vIndex[1], vIndex[2]);
	if (y[0] <= y[1]) swap(x[0], x[1], y[0], y[1], z[0], z[1], vIndex[0], vIndex[1]);
	if (y[1] <= y[2]) swap(x[1], x[2], y[1], y[2], z[1], z[2], vIndex[1], vIndex[2]);

	if (y[0] == y[1])
	{
		drawUpRaster_Ground(x, y, z, vIndex, surfaceindex, faceindex, ifTexture);
		return;
	}
	else if (y[2] == y[1])
	{
		drawLowRaster_Ground(x, y, z, vIndex, surfaceindex, faceindex, ifTexture);
		return;
	}
	//�ȷ�������Ҳ�������ǵ����ͣ���ֳ�������+������Ȼ��ݹ�
	int tmpx = int(((float)y[1] - (float)y[2])*((float)x[0] - (float)x[2]) / ((float)y[0] - (float)y[2]) + x[2]);
	float tmpz;
	if (z[0] == z[2])
	{
		tmpz = z[0];
	}
	else
	{
		tmpz = ((float)y[1] - (float)y[2])*(z[0] - z[2]) / ((float)y[0] - (float)y[2]) + z[2];
	}
	Vector4D_float* vec_up = new Vector4D_float[3];
	Vector4D_float* vec_down = new Vector4D_float[3];
	vec_up[0].setVec(x[0], y[0], z[0], 1);
	vec_up[1].setVec(x[1], y[1], z[1], 1);
	vec_up[2].setVec(tmpx, y[1], tmpz, 1);
	drawRaster_Ground(vec_up, vIndex, surfaceindex, faceindex, ifTexture);

	vec_down[0].setVec(x[2], y[2], z[2], 1);
	vec_down[1].setVec(x[1], y[1], z[1], 1);
	vec_down[2].setVec(tmpx, y[1], tmpz, 1);
	drawRaster_Ground(vec_down, vIndex, surfaceindex, faceindex, ifTexture);
	return;
}

//�����ڵ�����
void drawGround(Mesh& m, int mode)
{
	//��ʼ��
	groundMap = new float*[shadowMapHeight];
	for (int i = 0; i < shadowMapHeight; i++)
	{
		groundMap[i] = new float[shadowMapWidth];
		for (int j = 0; j < shadowMapWidth; j++)
			groundMap[i][j] = 1.01f;
	}
//	clearZbuffer();
	Matrix4D_float mat_Trans;
	coordTrans_Ground(m, mat_Trans);

	if (vertCoord_clipSpace_Ground != NULL) delete[] vertCoord_clipSpace_Ground;
	vertCoord_clipSpace_Ground = new Vector4D_float[m.m_numVerts];

	if (vertCoord_Screenf_Ground != NULL) delete[] vertCoord_Screenf_Ground;
	vertCoord_Screenf_Ground = new Vector4D_float[m.m_numVerts];

	if (vertCoord_worldSpace_Ground != NULL) delete[] vertCoord_worldSpace_Ground;
	vertCoord_worldSpace_Ground = new Vector4D_float[m.m_numVerts];

	if (vertZ_GroundToShadow != NULL) delete[] vertZ_GroundToShadow;
	vertZ_GroundToShadow = new Vector4D_float[m.m_numVerts];
	
	//��ģ�Ϳռ䵽��Ļ�ռ�ı任����
	float screen_X, screen_Y;
	for (int i = 0; i < m.m_numVerts; i++)
	{
		vertCoord_clipSpace_Ground[i] = m.m_pt[i]; //ģ������ϵ�е����ж��������
		vertCoord_clipSpace_Ground[i] = mat_Trans * vertCoord_clipSpace_Ground[i]; //�ü�����ϵ�ж�Ӧ�����ж��������

		vertCoord_worldSpace_Ground[i] = m.m_pt[i];
		vertCoord_worldSpace_Ground[i] = m_MDLtoWLD_Ground * vertCoord_worldSpace_Ground[i];//TODO:20170803 ��������ϵ�ж�Ӧ�����ж��������
																					 //--From clip space to Screen---
		screen_X = vertCoord_clipSpace_Ground[i].get_X()*800.0f / (vertCoord_clipSpace_Ground[i].get_W()*2.0f) + 800.0f / 2.0f;
		screen_Y = vertCoord_clipSpace_Ground[i].get_Y()*800.0f / (vertCoord_clipSpace_Ground[i].get_W()*2.0f) + 800.0f / 2.0f;
		vertCoord_Screenf_Ground[i].set_X(screen_X);
		vertCoord_Screenf_Ground[i].set_Y(screen_Y);
		vertCoord_Screenf_Ground[i].set_Z(vertCoord_clipSpace_Ground[i].get_Z() / vertCoord_clipSpace_Ground[i].get_W());//X��YΪ��Ļ�ռ�����ϵ�е����꣬ZΪ���
		//TODO 0804 �������ǰ�W��Ϊ1��
		vertCoord_Screenf_Ground[i].set_W(1);

		Vector4D_float P_clipSpace_shadow = m_WLDtoCLP_shadow * vertCoord_worldSpace_Ground[i];
		float screen_X = P_clipSpace_shadow.get_X()*float(shadowMapWidth) / (P_clipSpace_shadow.get_W()*2.0f) + float(shadowMapWidth) / 2.0f;
		float screen_Y = P_clipSpace_shadow.get_Y()*float(shadowMapHeight) / (P_clipSpace_shadow.get_W()*2.0f) + float(shadowMapHeight) / 2.0f;
		float screen_Z = P_clipSpace_shadow.get_Z() / P_clipSpace_shadow.get_W();
		vertZ_GroundToShadow[i].setVec(screen_X , screen_Y , screen_Z, 1); //�������ж�����shadowmap�µ����ֵ
	}
	int v1, v2, v3;
	Vector4D_float* vec = new Vector4D_float[3];

	//�����դ���л���
	int i = 0, j = 0;
	for (int i = 0; i < m.m_numSurfaces; i++)
	{
		for (int j = 0; j < m.m_surface[i].m_nFaces; j++)
		{
			v1 = m.m_surface[i].m_face[j].m_vert[0].m_vertIndex;
			v2 = m.m_surface[i].m_face[j].m_vert[1].m_vertIndex;
			v3 = m.m_surface[i].m_face[j].m_vert[2].m_vertIndex;
			int vIndex[3] = { v1,v2,v3 };
			vec[0].setVec(vertCoord_Screenf_Ground[v1].get_X(), vertCoord_Screenf_Ground[v1].get_Y(), vertCoord_Screenf_Ground[v1].get_Z(), 1);
			vec[1].setVec(vertCoord_Screenf_Ground[v2].get_X(), vertCoord_Screenf_Ground[v2].get_Y(), vertCoord_Screenf_Ground[v2].get_Z(), 1);
			vec[2].setVec(vertCoord_Screenf_Ground[v3].get_X(), vertCoord_Screenf_Ground[v3].get_Y(), vertCoord_Screenf_Ground[v3].get_Z(), 1);
			//glColor3f(GLfloat(0.5), GLfloat(0.5), GLfloat(0.5));
			drawRaster_Ground(vec, vIndex, i, j, false);
		}
	}
}
//���ӻ�����ground map����ʾ�����Ͻ�
void showGroundMap()
{
	for (int i = 0; i < shadowMapHeight; i++)
	{
		for (int j = 0; j < shadowMapWidth; j++)
		{
			glColor3f(groundMap[i][j], groundMap[i][j], groundMap[i][j]);
			if (groundMap[i][j] == 1.01f)
				glColor3f(1, 0, 0);
			glVertex2i(j+400, i+400);
		}
	}
}