#pragma once
////---------------------------------------------------------------------
////    Basic Mesh Drawing Function
////
////    Programmer:             Yang Xiaoyang
////    Last modification date: 2017-7-31
////    Creation date:          2017-7-19
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

//模型空间到世界空间的变换阵
Matrix4D_float m_MDLtoWLD;
//世界空间到ClipSpace中的变换阵
Matrix4D_float m_WLDtoCLP;
//裁剪空间中的顶点坐标数组
Vector4D_float* vertCoord_clipSpace;
//屏幕空间中的顶点坐标数组
Vector4D_float* vertCoord_Screenf;
//世界空间中的顶点坐标数组
Vector4D_float* vertCoord_worldSpace;
//每个顶点的RGB值，vertR_worldSpace[i][j]代表了第i个surface中的vertIndex=j(0~7不是0~3）的R值
float** vertR_worldSpace, ** vertG_worldSpace, ** vertB_worldSpace;
//网状体对象
Mesh m1,m2;
//互斥锁
std::mutex mtx1, mtx2;
//下三角光栅绘制多线程函数调用flag1（代表第i行有没有线程绘制过）,上三角用flag2
bool* flag1, *flag2; 
//光栅绘制函数声明
void drawRaster(Vector4D_float* vec,int* vIndex, int surfaceindex, int faceindex,bool ifTexture);
//多线程方式光栅绘制函数声明
void drawRaster_thread(Vector4D_float* vec, int surfaceindex, int faceindex, bool ifTexture);
//初始化纹理矩阵
void initTexture();
unsigned long up_t1_count, up_t2_count = 0, up_t3_count = 0, down_t1_count = 0, down_t2_count = 0, down_t3_count = 0;
//由于光照，叠加在物体表面的RGB值
float additional_R = 0, additional_G = 0, additional_B = 0;
//坐标转换函数：模型空间到世界空间，到视空间，到裁剪空间
void coordTrans(Mesh& m, Matrix4D_float& mat_Trans)
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
	mat2.MatrixShift(0, -0.3+y_shift, 0);
	mat_MDLtoWLD = mat2*mat1*m3*m4*m5;

	//将从模型坐标系到世界坐标系的变换矩阵存储出来，用于计算漫反射
	m_MDLtoWLD = mat_MDLtoWLD; 

	//---From world space to view space---
	mat1.MatrixShift(-1, -0.8, 3);
	mat2.MatrixRotate_X(-10);
	m3.MatrixRotate_Y(15);
	m4.NegnateZ();//必要，永远要有
	mat_WLDtoVIW = m4*m3*mat2*mat1*mat_WLDtoVIW;
	//---From view space to clip space NDC--- z range [-w,w]
	mat1.MatrixFrustum(60, 1, 1, 15);
	mat_VIWtoCLP = mat1;

	mat_Trans = mat_VIWtoCLP*mat_WLDtoVIW*mat_MDLtoWLD;
	//	mat_Trans.showMatrix(); //for testing
	return;
}
//底板的坐标转换
void coordTrans_Ground(Mesh& m, Matrix4D_float& mat_Trans)
{
	Matrix4D_float mat_MDLtoWLD, mat_WLDtoVIW, mat_VIWtoCLP;
	mat_MDLtoWLD.unitMatrix();
	mat_WLDtoVIW.unitMatrix();
	mat_VIWtoCLP.unitMatrix();
	//Transformation Operations;
	Matrix4D_float mat1, mat2, m3, m4, m5, m6;
	//---From model space to world space---
	mat1.MatrixRotate_X(0);
	mat2.MatrixShift(0, -1, 0);
	mat_MDLtoWLD = mat1*mat2;

	//将从模型坐标系到世界坐标系的变换矩阵存储出来，用于计算漫反射
	m_MDLtoWLD = mat_MDLtoWLD;

	//---From world space to view space---
	mat1.MatrixShift(-1, -0.8, 3);
	mat2.MatrixRotate_X(-10);
	m3.MatrixRotate_Y(15);
	m4.NegnateZ();//必要，永远要有
	mat_WLDtoVIW = m4*m3*mat2*mat1*mat_WLDtoVIW;
	//---From view space to clip space NDC--- z range [-w,w]
	mat1.MatrixFrustum(60, 1, 1, 15);
	mat_VIWtoCLP = mat1;

	mat_Trans = mat_VIWtoCLP*mat_WLDtoVIW*mat_MDLtoWLD;
	//	mat_Trans.showMatrix(); //for testing
	return;
}
//将bliss.bmp的内容读取到纹理矩阵中
void initBitmap()
{
	static GLint pixellength;
	static GLubyte* pixeldata;
	//打开文件
	FILE* pfile = fopen("bliss.bmp", "rb");
	if (pfile == 0) exit(0);
	//读取图像大小
	fseek(pfile, 0x0012, SEEK_SET);
	fread(&imagewidth, sizeof(imagewidth), 1, pfile);
	fread(&imageheight, sizeof(imageheight), 1, pfile);
	//计算像素数据长度
	pixellength = imagewidth * 3;
	while (pixellength % 4 != 0)pixellength++;
	pixellength *= imageheight;
	//读取像素数据
	pixeldata = (GLubyte*)malloc(pixellength);
	if (pixeldata == 0) exit(0);
	fseek(pfile, 54, SEEK_SET);
	fread(pixeldata, pixellength, 1, pfile);
	//把pixeldata中的像素数据输出到像素矩阵中
	txtr_RGB = new GLubyte**[imageheight];
	for (int i = 0; i < imageheight; i++)
	{
		txtr_RGB[i] = new GLubyte *[imageheight];
		for (int j = 0; j < imagewidth; j++)
		{
			txtr_RGB[i][j] = new GLubyte[3];
		}

	}
	GLubyte *pixeldata_temp = pixeldata;

	int extraByte_num = (pixellength / imageheight) % 3;
	for (int i = 0; i < imageheight; i++)
	{
		for (int j = 0; j < imagewidth; j++)
		{
			txtr_RGB[i][j][0] = *pixeldata_temp; //R
			pixeldata_temp++;
			txtr_RGB[i][j][1] = *pixeldata_temp; //G
			pixeldata_temp++;
			txtr_RGB[i][j][2] = *pixeldata_temp; //B
			pixeldata_temp++;
		}
		pixeldata_temp += extraByte_num;
	}
	//关闭文件
	fclose(pfile);
	free(pixeldata);
}
//将纹理矩阵设置为黑白格
void initBitmap2()
{
	imagewidth = 400;
	imageheight = 400;

	txtr_RGB = new GLubyte**[imageheight];
	for (int i = 0; i < imageheight; i++)
	{
		txtr_RGB[i] = new GLubyte *[imageheight];
		for (int j = 0; j < imagewidth; j++)
		{
			txtr_RGB[i][j] = new GLubyte[3];
		}
	}
	for (int i = 0; i < imageheight; i++)
	{
		for (int j = 0; j < imagewidth; j++)
		{
			if( (i % 80 < 40 && j % 80 < 40)|| (i % 80 > 40 && j % 80 > 40) )
			{
				txtr_RGB[i][j][0] = GLubyte(255); //R
				txtr_RGB[i][j][1] = GLubyte(255); //G
				txtr_RGB[i][j][2] = GLubyte(255); //B
			}
			else
			{
				txtr_RGB[i][j][0] = GLubyte(0); //R
				txtr_RGB[i][j][1] = GLubyte(0); //G
				txtr_RGB[i][j][2] = GLubyte(0); //B
			}
		}
	}
}
//初始化纹理映射
void initTexture()
{
	textCoord_U = new float ** [imageheight];
	textCoord_V = new float ** [imageheight];
	for (int i = 0; i < imageheight; i++)
	{
		textCoord_U[i] = new float*[imagewidth];
		textCoord_V[i] = new float*[imagewidth];
		for (int j = 0; j < imagewidth; j++)
		{
			textCoord_U[i][j] = new float[3];
			textCoord_V[i][j] = new float[3];
		}
	}
	for (int i = 0; i < 6; i++)
	{
		//surface 0 face 0
		textCoord_U[i][0][0] = 0;
		textCoord_V[i][0][0] = 0;
		textCoord_U[i][0][1] = 1;
		textCoord_V[i][0][1] = 0;
		textCoord_U[i][0][2] = 0;
		textCoord_V[i][0][2] = 1;
		//
		textCoord_U[i][1][0] = 0;
		textCoord_V[i][1][0] = 1;
		textCoord_U[i][1][1] = 1;
		textCoord_V[i][1][1] = 0;
		textCoord_U[i][1][2] = 1;
		textCoord_V[i][1][2] = 1;
	}

}
//画网状体函数
void drawMesh(Mesh& m,int mode)
{
	//初始化
	clearZbuffer();
	Matrix4D_float mat_Trans;
	coordTrans(m, mat_Trans);
	if (vertCoord_clipSpace != NULL) delete[] vertCoord_clipSpace;
	vertCoord_clipSpace = new Vector4D_float[m.m_numVerts];
	if (vertCoord_Screenf != NULL) delete[] vertCoord_Screenf;
	vertCoord_Screenf = new Vector4D_float[m.m_numVerts];
	//mode=5，点光源模式，需要得到所有顶点在世界坐标系中的坐标
	if (_showMode == 5)
	{
		if (vertCoord_worldSpace != NULL) delete[] vertCoord_worldSpace;
		vertCoord_worldSpace = new Vector4D_float[m.m_numVerts];

		if (vertR_worldSpace == NULL)
		{
			vertR_worldSpace = new float*[m.m_numSurfaces];
			for (int i = 0; i < m.m_numSurfaces; i++)
				vertR_worldSpace[i] = new float[m.m_numVerts];
		}
		if (vertG_worldSpace == NULL)
		{
			vertG_worldSpace = new float*[m.m_numSurfaces];
			for (int i = 0; i < m.m_numSurfaces; i++)
				vertG_worldSpace[i] = new float[m.m_numVerts];
		}
		if (vertB_worldSpace == NULL)
		{
			vertB_worldSpace = new float*[m.m_numSurfaces];
			for (int i = 0; i < m.m_numSurfaces; i++)
				vertB_worldSpace[i] = new float[m.m_numVerts];
		}

		for (int i = 0; i < m.m_numVerts; i++)
		{
			vertCoord_worldSpace[i] = m_MDLtoWLD*m.m_pt[i];

			Vector4D_float Ambient_RGB_vec;
			set_AmbientLightVec(Ambient_RGB_vec);
			Vector4D_float Diffuse_RGB_vec(diffuseOrigin_R, diffuseOrigin_G, diffuseOrigin_B, 0);
			Vector4D_float Diffuse_RGB_vec_trans;
			//光源向量=目标顶点的坐标向量-光源点的向量
			Vector4D_float L_vec(vec_WorldPointLight_x-vertCoord_worldSpace[i].get_X(), vec_WorldPointLight_x-vertCoord_worldSpace[i].get_Y(), vec_WorldPointLight_x-vertCoord_worldSpace[i].get_Z(), 0);
			for (int j = 0; j < m.m_numSurfaces; j++)
			{
				set_DiffuseLightVec_Point(Diffuse_RGB_vec, Diffuse_RGB_vec_trans, m.m_norm[j], m_MDLtoWLD, L_vec);
				float _R = Ambient_RGB_vec.get_X() + Diffuse_RGB_vec_trans.get_X();
				float _G = Ambient_RGB_vec.get_Y() + Diffuse_RGB_vec_trans.get_Y();
				float _B = Ambient_RGB_vec.get_Z() + Diffuse_RGB_vec_trans.get_Z();
				//记录每个顶点的RGB值
				vertR_worldSpace[j][i] = _R;
				vertG_worldSpace[j][i] = _G;
				vertB_worldSpace[j][i] = _B;
			}
		}
	}
	//从模型空间到屏幕空间的变换计算
	float screen_X, screen_Y;
	for (int i = 0; i < m.m_numVerts; i++)
	{
		vertCoord_clipSpace[i] = m.m_pt[i]; //模型坐标系中的所有顶点的坐标
		vertCoord_clipSpace[i] = mat_Trans * vertCoord_clipSpace[i]; //裁剪坐标系中对应的所有定点的坐标
		//--From clip space to Screen---
		screen_X = vertCoord_clipSpace[i].get_X()*800.0f / (vertCoord_clipSpace[i].get_W()*2.0f) + 800.0f / 2.0f;
		screen_Y = vertCoord_clipSpace[i].get_Y()*800.0f / (vertCoord_clipSpace[i].get_W()*2.0f) + 800.0f / 2.0f;
		vertCoord_Screenf[i].set_X(screen_X);
		vertCoord_Screenf[i].set_Y(screen_Y);
		vertCoord_Screenf[i].set_Z(vertCoord_clipSpace[i].get_Z() / vertCoord_clipSpace[i].get_W());//X，Y为屏幕空间坐标系中的坐标，Z为深度
	}
	int v1, v2, v3;
	Vector4D_float* vec = new Vector4D_float[3];
	
	int testcount = 0;
	//逐个光栅进行绘制
	for (int i = 0; i < m.m_numSurfaces; i++)
	{
		for (int j = 0; j < m.m_surface[i].m_nFaces; j++)
		{
			v1 = m.m_surface[i].m_face[j].m_vert[0].m_vertIndex;
			v2 = m.m_surface[i].m_face[j].m_vert[1].m_vertIndex;
			v3 = m.m_surface[i].m_face[j].m_vert[2].m_vertIndex;
			int vIndex[3] = { v1,v2,v3 };
			if (mode == 1) //线框模式
			{
				glColor3f(GLfloat(0), GLfloat(0), GLfloat(0));
				drawLine((int)vertCoord_Screenf[v1].get_X(), (int)vertCoord_Screenf[v1].get_Y(), (int)vertCoord_Screenf[v2].get_X(), (int)vertCoord_Screenf[v2].get_Y());
				drawLine((int)vertCoord_Screenf[v1].get_X(), (int)vertCoord_Screenf[v1].get_Y(), (int)vertCoord_Screenf[v3].get_X(), (int)vertCoord_Screenf[v3].get_Y());
				drawLine((int)vertCoord_Screenf[v2].get_X(), (int)vertCoord_Screenf[v2].get_Y(), (int)vertCoord_Screenf[v3].get_X(), (int)vertCoord_Screenf[v3].get_Y());
			}
			if (mode == 2) //无光照模式，无纹理
			{
				additional_R = 0, additional_G = 0, additional_B = 0;
				vec[0].setVec(vertCoord_Screenf[v1].get_X(), vertCoord_Screenf[v1].get_Y(), vertCoord_Screenf[v1].get_Z(), vertCoord_clipSpace[v1].get_W());
				vec[1].setVec(vertCoord_Screenf[v2].get_X(), vertCoord_Screenf[v2].get_Y(), vertCoord_Screenf[v2].get_Z(), vertCoord_clipSpace[v2].get_W());
				vec[2].setVec(vertCoord_Screenf[v3].get_X(), vertCoord_Screenf[v3].get_Y(), vertCoord_Screenf[v3].get_Z(), vertCoord_clipSpace[v3].get_W());
				int test2 = testcount % 2 == 1 ? (testcount - 1) : testcount;
				float t1 = test2 <= 3 ? ((1.0f + test2) / 4) : 0;
				float t2 = ((test2 <= 7) && (test2 > 3)) ? ((1.0f + test2 - 4) / 4) : 0;
				float t3 = ((test2 <= 11) && (test2 > 7)) ? ((1.0f + test2 - 8) / 4) : 0;
				glColor3f(GLfloat(t1), GLfloat(t2), GLfloat(t3));
				drawRaster(vec,vIndex, i, j, false);
				testcount++;
			}
			if (mode == 3) //无光照模式，有纹理
			{
				additional_R = 0, additional_G = 0, additional_B = 0;
				vec[0].setVec(vertCoord_Screenf[v1].get_X(), vertCoord_Screenf[v1].get_Y(), vertCoord_Screenf[v1].get_Z(), vertCoord_clipSpace[v1].get_W());
				vec[1].setVec(vertCoord_Screenf[v2].get_X(), vertCoord_Screenf[v2].get_Y(), vertCoord_Screenf[v2].get_Z(), vertCoord_clipSpace[v2].get_W());
				vec[2].setVec(vertCoord_Screenf[v3].get_X(), vertCoord_Screenf[v3].get_Y(), vertCoord_Screenf[v3].get_Z(), vertCoord_clipSpace[v3].get_W());
				int test2 = testcount % 2 == 1 ? (testcount - 1) : testcount;
				float t1 = test2 <= 3 ? ((1.0f + test2) / 4) : 0;
				float t2 = ((test2 <= 7) && (test2 > 3)) ? ((1.0f + test2 - 4) / 4) : 0;
				float t3 = ((test2 <= 11) && (test2 > 7)) ? ((1.0f + test2 - 8) / 4) : 0;
				if (testcount <= 11)
				{
					drawRaster(vec,vIndex, i, j, true);
				}
				else
				{
					glColor3f(GLfloat(t1), GLfloat(t2), GLfloat(t3));
					drawRaster(vec,vIndex, i, j, false);
				}
				testcount++;
			}
			if (mode == 4) //有光照模式（平行光源），无纹理
			{
				additional_R = 0, additional_G = 0, additional_B = 0;
				vec[0].setVec(vertCoord_Screenf[v1].get_X(), vertCoord_Screenf[v1].get_Y(), vertCoord_Screenf[v1].get_Z(), vertCoord_clipSpace[v1].get_W());
				vec[1].setVec(vertCoord_Screenf[v2].get_X(), vertCoord_Screenf[v2].get_Y(), vertCoord_Screenf[v2].get_Z(), vertCoord_clipSpace[v2].get_W());
				vec[2].setVec(vertCoord_Screenf[v3].get_X(), vertCoord_Screenf[v3].get_Y(), vertCoord_Screenf[v3].get_Z(), vertCoord_clipSpace[v3].get_W());

				Vector4D_float Ambient_RGB_vec;
				Vector4D_float Diffuse_RGB_vec(diffuseOrigin_R, diffuseOrigin_G, diffuseOrigin_B, 0);
				Vector4D_float Diffuse_RGB_vec_trans;
				Vector4D_float L_vec(vec_WorldParallelLight_x, vec_WorldParallelLight_y, vec_WorldParallelLight_z, 0);
				set_AmbientLightVec(Ambient_RGB_vec);
				set_DiffuseLightVec_Parallel(Diffuse_RGB_vec, Diffuse_RGB_vec_trans, m.m_norm[i], m_MDLtoWLD, L_vec);
				float _R = Ambient_RGB_vec.get_X() + Diffuse_RGB_vec_trans.get_X();
				float _G = Ambient_RGB_vec.get_Y() + Diffuse_RGB_vec_trans.get_Y();
				float _B = Ambient_RGB_vec.get_Z() + Diffuse_RGB_vec_trans.get_Z();
				glColor3f(GLfloat(_R), GLfloat(_G), GLfloat(_B));
				drawRaster(vec,vIndex, i, j, false);
			}
			if (mode == 5) //有光照模式（点光源），无纹理
			{
				additional_R = 0, additional_G = 0, additional_B = 0;
				vec[0].setVec(vertCoord_Screenf[v1].get_X(), vertCoord_Screenf[v1].get_Y(), vertCoord_Screenf[v1].get_Z(), vertCoord_clipSpace[v1].get_W());
				vec[1].setVec(vertCoord_Screenf[v2].get_X(), vertCoord_Screenf[v2].get_Y(), vertCoord_Screenf[v2].get_Z(), vertCoord_clipSpace[v2].get_W());
				vec[2].setVec(vertCoord_Screenf[v3].get_X(), vertCoord_Screenf[v3].get_Y(), vertCoord_Screenf[v3].get_Z(), vertCoord_clipSpace[v3].get_W());

				if (testcount <= 11)
				{
					drawRaster(vec,vIndex, i, j, false);
				}
				testcount++;
			}
		}
	}
}
//画地板
void drawGround(Mesh& m, int mode)
{
	//初始化
	//clearZbuffer();
	Matrix4D_float mat_Trans;
	coordTrans_Ground(m, mat_Trans);
	if (vertCoord_clipSpace != NULL) delete[] vertCoord_clipSpace;
	vertCoord_clipSpace = new Vector4D_float[m.m_numVerts];
	if (vertCoord_Screenf != NULL) delete[] vertCoord_Screenf;
	vertCoord_Screenf = new Vector4D_float[m.m_numVerts];
	//从模型空间到屏幕空间的变换计算
	float screen_X, screen_Y;
	for (int i = 0; i < m.m_numVerts; i++)
	{
		vertCoord_clipSpace[i] = m.m_pt[i]; //模型坐标系中的所有顶点的坐标
		vertCoord_clipSpace[i] = mat_Trans * vertCoord_clipSpace[i]; //裁剪坐标系中对应的所有定点的坐标
																	 //--From clip space to Screen---
		screen_X = vertCoord_clipSpace[i].get_X()*800.0f / (vertCoord_clipSpace[i].get_W()*2.0f) + 800.0f / 2.0f;
		screen_Y = vertCoord_clipSpace[i].get_Y()*800.0f / (vertCoord_clipSpace[i].get_W()*2.0f) + 800.0f / 2.0f;
		vertCoord_Screenf[i].set_X(screen_X);
		vertCoord_Screenf[i].set_Y(screen_Y);
		vertCoord_Screenf[i].set_Z(vertCoord_clipSpace[i].get_Z() / vertCoord_clipSpace[i].get_W());//X，Y为屏幕空间坐标系中的坐标，Z为深度
	}
	int v1, v2, v3;
	Vector4D_float* vec = new Vector4D_float[3];

	//逐个光栅进行绘制
	for (int i = 0; i < m.m_numSurfaces; i++)
	{
		for (int j = 0; j < m.m_surface[i].m_nFaces; j++)
		{
			v1 = m.m_surface[i].m_face[j].m_vert[0].m_vertIndex;
			v2 = m.m_surface[i].m_face[j].m_vert[1].m_vertIndex;
			v3 = m.m_surface[i].m_face[j].m_vert[2].m_vertIndex;
			int vIndex[3] = { v1,v2,v3 };
			vec[0].setVec(vertCoord_Screenf[v1].get_X(), vertCoord_Screenf[v1].get_Y(), vertCoord_Screenf[v1].get_Z(), vertCoord_clipSpace[v1].get_W());
			vec[1].setVec(vertCoord_Screenf[v2].get_X(), vertCoord_Screenf[v2].get_Y(), vertCoord_Screenf[v2].get_Z(), vertCoord_clipSpace[v2].get_W());
			vec[2].setVec(vertCoord_Screenf[v3].get_X(), vertCoord_Screenf[v3].get_Y(), vertCoord_Screenf[v3].get_Z(), vertCoord_clipSpace[v3].get_W());
			glColor3f(GLfloat(0.5), GLfloat(0.5), GLfloat(0.5));
			drawRaster(vec, vIndex, i, j, false);
		}
	}
}
//多线程模式的画网状体函数
void drawMesh_thread(Mesh& m, int mode)
{
	up_t1_count = 0;
	up_t2_count = 0;
	up_t3_count = 0;
	ClearScreenUnits();
	clearZbuffer();
	Matrix4D_float mat_Trans;
	coordTrans(m, mat_Trans);
	if (vertCoord_clipSpace != NULL) delete[] vertCoord_clipSpace;
	vertCoord_clipSpace = new Vector4D_float[m.m_numVerts];
	if (vertCoord_Screenf != NULL) delete[] vertCoord_Screenf;
	vertCoord_Screenf = new Vector4D_float[m.m_numVerts];
	float screen_X, screen_Y;
	for (int i = 0; i < m.m_numVerts; i++)
	{
		vertCoord_clipSpace[i] = m.m_pt[i]; //模型坐标系中的所有顶点的坐标
		vertCoord_clipSpace[i] = mat_Trans * vertCoord_clipSpace[i]; //裁剪坐标系中对应的所有定点的坐标
																	 //--From clip space to Screen---
		screen_X = vertCoord_clipSpace[i].get_X()*800.0f / (vertCoord_clipSpace[i].get_W()*2.0f) + 800.0f / 2.0f;
		screen_Y = vertCoord_clipSpace[i].get_Y()*800.0f / (vertCoord_clipSpace[i].get_W()*2.0f) + 800.0f / 2.0f;
		vertCoord_Screenf[i].set_X(screen_X);
		vertCoord_Screenf[i].set_Y(screen_Y);
		vertCoord_Screenf[i].set_Z(vertCoord_clipSpace[i].get_Z() / vertCoord_clipSpace[i].get_W());//X，Y为屏幕空间坐标系中的坐标，Z为深度
	}
	int v1, v2, v3;
	Vector4D_float* vec = new Vector4D_float[3];
	int testcount = 0;
	for (int i = 0; i < m.m_numSurfaces; i++)
	{
		for (int j = 0; j < m.m_surface[i].m_nFaces; j++)
		{
			v1 = m.m_surface[i].m_face[j].m_vert[0].m_vertIndex;
			v2 = m.m_surface[i].m_face[j].m_vert[1].m_vertIndex;
			v3 = m.m_surface[i].m_face[j].m_vert[2].m_vertIndex;
			//v0_index = v1, v1_index = v2, v2_index = v3; //保留下来v1,v2,v3最初对应的m_vertIndex，以便在打乱顺序后找到原来对应的index

			if (mode == 1) //线框模式
			{
				glColor3f(GLfloat(1), GLfloat(1), GLfloat(1));
				drawLine((int)vertCoord_Screenf[v1].get_X(), (int)vertCoord_Screenf[v1].get_Y(), (int)vertCoord_Screenf[v2].get_X(), (int)vertCoord_Screenf[v2].get_Y());
				drawLine((int)vertCoord_Screenf[v1].get_X(), (int)vertCoord_Screenf[v1].get_Y(), (int)vertCoord_Screenf[v3].get_X(), (int)vertCoord_Screenf[v3].get_Y());
				drawLine((int)vertCoord_Screenf[v2].get_X(), (int)vertCoord_Screenf[v2].get_Y(), (int)vertCoord_Screenf[v3].get_X(), (int)vertCoord_Screenf[v3].get_Y());
			}

			if (mode == 2) //无光照模式，无纹理
			{

				vec[0].setVec(vertCoord_Screenf[v1].get_X(), vertCoord_Screenf[v1].get_Y(), vertCoord_Screenf[v1].get_Z(), vertCoord_clipSpace[v1].get_W());
				vec[1].setVec(vertCoord_Screenf[v2].get_X(), vertCoord_Screenf[v2].get_Y(), vertCoord_Screenf[v2].get_Z(), vertCoord_clipSpace[v2].get_W());
				vec[2].setVec(vertCoord_Screenf[v3].get_X(), vertCoord_Screenf[v3].get_Y(), vertCoord_Screenf[v3].get_Z(), vertCoord_clipSpace[v3].get_W());

				int test2 = testcount % 2 == 1 ? (testcount - 1) : testcount;
				float t1 = test2 <= 3 ? ((1.0f + test2) / 4) : 0;
				float t2 = ((test2 <= 7) && (test2 > 3)) ? ((1.0f + test2 - 4) / 4) : 0;
				float t3 = ((test2 <= 11) && (test2 > 7)) ? ((1.0f + test2 - 8) / 4) : 0;

				//glColor3f(GLfloat(t1), GLfloat(t2), GLfloat(t3));
				//drawRaster_thread(vec, i, j, false);
				drawRaster_thread(vec, i, j, false);
			}

			if (mode == 3) //无光照模式，有纹理
			{
				vec[0].setVec(vertCoord_Screenf[v1].get_X(), vertCoord_Screenf[v1].get_Y(), vertCoord_Screenf[v1].get_Z(), vertCoord_clipSpace[v1].get_W());
				vec[1].setVec(vertCoord_Screenf[v2].get_X(), vertCoord_Screenf[v2].get_Y(), vertCoord_Screenf[v2].get_Z(), vertCoord_clipSpace[v2].get_W());
				vec[2].setVec(vertCoord_Screenf[v3].get_X(), vertCoord_Screenf[v3].get_Y(), vertCoord_Screenf[v3].get_Z(), vertCoord_clipSpace[v3].get_W());
				int test2 = testcount % 2 == 1 ? (testcount - 1) : testcount;
				float t1 = test2 <= 3 ? ((1.0f + test2) / 4) : 0;
				float t2 = ((test2 <= 7) && (test2 > 3)) ? ((1.0f + test2 - 4) / 4) : 0;
				float t3 = ((test2 <= 11) && (test2 > 7)) ? ((1.0f + test2 - 8) / 4) : 0;
				if (testcount <= 1)
				{
					drawRaster_thread(vec, i, j, true);
				}
				testcount++;
			}
			if (mode == 4) //有光照模式（平行光源），无纹理
			{
				vec[0].setVec(vertCoord_Screenf[v1].get_X(), vertCoord_Screenf[v1].get_Y(), vertCoord_Screenf[v1].get_Z(), vertCoord_clipSpace[v1].get_W());
				vec[1].setVec(vertCoord_Screenf[v2].get_X(), vertCoord_Screenf[v2].get_Y(), vertCoord_Screenf[v2].get_Z(), vertCoord_clipSpace[v2].get_W());
				vec[2].setVec(vertCoord_Screenf[v3].get_X(), vertCoord_Screenf[v3].get_Y(), vertCoord_Screenf[v3].get_Z(), vertCoord_clipSpace[v3].get_W());
				//对于每一个face，先根据光照确定好每个顶点的RGB值，再将上面的所有点进行插值
				Vector4D_float Ambient_RGB_vec;
				Vector4D_float Diffuse_RGB_vec(diffuseOrigin_R, diffuseOrigin_G, diffuseOrigin_B, 0);
				Vector4D_float Diffuse_RGB_vec_trans;
				Vector4D_float L_vec(vec_WorldParallelLight_x, vec_WorldParallelLight_y, vec_WorldParallelLight_z, 0);
				set_AmbientLightVec(Ambient_RGB_vec);
				set_DiffuseLightVec_Parallel(Diffuse_RGB_vec, Diffuse_RGB_vec_trans, m.m_norm[i], m_MDLtoWLD, L_vec);
				float _R = Ambient_RGB_vec.get_X() + Diffuse_RGB_vec_trans.get_X();
				float _G = Ambient_RGB_vec.get_Y() + Diffuse_RGB_vec_trans.get_Y();
				float _B = Ambient_RGB_vec.get_Z() + Diffuse_RGB_vec_trans.get_Z();
				glColor3f(GLfloat(_R), GLfloat(_G), GLfloat(_B));
				drawRaster_thread(vec, i, j, false);
			}
		}
	}
	if (mode == 2)
	{
		showScreenUnits();
	}
}
//交换函数
void swap(int& x1, int& x2, int& y1, int& y2, float& z1, float& z2, int& index1, int& index2)
{
	int tmpx = x1, tmpy = y1, tmpindex = index1;
	float tmpz = z1;
	x1 = x2;
	y1 = y2;
	z1 = z2;
	index1 = index2;
	x2 = tmpx;
	y2 = tmpy;
	z2 = tmpz;
	index2 = tmpindex;
}

void swap(int& x1, int& x2, int& y1, int& y2, float& z1, float& z2, float& w1, float& w2, int& index1, int& index2)
{
	int tmpx = x1, tmpy = y1, tmpindex = index1;
	float tmpz = z1,tmpw=w1;
	x1 = x2;
	y1 = y2;
	z1 = z2;
	w1 = w2;
	index1 = index2;
	x2 = tmpx;
	y2 = tmpy;
	z2 = tmpz;
	w2 = tmpw;
	index2 = tmpindex;
}

//将制定光栅上的指定屏幕坐标的点得到其对应的纹理像素的RGB
void setTextureRGB(int surfaceIndex, int faceIndex, float coordX_screen, float coordY_screen,float coordZ_screen)
{
	float m,n;
	int vertIndex_A = m1.m_surface[surfaceIndex].m_face[faceIndex].m_vert[0].m_vertIndex;
	int vertIndex_B = m1.m_surface[surfaceIndex].m_face[faceIndex].m_vert[1].m_vertIndex;
	int vertIndex_C = m1.m_surface[surfaceIndex].m_face[faceIndex].m_vert[2].m_vertIndex;

	float X_clip_A = vertCoord_clipSpace[vertIndex_A].get_X();
	float X_clip_B = vertCoord_clipSpace[vertIndex_B].get_X();
	float X_clip_C = vertCoord_clipSpace[vertIndex_C].get_X();
	float Y_clip_A = vertCoord_clipSpace[vertIndex_A].get_Y();
	float Y_clip_B = vertCoord_clipSpace[vertIndex_B].get_Y();
	float Y_clip_C = vertCoord_clipSpace[vertIndex_C].get_Y();
	float W_clip_A = vertCoord_clipSpace[vertIndex_A].get_W();
	float W_clip_B = vertCoord_clipSpace[vertIndex_B].get_W();
	float W_clip_C = vertCoord_clipSpace[vertIndex_C].get_W();

	float XW_NDC_A = X_clip_A / W_clip_A;
	float XW_NDC_B = X_clip_B / W_clip_B;
	float XW_NDC_C = X_clip_C / W_clip_C;
	float YW_NDC_A = Y_clip_A / W_clip_A;
	float YW_NDC_B = Y_clip_B / W_clip_B;
	float YW_NDC_C = Y_clip_C / W_clip_C;
	float oneoverW_NDC_A = 1 / W_clip_A;
	float oneoverW_NDC_B = 1 / W_clip_B;
	float oneoverW_NDC_C = 1 / W_clip_C;

	float XW_NDC_P = (2 * coordX_screen - maxWidth) / maxWidth;
	float YW_NDC_P = (2 * coordY_screen - maxHeight) / maxHeight;
	float oneoverW_NDC_P, W_clip_P;
	//在NDC空间里面对（Xclip/Wclip , Yclip/Wclip , ... , 1/Wclip)插值得到Y的1/Wclip
	//(xP-xA) = (xB-xA)*m + (xC-xA)*n
	//(yP-yA) = (yB-yA)*m + (yC-yA)*n
	//wP = (wB-wA)*m+(wC-wA)*n + wA
	//m = ((V_C - V_A)*(U_P - U_A) - (U_C - U_A)*(V_P - V_A)) / ((V_C - V_A)*(U_B - U_A) - (U_C - U_A)*(V_B - V_A));
	//n = ((V_B - V_A)*(U_P - U_A) - (U_B - U_A)*(V_P - V_A)) / ((V_B - V_A)*(U_C - U_A) - (U_B - U_A)*(V_C - V_A));
	m = ((YW_NDC_C - YW_NDC_A)*(XW_NDC_P - XW_NDC_A) - (XW_NDC_C - XW_NDC_A)*(YW_NDC_P - YW_NDC_A)) / ((YW_NDC_C - YW_NDC_A)*(XW_NDC_B - XW_NDC_A) - (XW_NDC_C - XW_NDC_A)*(YW_NDC_B - YW_NDC_A));
	n = ((YW_NDC_B - YW_NDC_A)*(XW_NDC_P - XW_NDC_A) - (XW_NDC_B - XW_NDC_A)*(YW_NDC_P - YW_NDC_A)) / ((YW_NDC_B - YW_NDC_A)*(XW_NDC_C - XW_NDC_A) - (XW_NDC_B - XW_NDC_A)*(YW_NDC_C - YW_NDC_A));
	oneoverW_NDC_P = (oneoverW_NDC_B - oneoverW_NDC_A)*m + (oneoverW_NDC_C - oneoverW_NDC_A)*n + oneoverW_NDC_A;
	W_clip_P = 1 / oneoverW_NDC_P;

	float X_clip_P = W_clip_P * XW_NDC_P;
	float Y_clip_P = W_clip_P * YW_NDC_P;

	//得到了Clip Space里面的P的(Xclip,Yclip,...,Wclip)后，在Clip Space里进行插值

	m = ((Y_clip_C - Y_clip_A)*(X_clip_P - X_clip_A) - (X_clip_C - X_clip_A)*(Y_clip_P - Y_clip_A)) / ((Y_clip_C - Y_clip_A)*(X_clip_B - X_clip_A) - (X_clip_C - X_clip_A)*(Y_clip_B - Y_clip_A));
	n = ((Y_clip_B - Y_clip_A)*(X_clip_P - X_clip_A) - (X_clip_B - X_clip_A)*(Y_clip_P - Y_clip_A)) / ((Y_clip_B - Y_clip_A)*(X_clip_C - X_clip_A) - (X_clip_B - X_clip_A)*(Y_clip_C - Y_clip_A));

	float TX_A = textCoord_U[surfaceIndex][faceIndex][0];
	float TX_B = textCoord_U[surfaceIndex][faceIndex][1];
	float TX_C = textCoord_U[surfaceIndex][faceIndex][2];
	float TY_A = textCoord_V[surfaceIndex][faceIndex][0];
	float TY_B = textCoord_V[surfaceIndex][faceIndex][1];
	float TY_C = textCoord_V[surfaceIndex][faceIndex][2];

	if (m <= 0)
		m = 0.0f;
	if (m >= 1)
		m = 1.0f;
	if (n <= 0)
		n = 0.0f;
	if (n >= 1)
		n = 1.0f;

	float TX_P= TX_A + (TX_B - TX_A)*m + (TX_C - TX_A)*n;
	float TY_P= TY_A + (TY_B - TY_A)*m + (TY_C - TY_A)*n;
	
	int U_PIXEL = int(TX_P*imagewidth);
	int V_PIXEL = int(TY_P*imageheight);
	if (U_PIXEL <= 0) U_PIXEL = 0;
	if (V_PIXEL <= 0) V_PIXEL = 0;
	if (U_PIXEL >= imagewidth) U_PIXEL = imagewidth - 1;
	if (V_PIXEL >= imageheight) V_PIXEL = imageheight - 1;

	//glColor3ub(txtr_RGB[V_PIXEL][U_PIXEL][0], txtr_RGB[V_PIXEL][U_PIXEL][1], txtr_RGB[V_PIXEL][U_PIXEL][2]);
	glColor3ub(txtr_RGB[V_PIXEL][U_PIXEL][0]+GLubyte(additional_R*255.0f), txtr_RGB[V_PIXEL][U_PIXEL][1]+GLubyte(additional_G*255.0f), txtr_RGB[V_PIXEL][U_PIXEL][2]+ GLubyte(additional_B*255.0f));
}

void setTextureRGB_thread(int surfaceIndex, int faceIndex, float coordX_screen, float coordY_screen, float coordZ_screen)
{
	float m, n;
	int vertIndex_A = m1.m_surface[surfaceIndex].m_face[faceIndex].m_vert[0].m_vertIndex;
	int vertIndex_B = m1.m_surface[surfaceIndex].m_face[faceIndex].m_vert[1].m_vertIndex;
	int vertIndex_C = m1.m_surface[surfaceIndex].m_face[faceIndex].m_vert[2].m_vertIndex;

	float X_clip_A = vertCoord_clipSpace[vertIndex_A].get_X();
	float X_clip_B = vertCoord_clipSpace[vertIndex_B].get_X();
	float X_clip_C = vertCoord_clipSpace[vertIndex_C].get_X();
	float Y_clip_A = vertCoord_clipSpace[vertIndex_A].get_Y();
	float Y_clip_B = vertCoord_clipSpace[vertIndex_B].get_Y();
	float Y_clip_C = vertCoord_clipSpace[vertIndex_C].get_Y();
	float W_clip_A = vertCoord_clipSpace[vertIndex_A].get_W();
	float W_clip_B = vertCoord_clipSpace[vertIndex_B].get_W();
	float W_clip_C = vertCoord_clipSpace[vertIndex_C].get_W();

	float XW_NDC_A = X_clip_A / W_clip_A;
	float XW_NDC_B = X_clip_B / W_clip_B;
	float XW_NDC_C = X_clip_C / W_clip_C;
	float YW_NDC_A = Y_clip_A / W_clip_A;
	float YW_NDC_B = Y_clip_B / W_clip_B;
	float YW_NDC_C = Y_clip_C / W_clip_C;
	float oneoverW_NDC_A = 1 / W_clip_A;
	float oneoverW_NDC_B = 1 / W_clip_B;
	float oneoverW_NDC_C = 1 / W_clip_C;

	float XW_NDC_P = (2 * coordX_screen - maxWidth) / maxWidth;
	float YW_NDC_P = (2 * coordY_screen - maxHeight) / maxHeight;
	float oneoverW_NDC_P, W_clip_P;
	//在NDC空间里面对（Xclip/Wclip , Yclip/Wclip , ... , 1/Wclip)插值得到Y的1/Wclip
	//(xP-xA) = (xB-xA)*m + (xC-xA)*n
	//(yP-yA) = (yB-yA)*m + (yC-yA)*n
	//wP = (wB-wA)*m+(wC-wA)*n + wA
	//m = ((V_C - V_A)*(U_P - U_A) - (U_C - U_A)*(V_P - V_A)) / ((V_C - V_A)*(U_B - U_A) - (U_C - U_A)*(V_B - V_A));
	//n = ((V_B - V_A)*(U_P - U_A) - (U_B - U_A)*(V_P - V_A)) / ((V_B - V_A)*(U_C - U_A) - (U_B - U_A)*(V_C - V_A));
	m = ((YW_NDC_C - YW_NDC_A)*(XW_NDC_P - XW_NDC_A) - (XW_NDC_C - XW_NDC_A)*(YW_NDC_P - YW_NDC_A)) / ((YW_NDC_C - YW_NDC_A)*(XW_NDC_B - XW_NDC_A) - (XW_NDC_C - XW_NDC_A)*(YW_NDC_B - YW_NDC_A));
	n = ((YW_NDC_B - YW_NDC_A)*(XW_NDC_P - XW_NDC_A) - (XW_NDC_B - XW_NDC_A)*(YW_NDC_P - YW_NDC_A)) / ((YW_NDC_B - YW_NDC_A)*(XW_NDC_C - XW_NDC_A) - (XW_NDC_B - XW_NDC_A)*(YW_NDC_C - YW_NDC_A));

	oneoverW_NDC_P = (oneoverW_NDC_B - oneoverW_NDC_A)*m + (oneoverW_NDC_C - oneoverW_NDC_A)*n + oneoverW_NDC_A;
	W_clip_P = 1 / oneoverW_NDC_P;

	float X_clip_P = W_clip_P * XW_NDC_P;
	float Y_clip_P = W_clip_P * YW_NDC_P;

	//得到了Clip Space里面的P的(Xclip,Yclip,...,Wclip)后，在Clip Space里进行插值

	m = ((Y_clip_C - Y_clip_A)*(X_clip_P - X_clip_A) - (X_clip_C - X_clip_A)*(Y_clip_P - Y_clip_A)) / ((Y_clip_C - Y_clip_A)*(X_clip_B - X_clip_A) - (X_clip_C - X_clip_A)*(Y_clip_B - Y_clip_A));
	n = ((Y_clip_B - Y_clip_A)*(X_clip_P - X_clip_A) - (X_clip_B - X_clip_A)*(Y_clip_P - Y_clip_A)) / ((Y_clip_B - Y_clip_A)*(X_clip_C - X_clip_A) - (X_clip_B - X_clip_A)*(Y_clip_C - Y_clip_A));

	float TX_A = textCoord_U[surfaceIndex][faceIndex][0];
	float TX_B = textCoord_U[surfaceIndex][faceIndex][1];
	float TX_C = textCoord_U[surfaceIndex][faceIndex][2];
	float TY_A = textCoord_V[surfaceIndex][faceIndex][0];
	float TY_B = textCoord_V[surfaceIndex][faceIndex][1];
	float TY_C = textCoord_V[surfaceIndex][faceIndex][2];

	if (m <= 0)
		m = 0.0f;
	if (m >= 1)
		m = 1.0f;
	if (n <= 0)
		n = 0.0f;
	if (n >= 1)
		n = 1.0f;

	float TX_P = TX_A + (TX_B - TX_A)*m + (TX_C - TX_A)*n;
	float TY_P = TY_A + (TY_B - TY_A)*m + (TY_C - TY_A)*n;

	int U_PIXEL = int(TX_P*imagewidth);
	int V_PIXEL = int(TY_P*imageheight);
	if (U_PIXEL <= 0) U_PIXEL = 0;
	if (V_PIXEL <= 0) V_PIXEL = 0;
	if (U_PIXEL >= imagewidth) U_PIXEL = imagewidth - 1;
	if (V_PIXEL >= imageheight) V_PIXEL = imageheight - 1;

	//glColor3ub(txtr_RGB[V_PIXEL][U_PIXEL][0], txtr_RGB[V_PIXEL][U_PIXEL][1], txtr_RGB[V_PIXEL][U_PIXEL][2]);
	drawScreenPixel(coordY_screen, coordX_screen, coordZ_screen, GLfloat(txtr_RGB[V_PIXEL][U_PIXEL][0]), GLfloat(txtr_RGB[V_PIXEL][U_PIXEL][1]), GLfloat(txtr_RGB[V_PIXEL][U_PIXEL][2]));
}

void solveUpRasterBound(int& x_left, int& x_right,float& z_left,float& z_right,const int* x, const int* y,const float* z,const int y0)
{
	int tmpx1,tmpx2;
	float tmpz1, tmpz2, tmpw1, tmpw2;
	bool intTransferMode = true;
	if (y0 == y[0])
	{
		x_left = x[0] <= x[1] ? x[0] : x[1];
		x_right = x[0] <= x[1] ? x[1] : x[0];
		z_left = x[0] <= x[1] ? z[0] : z[1];
		z_right = x[0] <= x[1] ? z[1] : z[0];
		return;
	}
	if (y0 == y[2])
	{
		x_left = x[2];
		x_right = x[2];
		z_left = z[2];
		z_right = z[2];
		return;
	}

	if(intTransferMode)
	{
		tmpx1 = int(((float)y0 - (float)y[2])*((float)x[0] - (float)x[2]) / ((float)y[0] - (float)y[2]) + (float)x[2]); //tmpx1是v[0]和v[2]之间的插值结果
		tmpx2 = int(((float)y0 - (float)y[2])*((float)x[1] - (float)x[2]) / ((float)y[1] - (float)y[2]) + (float)x[2]); //tmpx2是v[1]和v[2]之间的插值结果

		if (z[0] == z[2])
			tmpz1 = z[0];
		else
			tmpz1 = ((float)y0 - (float)y[2])*(z[0] - z[2]) / ((float)y[0] - (float)y[2]) + z[2]; //tmpz1是v[0]和v[2]之间的插值结果


		if (z[1] == z[2])
			tmpz2 = z[1];
		else
			tmpz2 = ((float)y0 - (float)y[2])*(z[1] - z[2]) / ((float)y[1] - (float)y[2]) + z[2]; //tmpz2是v[1]和v[2]之间的插值结果


	}
	x_left = tmpx1 <= tmpx2 ? tmpx1 : tmpx2;
	x_right= tmpx1 <= tmpx2 ? tmpx2 : tmpx1;
	z_left = tmpx1 <= tmpx2 ? tmpz1 : tmpz2;
	z_right = tmpx1 <= tmpx2 ? tmpz2 : tmpz1;

}


void solveUpRasterBound_thread(int& x_left, int& x_right, float& z_left, float& z_right, const int* x, const int* y, const float* z, const int y0)
{
	int tmpx1, tmpx2;
	float tmpz1, tmpz2;
	bool intTransferMode = true;
	if (y0 == y[0])
	{
		x_left = x[0] <= x[1] ? x[0] : x[1];
		x_right = x[0] <= x[1] ? x[1] : x[0];
		z_left = x[0] <= x[1] ? z[0] : z[1];
		z_right = x[0] <= x[1] ? z[1] : z[0];
		return;
	}
	if (y0 == y[2])
	{
		x_left = x[2];
		x_right = x[2];
		z_left = z[2];
		z_right = z[2];
		return;
	}

	if (intTransferMode)
	{
		tmpx1 = int(((float)y0 - (float)y[2])*((float)x[0] - (float)x[2]) / ((float)y[0] - (float)y[2]) + (float)x[2]); //tmpx1是v[0]和v[2]之间的插值结果
		tmpx2 = int(((float)y0 - (float)y[2])*((float)x[1] - (float)x[2]) / ((float)y[1] - (float)y[2]) + (float)x[2]); //tmpx2是v[1]和v[2]之间的插值结果

		if (z[0] == z[2])
			tmpz1 = z[0];
		else
			tmpz1 = ((float)y0 - (float)y[2])*(z[0] - z[2]) / ((float)y[0] - (float)y[2]) + z[2]; //tmpz1是v[0]和v[2]之间的插值结果

		if (z[1] == z[2])
			tmpz2 = z[1];
		else
			tmpz2 = ((float)y0 - (float)y[2])*(z[1] - z[2]) / ((float)y[1] - (float)y[2]) + z[2]; //tmpz2是v[1]和v[2]之间的插值结果

	}
	x_left = tmpx1 <= tmpx2 ? tmpx1 : tmpx2;
	x_right = tmpx1 <= tmpx2 ? tmpx2 : tmpx1;
	z_left = tmpx1 <= tmpx2 ? tmpz1 : tmpz2;
	z_right = tmpx1 <= tmpx2 ? tmpz2 : tmpz1;
}

void solveLowRasterBound(int& x_left, int& x_right, float& z_left, float& z_right, const int* x, const int* y, const float* z, const int y0)
{
	int tmpx1, tmpx2;
	float tmpz1, tmpz2;
	bool intTransferMode = true;
	if (y0 == y[0])
	{
		x_left = x[0];
		x_right = x[0];
		z_left = x[0];
		z_right = x[0];

		return;
	}
	if (y0 == y[2])
	{
		x_left = x[1] <= x[2] ? x[1] : x[2];
		x_right = x[1] <= x[2] ? x[2] : x[1];
		z_left = x[1] <= x[2] ? z[1] : z[2];
		z_right = x[1] <= x[2] ? z[2] : z[1];
		return;
	}

	if (intTransferMode)
	{
		tmpx1 = int(((float)y0 - (float)y[2])*((float)x[0] - (float)x[2]) / ((float)y[0] - (float)y[2]) + (float)x[2]); //tmpx1是v[0]和v[2]之间的插值结果
		tmpx2 = int(((float)y0 - (float)y[1])*((float)x[0] - (float)x[1]) / ((float)y[0] - (float)y[1]) + (float)x[1]); //tmpx2是v[0]和v[1]之间的插值结果

		if (z[0] == z[2])
			tmpz1 = z[0];
		else
			tmpz1 = ((float)y0 - (float)y[2])*(z[0] - z[2]) / ((float)y[0] - (float)y[2]) + z[2]; //tmpz1是v[0]和v[2]之间的插值结果

		if (z[0] == z[1])
			tmpz2 = z[0];
		else
			tmpz2 = ((float)y0 - (float)y[1])*(z[0] - z[1]) / ((float)y[0] - (float)y[1]) + z[1]; //tmpz2是v[0]和v[1]之间的插值结果
	}
	x_left = tmpx1 <= tmpx2 ? tmpx1 : tmpx2;
	x_right = tmpx1 <= tmpx2 ? tmpx2 : tmpx1;
	z_left = tmpx1 <= tmpx2 ? tmpz1 : tmpz2;
	z_right = tmpx1 <= tmpx2 ? tmpz2 : tmpz1;
}


void solveLowRasterBound_thread(int& x_left, int& x_right, float& z_left, float& z_right, const int* x, const int* y, const float* z, const int y0)
{
	int tmpx1, tmpx2;
	float tmpz1, tmpz2;
	bool intTransferMode = true;
	if (y0 == y[0])
	{
		x_left = x[0];
		x_right = x[0];
		z_left = x[0];
		z_right = x[0];

		return;
	}
	if (y0 == y[2])
	{
		x_left = x[1] <= x[2] ? x[1] : x[2];
		x_right = x[1] <= x[2] ? x[2] : x[1];
		z_left = x[1] <= x[2] ? z[1] : z[2];
		z_right = x[1] <= x[2] ? z[2] : z[1];
		return;
	}

	if (intTransferMode)
	{
		tmpx1 = int(((float)y0 - (float)y[2])*((float)x[0] - (float)x[2]) / ((float)y[0] - (float)y[2]) + (float)x[2]); //tmpx1是v[0]和v[2]之间的插值结果
		tmpx2 = int(((float)y0 - (float)y[1])*((float)x[0] - (float)x[1]) / ((float)y[0] - (float)y[1]) + (float)x[1]); //tmpx2是v[0]和v[1]之间的插值结果

		if (z[0] == z[2])
			tmpz1 = z[0];
		else
			tmpz1 = ((float)y0 - (float)y[2])*(z[0] - z[2]) / ((float)y[0] - (float)y[2]) + z[2]; //tmpz1是v[0]和v[2]之间的插值结果

		if (z[0] == z[1])
			tmpz2 = z[0];
		else
			tmpz2 = ((float)y0 - (float)y[1])*(z[0] - z[1]) / ((float)y[0] - (float)y[1]) + z[1]; //tmpz2是v[0]和v[1]之间的插值结果
	}
	x_left = tmpx1 <= tmpx2 ? tmpx1 : tmpx2;
	x_right = tmpx1 <= tmpx2 ? tmpx2 : tmpx1;
	z_left = tmpx1 <= tmpx2 ? tmpz1 : tmpz2;
	z_right = tmpx1 <= tmpx2 ? tmpz2 : tmpz1;
}

void drawUpRaster(int* x,int* y,float* z,int* vertexIndex, int surfaceindex, int faceindex, bool ifTexture)
{
	int x_left, x_right;
	float z_left, z_right, z_temp;
	for (int i = y[0]; i >= y[2]; i--)
	{
		solveUpRasterBound(x_left, x_right, z_left, z_right,  x, y, z, i); //get the proper x/z_left, x/z_right

		//不采用点光源形式
		if (_showMode != 5)
		{
			if (x_left == x_right) //只有一个点，左右边界重合
			{
				//TODO 在这里设定好RGB值
				if (ifTexture)
					//setTextureRGB(surfaceindex, faceindex, float(x_left), float(i));
					setTextureRGB(surfaceindex, faceindex, float(x_left), float(i), float(z_left));
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
						//TODO 20170725 补充完插值算法
						z_temp = (z_right - z_left)*((float)j - (float)x_left) / ((float)x_right - (float)x_left) + z_left;
					}
					if (ifTexture)
					{
						//setTextureRGB(surfaceindex, faceindex, float(j), float(i));
						setTextureRGB(surfaceindex, faceindex, float(j), float(i), z_temp);
					}
					drawPixel(j, i, z_temp);
				}
			}
		}
		//采用点光源形式
		else if (_showMode == 5)
		{
			Vector4D_float vecA_R(vertCoord_Screenf[vertexIndex[0]].get_X(), vertCoord_Screenf[vertexIndex[0]].get_Y(), 0, vertR_worldSpace[surfaceindex][vertexIndex[0]]);
			Vector4D_float vecA_G(vertCoord_Screenf[vertexIndex[0]].get_X(), vertCoord_Screenf[vertexIndex[0]].get_Y(), 0, vertG_worldSpace[surfaceindex][vertexIndex[0]]);
			Vector4D_float vecA_B(vertCoord_Screenf[vertexIndex[0]].get_X(), vertCoord_Screenf[vertexIndex[0]].get_Y(), 0, vertB_worldSpace[surfaceindex][vertexIndex[0]]);

			Vector4D_float vecB_R(vertCoord_Screenf[vertexIndex[1]].get_X(), vertCoord_Screenf[vertexIndex[1]].get_Y(), 0, vertR_worldSpace[surfaceindex][vertexIndex[1]]);
			Vector4D_float vecB_G(vertCoord_Screenf[vertexIndex[1]].get_X(), vertCoord_Screenf[vertexIndex[1]].get_Y(), 0, vertG_worldSpace[surfaceindex][vertexIndex[1]]);
			Vector4D_float vecB_B(vertCoord_Screenf[vertexIndex[1]].get_X(), vertCoord_Screenf[vertexIndex[1]].get_Y(), 0, vertB_worldSpace[surfaceindex][vertexIndex[1]]);

			Vector4D_float vecC_R(vertCoord_Screenf[vertexIndex[2]].get_X(), vertCoord_Screenf[vertexIndex[2]].get_Y(), 0, vertR_worldSpace[surfaceindex][vertexIndex[2]]);
			Vector4D_float vecC_G(vertCoord_Screenf[vertexIndex[2]].get_X(), vertCoord_Screenf[vertexIndex[2]].get_Y(), 0, vertG_worldSpace[surfaceindex][vertexIndex[2]]);
			Vector4D_float vecC_B(vertCoord_Screenf[vertexIndex[2]].get_X(), vertCoord_Screenf[vertexIndex[2]].get_Y(), 0, vertB_worldSpace[surfaceindex][vertexIndex[2]]);
			if (x_left == x_right) //只有一个点，左右边界重合
			{
				Vector4D_float vecP_R(float(x_left), float(i), 0, 0);
				Vector4D_float vecP_G(float(x_left), float(i), 0, 0);
				Vector4D_float vecP_B(float(x_left), float(i), 0, 0);
				glColor3f(_2D_Interpolation(vecA_R, vecB_R, vecC_R, vecP_R), _2D_Interpolation(vecA_G, vecB_G, vecC_G, vecP_G), _2D_Interpolation(vecA_B, vecB_B, vecC_B, vecP_B));
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
					Vector4D_float vecP_R(float(j), float(i), 0, 0);
					Vector4D_float vecP_G(float(j), float(i), 0, 0);
					Vector4D_float vecP_B(float(j), float(i), 0, 0);
					glColor3f(_2D_Interpolation(vecA_R, vecB_R, vecC_R, vecP_R), _2D_Interpolation(vecA_G, vecB_G, vecC_G, vecP_G), _2D_Interpolation(vecA_B, vecB_B, vecC_B, vecP_B));
					drawPixel(j, i, z_temp);
				}
			}
		}
		
	}
}


void drawUpLine_thread(int* x, int* y, float* z, int surfaceindex, int faceindex, bool ifTexture,int threadName)
{
	threadCount++;
	int x_left, x_right;
	float z_left, z_right, z_temp;
	bool flag_temp;

	int testcount = surfaceindex * 2 + faceindex;
	int test2 = testcount % 2 == 1 ? (testcount - 1) : testcount;
	float t1 = test2 <= 3 ? ((1.0f + test2) / 4) : 0;
	float t2 = ((test2 <= 7) && (test2 > 3)) ? ((1.0f + test2 - 4) / 4) : 0;
	float t3 = ((test2 <= 11) && (test2 > 7)) ? ((1.0f + test2 - 8) / 4) : 0;


	for (int i = y[0]; i >= y[2]; i--)
	{
		flag_temp = false;
		mtx2.lock();
		if (flag2[i - y[2]] == false)
		{
			mtx2.unlock();
			flag2[i - y[2]] = true;
			flag_temp = true;
			solveUpRasterBound_thread(x_left, x_right, z_left, z_right, x, y, z, i); //get the proper x/z_left, x/z_right
			if (x_left == x_right) //只有一个点，左右边界重合
			{
				if (ifTexture)
				{
					setTextureRGB_thread(surfaceindex, faceindex, float(x_left), float(i), float(z_left));
				}
				else
				{
					//fortest color
					//drawScreenPixel(i, x_left, z_left, 0.5, 0.5, 0.5);
					drawScreenPixel(i, x_left, z_left, t1, t2, t3);
				}
			}
			else
			{
				for (int j = x_left; j <= x_right; j++)
				{
					if (z_left == z_right)
						z_temp = z_left;
					else
					{
						z_temp = (z_right - z_left)*((float)j - (float)x_left) / ((float)x_right - (float)x_left) + z_left;
					}
					if (ifTexture)
					{
						setTextureRGB_thread(surfaceindex, faceindex, float(j), float(i), float(z_temp));
					}
					else
					{
						//drawScreenPixel(i, j, z_temp, 0.5, 0.5, 0.5);
						drawScreenPixel(i, j, z_temp, t1, t2, t3);
					}
				}
			}
		}
		if (flag_temp == false)
			mtx2.unlock();
	}
	if (threadName == 1)
		up_t1_count++;
	else if (threadName == 2)
		up_t2_count++;
	else if (threadName == 3)
		up_t3_count++;
}

void drawUpRaster_thread(int* x, int* y, float* z, float* w, int surfaceindex, int faceindex, bool ifTexture)
{
	if (flag2 != NULL)
		delete[] flag2;
	flag2 = new bool[y[0] - y[2] + 1];
	for (int i = 0; i < (y[0] - y[2] + 1); i++)
	{
		flag2[i] = false;
	}
	thread t1(drawUpLine_thread, x, y, z, surfaceindex, faceindex, ifTexture,1);
	thread t2(drawUpLine_thread, x, y, z, surfaceindex, faceindex, ifTexture,2);
	thread t3(drawUpLine_thread, x, y, z, surfaceindex, faceindex, ifTexture,3);
	t1.join();
	t2.join();
	t3.join();
}

void drawLowRaster(int* x, int* y, float* z, int* vertexIndex,int surfaceindex, int faceindex, bool ifTexture)
{
	int x_left, x_right;
	float z_left, z_right, z_temp;
	for (int i = y[0]; i >= y[2]; i--)
	{

		solveLowRasterBound(x_left, x_right, z_left, z_right, x, y, z, i); //get the proper x/z_left, x/z_right
		if (_showMode != 5)
		{
			if (x_left == x_right) //只有一个点，左右边界重合
			{
				if (ifTexture)
					//setTextureRGB(surfaceindex, faceindex, float(x_left), float(i));
					setTextureRGB(surfaceindex, faceindex, float(x_left), float(i), float(z_left));
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
						z_temp = (z_right - z_left)*((float)j - (float)x_left) / ((float)x_right - (float)x_left) + z_left;
					}
					if (ifTexture)
						//setTextureRGB(surfaceindex, faceindex, float(j), float(i));
						setTextureRGB(surfaceindex, faceindex, float(j), float(i), z_temp);
					drawPixel(j, i, z_temp);

				}
			}
		}
		if (_showMode == 5)
		{
			Vector4D_float vecA_R(vertCoord_Screenf[vertexIndex[0]].get_X(), vertCoord_Screenf[vertexIndex[0]].get_Y(), 0, vertR_worldSpace[surfaceindex][vertexIndex[0]]);
			Vector4D_float vecA_G(vertCoord_Screenf[vertexIndex[0]].get_X(), vertCoord_Screenf[vertexIndex[0]].get_Y(), 0, vertG_worldSpace[surfaceindex][vertexIndex[0]]);
			Vector4D_float vecA_B(vertCoord_Screenf[vertexIndex[0]].get_X(), vertCoord_Screenf[vertexIndex[0]].get_Y(), 0, vertB_worldSpace[surfaceindex][vertexIndex[0]]);
			Vector4D_float vecB_R(vertCoord_Screenf[vertexIndex[1]].get_X(), vertCoord_Screenf[vertexIndex[1]].get_Y(), 0, vertR_worldSpace[surfaceindex][vertexIndex[1]]);
			Vector4D_float vecB_G(vertCoord_Screenf[vertexIndex[1]].get_X(), vertCoord_Screenf[vertexIndex[1]].get_Y(), 0, vertG_worldSpace[surfaceindex][vertexIndex[1]]);
			Vector4D_float vecB_B(vertCoord_Screenf[vertexIndex[1]].get_X(), vertCoord_Screenf[vertexIndex[1]].get_Y(), 0, vertB_worldSpace[surfaceindex][vertexIndex[1]]);
			Vector4D_float vecC_R(vertCoord_Screenf[vertexIndex[2]].get_X(), vertCoord_Screenf[vertexIndex[2]].get_Y(), 0, vertR_worldSpace[surfaceindex][vertexIndex[2]]);
			Vector4D_float vecC_G(vertCoord_Screenf[vertexIndex[2]].get_X(), vertCoord_Screenf[vertexIndex[2]].get_Y(), 0, vertG_worldSpace[surfaceindex][vertexIndex[2]]);
			Vector4D_float vecC_B(vertCoord_Screenf[vertexIndex[2]].get_X(), vertCoord_Screenf[vertexIndex[2]].get_Y(), 0, vertB_worldSpace[surfaceindex][vertexIndex[2]]);
			if (x_left == x_right) //只有一个点，左右边界重合
			{
				Vector4D_float vecP_R(float(x_left), float(i), 0, 0);
				Vector4D_float vecP_G(float(x_left), float(i), 0, 0);
				Vector4D_float vecP_B(float(x_left), float(i), 0, 0);
				glColor3f(_2D_Interpolation(vecA_R, vecB_R, vecC_R, vecP_R), _2D_Interpolation(vecA_G, vecB_G, vecC_G, vecP_G), _2D_Interpolation(vecA_B, vecB_B, vecC_B, vecP_B));
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
					Vector4D_float vecP_R(float(j), float(i), 0, 0);
					Vector4D_float vecP_G(float(j), float(i), 0, 0);
					Vector4D_float vecP_B(float(j), float(i), 0, 0);
					glColor3f(_2D_Interpolation(vecA_R, vecB_R, vecC_R, vecP_R), _2D_Interpolation(vecA_G, vecB_G, vecC_G, vecP_G), _2D_Interpolation(vecA_B, vecB_B, vecC_B, vecP_B));
					drawPixel(j, i, z_temp);
				}
			}
		}
	}
}

void drawLowLine_thread(int* x, int* y, float* z, int surfaceindex, int faceindex, bool ifTexture)
{

	threadCount++;
	int x_left, x_right;
	float z_left, z_right, z_temp;
	bool flag_temp;


	int testcount = surfaceindex * 2 + faceindex;
	int test2 = testcount % 2 == 1 ? (testcount - 1) : testcount;
	float t1 = test2 <= 3 ? ((1.0f + test2) / 4) : 0;
	float t2 = ((test2 <= 7) && (test2 > 3)) ? ((1.0f + test2 - 4) / 4) : 0;
	float t3 = ((test2 <= 11) && (test2 > 7)) ? ((1.0f + test2 - 8) / 4) : 0;


	for (int i = y[0]; i >= y[2]; i--)
	{
		flag_temp = false;
		mtx1.lock();
		if (flag1[i - y[2]] == false)
		{
			mtx1.unlock();
			flag1[i - y[2]] = true;
			flag_temp = true;
			solveLowRasterBound_thread(x_left, x_right, z_left, z_right, x, y, z, i); //get the proper x/z_left, x/z_right
			if (x_left == x_right) //只有一个点，左右边界重合
			{
				if (ifTexture)
				{
					setTextureRGB_thread(surfaceindex, faceindex, float(x_left), float(i), float(z_left));
				}
				else
				{
					//drawScreenPixel(i, x_left, z_left, 0.5, 0.5, 0.5);
					drawScreenPixel(i, x_left, z_left, t1, t2, t3);
				}
			}
			else
			{
				for (int j = x_left; j <= x_right; j++)
				{
					if (z_left == z_right)
						z_temp = z_left;
					else
					{
						z_temp = (z_right - z_left)*((float)j - (float)x_left) / ((float)x_right - (float)x_left) + z_left;
					}
					if (ifTexture)
					{
						setTextureRGB_thread(surfaceindex, faceindex, float(j), float(i), float(z_temp));
					}
					else
					{
						//drawScreenPixel(i, j, z_temp, 0.5, 0.5, 0.5);
						drawScreenPixel(i, j, z_temp, t1, t2, t3);
					}
				}
			}
		}
		if (flag_temp == false)
			mtx1.unlock();
	}
}

void drawLowRaster_thread(int* x, int* y, float* z, int surfaceindex, int faceindex, bool ifTexture)
{
	if (flag1 != NULL)
		delete[] flag1;
	flag1 = new bool[y[0] - y[2] + 1];
	for (int i = 0; i < (y[0] - y[2] + 1); i++)
	{
		flag1[i] = false;
	}
	thread t1(drawLowLine_thread, x, y, z, surfaceindex, faceindex, ifTexture);
	thread t2(drawLowLine_thread, x, y, z, surfaceindex, faceindex, ifTexture);
	thread t3(drawLowLine_thread, x, y, z, surfaceindex, faceindex, ifTexture);
	t1.join();
	t2.join();
	t3.join();
}

void drawRaster(Vector4D_float* vec,int* vIndex, int surfaceindex, int faceindex, bool ifTexture)
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
		drawUpRaster(x, y, z,vIndex,surfaceindex, faceindex, ifTexture);
		return;
	}
	else if (y[2] == y[1])
	{
		drawLowRaster(x, y, z, vIndex,surfaceindex, faceindex, ifTexture);
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
	drawRaster(vec_up,vIndex, surfaceindex, faceindex, ifTexture);
	
	vec_down[0].setVec(x[2], y[2], z[2], 1);
	vec_down[1].setVec(x[1], y[1], z[1], 1);
	vec_down[2].setVec(tmpx, y[1], tmpz, 1);
	drawRaster(vec_down,vIndex, surfaceindex, faceindex, ifTexture);
	
	return;
}

void drawRaster_thread(Vector4D_float* vec, int surfaceindex, int faceindex, bool ifTexture)
{


	int x[3], y[3];
	x[0] = vec[0].get_X(), x[1] = vec[1].get_X(), x[2] = vec[2].get_X(), y[0] = vec[0].get_Y(), y[1] = vec[1].get_Y(), y[2] = vec[2].get_Y(); //(x[0],y[0])最高点，(x[2],y[2])最低点
	float z[3], w[3];
	z[0] = vec[0].get_Z(), z[1] = vec[1].get_Z(), z[2] = vec[2].get_Z();
	w[0] = vec[0].get_W(), w[1] = vec[1].get_W(), w[1] = vec[1].get_W();
	if (y[1] <= y[2]) swap(x[1], x[2], y[1], y[2], z[1], z[2], w[1], w[2], v1_index, v2_index);
	if (y[0] <= y[1]) swap(x[0], x[1], y[0], y[1], z[0], z[1], w[0], w[1], v1_index, v0_index);
	if (y[1] <= y[2]) swap(x[1], x[2], y[1], y[2], z[1], z[2], w[1], w[2], v1_index, v2_index);



	if (y[0] == y[1])
	{
		drawUpRaster_thread(x, y, z, w, surfaceindex, faceindex, ifTexture);
		return;
	}
	else if (y[2] == y[1])
	{
		drawLowRaster_thread(x, y, z, surfaceindex, faceindex, ifTexture);
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
	drawRaster_thread(vec_up, surfaceindex, faceindex, ifTexture);

	vec_down[0].setVec(x[2], y[2], z[2], 1);
	vec_down[1].setVec(x[1], y[1], z[1], 1);
	vec_down[2].setVec(tmpx, y[1], tmpz, 1);
	drawRaster_thread(vec_down, surfaceindex, faceindex, ifTexture);

	return;
}

//用于测试求直线和平面的交点算法是否正确
/*
void surfaceLinetest()
{
	Line4D_float L1;
	Surface4D_float S1;
	L1.setP_LightSource(Vector4D_float(1, 1, 0.8, 1));
	L1.setP_Vertex(Vector4D_float(0.5, 0.5, 0.5, 1));
	S1.setP(Vector4D_float(0, 0, 0, 1));
	S1.setN(Vector4D_float(0, 0, 1, 0));
	Vector4D_float vec(intersectionPoint(L1, S1));
}
*/
