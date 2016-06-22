/*
	Mech.cpp and Mech.h are the files that make
	the Mech Engine what it is. The ability to
	click to move, select units, tell a story,
	etc... All those elements will be addressed
	here. 
*/
#ifndef MY_DIRECT_X_H
#define MY_DIRECT_X_H
#include "MyDirectX.h"
#endif

#ifndef MECH_H
#define MECH_H
#include "Mech.h"
#endif

// constants for functions
const double PI = 3.1415926535;
const double PI_under_180 = 180.0f / PI;
const double PI_over_180 = PI / 180.0f;

// math functions
double toRadians(double degrees)
{
	return degrees * PI_over_180;
}

double toDegrees(double radians)
{
	return radians * PI_under_180;
}

double wrap(double value, double bounds)
{
	double result = fmod(value, bounds);
	if (result < 0) result += bounds;
	return result;
}

double wrapAngleDegs(double degs)
{
	return wrap(degs, 360.0);
}

double LinearVelocityX(double angle)
{
	if (angle < 0) angle = 360 + angle;
	return cos(angle * PI_over_180);
}

double LinearVelocityY(double angle)
{
	if (angle < 0) angle = 360 + angle;
	return sin(angle * PI_over_180);
}

// set values for the unit
void UNIT::setUnit(std::string filename, std::string unitName, float speed)
{
	loadModel(filename);
	name = unitName;
	speedMult = speed;
	endPosition = translate;
}

void UNIT::moveUnit(D3DXVECTOR3 endPos)
{
	D3DXVec3Lerp(&translate, &translate, &endPos, speedMult);
}