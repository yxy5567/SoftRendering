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

//二维数组分别存储遮挡物的shadow map和被遮挡物的map
float ** _shadowMap;
float ** groundMap;
//网状体对象
Mesh m1_shadow;
//int testcount_shadowmap;
//int testcount_drawground;
//顶点坐标数组
Vector4D_float* vertCoord_Screenf_shadow;
Vector4D_float* vertCoord_clipSpace_shadow;
Vector4D_float* vertCoord_worldSpace_shadow;
//暂时用不上
Vector4D_float* vertZ_GroundToShadow;
//shadow map中世界空间到裁剪空间中的坐标
Matrix4D_float m_WLDtoCLP_shadow, m_MDLtoWLD_shadow, m_MDLtoCLP_shadow;
//函数声明
bool isInShadow(const Vector4D_float P_worldSpace);

//将shadow map可视化地显示在左下角
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
//初始化shadowmap，每一帧需要重新初始化一次
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
//创建shadow map时进行的坐标变换，并且存储中间矩阵
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
	m4.NegnateZ();//必要，永远要有
	mat_WLDtoVIW = m4*m3*mat2*mat1*mat_WLDtoVIW;
	//---From view space to clip space NDC--- z range [-w,w]
	mat1.MatrixFrustum(60, 1, 1, 15);
	mat_VIWtoCLP = mat1;
	//存储中间矩阵
	m_WLDtoCLP_shadow = mat_VIWtoCLP*mat_WLDtoVIW;
	m_MDLtoWLD_shadow = mat_MDLtoWLD;
	m_MDLtoCLP_shadow = mat_Trans;
	mat_Trans = mat_VIWtoCLP*mat_WLDtoVIW*mat_MDLtoWLD;
//		mat_Trans.showMatrix(); //for testing
	return;
}
//将制定片元的信息存储到shadowmap中
void drawPixel_shadow(int x,int y,float z)
{
	if (x >= 0 && x <= shadowMapWidth - 1 && y >= 0 && y <= shadowMapHeight - 1)
	{
		//SOLVED BUG(2017 08 08）：横纵坐标弄反了
		if (_shadowMap[y][x] > z)
		{
			_shadowMap[y][x] = z;
		}
	}
}
//生成shadow map时画上三角光栅
void drawUpRaster_shadow(int* x, int* y, float* z, int* vertexIndex, int surfaceindex, int faceindex, bool ifTexture)
{
	int x_left, x_right;
	float z_left, z_right, z_temp;
	for (int i = y[0]; i >= y[2]; i--)
	{
		//get the proper x/z_left, x/z_right
		solveUpRasterBound(x_left, x_right, z_left, z_right, x, y, z, i); 
		//只有一个点，左右边界重合
		if (x_left == x_right) 
		{
			//在这里改变shadowMap数组的值
			drawPixel_shadow(x_left, i, z_left);
		}
		//左右边界不重合	
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
				//在这里改变shadowMap数组的值
				drawPixel_shadow(j, i, z_temp);
			}
		}
	}
}
//生成shadow map时画下三角光栅
void drawLowRaster_shadow(int* x, int* y, float* z, int* vertexIndex, int surfaceindex, int faceindex, bool ifTexture)
{
	int x_left, x_right;
	float z_left, z_right, z_temp;
	for (int i = y[0]; i >= y[2]; i--)
	{
		//get the proper x/z_left, x/z_right
		solveLowRasterBound(x_left, x_right, z_left, z_right, x, y, z, i); 
		//只有一个点，左右边界重合
		if (x_left == x_right) 
		{
			//在这里改变shadowMap数组的值
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
				//在这里改变shadowMap数组的值
				drawPixel_shadow(j, i, z_temp);
			}
		}
	}
}
//生成shadow map时光栅绘制函数
void drawRaster_shadow(Vector4D_float* vec, int* vIndex, int surfaceindex, int faceindex, bool ifTexture)
{
	int x[3], y[3];
	x[0] = vec[0].get_X(), x[1] = vec[1].get_X(), x[2] = vec[2].get_X(), y[0] = vec[0].get_Y(), y[1] = vec[1].get_Y(), y[2] = vec[2].get_Y(); //(x[0],y[0])最高点，(x[2],y[2])最低点
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
	//既非上三角也非下三角的类型，拆分成上三角+下三角然后递归
	int tmpx = int(((float)y[1] - (float)y[2])*((float)x[0] - (float)x[2]) / ((float)y[0] - (float)y[2]) + x[2]);
	float tmpz;
	if (z[0] == z[2])
	{
		tmpz = z[0];
	}
	else
	{
		//TODO:20170804 NDC中Z的插值
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
//生成shadow map时画网状体函数
void drawMesh_shadow(Mesh& m, int mode)
{
	//初始化
	clearShadowMap();
	Matrix4D_float mat_Trans;
	//坐标转换（从模型坐标系到裁剪坐标系）
	coordTrans_shadowMap(m1_shadow, mat_Trans);
	//顶点的屏幕坐标
	if (vertCoord_Screenf_shadow != NULL) delete[] vertCoord_Screenf_shadow;
	vertCoord_Screenf_shadow = new Vector4D_float[m.m_numVerts];
	//顶点的裁剪空间坐标
	if (vertCoord_clipSpace_shadow != NULL) delete[] vertCoord_clipSpace_shadow;
	vertCoord_clipSpace_shadow = new Vector4D_float[m.m_numVerts];
	//顶点的世界空间坐标
	if (vertCoord_worldSpace_shadow != NULL) delete[] vertCoord_worldSpace_shadow;
	vertCoord_worldSpace_shadow = new Vector4D_float[m.m_numVerts];
	
	//从模型空间到屏幕空间的变换计算
	float screen_X, screen_Y;
	for (int i = 0; i < m.m_numVerts; i++)
	{
		vertCoord_clipSpace_shadow[i] = m.m_pt[i]; //模型坐标系中的所有顶点的坐标
		vertCoord_clipSpace_shadow[i] = mat_Trans * vertCoord_clipSpace_shadow[i]; //裁剪坐标系中对应的所有顶点的坐标

		//从裁剪坐标系到shadow map的“虚拟屏幕”空间
		screen_X = vertCoord_clipSpace_shadow[i].get_X()*float(shadowMapWidth) / (vertCoord_clipSpace_shadow[i].get_W()*2.0f) + float(shadowMapWidth) / 2.0f;
		screen_Y = vertCoord_clipSpace_shadow[i].get_Y()*float(shadowMapHeight) / (vertCoord_clipSpace_shadow[i].get_W()*2.0f) + float(shadowMapHeight) / 2.0f;
		//X，Y为像素坐标，Z为深度
		vertCoord_Screenf_shadow[i].set_X(screen_X);
		vertCoord_Screenf_shadow[i].set_Y(screen_Y);
		vertCoord_Screenf_shadow[i].set_Z(vertCoord_clipSpace_shadow[i].get_Z() / vertCoord_clipSpace_shadow[i].get_W());
	}
	int v1, v2, v3;
	Vector4D_float* vec = new Vector4D_float[3];
	//逐个光栅进行绘制
	int i = 0, j = 0;
	for (int i = 0; i < m.m_numSurfaces; i++)
	{
		for (int j = 0; j < m.m_surface[i].m_nFaces; j++)
		{
			v1 = m.m_surface[i].m_face[j].m_vert[0].m_vertIndex;
			v2 = m.m_surface[i].m_face[j].m_vert[1].m_vertIndex;
			v3 = m.m_surface[i].m_face[j].m_vert[2].m_vertIndex;
			int vIndex[3] = { v1,v2,v3 };
			//将每个光栅的信息存到shadow map中
			vec[0].setVec(vertCoord_Screenf_shadow[v1].get_X(), vertCoord_Screenf_shadow[v1].get_Y(), vertCoord_Screenf_shadow[v1].get_Z(), vertCoord_Screenf_shadow[v1].get_W());
			vec[1].setVec(vertCoord_Screenf_shadow[v2].get_X(), vertCoord_Screenf_shadow[v2].get_Y(), vertCoord_Screenf_shadow[v2].get_Z(), vertCoord_Screenf_shadow[v2].get_W());
			vec[2].setVec(vertCoord_Screenf_shadow[v3].get_X(), vertCoord_Screenf_shadow[v3].get_Y(), vertCoord_Screenf_shadow[v3].get_Z(), vertCoord_Screenf_shadow[v3].get_W());
			drawRaster_shadow(vec, vIndex, i, j, false);
		}
	}
}
//判断Clip Space中的某点是否在shadow中（被遮挡，其Z值大于shadow map中对应片元的Z值），并将该点信息存到Ground Map中
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
	//当深度低于ground map中对应点的深度时，将其信息更新
	if (groundMap[_Y][_X] > screen_Z)
		groundMap[_Y][_X] = screen_Z;
	//判断是否在shadow中
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
//根据surfaceindex决定其颜色
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

//画被遮挡物体的上三角光栅，从屏幕坐标计算出World Space中的坐标，并判断是否在阴影中
void drawUpRaster_Ground(int* x, int* y, float* z, int* vertexIndex, int surfaceindex, int faceindex, bool ifTexture)
{
	int x_left, x_right;
	float z_left, z_right, z_temp;
	for (int i = y[0]; i >= y[2]; i--)
	{
		solveUpRasterBound(x_left, x_right, z_left, z_right, x, y, z, i); //get the proper x/z_left, x/z_right

		if (x_left == x_right) //只有一个点，左右边界重合
		{
			//这里得到该片元的世界空间中的坐标
			Vector4D_float v = getCoordWorld_Ground(m2, surfaceindex, faceindex, x_left, i);
			//判断世界空间中该坐标是否在阴影中，并更新ground map
			if (isInShadow(v))
			{
				//在阴影中
				glColor3f(0, 0, 0);
			}
			else
			{
				//不在阴影中
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

//下三角
void drawLowRaster_Ground(int* x, int* y, float* z, int* vertexIndex, int surfaceindex, int faceindex, bool ifTexture)
{
	int x_left, x_right;
	float z_left, z_right, z_temp;
	for (int i = y[0]; i >= y[2]; i--)
	{
		solveLowRasterBound(x_left, x_right, z_left, z_right, x, y, z, i); //get the proper x/z_left, x/z_right

		if (x_left == x_right) //只有一个点，左右边界重合
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
//画被遮挡物体的光栅
void drawRaster_Ground(Vector4D_float* vec, int* vIndex, int surfaceindex, int faceindex, bool ifTexture)
{

	int x[3], y[3];
	x[0] = vec[0].get_X(), x[1] = vec[1].get_X(), x[2] = vec[2].get_X(), y[0] = vec[0].get_Y(), y[1] = vec[1].get_Y(), y[2] = vec[2].get_Y(); //(x[0],y[0])最高点，(x[2],y[2])最低点
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
	//既非上三角也非下三角的类型，拆分成上三角+下三角然后递归
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

//画被遮挡物体
void drawGround(Mesh& m, int mode)
{
	//初始化
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
	
	//从模型空间到屏幕空间的变换计算
	float screen_X, screen_Y;
	for (int i = 0; i < m.m_numVerts; i++)
	{
		vertCoord_clipSpace_Ground[i] = m.m_pt[i]; //模型坐标系中的所有顶点的坐标
		vertCoord_clipSpace_Ground[i] = mat_Trans * vertCoord_clipSpace_Ground[i]; //裁剪坐标系中对应的所有定点的坐标

		vertCoord_worldSpace_Ground[i] = m.m_pt[i];
		vertCoord_worldSpace_Ground[i] = m_MDLtoWLD_Ground * vertCoord_worldSpace_Ground[i];//TODO:20170803 世界坐标系中对应的所有顶点的坐标
																					 //--From clip space to Screen---
		screen_X = vertCoord_clipSpace_Ground[i].get_X()*800.0f / (vertCoord_clipSpace_Ground[i].get_W()*2.0f) + 800.0f / 2.0f;
		screen_Y = vertCoord_clipSpace_Ground[i].get_Y()*800.0f / (vertCoord_clipSpace_Ground[i].get_W()*2.0f) + 800.0f / 2.0f;
		vertCoord_Screenf_Ground[i].set_X(screen_X);
		vertCoord_Screenf_Ground[i].set_Y(screen_Y);
		vertCoord_Screenf_Ground[i].set_Z(vertCoord_clipSpace_Ground[i].get_Z() / vertCoord_clipSpace_Ground[i].get_W());//X，Y为屏幕空间坐标系中的坐标，Z为深度
		//TODO 0804 这里忘记把W置为1了
		vertCoord_Screenf_Ground[i].set_W(1);

		Vector4D_float P_clipSpace_shadow = m_WLDtoCLP_shadow * vertCoord_worldSpace_Ground[i];
		float screen_X = P_clipSpace_shadow.get_X()*float(shadowMapWidth) / (P_clipSpace_shadow.get_W()*2.0f) + float(shadowMapWidth) / 2.0f;
		float screen_Y = P_clipSpace_shadow.get_Y()*float(shadowMapHeight) / (P_clipSpace_shadow.get_W()*2.0f) + float(shadowMapHeight) / 2.0f;
		float screen_Z = P_clipSpace_shadow.get_Z() / P_clipSpace_shadow.get_W();
		vertZ_GroundToShadow[i].setVec(screen_X , screen_Y , screen_Z, 1); //存了所有顶点在shadowmap下的深度值
	}
	int v1, v2, v3;
	Vector4D_float* vec = new Vector4D_float[3];

	//逐个光栅进行绘制
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
//可视化表现ground map，显示在右上角
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