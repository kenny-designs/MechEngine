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
	D3DXVECTOR3 prevPos, magnitude;
	double unitAngle = 0.0f;
	prevPos = translate;
	D3DXVec3Lerp(&translate, &translate, &endPos, speedMult);
	magnitude = translate - prevPos;

	// assumes rotation of unit starts by facing camera at rotation.x = 0
	if (magnitude.x > 0.0f && magnitude.z > 0.0f) // Quadrant 1 
	{
		unitAngle = atan(magnitude.x / magnitude.z);
		unitAngle = std::abs(toDegrees(unitAngle));
	}
	else if (magnitude.x < 0.0f && magnitude.z > 0.0f) // Quadrant 2
	{
		unitAngle = atan(magnitude.z / magnitude.x);
		unitAngle = std::abs(toDegrees(unitAngle)) + 270;
	}
	else if (magnitude.x < 0.0f && magnitude.z < 0.0f) // Quadrant 3
	{
		unitAngle = atan(magnitude.x / magnitude.z);
		unitAngle = std::abs(toDegrees(unitAngle)) + 180;
	}
	else if (magnitude.x > 0.0f && magnitude.z < 0.0f) // Quadrant 4
	{
		unitAngle = atan(magnitude.z / magnitude.x);
		unitAngle = std::abs(toDegrees(unitAngle)) + 90;
	}

	rotate.x = (float)unitAngle + 180.0f; // Apply rotation
}