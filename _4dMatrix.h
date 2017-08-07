#pragma once
////---------------------------------------------------------------------
////    Basic 4D Matrix class
////
////    Programmer:             Yang Xiaoyang
////    Last modification date: 2017-7-31
////    Creation date:          2017-7-13
////---------------------------------------------------------------------
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <iostream>
#include <iomanip>
#include "_4dVector.h"
#include "_BasicParameter.h"
using namespace std;
//---Matrix4D_float---
class Matrix4D_float
{
public:
	//�����е�Ԫ�����飨��ά��
	float** m_elem;
	//���캯��
	Matrix4D_float()
	{
		//Ĭ��4*4����
		m_elem = new float*[matrix_M];
		for (int i = 0; i < matrix_N; i++)
			m_elem[i] = new float[matrix_N];
		for (int i = 0; i < matrix_M; i++)
			for (int j = 0; j < matrix_N; j++)
				m_elem[i][j] = 0.0f;
	}
	//�������캯��
	Matrix4D_float(const Matrix4D_float& Mat)
	{
		m_elem = new float*[matrix_M];
		for (int i = 0; i < matrix_N; i++)
			m_elem[i] = new float[matrix_N];
		for (int i = 0; i < matrix_M; i++)
			for (int j = 0; j < matrix_N; j++)
				m_elem[i][j] = Mat.m_elem[i][j];
	}
	//��ֵ���캯��
	Matrix4D_float(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33)
	{
		m_elem = new float*[matrix_M];
		for (int i = 0; i < matrix_N; i++)
			m_elem[i] = new float[matrix_N];
		m_elem[0][0] = m00; m_elem[0][1] = m01; m_elem[0][2] = m02; m_elem[0][3] = m03;
		m_elem[1][0] = m10; m_elem[1][1] = m11; m_elem[1][2] = m12; m_elem[1][3] = m13;
		m_elem[2][0] = m20; m_elem[2][1] = m21; m_elem[2][2] = m22; m_elem[2][3] = m23;
		m_elem[3][0] = m30; m_elem[3][1] = m31; m_elem[3][2] = m32; m_elem[3][3] = m33;
	}
	//��������
	~Matrix4D_float() 
	{
		for (int i = 0; i < matrix_M; i++)
			delete[] m_elem[i];
		delete[] m_elem;
	}
	//��ֵ����
	void setMatrix4D_float(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33)
	{
		m_elem[0][0] = m00; m_elem[0][1] = m01; m_elem[0][2] = m02; m_elem[0][3] = m03;
		m_elem[1][0] = m10; m_elem[1][1] = m11; m_elem[1][2] = m12; m_elem[1][3] = m13;
		m_elem[2][0] = m20; m_elem[2][1] = m21; m_elem[2][2] = m22; m_elem[2][3] = m23;
		m_elem[3][0] = m30; m_elem[3][1] = m31; m_elem[3][2] = m32; m_elem[3][3] = m33;
	}
	//��λ��
	void unitMatrix()
	{
		setMatrix4D_float(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
	}
	//���ز��������ں�"="
	Matrix4D_float& operator=(const Matrix4D_float Mat) //Assignment
	{
		for (int i = 0; i < matrix_M; i++)
			for (int j = 0; j < matrix_N; j++)
				m_elem[i][j] = Mat.m_elem[i][j];
		return(*this);
	}
	//���ز�����ȡ������"-"����һ������
	Matrix4D_float operator-() //Negation
	{
		return(Matrix4D_float(-m_elem[0][0], -m_elem[0][1], -m_elem[0][2], -m_elem[0][3], -m_elem[1][0], -m_elem[1][1], -m_elem[1][2], -m_elem[1][3], -m_elem[2][0], -m_elem[2][1], -m_elem[2][2], -m_elem[2][3], -m_elem[3][0], -m_elem[3][1], -m_elem[3][2], -m_elem[3][3]));
	}
	//���ز������Ӻ�"+"����һ������
	Matrix4D_float operator+(const Matrix4D_float& Mat) //Sum
	{
		return(Matrix4D_float(m_elem[0][0] + Mat.m_elem[0][0], m_elem[0][1] + Mat.m_elem[0][1], m_elem[0][2] + Mat.m_elem[0][2], m_elem[0][3] + Mat.m_elem[0][3], m_elem[1][0] + Mat.m_elem[1][0], m_elem[1][1] + Mat.m_elem[1][1], m_elem[1][2] + Mat.m_elem[1][2], m_elem[1][3] + Mat.m_elem[1][3], m_elem[2][0] + Mat.m_elem[2][0], m_elem[2][1] + Mat.m_elem[2][1], m_elem[2][2] + Mat.m_elem[2][2], m_elem[2][3] + Mat.m_elem[2][3], m_elem[3][0] + Mat.m_elem[3][0], m_elem[3][1] + Mat.m_elem[3][1], m_elem[3][2] + Mat.m_elem[3][2], m_elem[3][3] + Mat.m_elem[3][3]));
	}
	//���ز���������"-"����һ������
	Matrix4D_float operator-(const Matrix4D_float& Mat) //Minus
	{
		return(Matrix4D_float(m_elem[0][0] - Mat.m_elem[0][0], m_elem[0][1] - Mat.m_elem[0][1], m_elem[0][2] - Mat.m_elem[0][2], m_elem[0][3] - Mat.m_elem[0][3], m_elem[1][0] - Mat.m_elem[1][0], m_elem[1][1] - Mat.m_elem[1][1], m_elem[1][2] - Mat.m_elem[1][2], m_elem[1][3] - Mat.m_elem[1][3], m_elem[2][0] - Mat.m_elem[2][0], m_elem[2][1] - Mat.m_elem[2][1], m_elem[2][2] - Mat.m_elem[2][2], m_elem[2][3] - Mat.m_elem[2][3], m_elem[3][0] - Mat.m_elem[3][0], m_elem[3][1] - Mat.m_elem[3][1], m_elem[3][2] - Mat.m_elem[3][2], m_elem[3][3] - Mat.m_elem[3][3]));
	}
	//���ز������˺�"*"����һ������
	Matrix4D_float operator*(const Matrix4D_float& Mat) //Product
	{
		float m00 = m_elem[0][0] * Mat.m_elem[0][0] + m_elem[0][1] * Mat.m_elem[1][0] + m_elem[0][2] * Mat.m_elem[2][0]+ m_elem[0][3] * Mat.m_elem[3][0];
		float m01 = m_elem[0][0] * Mat.m_elem[0][1] + m_elem[0][1] * Mat.m_elem[1][1] + m_elem[0][2] * Mat.m_elem[2][1] + m_elem[0][3] * Mat.m_elem[3][1];
		float m02 = m_elem[0][0] * Mat.m_elem[0][2] + m_elem[0][1] * Mat.m_elem[1][2] + m_elem[0][2] * Mat.m_elem[2][2] + m_elem[0][3] * Mat.m_elem[3][2];
		float m03 = m_elem[0][0] * Mat.m_elem[0][3] + m_elem[0][1] * Mat.m_elem[1][3] + m_elem[0][2] * Mat.m_elem[2][3] + m_elem[0][3] * Mat.m_elem[3][3];

		float m10 = m_elem[1][0] * Mat.m_elem[0][0] + m_elem[1][1] * Mat.m_elem[1][0] + m_elem[1][2] * Mat.m_elem[2][0] + m_elem[1][3] * Mat.m_elem[3][0];
		float m11 = m_elem[1][0] * Mat.m_elem[0][1] + m_elem[1][1] * Mat.m_elem[1][1] + m_elem[1][2] * Mat.m_elem[2][1] + m_elem[1][3] * Mat.m_elem[3][1];
		float m12 = m_elem[1][0] * Mat.m_elem[0][2] + m_elem[1][1] * Mat.m_elem[1][2] + m_elem[1][2] * Mat.m_elem[2][2] + m_elem[1][3] * Mat.m_elem[3][2];
		float m13 = m_elem[1][0] * Mat.m_elem[0][3] + m_elem[1][1] * Mat.m_elem[1][3] + m_elem[1][2] * Mat.m_elem[2][3] + m_elem[1][3] * Mat.m_elem[3][3];

		float m20 = m_elem[2][0] * Mat.m_elem[0][0] + m_elem[2][1] * Mat.m_elem[1][0] + m_elem[2][2] * Mat.m_elem[2][0] + m_elem[2][3] * Mat.m_elem[3][0];
		float m21 = m_elem[2][0] * Mat.m_elem[0][1] + m_elem[2][1] * Mat.m_elem[1][1] + m_elem[2][2] * Mat.m_elem[2][1] + m_elem[2][3] * Mat.m_elem[3][1];
		float m22 = m_elem[2][0] * Mat.m_elem[0][2] + m_elem[2][1] * Mat.m_elem[1][2] + m_elem[2][2] * Mat.m_elem[2][2] + m_elem[2][3] * Mat.m_elem[3][2];
		float m23 = m_elem[2][0] * Mat.m_elem[0][3] + m_elem[2][1] * Mat.m_elem[1][3] + m_elem[2][2] * Mat.m_elem[2][3] + m_elem[2][3] * Mat.m_elem[3][3];

		float m30 = m_elem[3][0] * Mat.m_elem[0][0] + m_elem[3][1] * Mat.m_elem[1][0] + m_elem[3][2] * Mat.m_elem[2][0] + m_elem[3][3] * Mat.m_elem[3][0];
		float m31 = m_elem[3][0] * Mat.m_elem[0][1] + m_elem[3][1] * Mat.m_elem[1][1] + m_elem[3][2] * Mat.m_elem[2][1] + m_elem[3][3] * Mat.m_elem[3][1];
		float m32 = m_elem[3][0] * Mat.m_elem[0][2] + m_elem[3][1] * Mat.m_elem[1][2] + m_elem[3][2] * Mat.m_elem[2][2] + m_elem[3][3] * Mat.m_elem[3][2];
		float m33 = m_elem[3][0] * Mat.m_elem[0][3] + m_elem[3][1] * Mat.m_elem[1][3] + m_elem[3][2] * Mat.m_elem[2][3] + m_elem[3][3] * Mat.m_elem[3][3];

		return(Matrix4D_float(m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33));
	}
	//---���Ա任���---
	//ƽ�Ʊ任�����þ������ֵΪ��Ӧ�任����ˣ�����
	void MatrixShift(float x_shift, float y_shift, float z_shift)
	{
		setMatrix4D_float(1, 0, 0, x_shift, 0, 1, 0, y_shift, 0, 0, 1, z_shift, 0, 0, 0, 1);
	}
	//�����任�����þ������ֵΪ��Ӧ�任����ˣ�����
	void MatrixScale(float x_scale, float y_scale, float z_scale)
	{
		setMatrix4D_float(x_scale, 0, 0, 0, 0, y_scale, 0, 0, 0, 0, z_scale, 0, 0, 0, 0, 1);
	}
	//��X����ת�任�����þ������ֵΪ��Ӧ�任����ˣ�����
	void MatrixRotate_X(float x_Angle)
	{
		setMatrix4D_float(
			1, 0, 0, 0,
			0, cos(x_Angle*PI / 180), -sin(x_Angle*PI / 180), 0,
			0, sin(x_Angle*PI / 180), cos(x_Angle*PI / 180), 0,
			0, 0, 0, 1
			);
	}
	//��Y����ת�任�����þ������ֵΪ��Ӧ�任����ˣ�����
	void MatrixRotate_Y(float y_Angle)
	{
		setMatrix4D_float(
			cos(y_Angle*PI / 180), 0, sin(y_Angle*PI / 180), 0,
			0, 1, 0, 0,
			-sin(y_Angle*PI / 180), 0, cos(y_Angle*PI / 180), 0,
			0, 0, 0, 1
		);
	}
	//��Z����ת�任�����þ������ֵΪ��Ӧ�任����ˣ�����
	void MatrixRotate_Z(float z_Angle)
	{
		setMatrix4D_float(
			cos(z_Angle*PI / 180), -sin(z_Angle*PI / 180), 0, 0,
			sin(z_Angle*PI / 180), cos(z_Angle*PI / 180), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		);
	}
	//Zֵȡ���任�����ڴ�World Space������ϵ��ת����View Space������ϵ����ת�������þ������ֵΪ��Ӧ�任����ˣ�����
	void NegnateZ()
	{
		setMatrix4D_float(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1);
	}
	//View Space��Clip Space��ͶӰ���󣬽��þ������ֵΪ��Ӧ�任����ˣ�����
	void MatrixFrustum(float FOV, float clip_aspect, float clip_near, float clip_far)
	{
		setMatrix4D_float(
			1.0f / (tan(FOV*PI / 360.0f)*clip_aspect), 0, 0, 0,
			0, 1.0f / tan(FOV*PI / 360.0f), 0, 0,
			0, 0, -1.0f*(clip_far + clip_near) / (clip_far - clip_near), -2.0f*(clip_near*clip_far) / (clip_far - clip_near), 
			0, 0, -1, 0
		);
	}
	//������ʾ
	void showMatrix()
	{
		cout.flags(ios::right); //�Ҷ���
		for (int i = 0; i < matrix_M; i++)
		{
			for (int j = 0; j < matrix_N; j++)
				cout << setw(10) << setprecision(5) << m_elem[i][j];
			cout << endl;
		}
		cout << endl;
	}
};

//���ز������˺�"*"������Ϊ �Ľ׾������*��ά�������� �����Ҳ��ɵߵ�
Vector4D_float operator*(Matrix4D_float mat, Vector4D_float vec)
{
	float x = mat.m_elem[0][0] * vec.get_X() + mat.m_elem[0][1] * vec.get_Y() + mat.m_elem[0][2] * vec.get_Z() + mat.m_elem[0][3] * vec.get_W();
	float y = mat.m_elem[1][0] * vec.get_X() + mat.m_elem[1][1] * vec.get_Y() + mat.m_elem[1][2] * vec.get_Z() + mat.m_elem[1][3] * vec.get_W();
	float z = mat.m_elem[2][0] * vec.get_X() + mat.m_elem[2][1] * vec.get_Y() + mat.m_elem[2][2] * vec.get_Z() + mat.m_elem[2][3] * vec.get_W();
	float w = mat.m_elem[3][0] * vec.get_X() + mat.m_elem[3][1] * vec.get_Y() + mat.m_elem[3][2] * vec.get_Z() + mat.m_elem[3][3] * vec.get_W();
	Vector4D_float v(x, y, z, w);
	return(v);
}

