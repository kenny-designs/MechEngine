#pragma once

#ifndef MY_DIRECT_X_H
#define MY_DIRECT_X_H
#include "MyDirectX.h"
#endif

// define UNIT class which derives from the MODEL class in MyDirectX.h
class UNIT : public MODEL {
public:
	std::string name; // the units name
	float speedMult; // how quickly it moves
	D3DXVECTOR3 endPosition; // the position the UNIT should move to

	void setUnit(std::string filename, std::string unitName, float speed); // sets values for the unit
	void moveUnit(D3DXVECTOR3 endPos); // move unit to selected position
};

// math values
extern const double PI;
extern const double PI_under_180;
extern const double PI_over_180;

// math functions
double toRadians(double degrees);
double toDegrees(double radians);
double wrap(double value, double bounds);
double wrapAngleDegs(double degs);
double LinearVelocityX(double angle);
double LinearVelocityY(double angle);

// allow quick string conversion anywhere in the program
template <class T>
std::string static ToString(const T & t, int places = 2)
{
	std::ostringstream oss;
	oss.precision(places);
	oss.setf(std::ios_base::fixed);
	oss << t;
	return oss.str();
}

// ray casting and picking functions
// rayIntersect casts a ray and returns whether or not it intersected
// with the passed in mesh. Also, gives the location of intersection.
template <class T>
bool rayIntersect(int mouseX, int mouseY, T &ourMesh, CAMERA cam,
	float screenWidth, float screenHeight, D3DXVECTOR3 &intersectPos)
{
	float pointX, pointY;
	D3DXMATRIX projectionMatrix, viewMatrix, inverseViewMatrix,
		worldMatrix, translateMatrix, rotationMatrix, scaleMatrix,
		inverseWorldMatrix;
	D3DXVECTOR3 direction, origin, rayOrigin, rayDirection;
	BOOL hasHit;
	float distanceToCollision;

	// set up variables
	projectionMatrix = cam.m_projectionMatrix;
	viewMatrix = cam.m_viewMatrix;
	worldMatrix = cam.m_worldMatrix;
	origin = cam.position;

	// Converts mouse cursor to -1 to +1 range
	pointX = ((2.0f * (float)mouseX) / (float)screenWidth) - 1.0f;
	pointY = (((2.0f * (float)mouseY) / (float)screenHeight) - 1.0f) * -1.0f;

	// Adjust the points using the projection matrix to account for the
	// aspect ratio of the viewport
	pointX = pointX / projectionMatrix._11;
	pointY = pointY / projectionMatrix._22;

	// Get the inverse of the view matrix
	D3DXMatrixInverse(&inverseViewMatrix, NULL, &viewMatrix);

	// Calculate the direction of the picking ray in view space
	direction.x = (pointX * inverseViewMatrix._11) + (pointY * inverseViewMatrix._21) + inverseViewMatrix._31;
	direction.y = (pointX * inverseViewMatrix._12) + (pointY * inverseViewMatrix._22) + inverseViewMatrix._32;
	direction.z = (pointX * inverseViewMatrix._13) + (pointY * inverseViewMatrix._23) + inverseViewMatrix._33;

	// Get world matrix and translate to location of mesh
	D3DXMatrixTranslation(&translateMatrix,
		ourMesh.translate.x,
		ourMesh.translate.y,
		ourMesh.translate.z);

	// do the same but for rotation
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix,
		(float)toRadians(ourMesh.rotate.x),
		(float)toRadians(ourMesh.rotate.y),
		(float)toRadians(ourMesh.rotate.z));

	// and now for scale
	D3DXMatrixScaling(&scaleMatrix,
		ourMesh.scale.x,
		ourMesh.scale.y,
		ourMesh.scale.z);

	D3DXMATRIX tempMat;
	D3DXMatrixMultiply(&tempMat, &scaleMatrix, &rotationMatrix);
	D3DXMatrixMultiply(&tempMat, &tempMat, &translateMatrix);
	D3DXMatrixMultiply(&worldMatrix, &worldMatrix, &tempMat);

	// Now get the inverse of the translated world matrix
	D3DXMatrixInverse(&inverseWorldMatrix, NULL, &worldMatrix);

	// Now transform the ray origin and the ray direction from view space to world space
	D3DXVec3TransformCoord(&rayOrigin, &origin, &inverseWorldMatrix);
	D3DXVec3TransformNormal(&rayDirection, &direction, &inverseWorldMatrix);

	// Normalize the ray direction
	D3DXVec3Normalize(&rayDirection, &rayDirection);

	// Now perform intersection test
	D3DXIntersect(ourMesh.mesh, &rayOrigin, &rayDirection, &hasHit,
		NULL, NULL, NULL, &distanceToCollision, NULL, NULL);

	if (intersectPos)
	{
		D3DXVec3TransformCoord(&intersectPos, &(rayDirection * distanceToCollision + rayOrigin), &worldMatrix);
	}

	return hasHit;
}