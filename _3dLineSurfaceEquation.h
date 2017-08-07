#pragma once
#include "_4dVector.h"

class Line4D_float
{
public:
	Vector4D_float P1, P2;
	void setP_LightSource(const Vector4D_float& P) { P2 = P; }
	void setP_Vertex(const Vector4D_float& P) { P1 = P; }
};

class Surface4D_float
{
public:
	Vector4D_float N, P;
	void setN(const Vector4D_float& N1) { N = N1; }
	void setP(const Vector4D_float& P1) { P = P1; }
};

Vector4D_float intersectionPoint(const Line4D_float& L1, Surface4D_float& S1)
{
	Vector4D_float temp(L1.P1.get_X() - L1.P2.get_X(), L1.P1.get_Y() - L1.P2.get_Y(), L1.P1.get_Z() - L1.P2.get_Z(), L1.P1.get_W() - L1.P2.get_W());
	if (temp*S1.N == 0)
	{
		return(Vector4D_float(0, 0, 0, 0));
	}
	float t,paramLeft,paramRight;
	//平面：Xn(X-X0) + Yn(Y-Y0) + Zn(Z-Z0) = 0
	//直线：X=X1+(X2-X1)t ; Y=Y1+(Y2-Y1)t ; Z=Z1+(Z2-Z1)t
	//联立：Xn(X2-X1)t + Yn(Y2-Y1)t + Zn(Z2-Z1)t = XnX0 + YnY0 + ZnZ0 - ( XnX1 +YnY1 +ZnZ1)
	paramLeft = (S1.N.get_X()*(L1.P2.get_X() - L1.P1.get_X())) + (S1.N.get_Y()*(L1.P2.get_Y() - L1.P1.get_Y())) + (S1.N.get_Z()*(L1.P2.get_Z() - L1.P1.get_Z()));
	paramRight = S1.N.get_X()*S1.P.get_X() + S1.N.get_Y()*S1.P.get_Y() + S1.N.get_Z()*S1.P.get_Z() - (S1.N.get_X()*L1.P1.get_X() + S1.N.get_Y()*L1.P1.get_Y() + S1.N.get_Z()*L1.P1.get_Z());
	t = paramRight / paramLeft;
	float _X = L1.P1.get_X() + (L1.P2.get_X() - L1.P1.get_X())*t;
	float _Y = L1.P1.get_Y() + (L1.P2.get_Y() - L1.P1.get_Y())*t;
	float _Z = L1.P1.get_Z() + (L1.P2.get_Z() - L1.P1.get_Z())*t;
	return(Vector4D_float(_X, _Y, _Z, 1));
}