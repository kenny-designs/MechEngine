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

// moves unit and rotates it based on direction moved
void UNIT::moveUnit(D3DXVECTOR3 endPos)
{
	D3DXVECTOR3 unitDir;
	double unitAngle = 0;

	unitDir = endPos - translate; // direction unit is moving in

	if (std::abs(unitDir.x) < 0.05f && std::abs(unitDir.z) < 0.05f) // reached destination
	{
		unitAngle = (double)rotate.x;
		unitDir *= 0;
		endPosition = translate;
	}
	else if (unitDir.x > 0.0f && unitDir.z > 0.0f) // Quadrant 1 
	{
		unitAngle = atan(unitDir.x / unitDir.z);
		unitAngle = std::abs(toDegrees(unitAngle) + 180);
	}
	else if (unitDir.x < 0.0f && unitDir.z > 0.0f) // Quadrant 2
	{
		unitAngle = atan(unitDir.z / unitDir.x);
		unitAngle = std::abs(toDegrees(unitAngle)) + 450;
	}
	else if (unitDir.x < 0.0f && unitDir.z < 0.0f) // Quadrant 3
	{
		unitAngle = atan(unitDir.x / unitDir.z);
		unitAngle = std::abs(toDegrees(unitAngle)) + 360;
	}
	else if (unitDir.x > 0.0f && unitDir.z < 0.0f) // Quadrant 4
	{
		unitAngle = atan(unitDir.z / unitDir.x);
		unitAngle = std::abs(toDegrees(unitAngle)) + 270;
	}

	D3DXVec3Normalize(&unitDir, &unitDir); // normalize vector

	translate += unitDir * speedMult; // move unit

	rotate.x = (float)unitAngle; // set rotation
}