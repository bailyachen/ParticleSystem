#include "DOF.h"

void DOF::SetValue(float v)
{
	if (v < min) value = min;
	else if (v > max) value = max;
	else value = v;
}

float DOF::GetValue()
{
	return value;
}

void DOF::SetMinMax(float mi, float ma)
{
	min = mi;
	max = ma;
}

float DOF::GetMin()
{
	return min;
}

float DOF::GetMax()
{
	return max;
}

