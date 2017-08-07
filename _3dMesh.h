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
	//�ö����ڶ��������е�index
	int m_vertIndex;
	//�ö�����Դ����ͼ���ж�Ӧ�ĵ��ڵ������е�index
	int m_textureIndex;
	//�ö���ķ������ڷ����������е�index
	int m_normIndex;
};
//---Face---����һ����դ�������Σ�
class Face
{
public:
	//�ù�դ�����Ķ�������һ��Ϊ3��
	int m_nVerts;
	//��������
	VertexID* m_vert;
	Face() { m_nVerts = 0; m_vert = NULL; }
	~Face() { delete[] m_vert; m_nVerts = 0; }
};
//---Surface---����һ���棨���������դ��
class Surface
{
public:
	//��������Ĺ�դ��
	int m_nFaces;
	//��դ����
	Face* m_face;
	Surface() { m_nFaces = 0; m_face = NULL; }
	~Surface() { delete[] m_face; m_nFaces = 0; }
};
//---Mesh---����һ����״��
class Mesh
{
public:
	//�������������
	int m_numVerts;
	//�������飬���˶�������꣨��ά��wֵΪ1��
	Vector4D_float* m_pt;
	//����Ķ�����Ŀ
	int m_numNormals;
	//���������飬���˷����������꣨��ά��
	Vector4D_float* m_norm; 
	//���������Դ����Ŀ
	int m_uv_texture;
	//����Դ����
	Vector4D_float* m_texture;
	//���������Ŀ
	int m_numSurfaces;
	//������
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
	//��ʱ�ò���
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
	//��Mesh�Ķ����ʼ������Ϊһ��������
	void initialCube()
	{
		m_numVerts = 8;
		m_numNormals = 6;
		m_uv_texture = 6;
		m_numSurfaces = 6;
		m_pt = new Vector4D_float[m_numVerts];
		m_norm = new Vector4D_float[m_numNormals];
//		m_texture = new Vector4D_float[m_uv_texture]; //��ʱ�ò���
		m_surface = new Surface[m_numSurfaces];
		//��ÿ��surface�½�����Ӧ��Ŀ��Face����
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
		//		m_texture = new Vector4D_float[m_uv_texture]; //��ʱ�ò���
		m_surface = new Surface[m_numSurfaces];
		//��ÿ��surface�½�����Ӧ��Ŀ��Face����
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
	//��ȡobj�ļ�
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
			//���������ڶ��ת����ʹ��ͬһ��stringstream���󣬼�ס��ÿ��ת��ǰҪʹ��.str(string����)����.clear()���������stringstream������ȱһ���ɣ�ǰ�߸�ֵ����������״̬
			//�ڶ��ת�����ظ�ʹ��ͬһ��stringstream��������ÿ�ζ�����һ���µĶ��󣩶������ĺô�����Ч�ʡ�stringstream����Ĺ������������ͨ���Ƿǳ��ķ�CPUʱ��ġ�
			strstrm >> typetag;
			if (linetmp != "") //�����һ�����У��ڸô�ѭ����typetag��ȻΪ��һ�ε�ֵ���ͻ���ۼ�һ��
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

		ReadFile.open(fileName, ios::in); //����ÿһ��Surface�ֱ��Ӧ����Face
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
		m_surface[n_sf].m_nFaces = n_f; //���һ��surface
		ReadFile.close();

		for (int i = 0; i < m_numSurfaces; i++) //��ÿ��surface�½�����Ӧ��Ŀ��Face����
		{
			m_surface[i].m_face = new Face[m_surface[i].m_nFaces];
			for (int j = 0; j < m_surface[i].m_nFaces; j++)
			{
				m_surface[i].m_face[j].m_nVerts = 3;
				m_surface[i].m_face[j].m_vert = new VertexID[3];
			}
		}


		int n_pt = 0, n_nm = 0, n_tu = 0;
		n_sf=-1; //n_sf=-1����Ϊ��һ�γ���"s 1"��ʱ��,n_sf++�պõ�0
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
				else if (typetag == "f") //��ֵ����m_face[n_f]��һ��������������
				{
					for (int i = 0; i <= 2; i++)
					{
						strstrm >> tmpValue;
						//TODO:������"c/b/a"������string���뵽m_face[n_f].m_vert[i]�С�����int m_vertIndexȡc-1; m_textureIndexȡb-1;m_normIndexȡa-1��
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

