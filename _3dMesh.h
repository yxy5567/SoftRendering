#pragma once
////---------------------------------------------------------------------
////    Basic 3D Vertex, Face and Mesh class
////
////    Programmer:             Yang Xiaoyang
////    Last modification date: 2017-7-31
////    Creation date:          2017-7-13
////---------------------------------------------------------------------
#include "_4dVector.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#ifndef NULL
#define NULL 0
#endif
using namespace std;
//---VertexID---
class VertexID
{
public:
	//该顶点在顶点数组中的index
	int m_vertIndex;
	//该顶点在源纹理图案中对应的点在点数组中的index
	int m_textureIndex;
	//该顶点的法向量在法向量数组中的index
	int m_normIndex;
};
//---Face---代表一个光栅（三角形）
class Face
{
public:
	//该光栅包含的顶点数（一般为3）
	int m_nVerts;
	//顶点数组
	VertexID* m_vert;
	Face() { m_nVerts = 0; m_vert = NULL; }
	~Face() { delete[] m_vert; m_nVerts = 0; }
};
//---Surface---代表一个面（包含多个光栅）
class Surface
{
public:
	//该面包含的光栅数
	int m_nFaces;
	//光栅数组
	Face* m_face;
	Surface() { m_nFaces = 0; m_face = NULL; }
	~Surface() { delete[] m_face; m_nFaces = 0; }
};
//---Mesh---代表一个网状体
class Mesh
{
public:
	//该体包含的面数
	int m_numVerts;
	//顶点数组，存了顶点的坐标（四维，w值为1）
	Vector4D_float* m_pt;
	//该体的顶点数目
	int m_numNormals;
	//法向量数组，存了法向量的坐标（四维）
	Vector4D_float* m_norm; 
	//该体的纹理源的数目
	int m_uv_texture;
	//纹理源数组
	Vector4D_float* m_texture;
	//该体的面数目
	int m_numSurfaces;
	//面数组
	Surface* m_surface;

	Mesh()
	{
		m_numVerts=0;
		m_pt=NULL;
		m_numNormals=0;
		m_norm=NULL;
		m_uv_texture=0;
		m_texture=NULL;
		m_numSurfaces=0;
		m_surface=NULL;
	};
/*
	//暂时用不上
	Mesh(int nVerts, int nNorms, int nTexs, int nFaces) 
	{
		m_pt = new Vector4D_float[nVerts];
		m_numVerts = nVerts;
		m_norm = new Vector4D_float[nNorms];
		m_numNormals = nNorms;
		m_uv_texture = nTexs;
		m_texture = new Vector4D_float[nTexs];
		m_face = new Face[nFaces];
		m_numFaces = nFaces;
	};
*/  
	~Mesh()
	{
		delete[] m_pt;
		delete[] m_norm;
	//	for (int i = 0; i < m_numSurfaces; i++)
	//		delete[] m_surface[i].m_face;
		delete[] m_surface;
		delete[] m_texture;
		m_numVerts = 0;
		m_pt = NULL;
		m_numNormals = 0;
		m_norm = NULL;
		m_uv_texture = 0;
		m_texture = NULL;
		m_numSurfaces = 0;
		m_surface = NULL;
	};
	//将Mesh的对象初始化，存为一个正方体
	void initialCube()
	{
		m_numVerts = 8;
		m_numNormals = 6;
		m_uv_texture = 6;
		m_numSurfaces = 6;
		m_pt = new Vector4D_float[m_numVerts];
		m_norm = new Vector4D_float[m_numNormals];
//		m_texture = new Vector4D_float[m_uv_texture]; //暂时用不上
		m_surface = new Surface[m_numSurfaces];
		//在每个surface下建立对应数目的Face对象
		for (int i = 0; i < m_numSurfaces; i++) 
		{
			m_surface[i].m_nFaces = 2;
			m_surface[i].m_face = new Face[m_surface[i].m_nFaces];
			for (int j = 0; j < m_surface[i].m_nFaces; j++)
			{
				m_surface[i].m_face[j].m_nVerts = 3;
				m_surface[i].m_face[j].m_vert = new VertexID[3];
			}
		}
		m_pt[0].setVec(-0.5, -0.5, 0.5, 1);
		m_pt[1].setVec(0.5, -0.5, 0.5, 1);
		m_pt[2].setVec(-0.5, 0.5, 0.5, 1);
		m_pt[3].setVec(0.5, 0.5, 0.5, 1);
		m_pt[4].setVec(-0.5, 0.5, -0.5, 1);
		m_pt[5].setVec(0.5, 0.5, -0.5, 1);
		m_pt[6].setVec(-0.5, -0.5, -0.5, 1);
		m_pt[7].setVec(0.5, -0.5, -0.5, 1);

		m_norm[0].setVec(0, 0, 1, 0);
		m_norm[1].setVec(0, 1, 0, 0);
		m_norm[2].setVec(0, 0, -1, 0);
		m_norm[3].setVec(0, -1, 0, 0);
		m_norm[4].setVec(1, 0, 0, 0);
		m_norm[5].setVec(-1, 0, 0, 0);

		m_surface[0].m_face[0].m_vert[0].m_vertIndex = 0;
		m_surface[0].m_face[0].m_vert[1].m_vertIndex = 1;
		m_surface[0].m_face[0].m_vert[2].m_vertIndex = 2;
		m_surface[0].m_face[1].m_vert[0].m_vertIndex = 3-1;
		m_surface[0].m_face[1].m_vert[1].m_vertIndex = 2-1;
		m_surface[0].m_face[1].m_vert[2].m_vertIndex = 4-1;

		m_surface[1].m_face[0].m_vert[0].m_vertIndex = 3-1;
		m_surface[1].m_face[0].m_vert[1].m_vertIndex = 4-1;
		m_surface[1].m_face[0].m_vert[2].m_vertIndex = 5-1;
		m_surface[1].m_face[1].m_vert[0].m_vertIndex = 5-1;
		m_surface[1].m_face[1].m_vert[1].m_vertIndex = 4-1;
		m_surface[1].m_face[1].m_vert[2].m_vertIndex = 6-1;

		m_surface[2].m_face[0].m_vert[0].m_vertIndex = 5-1;
		m_surface[2].m_face[0].m_vert[1].m_vertIndex = 6-1;
		m_surface[2].m_face[0].m_vert[2].m_vertIndex = 7-1;
		m_surface[2].m_face[1].m_vert[0].m_vertIndex = 7-1;
		m_surface[2].m_face[1].m_vert[1].m_vertIndex = 6-1;
		m_surface[2].m_face[1].m_vert[2].m_vertIndex = 8-1;

		m_surface[3].m_face[0].m_vert[0].m_vertIndex = 7-1;
		m_surface[3].m_face[0].m_vert[1].m_vertIndex = 8-1;
		m_surface[3].m_face[0].m_vert[2].m_vertIndex = 1-1;
		m_surface[3].m_face[1].m_vert[0].m_vertIndex = 1-1;
		m_surface[3].m_face[1].m_vert[1].m_vertIndex = 8-1;
		m_surface[3].m_face[1].m_vert[2].m_vertIndex = 2-1;

		m_surface[4].m_face[0].m_vert[0].m_vertIndex = 2-1;
		m_surface[4].m_face[0].m_vert[1].m_vertIndex = 8-1;
		m_surface[4].m_face[0].m_vert[2].m_vertIndex = 4-1;
		m_surface[4].m_face[1].m_vert[0].m_vertIndex = 4-1;
		m_surface[4].m_face[1].m_vert[1].m_vertIndex = 8-1;
		m_surface[4].m_face[1].m_vert[2].m_vertIndex = 6-1;

		m_surface[5].m_face[0].m_vert[0].m_vertIndex = 7-1;
		m_surface[5].m_face[0].m_vert[1].m_vertIndex = 1-1;
		m_surface[5].m_face[0].m_vert[2].m_vertIndex = 5-1;
		m_surface[5].m_face[1].m_vert[0].m_vertIndex = 5-1;
		m_surface[5].m_face[1].m_vert[1].m_vertIndex = 1-1;
		m_surface[5].m_face[1].m_vert[2].m_vertIndex = 3-1;
	};
	//
	void initialGND()
	{
		m_numVerts = 4;
	//	m_numNormals = 6;
	//	m_uv_texture = 6;
		m_numSurfaces = 1;
		m_pt = new Vector4D_float[m_numVerts];
	//	m_norm = new Vector4D_float[m_numNormals];
		//		m_texture = new Vector4D_float[m_uv_texture]; //暂时用不上
		m_surface = new Surface[m_numSurfaces];
		//在每个surface下建立对应数目的Face对象
		for (int i = 0; i < m_numSurfaces; i++)
		{
			m_surface[i].m_nFaces = 2;
			m_surface[i].m_face = new Face[m_surface[i].m_nFaces];
			for (int j = 0; j < m_surface[i].m_nFaces; j++)
			{
				m_surface[i].m_face[j].m_nVerts = 3;
				m_surface[i].m_face[j].m_vert = new VertexID[3];
			}
		}
		m_pt[0].setVec(-2, -0.2, -2, 1);
		m_pt[1].setVec(2, -0.2, -2, 1);
		m_pt[2].setVec(-2, -0.2, 2, 1);
		m_pt[3].setVec(2, -0.2, 2, 1);

		m_surface[0].m_face[0].m_vert[0].m_vertIndex = 0;
		m_surface[0].m_face[0].m_vert[1].m_vertIndex = 1;
		m_surface[0].m_face[0].m_vert[2].m_vertIndex = 2;
		m_surface[0].m_face[1].m_vert[0].m_vertIndex = 1;
		m_surface[0].m_face[1].m_vert[1].m_vertIndex = 2;
		m_surface[0].m_face[1].m_vert[2].m_vertIndex = 3;
	};
	//读取obj文件
	void readFile(char* fileName)
	{
		int nVerts, nNorms, nTexs, nSurfaces;
		nVerts = nNorms = nTexs = nSurfaces = 0;
		std::stringstream strstrm;
		std::ifstream ReadFile;
		std::string linetmp;
		string typetag;

		ReadFile.open(fileName, ios::in);
		if (ReadFile.fail())
		{
			cout << "read file failed" <<endl ;
			return;
		}
		while (getline(ReadFile, linetmp, '\n'))
		{
			strstrm.str(linetmp);
			strstrm.clear();
			//如果你打算在多次转换中使用同一个stringstream对象，记住再每次转换前要使用.str(string对象)加上.clear()方法来清空stringstream；二者缺一不可，前者赋值，后者重置状态
			//在多次转换中重复使用同一个stringstream（而不是每次都创建一个新的对象）对象最大的好处在于效率。stringstream对象的构造和析构函数通常是非常耗费CPU时间的。
			strstrm >> typetag;
			if (linetmp != "") //如果是一个空行，在该次循环中typetag仍然为上一次的值，就会多累加一次
			{
				if (typetag == "v") nVerts++;
				if (typetag == "vt") nTexs++;
				if (typetag == "vn") nNorms++;
				if (typetag == "s") nSurfaces++;
			}
		}
		ReadFile.close();

		m_numVerts = nVerts;
		m_numNormals = nTexs;
		m_uv_texture = nNorms;
		m_numSurfaces = nSurfaces;
		m_pt = new Vector4D_float[m_numVerts];
		m_norm = new Vector4D_float[m_numNormals];
		m_texture = new Vector4D_float[m_uv_texture];
		m_surface = new Surface[m_numSurfaces];

		ReadFile.open(fileName, ios::in); //读出每一个Surface分别对应多少Face
		int n_sf = -1, n_f = 0;
		while (getline(ReadFile, linetmp, '\n'))
		{
			strstrm.str(linetmp);
			strstrm.clear();
			strstrm >> typetag;
			if (linetmp != "")
			{
				if (typetag == "s")
				{
					if (n_sf >= 0)
						m_surface[n_sf].m_nFaces = n_f;
					n_f = 0;
					n_sf++;
				}
				if (typetag == "f")
				{
					n_f++;
				}
			}
		}						
		m_surface[n_sf].m_nFaces = n_f; //最后一个surface
		ReadFile.close();

		for (int i = 0; i < m_numSurfaces; i++) //在每个surface下建立对应数目的Face对象
		{
			m_surface[i].m_face = new Face[m_surface[i].m_nFaces];
			for (int j = 0; j < m_surface[i].m_nFaces; j++)
			{
				m_surface[i].m_face[j].m_nVerts = 3;
				m_surface[i].m_face[j].m_vert = new VertexID[3];
			}
		}


		int n_pt = 0, n_nm = 0, n_tu = 0;
		n_sf=-1; //n_sf=-1是因为第一次出现"s 1"的时候,n_sf++刚好到0
		string tmpValue;
		ReadFile.open(fileName, ios::in);
		while (getline(ReadFile, linetmp, '\n'))
		{
			strstrm.str(linetmp);
			strstrm.clear();
			strstrm >> typetag;
			if (linetmp != "")
			{
				if (typetag == "v")
				{
					strstrm >> tmpValue;
					m_pt[n_pt].set_X(atof(tmpValue.c_str()));
					strstrm >> tmpValue;
					m_pt[n_pt].set_Y(atof(tmpValue.c_str()));
					strstrm >> tmpValue;
					m_pt[n_pt].set_Z(atof(tmpValue.c_str()));
					m_pt[n_pt].set_W(1.0f);
					n_pt++;
				}
				else if (typetag == "vt")
				{
					strstrm >> tmpValue;
					m_texture[n_tu].set_X(atof(tmpValue.c_str()));
					strstrm >> tmpValue;
					m_texture[n_tu].set_Y(atof(tmpValue.c_str()));
					m_texture[n_tu].set_Z(0);
					m_texture[n_tu].set_W(1.0f);
					n_tu++;
				}
				else if (typetag == "vn")
				{
					strstrm >> tmpValue;
					m_norm[n_nm].set_X(atof(tmpValue.c_str()));
					strstrm >> tmpValue;
					m_norm[n_nm].set_Y(atof(tmpValue.c_str()));
					strstrm >> tmpValue;
					m_norm[n_nm].set_Z(atof(tmpValue.c_str()));
					m_norm[n_nm].set_W(1.0f);
					n_nm++;
				}
				else if (typetag == "s")
				{
					n_f = 0;
					n_sf++;
				}
				else if (typetag == "f") //数值导入m_face[n_f]，一个面有三个顶点
				{
					for (int i = 0; i <= 2; i++)
					{
						strstrm >> tmpValue;
						//TODO:将类似"c/b/a"这样的string导入到m_face[n_f].m_vert[i]中。其中int m_vertIndex取c-1; m_textureIndex取b-1;m_normIndex取a-1。
 						m_surface[n_sf].m_face[n_f].m_vert[i].m_vertIndex = (int)(tmpValue[0] - '1');
						m_surface[n_sf].m_face[n_f].m_vert[i].m_textureIndex = (int)(tmpValue[2] - '1');
						m_surface[n_sf].m_face[n_f].m_vert[i].m_normIndex = (int)(tmpValue[4] - '1');
					}
					n_f++;
				}
			}
		}
		ReadFile.close();
	};
};

