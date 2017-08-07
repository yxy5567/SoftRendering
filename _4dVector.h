#pragma once
////---------------------------------------------------------------------
////    Basic 4D vector class
////
////    Programmer:             Yang Xiaoyang
////    Last modification date: 2017-7-31
////    Creation date:          2017-7-14
////---------------------------------------------------------------------
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "_BasicParameter.h"
//---Vector4D_float---
class Vector4D_float
{
public:
	//构造函数
	Vector4D_float() : m_X(0.0f), m_Y(0.0f), m_Z(0.0f),m_W(0.0f) {}	//Memo：构造函数后加冒号，成员变量初始化
	//拷贝构造函数
	Vector4D_float(const Vector4D_float& Vec) : m_X(Vec.m_X), m_Y(Vec.m_Y), m_Z(Vec.m_Z),m_W(Vec.m_W) {}
	//赋值构造函数
	Vector4D_float(float X, float Y, float Z, float W) :m_X(X), m_Y(Y), m_Z(Z), m_W(W){}
	//~Vector4D_float() { ; }
	float get_X() const { return(m_X); }
	void set_X(const float X) { m_X = X; }
	float get_Y() const { return(m_Y); }
	void set_Y(const float Y) { m_Y = Y; }
	float get_Z() const { return(m_Z); }
	void set_Z(const float Z) { m_Z = Z; }
	float get_W() const { return(m_W); }
	void set_W(const float W) { m_W = W; }
	//赋值函数
	void setVec(const float X, const float Y, const float Z, const float W)
	{
		m_X = X;
		m_Y = Y;
		m_Z = Z;
		m_W = W;
	}
	//Array access to X,Y,Z,W
	float& operator[](unsigned int i)
	{
		switch (i)
		{
		case 0: return(m_X);
		case 1: return(m_Y);
		case 2: return(m_Z);
		case 3: return(m_W);
		default: return(m_W);
		}
	}
	//Assignment
	Vector4D_float& operator=(const Vector4D_float Vec) 
	{
		m_X = Vec.m_X; m_Y = Vec.m_Y; m_Z = Vec.m_Z;  m_W = Vec.m_W; return(*this);
	}
	//Negation
	Vector4D_float operator-() 
	{
		return(Vector4D_float(-m_X, -m_Y, -m_Z, -m_W));
	}
	//Sum
	Vector4D_float operator+(const Vector4D_float& Vec) 
	{
		return(Vector4D_float(m_X + Vec.m_X, m_Y + Vec.m_Y, m_Z + Vec.m_Z, m_W + Vec.m_W));
	}
	//Minus
	Vector4D_float operator-(const Vector4D_float& Vec) 
	{
		return(Vector4D_float(m_X - Vec.m_X, m_Y - Vec.m_Y, m_Z - Vec.m_Z, m_W - Vec.m_W));
	}
	//Dot Product
	float operator*(const Vector4D_float& Vec) const 
	{
		return(m_X*Vec.m_X + m_Y*Vec.m_Y + m_Z*Vec.m_Z + m_W*Vec.m_W);
	}
	//Scale
	Vector4D_float operator*(const float Scale) const 
	{
		return(Vector4D_float(m_X*Scale, m_Y*Scale, m_Z*Scale, m_W*Scale));
	}
	Vector4D_float operator/(const float Scale) const
	{
		return(Vector4D_float(operator*(1.0f / Scale)));
	}
	//Vector Length ^2
	float Magnitude2() const 
	{
		return((m_X)*(m_X)+(m_Y)*(m_Y)+(m_Z)*(m_Z)+(m_W)*(m_W));
	}
	//Vector Length
	float Magnitude() const 
	{
		return(static_cast<float>(sqrt(Magnitude2())));
	}
	//绝对值
	void fabs()
	{
		if (m_X < 0.0f)	m_X = -m_X;
		if (m_Y < 0.0f)	m_Y = -m_Y;
		if (m_Z < 0.0f)	m_Z = -m_Z;
		if (m_W < 0.0f)	m_W = -m_W;
	}
	//单位化
	void Unit()
	{
		float len = Magnitude();
		if (len == 0.0f)
		{
			m_X = len; m_Y = len; m_Z = len; m_W = len;
		}
		else
		{
			len = 1.0f / len;
			m_X = m_X*len;
			m_Y = m_Y*len;
			m_Z = m_Z*len;
			m_W = m_W*len;
		}
	}
	//Cross Product
	Vector4D_float operator%(const Vector4D_float& Vec)  const 
	{
		return(Vector4D_float((m_Y*Vec.m_Z) - (m_Z*Vec.m_Y),
			(m_Z*Vec.m_X) - (m_X*Vec.m_Z),
			(m_X*Vec.m_Y) - (m_Y*Vec.m_X),
			m_W));
	}
	int operator==(const Vector4D_float& Cmp) const
	{
		return((m_X == Cmp.m_X) && (m_Y == Cmp.m_Y) && (m_Z == Cmp.m_Z) && (m_W == Cmp.m_W));
	}
protected:
	float m_X;
	float m_Y;
	float m_Z;
	float m_W;
};
// vec_A ( x_A , y_A , ... , value_A ) , vec_B , vec_C为已知信息的向量。知道了P的部分维信息，插值得到P的Value（W分量）
float _2D_Interpolation(const Vector4D_float& vec_A, const Vector4D_float& vec_B, const Vector4D_float& vec_C, Vector4D_float& vec_P)
{
	//(xP-xA) = (xB-xA)*m + (xC-xA)*n
	//(yP-yA) = (yB-yA)*m + (yC-yA)*n
	//wP = (wB-wA)*m+(wC-wA)*n + wA
	//m = ((V_C - V_A)*(U_P - U_A) - (U_C - U_A)*(V_P - V_A)) / ((V_C - V_A)*(U_B - U_A) - (U_C - U_A)*(V_B - V_A));
	//n = ((V_B - V_A)*(U_P - U_A) - (U_B - U_A)*(V_P - V_A)) / ((V_B - V_A)*(U_C - U_A) - (U_B - U_A)*(V_C - V_A));
	float U_A = vec_A.get_X(), V_A = vec_A.get_Y(), W_A = vec_A.get_W();
	float U_B = vec_B.get_X(), V_B = vec_B.get_Y(), W_B = vec_B.get_W();
	float U_C = vec_C.get_X(), V_C = vec_C.get_Y(), W_C = vec_C.get_W();
	float U_P = vec_P.get_X(), V_P = vec_P.get_Y(), W_P;
	float m, n;
	if (U_A == U_P&&V_A == V_P)
		return(W_A);
	if (U_B == U_P&&V_B == V_P)
		return(W_B);
	if (U_C == U_P&&V_C == V_P)
		return(W_C);
	m = ((V_C - V_A)*(U_P - U_A) - (U_C - U_A)*(V_P - V_A)) / ((V_C - V_A)*(U_B - U_A) - (U_C - U_A)*(V_B - V_A));
	n = ((V_B - V_A)*(U_P - U_A) - (U_B - U_A)*(V_P - V_A)) / ((V_B - V_A)*(U_C - U_A) - (U_B - U_A)*(V_C - V_A));
	W_P = (W_B - W_A)*m + (W_C - W_A)*n + W_A;
	return(W_P);
}