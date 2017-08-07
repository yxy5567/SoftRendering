#pragma once
////---------------------------------------------------------------------
////    Basic Lighting Function
////
////    Programmer:             Yang Xiaoyang
////    Last modification date: 2017-7-31
////    Creation date:          2017-7-26
////---------------------------------------------------------------------
#include "_4dMatrix.h"

//diffuse Param in world space

//将环境光设定为默认值
void set_AmbientLightVec(Vector4D_float& am_vec)
{
	am_vec.setVec(vec_AmbientLight_x, vec_AmbientLight_y, vec_AmbientLight_z, 0);
}
//计算平行光源的漫反射光，dif_vec_origin是世界空间中的光源向量，N_mdl是模型空间中的法向量
void set_DiffuseLightVec_Parallel(const Vector4D_float dif_vec_origin,Vector4D_float& dif_vec,const Vector4D_float &N_mdl, const Matrix4D_float &mat_MDLtoWLD, const Vector4D_float &L_wld) //
{
	Vector4D_float N_wld = mat_MDLtoWLD * N_mdl;
	N_wld = N_wld / N_wld.Magnitude();
	Vector4D_float L_wld_copy = L_wld / L_wld.Magnitude();
	//float DiffuseParam = (N_wld * L_wld_copy)>0? (N_wld * L_wld_copy):0;
	float DiffuseParam = ((N_wld * L_wld_copy)+1)/2;
	if (DiffuseParam < 0)
	{
		cout<<"-1"<<endl;
	}
	dif_vec.setVec(dif_vec_origin.get_X()*DiffuseParam, dif_vec_origin.get_Y()*DiffuseParam, dif_vec_origin.get_Z()*DiffuseParam, 0);
}
void set_DiffuseLightVec_Point(const Vector4D_float dif_vec_origin, Vector4D_float& dif_vec, const Vector4D_float &N_mdl, const Matrix4D_float &mat_MDLtoWLD, const Vector4D_float &L_wld) //
{
	Vector4D_float N_wld = mat_MDLtoWLD * N_mdl;
	N_wld = N_wld / N_wld.Magnitude();
	float distance = L_wld.Magnitude();
	Vector4D_float L_wld_copy = L_wld / L_wld.Magnitude();
	float DiffuseParam = (N_wld * L_wld_copy)>0? (N_wld * L_wld_copy):0;
	//float DiffuseParam = ((N_wld * L_wld_copy) + 1) / 2;
	DiffuseParam /= L_wld.Magnitude() / 10;
	if (DiffuseParam < 0)
	{
		cout << "-1" << endl;
	}
	dif_vec.setVec(dif_vec_origin.get_X()*DiffuseParam, dif_vec_origin.get_Y()*DiffuseParam, dif_vec_origin.get_Z()*DiffuseParam, 0);
}
//计算点光源的漫反射光
/*
void set_DiffuseLightVec_Point(const Vector4D_float dif_vec_origin, Vector4D_float& dif_vec, const Vector4D_float &N_mdl, const Matrix4D_float &mat_MDLtoWLD, const Vector4D_float &position_MDL) //
{
	Vector4D_float position_WLD = mat_MDLtoWLD * position_WLD; //待求点在世界坐标系中的坐标
	Vector4D_float lightSourcePosition_WLD(vec_WorldPointLight_x, vec_WorldPointLight_y, vec_WorldPointLight_z, 1); //点光源在世界坐标系中的坐标
	Vector4D_float L_wld = lightSourcePosition_WLD - position_WLD;
	Vector4D_float N_wld = mat_MDLtoWLD * N_mdl;
	N_wld = N_wld / N_wld.Magnitude();
	Vector4D_float L_wld_copy = L_wld / L_wld.Magnitude();
	float DiffuseParam = N_wld * L_wld_copy;
	dif_vec.setVec(dif_vec_origin.get_X()*DiffuseParam, dif_vec_origin.get_Y()*DiffuseParam, dif_vec_origin.get_Z()*DiffuseParam, 0);
}
*/



