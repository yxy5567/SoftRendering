#pragma once
#include"_BasicParameter.h"
const float accele = -0.4;
const float v0 = 1;
float velocity_current = 0;
float y_current = 0;
int rebound_count = -1;

const int flaster_Num=4;
float flaster_Param = 2.5;
bool flaster_mode = false;



//自由落体反弹

float basic_FreeFall(float t,float y0,float v0)
{
	//y=y0+v0t+1/2at^2
	float y = y0 + v0*t + 0.5f * accele*t*t;
	return(y);
}

float basic_Rebound(float decay_Param,float v_reb)
{
	if (v_reb < 0)
		return(-1.0f*v_reb*decay_Param);
	else return(v_reb);
}

void basic_FreeFallnRebound(float tSpan_clock ,float decay_Param )
{
	if (flaster_mode == true)
	{
		if (rebound_count != -1 && rebound_count <= flaster_Num)
		{
			cubeScale_y = 1.0f - 0.03f*flaster_Param*rebound_count;
			rebound_count++;
			y_shift = y_shift - 0.03f*flaster_Param;
			return;
		}
		if (rebound_count != -1 && rebound_count <= 2 * flaster_Num)
		{
			cubeScale_y = 1.0f - 0.03f*flaster_Param*flaster_Num + (rebound_count - flaster_Num)*flaster_Param*0.03f;
			rebound_count++;
			if (rebound_count == 2 * flaster_Num + 1)
				rebound_count = -1;
			y_shift = y_shift + 0.03f*flaster_Param;
			return;
		}
	}
	//会发生反弹

	if (velocity_current < 0 && (-velocity_current*tSpan_clock - 0.5f*accele*tSpan_clock*tSpan_clock)>y_current)
	{
		//-vt-0.5att=y_current t=  -v+sqrt(v*v-2ay)  /   a
		float t1=(-1.0f*velocity_current-sqrt(velocity_current*velocity_current-2.0f*accele*y_current))/accele;
		float t2 = tSpan_clock - t1;
		float vGround = (-1.0*velocity_current + t1*accele)*decay_Param;
		velocity_current = vGround + accele*t2;
		y_current = vGround*t2 + 0.5f*accele*t2*t2;
		y_shift = y_current;
		if (flaster_mode == true)
		{
			rebound_count = 0;
			flaster_Param *= 0.95;
		}
		return;
	}
	//不会发生反弹
	else
	{
		if (flaster_mode == true)
		{
			rebound_count = -1;
			cubeScale_y = 1;
		}
		y_current = y_current + velocity_current*tSpan_clock + 0.5f*accele*tSpan_clock*tSpan_clock;
		velocity_current = velocity_current + accele*tSpan_clock;
		y_shift = y_current;
		return;
	}
	
}